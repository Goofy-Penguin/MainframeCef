#pragma once

#include <mainframe/ui/element.h>
#include <mainframe/cef/cefclient.h>
#include <mainframe/cef/cefapp.h>
#include <mainframe/cef/cefrenderframe.h>

#include <include/cef_browser.h>

namespace mainframe {
	namespace cef_ {
		class WebBrowser : public mainframe::ui::Element {
			std::shared_ptr<CefRenderFrame> frame;
			CefRefPtr<::CefBrowser> browser;
			CefRefPtr<::CefBrowserHost> host;

			bool isDragging = false;
			mainframe::math::Vector2i dragPos;
			mainframe::math::AABBi dragRect;
			mainframe::math::Vector2i lastMousePos;

		public:
			WebBrowser();
			~WebBrowser();
			void loadUrl(const std::string& url);

			void openDevTools();

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
			virtual void keyDown(unsigned int key, mainframe::ui::ModifierKey mods, bool repeating) override;
			virtual void keyUp(unsigned int key, mainframe::ui::ModifierKey mods) override;
			virtual void keyChar(unsigned int key) override;
		};
	}
}