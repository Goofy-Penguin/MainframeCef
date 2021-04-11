#include <mainframe/cef/cef.h>
#include <mainframe/cef/cefhelper.h>
#include <mainframe/cef/elms/webbrowser.h>
#include <GLFW/glfw3.h>

namespace mainframe::cef_ {
	WebBrowser::WebBrowser() {
		auto& cef = CefEngine::instance();
		auto client = cef.getApp()->getClient();

		auto& binding = client->createBrowser("about:blank");
		binding.second = this;

		browser = binding.first;
		host = browser->GetHost();
		host->WasResized();

		frame = client->getRenderer()->getFrame(browser->GetIdentifier());
	}

	WebBrowser::~WebBrowser() {
		CefEngine::instance().getApp()->getClient()->destroyBrowser(browser->GetIdentifier());
	}

	void WebBrowser::setSize(const math::Vector2i& size) {
		mainframe::ui::Element::setSize(size);
		frame->setSize(size);
		host->WasResized();
	}

	void WebBrowser::update() {

	}

	void WebBrowser::injectJS(const std::string& js) {
		CefRefPtr<CefFrame> frame = browser->GetMainFrame();
		frame->ExecuteJavaScript(js, frame->GetURL(), 0);
	}

	void WebBrowser::setDragArea(const math::AABBi& aabb) {
		dragRect = aabb;
	}

	void WebBrowser::draw(render::Stencil& stencil) {
		stencil.drawTexture({0, 0}, getSize(), frame->getGLHandle());


		/*auto client = CefEngine::instance().getApp()->getClient();
		stencil.drawTexture({0, 0}, getSize(), client->getRenderer()->dragTexture->getGLHandle());*/

		// TODO: draw dragging texture
	}

	void WebBrowser::queryUI(const std::string& queryId, const CefString& data) {
		auto client = CefEngine::instance().getApp()->getClient();
		client->queryUI(browser->GetIdentifier(), queryId, data);
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

	void WebBrowser::setFocused(bool focused_) {
		Element::setFocused(focused_);

		if (browser == nullptr) return;
		browser->GetHost()->SendFocusEvent(focused_);
	}


	bool WebBrowser::hitTest(const math::Vector2i& mousePos) {
		return mainframe::ui::Element::hitTest(mousePos) && frame->hitTest(mousePos - getPos());
	}

	void WebBrowser::mouseDown(const math::Vector2i& mousePos, unsigned int button, mainframe::ui::ModifierKey mods) {
		auto cefbtn = static_cast<CefBrowserHost::MouseButtonType>(button);
		if (cefbtn > CefBrowserHost::MouseButtonType::MBT_RIGHT) return;

		CefMouseEvent evt;
		evt.x = mousePos.x;
		evt.y = mousePos.y;
		evt.modifiers = Helper::mouseGlfwToCefEvent(cefbtn);

		if (dragRect.surfaceArea() > 0 && !isDragging) {
			if (dragRect.contains(mousePos)) {
				dragStartPos = mousePos;
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

		isDragging = false;
		browser->GetHost()->SendMouseClickEvent(evt, cefbtn, true, 1);

		auto client = CefEngine::instance().getApp()->getClient();
		if (client->getGlobalDragData() != nullptr) {
			this->browser->GetHost()->DragTargetDrop(evt);
			this->browser->GetHost()->DragSourceEndedAt(evt.x, evt.y, DRAG_OPERATION_NONE);
			this->browser->GetHost()->DragSourceSystemDragEnded();
		}
	}

	void WebBrowser::mouseEnter() {
		auto client = CefEngine::instance().getApp()->getClient();
		if (client->getGlobalDragData() == nullptr) return;

		CefMouseEvent evt;
		evt.x = lastMousePos.x;
		evt.y = lastMousePos.y;
		evt.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;

		this->browser->GetHost()->DragTargetDragEnter(client->getGlobalDragData(), evt, DRAG_OPERATION_EVERY);
	}

	void WebBrowser::mouseLeave() {
		auto client = CefEngine::instance().getApp()->getClient();
		if (client->getGlobalDragData() == nullptr) return;

		this->browser->GetHost()->DragTargetDragLeave();
	}

	void WebBrowser::mouseScroll(const math::Vector2i& mousePos, const math::Vector2i& offset) {
		if(!this->getHovering()) return;

		CefMouseEvent evt;
		evt.x = mousePos.x;
		evt.y = mousePos.y;

		browser->GetHost()->SendMouseWheelEvent(evt, offset.x * 2, offset.y * 2);
	}

	void WebBrowser::setHovering(bool hovering) {
		mainframe::ui::Element::setHovering(hovering);
		hovering ? mouseEnter() : mouseLeave();
	}

	void WebBrowser::mouseMove(const math::Vector2i& mousePos) {
		if (isDragging) onBrowserDrag(mousePos + getPos());

		CefMouseEvent evt;
		evt.x = mousePos.x;
		evt.y = mousePos.y;
		evt.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;

		lastMousePos = mousePos;
		browser->GetHost()->DragTargetDragOver(evt, DRAG_OPERATION_EVERY);
		browser->GetHost()->SendMouseMoveEvent(evt, !this->getHovering());
	}

	void WebBrowser::onBrowserDrag(const math::Vector2i& mousePos) {
		this->setPos({mousePos.x - dragStartPos.x, mousePos.y - dragStartPos.y});
	}

	void WebBrowser::keyDown(unsigned int key, unsigned int scancode, mainframe::ui::ModifierKey mods, bool repeating) {
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

	void WebBrowser::keyUp(unsigned int key, unsigned int scancode, mainframe::ui::ModifierKey mods) {
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
