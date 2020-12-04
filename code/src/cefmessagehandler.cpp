#include <mainframe/cef/cefmessagehandler.h>

#include <include/cef_base.h>
#include <include/cef_app.h>
#include <include/cef_client.h>

#include <filesystem>

namespace mainframe {
	namespace cef_ {
		CefRefPtr<CefMessageRouterBrowserSide::Callback> CefMessageHandler::queryUI(int browserId, const std::string& queryId) {
			std::string callbackId = persistIdentifier + queryId + std::to_string(browserId);

			if (this->m_persistentCallbacks.find(callbackId) == this->m_persistentCallbacks.end()) return nullptr;
			return this->m_persistentCallbacks[callbackId];
		}

		void CefMessageHandler::registerGlobalQuery(const std::string& queryId, const std::function<CallbackFunc>& callback) {
			if (callback == nullptr) return;
			this->m_globalCallbacks[globalIdentifier + queryId] += callback;
		}

		bool CefMessageHandler::OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<CefMessageRouterBrowserSide::Callback> callback) {
			if (callback == nullptr) return false;

			// If the request is persistent, it means the client wants a server <-> connection, else it's just a one timer
			const std::string& message = request;
			int browserId = browser->GetIdentifier();

			if (persistent) {
				// Check if the message contains the unique persist identifier
				if (message.find(persistIdentifier) == 0) {
					std::string callbackId = message + std::to_string(browserId);
					if (this->m_persistentCallbacks.find(callbackId) == this->m_persistentCallbacks.end()) {
						this->m_persistentCallbacks[callbackId] = callback;
						this->m_persistentCallbacksIndexer[query_id] = callbackId;
					}
				} else {
					// Badly formed message
					callback->Failure(500, "Persistent requests must start with " + persistIdentifier);
					return true;
				}
			} else {
				if (message.find('{') == std::string::npos || message.find('}') == std::string::npos || message.find("event") == std::string::npos) {
					callback->Failure(500, "Invalid request, needs to contain event and it needs to be a json object");
					return true;
				}

				nlohmann::json jsonRequest = nlohmann::json::parse(message);
				std::string jsonEvent = jsonRequest["event"];

				if (jsonEvent.find(globalIdentifier) != std::string::npos) {
					if (this->m_globalCallbacks.find(jsonEvent) == this->m_globalCallbacks.end()) {
						callback->Failure(500, "Global requests not registered");
						return true;
					}

					this->m_globalCallbacks[jsonEvent](browserId, jsonRequest, callback);
				} else {
					//this->onUIQuery(browserId, jsonRequest, callback);
				}
			}

			return true;
		}

		void CefMessageHandler::OnQueryCanceled(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 query_id) {
			std::string requestId = this->m_persistentCallbacksIndexer[query_id];
			if (requestId.empty()) return;

			CefRefPtr<CefMessageRouterBrowserSide::Callback> callback = this->m_persistentCallbacks[requestId];
			if (callback == nullptr) return;

			this->m_persistentCallbacksIndexer.erase(query_id);
			this->m_persistentCallbacks.erase(requestId);
			callback->Failure(500, "Canceled");
		}
	}
}