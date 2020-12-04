#pragma once

#include <include/cef_app.h>
#include <include/cef_render_handler.h>
#include <include/wrapper/cef_message_router.h>

namespace mainframe {
	namespace cef_ {
		class CefProcessHandler : public CefRenderProcessHandler {
		public:
			CefRefPtr<CefMessageRouterRendererSide> messageRouter = nullptr;

			virtual void OnWebKitInitialized() override;
			virtual void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override;
			virtual void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override;
			virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;

			IMPLEMENT_REFCOUNTING(CefProcessHandler);
		};
	}
}