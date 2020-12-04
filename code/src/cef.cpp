#include <mainframe/cef/cef.h>

#include <include/cef_base.h>
#include <include/cef_app.h>
#include <include/cef_client.h>

#include <filesystem>

namespace mainframe {
	namespace cef_ {
		CefEngine& CefEngine::instance() {
			static CefEngine c;
			return c;
		}

		::CefRefPtr<CefApp> CefEngine::getApp() {
			return app;
		}

		int CefEngine::init() {
			#ifdef _WIN32
				CefEnableHighDPISupport();
			#endif

			app = new CefApp(*this);

			CefSettings settings;
			settings.no_sandbox = 1;
			settings.multi_threaded_message_loop = 0;
			settings.windowless_rendering_enabled = 1;
			settings.external_message_pump = 0;

			settings.persist_session_cookies = 0;
			settings.persist_user_preferences = 0;
			settings.log_severity = LOGSEVERITY_DISABLE;
			CefString(&settings.log_file) = "cef.log";

			CefString(&settings.locale) = "en-US";
			CefString(&settings.cache_path) = std::filesystem::current_path().string() + "/cache";

			CefMainArgs main_args;
			int exit_code = CefExecuteProcess(main_args, app.get(), nullptr);
			if (exit_code >= 0) {
				return exit_code;
			}

			bool result = CefInitialize(main_args, settings, app.get(), nullptr);
			if (!result) {
				return 1;
			}

			inited = true;
			return 0;
		}

		CefEngine::~CefEngine() {
			if (!inited) return;

			app->shutdown();

			// ¯\_(-_-)_/¯
			for (int i = 0; i < 3; i++) CefDoMessageLoopWork();
			CefShutdown();
		}

		void CefEngine::tick() {
			CefDoMessageLoopWork();
			app->update();
		}
	}
}