#include <mainframe/cef/cef.h>
#include <mainframe/cef/cefhelper.h>
#include <mainframe/cef/elms/webbrowser.h>
#include <GLFW/glfw3.h>

namespace mainframe {
	namespace cef_ {
		WebBrowser::WebBrowser() {
			auto& cef = CefEngine::instance();
			auto client = cef.getApp()->getClient();

			browser = client->createBrowser("about:blank");
			host = browser->GetHost();
			host->WasResized();

			frame = client->getRenderer()->getFrame(browser->GetIdentifier());
		}

		void WebBrowser::setSize(const math::Vector2i& size) {
			mainframe::ui::Element::setSize(size);
			frame->setSize(size);
			host->WasResized();
		}

		void WebBrowser::update() {

		}

		void WebBrowser::draw(render::Stencil& stencil) {
			stencil.drawTexture({0, 0}, getSize(), frame->getGLHandle());

			// TODO: draw dragging texture
		}

		void WebBrowser::loadUrl(const std::string& url) {
			// TODO: add limiter on urls

			// Clear cookies
			CefRefPtr<CefCookieManager> cookieManager = CefCookieManager::GetGlobalManager(nullptr);
			if (cookieManager != nullptr) {
				cookieManager->DeleteCookies(url, "", nullptr);
			}

			browser->GetMainFrame()->LoadURL(url);
		}

		WebBrowser::~WebBrowser() {
			CefEngine::instance().getApp()->getClient()->destroyBrowser(browser->GetIdentifier());
		}

		void WebBrowser::setFocused(bool focused_) {
			Element::setFocused(focused_);

			if (browser == nullptr) return;
			browser->GetHost()->SendFocusEvent(focused_);
		}


		bool WebBrowser::hitTest(const math::Vector2i& mousePos) {
			return mainframe::ui::Element::hitTest(mousePos) && frame->hitTest(mousePos);
		}

		void WebBrowser::mouseDown(const math::Vector2i& mousePos, unsigned int button, mainframe::ui::ModifierKey mods) {
			auto cefbtn = static_cast<CefBrowserHost::MouseButtonType>(button);
			if (cefbtn > CefBrowserHost::MouseButtonType::MBT_RIGHT) return;

			CefMouseEvent evt;
			evt.x = mousePos.x;
			evt.y = mousePos.y;
			evt.modifiers = Helper::mouseGlfwToCefEvent(cefbtn);

			if (this->dragRect.surfaceArea() > 0 && !isDragging) {
				if (dragRect.contains(mousePos)) {
					dragPos = mousePos;
					isDragging = true;
				}
			}

			browser->GetHost()->SendMouseClickEvent(evt, cefbtn, false, 1);
		}

		void WebBrowser::mouseUp(const math::Vector2i& mousePos, unsigned int button, mainframe::ui::ModifierKey mods) {
			auto cefbtn = static_cast<CefBrowserHost::MouseButtonType>(button);
			if (cefbtn > CefBrowserHost::MouseButtonType::MBT_RIGHT) return;

			CefMouseEvent evt;
			evt.x = mousePos.x;
			evt.y = mousePos.y;
			evt.modifiers = Helper::mouseGlfwToCefEvent(cefbtn);

			if (dragRect.surfaceArea() > 0) {
				isDragging = false;
			}

			browser->GetHost()->SendMouseClickEvent(evt, cefbtn, true, 1);

			auto client = CefEngine::instance().getApp()->getClient();
			if (client->getGlobalDragData() != nullptr) {
				this->browser->GetHost()->DragTargetDrop(evt);
				this->browser->GetHost()->DragSourceEndedAt(evt.x, evt.y, DRAG_OPERATION_NONE);
				this->browser->GetHost()->DragSourceSystemDragEnded();
			}
		}

		void WebBrowser::mouseScroll(const math::Vector2i& mousePos, const math::Vector2i& offset) {
			// TODO: not working yet

			CefMouseEvent evt;
			evt.x = offset.x * 25;
			evt.y = offset.y * 25;
			evt.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;

			browser->GetHost()->SendMouseWheelEvent(evt, offset.x, offset.y);
		}

		void WebBrowser::setHovering(bool hovering) {
			CefMouseEvent evt;
			evt.x = lastMousePos.x;
			evt.y = lastMousePos.y;
			evt.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;

			browser->GetHost()->SendMouseMoveEvent(evt, !hovering);
		}

		void WebBrowser::mouseMove(const math::Vector2i& mousePos) {
			CefMouseEvent evt;
			evt.x = mousePos.x;
			evt.y = mousePos.y;
			evt.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;

			lastMousePos = mousePos;

			browser->GetHost()->DragTargetDragOver(evt, DRAG_OPERATION_EVERY);
			browser->GetHost()->SendMouseMoveEvent(evt, false);
		}

		void WebBrowser::keyDown(unsigned int key, mainframe::ui::ModifierKey mods, bool repeating) {
			if (key == GLFW_KEY_F2) openDevTools();

			int correctedKey = Helper::convertGlfwKeyToCef(key);

			CefKeyEvent evt;
			evt.is_system_key = 0;
			evt.windows_key_code = correctedKey;
			evt.native_key_code = correctedKey;
			evt.type = cef_key_event_type_t::KEYEVENT_KEYDOWN;
			evt.modifiers = Helper::getCefKeyboardModifiers(key, mods);

			if ((evt.modifiers & EVENTFLAG_CONTROL_DOWN) > 0) {
				if (key == GLFW_KEY_C) browser->GetFocusedFrame()->Copy();
				else if (key == GLFW_KEY_V) browser->GetFocusedFrame()->Paste();
				else if (key == GLFW_KEY_X) browser->GetFocusedFrame()->Cut();
			}

			this->browser->GetHost()->SendKeyEvent(evt);
		}

		void WebBrowser::keyUp(unsigned int key, mainframe::ui::ModifierKey mods) {
			int correctedKey = Helper::convertGlfwKeyToCef(key);

			CefKeyEvent evt;
			evt.is_system_key = 0;
			evt.windows_key_code = correctedKey;
			evt.native_key_code = correctedKey;
			evt.type = cef_key_event_type_t::KEYEVENT_KEYUP;
			evt.modifiers = Helper::getCefKeyboardModifiers(key, mods);

			this->browser->GetHost()->SendKeyEvent(evt);
		}

		void WebBrowser::keyChar(unsigned int key) {
			int correctedKey = Helper::convertGlfwKeyToCef(key);

			CefKeyEvent evt;
			evt.is_system_key = 0;
			evt.windows_key_code = correctedKey;
			evt.native_key_code = correctedKey;
			evt.type = cef_key_event_type_t::KEYEVENT_CHAR;
			evt.modifiers = Helper::getCefKeyboardModifiers(key, mainframe::ui::ModifierKey::none);

			this->browser->GetHost()->SendKeyEvent(evt);
		}


		void WebBrowser::openDevTools() {
#ifndef _DEBUG
			return;
#endif

			CefWindowInfo devpopup;
#ifdef _WIN32
			devpopup.SetAsWindowless(GetConsoleWindow());
			devpopup.SetAsPopup(nullptr, "DevTools");
#else
			devpopup.SetAsWindowless(0);
			// Linux build of CEF does not seem to support SetAsPopup
#endif

			browser->GetHost()->ShowDevTools(devpopup, nullptr, CefBrowserSettings(), CefPoint());
		}
	}
}
