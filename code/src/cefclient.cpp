#include <mainframe/cef/cefclient.h>

#include <include/cef_base.h>
#include <include/cef_app.h>
#include <include/cef_client.h>
#include <include/cef_browser_process_handler.h>
#include <include/base/cef_logging.h>

#include <filesystem>
#include <memory>
#include <glfw/glfw3.h>

namespace mainframe {
	namespace cef_ {
		CefClient::CefClient() {
			//messageHandler.onUIQuery += [this](int browserId, nlohmann::json& jsonData, CefRefPtr<CefMessageRouterBrowserSide::Callback> callback) {
			//	this->onUIQuery(browserId, jsonData, callback);
			//};

			renderer = new CefRenderer();
		}

		CefClient::~CefClient() {
			std::vector<int> browserIds;
			for (std::map<int, CefRefPtr<::CefBrowser>>::iterator it = browsers.begin(); it != browsers.end(); ++it) {
				browserIds.push_back(it->first);
			}

			for (auto& id : browserIds) {
				destroyBrowser(id);
			}

			browsers.clear();
			messageRouter->RemoveHandler(&messageHandler);
		}

		void CefClient::update() {
			for (auto& pair : browsers) {
				// TODO: handle mouse movement
			}
		}

		CefRefPtr<CefDragData> CefClient::getGlobalDragData() {
			return globalDragData;
		}

		CefRefPtr<CefRenderer> CefClient::getRenderer() {
			return renderer;
		}

		CefRefPtr<::CefBrowser> CefClient::createBrowser(const std::string& url) {
			CefWindowInfo window_info;
			window_info.windowless_rendering_enabled = 1;

			CefBrowserSettings browserSettings;
			browserSettings.windowless_frame_rate = 30;

			CefRefPtr<::CefBrowser> browser = CefBrowserHost::CreateBrowserSync(window_info, this, url, browserSettings, nullptr, nullptr);
			if (browser == nullptr) return nullptr;

			auto browserId = browser->GetIdentifier();
			renderer->createFrame(browserId);

			return this->browsers[browserId] = browser;
		}

		void CefClient::destroyBrowser(int id) {
			auto& browser = this->browsers[id];
			if (browser == nullptr) return;

			browser->StopLoad();
			browser->GetHost()->CloseDevTools();
			browser->GetHost()->CloseBrowser(true);

			this->browsers.erase(id);
		}

		void CefClient::goBackOrForward(int browserId, bool goBack) {
			auto& browser = this->browsers[browserId];
			if (browser == nullptr) return;

			if (goBack) browser->GoBack();
			else browser->GoForward();
		}

		void CefClient::browserVisibility(int browserId, bool hidden) {
			auto& browser = this->browsers[browserId];
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

			/*
			auto& webb = this->browsers[browser->GetIdentifier()];
			if (webb == nullptr) return;
			if (regions.size() <= 0) return;

			// TODO: Merge rectangles or store the current
			for (auto& p : regions) {
				if (!p.draggable) continue;
				webb->dragArea = new TomatoLib::Rectangle(
					static_cast<float>(p.bounds.x),
					static_cast<float>(p.bounds.y),
					static_cast<float>(p.bounds.width),
					static_cast<float>(p.bounds.height));
			}
			*/
		}

		bool CefClient::OnDragEnter(CefRefPtr<::CefBrowser> browser, CefRefPtr<CefDragData> dragData, DragOperationsMask mask) {
			CEF_REQUIRE_UI_THREAD();

			this->globalDragData = dragData;
			return false;
		}

		bool CefClient::OnProcessMessageReceived(CefRefPtr<::CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) {
			CEF_REQUIRE_UI_THREAD();
			this->onProcessMessage(message);

			if (this->messageRouter == nullptr) return false;
			return this->messageRouter->OnProcessMessageReceived(browser, frame, source_process, message);
		}

		void CefClient::OnLoadStart(CefRefPtr<::CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transition_type) {
			CEF_REQUIRE_UI_THREAD();
			if (!frame->IsMain()) return;

			this->onLoadingStart();
		}

		void CefClient::OnLoadEnd(CefRefPtr<::CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) {
			CEF_REQUIRE_UI_THREAD();
			if (!frame->IsMain()) return;

			this->onLoadingEnd(frame->GetURL(), httpStatusCode);
		}

		void CefClient::OnAfterCreated(CefRefPtr<::CefBrowser> browser) {
			CEF_REQUIRE_UI_THREAD();
			if (this->messageRouter != nullptr) return;

			// Create the browser-side router for query handling.
			CefMessageRouterConfig config;
			config.js_query_function = "queryUI";
			config.js_cancel_function = "cancelQueryUI";
			this->messageRouter = CefMessageRouterBrowserSide::Create(config);

			// Register handlers with the router.
			this->messageRouter->AddHandler(&this->messageHandler, false);
		}

		void CefClient::OnBeforeClose(CefRefPtr<::CefBrowser> browser) {
			CEF_REQUIRE_UI_THREAD();
		}

		void CefClient::OnRenderProcessTerminated(CefRefPtr<::CefBrowser> browser, TerminationStatus status) {
			CEF_REQUIRE_UI_THREAD();

			if (this->messageRouter == nullptr) return;
			this->messageRouter->OnRenderProcessTerminated(browser);
		}

		bool CefClient::OnBeforeBrowse(CefRefPtr<::CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool user_gesture, bool is_redirect) {
			CEF_REQUIRE_UI_THREAD();
			if (this->messageRouter == nullptr) return false;

			this->messageRouter->OnBeforeBrowse(browser, frame);
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
