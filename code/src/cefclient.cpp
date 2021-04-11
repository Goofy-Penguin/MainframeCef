#include <mainframe/cef/cefclient.h>

#include <include/cef_base.h>
#include <include/cef_app.h>
#include <include/cef_client.h>
#include <include/cef_browser_process_handler.h>
#include <include/base/cef_logging.h>

#include <filesystem>
#include <memory>
#include <GLFW/glfw3.h>

namespace mainframe {
	namespace cef_ {
		CefClient::CefClient() {
			renderer = new CefRenderer();
		}

		CefClient::~CefClient() {
			std::vector<int> browserIds;
			for (auto it = browsers.begin(); it != browsers.end(); ++it) {
				browserIds.push_back(it->first);
			}

			for (auto& id : browserIds) {
				destroyBrowser(id);
			}

			browsers.clear();
			messageRouter->RemoveHandler(&messageHandler);
		}

		void CefClient::update() {

		}

		CefRefPtr<CefDragData> CefClient::getGlobalDragData() {
			return globalDragData;
		}

		CefRefPtr<CefRenderer> CefClient::getRenderer() {
			return renderer;
		}

		std::pair<CefRefPtr<::CefBrowser>, WebBrowser*>& CefClient::createBrowser(const std::string& url) {
			CefWindowInfo window_info;
			window_info.windowless_rendering_enabled = 1;

			CefBrowserSettings browserSettings;
			browserSettings.windowless_frame_rate = 60;

			CefRefPtr<::CefBrowser> browser = CefBrowserHost::CreateBrowserSync(window_info, this, url, browserSettings, nullptr, nullptr);
			if (browser == nullptr) throw std::runtime_error("error while creating CEF browser object");

			auto browserId = browser->GetIdentifier();
			renderer->createFrame(browserId);

			browsers[browserId].first = browser;
			return browsers[browserId];
		}

		std::pair<CefRefPtr<::CefBrowser>, WebBrowser*>& CefClient::getBrowser(int identifier) {
			return browsers[identifier];
		}

		void CefClient::destroyBrowser(int id) {
			auto& browser = browsers[id].first;
			if (browser == nullptr) return;

			browser->StopLoad();
			browser->GetHost()->CloseDevTools();
			browser->GetHost()->CloseBrowser(true);

			browsers.erase(id);
		}

		void CefClient::goBackOrForward(int browserId, bool goBack) {
			auto& browser = browsers[browserId].first;
			if (browser == nullptr) return;

			if (goBack) browser->GoBack();
			else browser->GoForward();
		}

		void CefClient::browserVisibility(int browserId, bool hidden) {
			auto& browser = browsers[browserId].first;
			if (browser == nullptr) return;

			browser->GetHost()->WasHidden(hidden);
		}

		CefBrowserHost::MouseButtonType CefClient::glMouseToCef(int mousebtn) {
			switch (mousebtn) {
				case GLFW_MOUSE_BUTTON_LEFT:
					return CefBrowserHost::MouseButtonType::MBT_LEFT;
				case GLFW_MOUSE_BUTTON_MIDDLE:
					return CefBrowserHost::MouseButtonType::MBT_MIDDLE;
				case GLFW_MOUSE_BUTTON_RIGHT:
					return CefBrowserHost::MouseButtonType::MBT_RIGHT;

				default:
					return CefBrowserHost::MouseButtonType::MBT_LEFT;
			}
		}

		void CefClient::OnDraggableRegionsChanged(CefRefPtr<::CefBrowser> browser, CefRefPtr<CefFrame> frame, const std::vector<CefDraggableRegion>& regions) {
			CEF_REQUIRE_UI_THREAD();

			if (regions.size() <= 0) return;

			auto webb = browsers[browser->GetIdentifier()].second;
			if (webb == nullptr) return;

			// TODO: Merge rectangles or store the current
			for (auto& p : regions) {
				if (!p.draggable) continue;

				webb->setDragArea({
					p.bounds.x,
					p.bounds.y,
					p.bounds.width,
					p.bounds.height});
			}
		}

		bool CefClient::OnDragEnter(CefRefPtr<::CefBrowser> browser, CefRefPtr<CefDragData> dragData, DragOperationsMask mask) {
			CEF_REQUIRE_UI_THREAD();

			globalDragData = dragData;
			return false;
		}

		bool CefClient::OnProcessMessageReceived(CefRefPtr<::CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) {
			CEF_REQUIRE_UI_THREAD();
			onProcessMessage(message);

			if (messageRouter == nullptr) return false;
			return messageRouter->OnProcessMessageReceived(browser, frame, source_process, message);
		}

		void CefClient::OnLoadStart(CefRefPtr<::CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transition_type) {
			CEF_REQUIRE_UI_THREAD();
			if (!frame->IsMain()) return;

			onLoadingStart();
		}

		void CefClient::OnLoadEnd(CefRefPtr<::CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) {
			CEF_REQUIRE_UI_THREAD();
			if (!frame->IsMain()) return;

			onLoadingEnd(frame->GetURL(), httpStatusCode);

			auto& webb = browsers[browser->GetIdentifier()].second;
			if (webb != nullptr) webb->onLoaded();
		}

		void CefClient::OnAfterCreated(CefRefPtr<::CefBrowser> browser) {
			CEF_REQUIRE_UI_THREAD();
			if (messageRouter != nullptr) return;

			// Create the browser-side router for query handling.
			CefMessageRouterConfig config;
			config.js_query_function = "queryUI";
			config.js_cancel_function = "cancelQueryUI";
			messageRouter = CefMessageRouterBrowserSide::Create(config);

			// Register handlers with the router.
			messageRouter->AddHandler(&messageHandler, false);
		}

		bool CefClient::queryUI(int browserId, const std::string& queryId, const CefString& data) {
			auto callback = messageHandler.queryUI(browserId, queryId);
			if (callback == nullptr) return false;

			callback->Success(data);
			return true;
		}

		void CefClient::OnBeforeClose(CefRefPtr<::CefBrowser> browser) {
			CEF_REQUIRE_UI_THREAD();
		}

		void CefClient::OnRenderProcessTerminated(CefRefPtr<::CefBrowser> browser, TerminationStatus status) {
			CEF_REQUIRE_UI_THREAD();

			if (messageRouter == nullptr) return;
			messageRouter->OnRenderProcessTerminated(browser);
		}

		bool CefClient::OnBeforeBrowse(CefRefPtr<::CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool user_gesture, bool is_redirect) {
			CEF_REQUIRE_UI_THREAD();
			if (messageRouter == nullptr) return false;

			messageRouter->OnBeforeBrowse(browser, frame);
			return false;
		}

		bool CefClient::RunContextMenu(CefRefPtr<::CefBrowser> browser, CefRefPtr<CefFrame> frame,CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model, CefRefPtr<CefRunContextMenuCallback> callback) {
			CEF_REQUIRE_UI_THREAD();

			return true; // Override default context menu
		}


		CefRefPtr<CefDragHandler> CefClient::GetDragHandler() {
			return this;
		}

		CefRefPtr<CefRenderHandler> CefClient::GetRenderHandler() {
			return renderer;
		}

		CefRefPtr<CefContextMenuHandler> CefClient::GetContextMenuHandler() {
			return this;
		}

		CefRefPtr<CefLifeSpanHandler> CefClient::GetLifeSpanHandler() {
			return this;
		}

		CefRefPtr<CefRequestHandler> CefClient::GetRequestHandler() {
			return this;
		}

		CefRefPtr<CefLoadHandler> CefClient::GetLoadHandler() {
			return this;
		}
	}
}
