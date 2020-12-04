#pragma once

#include <include/wrapper/cef_message_router.h>
#include <include/wrapper/cef_helpers.h>
#include <nlohmann/json.hpp>
#include <mainframe/utils/event.hpp>

namespace mainframe {
	namespace cef_ {
		class CefMessageHandler : public CefMessageRouterBrowserSide::Handler {
		public:
			using CallbackFunc = void(int browserId, nlohmann::json& jsonData, CefRefPtr<CefMessageRouterBrowserSide::Callback> callback);

		private:
			std::map<std::string, CefRefPtr<CefMessageRouterBrowserSide::Callback>> m_persistentCallbacks;
			std::map<int64, std::string> m_persistentCallbacksIndexer;
			std::map<std::string, mainframe::utils::Event<int, nlohmann::json&, CefRefPtr<CefMessageRouterBrowserSide::Callback>>> m_globalCallbacks;

			const std::string persistIdentifier = "LISTENER_";
			const std::string globalIdentifier = "GLOBAL_";

		public:
			CefMessageHandler() = default;
			~CefMessageHandler() = default;

			CefRefPtr<CefMessageRouterBrowserSide::Callback> queryUI(int browserId, const std::string& queryId);
			void registerGlobalQuery(const std::string& queryId, const std::function<CallbackFunc>& callback);

			// Overrides
			virtual bool OnQuery(CefRefPtr<::CefBrowser> browser,
								 CefRefPtr<CefFrame> frame,
								 int64 query_id,
								 const CefString& request,
								 bool persistent,
								 CefRefPtr<Callback> callback) override;

			virtual void OnQueryCanceled(CefRefPtr<::CefBrowser> browser,
										 CefRefPtr<CefFrame> frame,
										 int64 query_id) override;

			mainframe::utils::Event<CallbackFunc> onUIQuery;
			DISALLOW_COPY_AND_ASSIGN(CefMessageHandler);
		};
	}
}