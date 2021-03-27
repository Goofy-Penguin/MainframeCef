#pragma once

#include <include/wrapper/cef_message_router.h>
#include <include/wrapper/cef_helpers.h>
#include <nlohmann/json.hpp>
#include <mainframe/utils/event.hpp>

namespace mainframe {
	namespace cef_ {
		class CefMessageHandler : public CefMessageRouterBrowserSide::Handler {
		public:
			CefMessageHandler() = default;
			~CefMessageHandler() = default;

			CefRefPtr<CefMessageRouterBrowserSide::Callback> queryUI(int browserId, const std::string& queryId);

			// Overrides
			virtual bool OnQuery(CefRefPtr<::CefBrowser> browser, CefRefPtr<CefFrame> frame,int64 query_id, const CefString& request, bool persistent,CefRefPtr<Callback> callback) override;

			DISALLOW_COPY_AND_ASSIGN(CefMessageHandler);
		};
	}
}