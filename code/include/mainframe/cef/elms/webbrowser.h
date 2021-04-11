#pragma once

#include <mainframe/ui/element.h>
#include <mainframe/utils/eventnamed.hpp>
#include <mainframe/cef/cefclient.h>
#include <mainframe/cef/cefapp.h>
#include <mainframe/cef/cefrenderframe.h>

#include <include/cef_browser.h>

namespace mainframe {
	namespace cef_ {
		class WebBrowser;
		class CefBrowserWrapper : public ::CefBrowser {
		public:
			WebBrowser* obj;
		};

		class WebBrowser : public mainframe::ui::Element {
			std::shared_ptr<CefRenderFrame> frame;
			CefRefPtr<::CefBrowser> browser;
			CefRefPtr<::CefBrowserHost> host;

			bool isDragging = false;
			mainframe::math::Vector2i dragStartPos;
			mainframe::math::Vector2i lastMousePos;
			mainframe::math::AABBi dragRect;

		public:
			mainframe::utils::EventNamed<const nlohmann::json&, CefRefPtr<CefMessageRouterBrowserSide::Callback>> onEvent;
			mainframe::utils::EventNamed<> onLoaded;

			WebBrowser();
			~WebBrowser();

			void loadUrl(const std::string& url);

			void injectCSS(const std::string& cssUrl, bool async = false);
			void injectJSScript(const std::string& jsUrl, bool async = false, bool announce = false);

			void injectJS(const std::string& js);

			void openDevTools();
			void setDragArea(const math::AABBi& aabb);
			void onBrowserDrag(const math::Vector2i& mousePos);
			void queryUI(const std::string& queryId, const CefString& data);

		 	void mouseEnter();
			void mouseLeave();

			virtual void setSize(const math::Vector2i& size) override;
			virtual void update() override;
			virtual void draw(render::Stencil& stencil) override;
			virtual void setFocused(bool focused_) override;
			virtual void setHovering(bool hovering) override;

			virtual bool hitTest(const math::Vector2i& mousePos) override;
			virtual void mouseDown(const math::Vector2i& mousePos, unsigned int button, mainframe::ui::ModifierKey mods) override;
			virtual void mouseUp(const math::Vector2i& mousePos, unsigned int button, mainframe::ui::ModifierKey mods) override;
			virtual void mouseScroll(const math::Vector2i& mousePos, const math::Vector2i& offset) override;
			virtual void mouseMove(const math::Vector2i& mousePos) override;
			virtual void keyDown(unsigned int key, unsigned int scancode, mainframe::ui::ModifierKey mods, bool repeating) override;
			virtual void keyUp(unsigned int key, unsigned int scancode, mainframe::ui::ModifierKey mods) override;
			virtual void keyChar(unsigned int key) override;
		};
	}
}