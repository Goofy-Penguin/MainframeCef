#include <mainframe/cef/cef.h>
#include <mainframe/cef/cefapp.h>
#include <mainframe/cef/cefclient.h>
#include <mainframe/cef/cefprocesshandler.h>

#include <include/cef_base.h>
#include <include/cef_app.h>
#include <include/cef_client.h>

#include <filesystem>

namespace mainframe {
	namespace cef_ {
		CefApp::CefApp(CefEngine& cef_) : cef(cef_) {
			processHandler = new CefProcessHandler();
			client = new CefClient();
		}

		CefApp::~CefApp() {
			shutdown();
		}

		void CefApp::shutdown() {
			client = nullptr;
			processHandler = nullptr;
		}

		CefRefPtr<CefClient> CefApp::getClient() {
			return client;
		}

		void CefApp::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) {
			if (!process_type.empty()) return;

			// Speed up CEF! (Based on https://github.com/cefsharp/CefSharp/blob/3f5dc4248ae10cff500d37e120048101faea4646/CefSharp.Core.Runtime/CefSettingsBase.h)
			// (you'll loose WebGL support but gain increased FPS and reduced CPU usage).
            command_line->AppendSwitchWithValue("disable-features", "CalculateNativeWinOcclusion,WinUseBrowserSpellChecker");
            command_line->AppendSwitchWithValue("autoplay-policy", "no-user-gesture-required");

			command_line->AppendSwitch("no-proxy-server");
			command_line->AppendSwitch("disable-pdf-extension");
			command_line->AppendSwitch("disable-plugins-discovery");
			command_line->AppendSwitch("disable-plugins");
		}

		CefRefPtr<CefRenderProcessHandler> CefApp::GetRenderProcessHandler() {
			return processHandler;
		}

		void CefApp::OnContextInitialized() {

		}

		void CefApp::OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line) {

		}

		void CefApp::update() {
			client->update();
		}
	}
}