#pragma once

#include <include/cef_app.h>

namespace mainframe {
	namespace cef_ {
		class CefEngine;
		class CefClient;
		class CefProcessHandler;

		class CefApp : public ::CefApp, public CefBrowserProcessHandler {
			CefRefPtr<mainframe::cef_::CefClient> client;
			CefEngine& cef;

		public:
			CefRefPtr<CefProcessHandler> processHandler;

			CefApp(CefEngine& cef_);

			void shutdown();
			void update();

			CefRefPtr<CefClient> getClient();

			virtual void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) override;
			virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;

			virtual void OnContextInitialized() override;
			virtual void OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line) override;

			IMPLEMENT_REFCOUNTING(CefApp);
			DISALLOW_COPY_AND_ASSIGN(CefApp);
		};
	}
}