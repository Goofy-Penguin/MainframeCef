#pragma once

#include <include/cef_client.h>
#include <mainframe/cef/cefrenderer.h>
#include <mainframe/cef/cefmessagehandler.h>
#include <mainframe/cef/elms/webbrowser.h>
#include <mainframe/utils/event.hpp>

namespace mainframe {
	namespace cef_ {
		class WebBrowser;

		class CefClient :
			public ::CefClient,
			public CefRequestHandler,
			public CefLoadHandler,
			public CefLifeSpanHandler,
			public CefDragHandler,
			public CefContextMenuHandler {

			CefRefPtr<CefRenderer> renderer;
			std::map<int, CefRefPtr<::CefBrowser>> browsers;

			// Handles the browser side of query routing.
			CefRefPtr<CefMessageRouterBrowserSide> messageRouter;
			CefRefPtr<CefDragData> globalDragData;

			CefMessageHandler messageHandler;

			WebBrowser* m_focused = nullptr;

		public:
			~CefClient();
			CefClient();

			void update();
			CefRefPtr<CefRenderer> getRenderer();

			CefRefPtr<CefDragData> getGlobalDragData();

			// EVENTS
			mainframe::utils::Event<CefRefPtr<CefProcessMessage>> onProcessMessage;
			mainframe::utils::Event<> onLoadingStart;
			mainframe::utils::Event<const std::string&, int> onLoadingEnd;
			mainframe::utils::Event<CefMessageHandler::CallbackFunc> onUIQuery;

			CefRefPtr<::CefBrowser> createBrowser(const std::string& url);
			void destroyBrowser(int id);

			void goBackOrForward(int browserId, bool goBack);
			void browserVisibility(int browserId, bool hidden);

			CefBrowserHost::MouseButtonType glMouseToCef(int mousebtn);

			// OVERRIDES
			virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override;
			virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
			virtual CefRefPtr<CefRequestHandler> GetRequestHandler() override;
			virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override;
			virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override;
			virtual CefRefPtr<CefDragHandler> GetDragHandler() override;

			virtual bool OnDragEnter(CefRefPtr<::CefBrowser> browser, CefRefPtr<CefDragData> dragData, DragOperationsMask mask) override;
			virtual void OnDraggableRegionsChanged(CefRefPtr<::CefBrowser> browser, CefRefPtr<CefFrame> frame, const std::vector<CefDraggableRegion>& regions) override;
			virtual bool OnProcessMessageReceived(CefRefPtr<::CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;
			virtual void OnLoadStart(CefRefPtr<::CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transition_type) override;
			virtual void OnLoadEnd(CefRefPtr<::CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) override;
			virtual void OnAfterCreated(CefRefPtr<::CefBrowser> browser) override;
			virtual void OnBeforeClose(CefRefPtr<::CefBrowser> browser) override;
			virtual void OnRenderProcessTerminated(CefRefPtr<::CefBrowser> browser, TerminationStatus status) override;
			virtual bool OnBeforeBrowse(CefRefPtr<::CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool user_gesture, bool is_redirect) override;
			virtual bool RunContextMenu(CefRefPtr<::CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model, CefRefPtr<CefRunContextMenuCallback> callback) override;

			IMPLEMENT_REFCOUNTING(CefClient);
		};
	}
}
