#include <mainframe/cef/cefprocesshandler.h>

#include <include/cef_base.h>
#include <include/cef_app.h>
#include <include/cef_client.h>

#include <filesystem>

namespace mainframe {
	namespace cef_ {
		void CefProcessHandler::OnWebKitInitialized() {
			CefMessageRouterConfig config;
			config.js_query_function = "queryUI";
			config.js_cancel_function = "cancelQueryUI";

			this->messageRouter = CefMessageRouterRendererSide::Create(config);
		}

		void CefProcessHandler::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) {
  			CefRefPtr<CefV8Value> object = context->GetGlobal();
			// Inject the game location on the browser, so we can easily access it
			object->SetValue("gameLocation",  CefV8Value::CreateString(std::filesystem::current_path().string()), V8_PROPERTY_ATTRIBUTE_NONE);

			this->messageRouter->OnContextCreated(browser, frame, context);
		}

		void CefProcessHandler::OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) {
			this->messageRouter->OnContextReleased(browser, frame, context);
		}

		bool CefProcessHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) {
			return this->messageRouter->OnProcessMessageReceived(browser, frame, source_process, message);
		}
	}
}