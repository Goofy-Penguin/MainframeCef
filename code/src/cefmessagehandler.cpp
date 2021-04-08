#include <mainframe/cef/cefmessagehandler.h>
#include <mainframe/cef/elms/webbrowser.h>
#include <mainframe/cef/cef.h>

#include <include/cef_base.h>
#include <include/cef_app.h>
#include <include/cef_client.h>

#include <fmt/printf.h>
#include <filesystem>

namespace mainframe::cef_ {
	CefRefPtr<CefMessageRouterBrowserSide::Callback> CefMessageHandler::queryUI(int browserId, const std::string& queryId) {
		auto client = CefEngine::instance().getApp()->getClient();
		auto ourBrowser = client->getBrowser(browserId).second;
		if (ourBrowser == nullptr) return nullptr;

		auto event = persistentEvents.find(std::to_string(browserId) + "_" + queryId);
		if (event == persistentEvents.end()) return nullptr;

		return event->second.first;
	}

	bool CefMessageHandler::OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<CefMessageRouterBrowserSide::Callback> callback) {
		auto client = CefEngine::instance().getApp()->getClient();
		auto ourBrowser = client->getBrowser(browser->GetIdentifier()).second;
		if (ourBrowser == nullptr) return false;

		const std::string& message = request;

		if (persistent) {
			persistentEvents[std::to_string(browser->GetIdentifier()) + "_" + message] = std::make_pair(callback, query_id);
			return true;
		}

		nlohmann::json reqData;

		try {
			reqData = nlohmann::json::parse(message);
		} catch (const nlohmann::json::exception& err) {
			if (callback) callback->Failure(500, "Data must be valid json");
			return true;
		}

		auto nameNode = reqData.find("event");
		if (nameNode == reqData.end()) {
			if (callback) callback->Failure(500, "Data must be table with `event` key");
			return true;
		}

		if (!nameNode->is_string()) {
			if (callback) callback->Failure(500, "Key `event` must be string");
			return true;
		}

		std::string eventName = *nameNode;
		auto listener = ourBrowser->onEvent.find(eventName);
		if (listener == ourBrowser->onEvent.end()) {
			// try global
			listener = client->onEvent.find(eventName);

			if (listener == client->onEvent.end()) {
				fmt::print("event '{}' not found", eventName);
				if (callback) callback->Failure(404, "Event not found");
				return true;
			}
		}

		(*listener).second(reqData, callback);
		return true;
	}

	void CefMessageHandler::OnQueryCanceled(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 query_id) {
		std::string name;
		for (auto& pair : persistentEvents) {
			if (pair.second.second != query_id) continue;

			pair.second.first->Failure(500, "Canceled");
			name = pair.first;
			break;
		}

		if (name.empty()) return;
		persistentEvents.erase(name);
	}
}
