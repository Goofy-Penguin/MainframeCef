#include <mainframe/cef/cefhelper.h>
#include <GLFW/glfw3.h>

using namespace mainframe::ui;

namespace mainframe {
	namespace cef_ {
		uint32_t Helper::mouseGlfwToCefEvent(CefBrowserHost::MouseButtonType btn) {
			switch (btn) {
				case CefBrowserHost::MouseButtonType::MBT_LEFT: return EVENTFLAG_LEFT_MOUSE_BUTTON;
				case CefBrowserHost::MouseButtonType::MBT_MIDDLE: return EVENTFLAG_MIDDLE_MOUSE_BUTTON;
				case CefBrowserHost::MouseButtonType::MBT_RIGHT: return EVENTFLAG_RIGHT_MOUSE_BUTTON;
				default: return 0;
			}
		}

		int Helper::convertGlfwKeyToCef(int key) {
			switch (key) {
				case GLFW_KEY_ESCAPE: return 27;
				case GLFW_KEY_ENTER: return 13;
				case GLFW_KEY_TAB: return 9;
				case GLFW_KEY_BACKSPACE: return 8;
				case GLFW_KEY_INSERT: return 45;
				case GLFW_KEY_DELETE: return 46;
				case GLFW_KEY_RIGHT: return 39;
				case GLFW_KEY_LEFT: return 37;
				case GLFW_KEY_DOWN: return 40;
				case GLFW_KEY_UP: return 38;
				case GLFW_KEY_PAGE_UP: return 33;
				case GLFW_KEY_PAGE_DOWN: return 34;
				case GLFW_KEY_HOME: return 36;
				case GLFW_KEY_END: return 37;
				case GLFW_KEY_CAPS_LOCK: return 20;
				case GLFW_KEY_SCROLL_LOCK: return 145;
				case GLFW_KEY_NUM_LOCK: return 144;
				case GLFW_KEY_F1: return 112;
				case GLFW_KEY_F2: return 113;
				case GLFW_KEY_F3: return 114;
				case GLFW_KEY_F4: return 115;
				case GLFW_KEY_F5: return 116;
				case GLFW_KEY_F6: return 117;
				case GLFW_KEY_F7: return 118;
				case GLFW_KEY_F8: return 119;
				case GLFW_KEY_F9: return 120;
				case GLFW_KEY_F10: return 121;
				case GLFW_KEY_F11: return 122;
				case GLFW_KEY_F12: return 123;
				case GLFW_KEY_KP_DECIMAL: return 46;
				case GLFW_KEY_KP_DIVIDE: return 111;
				case GLFW_KEY_KP_MULTIPLY: return 106;
				case GLFW_KEY_KP_SUBTRACT: return 109;
				case GLFW_KEY_KP_ADD: return 107;
				case GLFW_KEY_KP_ENTER: return 13;
				case GLFW_KEY_KP_EQUAL: return 187;

				case GLFW_KEY_LEFT_SHIFT:
				case GLFW_KEY_RIGHT_SHIFT: return 16;

				case GLFW_KEY_LEFT_CONTROL:
				case GLFW_KEY_RIGHT_CONTROL: return 17;

				case GLFW_KEY_LEFT_ALT:
				case GLFW_KEY_RIGHT_ALT: return 18;

				case GLFW_KEY_LEFT_SUPER:
				case GLFW_KEY_RIGHT_SUPER: return 91;
				default: return key;
			}
		}

		int Helper::getCefKeyboardModifiers(int key, ModifierKey mods) {
			int modifiers = 0;

			if ((mods & ModifierKey::isLeft) != ModifierKey::none) modifiers |= EVENTFLAG_IS_LEFT;
			if ((mods & ModifierKey::isRight) != ModifierKey::none) modifiers |= EVENTFLAG_IS_RIGHT;

			if ((mods & ModifierKey::alt) != ModifierKey::none) modifiers |= EVENTFLAG_ALT_DOWN;
			if ((mods & ModifierKey::ctrl) != ModifierKey::none) modifiers |= EVENTFLAG_CONTROL_DOWN;
			if ((mods & ModifierKey::shift) != ModifierKey::none) modifiers |= EVENTFLAG_SHIFT_DOWN;

#ifdef _WIN32
			// Low bit set from GetKeyState indicates "toggled".
			if (::GetKeyState(VK_NUMLOCK) & 1) modifiers |= EVENTFLAG_NUM_LOCK_ON;
			if (::GetKeyState(VK_CAPITAL) & 1) modifiers |= EVENTFLAG_CAPS_LOCK_ON;
#else
			// TODO: support caps lock (why?) on non-windows
#endif

			switch (key) {
				// control pad
				case GLFW_KEY_INSERT:
				case GLFW_KEY_DELETE:
				case GLFW_KEY_HOME:
				case GLFW_KEY_END:
				case GLFW_KEY_PAGE_UP:
				case GLFW_KEY_PAGE_DOWN:

					// arrows
				case GLFW_KEY_LEFT:
				case GLFW_KEY_RIGHT:
				case GLFW_KEY_UP:
				case GLFW_KEY_DOWN:

					// numpad
				case GLFW_KEY_KP_0:
				case GLFW_KEY_KP_1:
				case GLFW_KEY_KP_2:
				case GLFW_KEY_KP_3:
				case GLFW_KEY_KP_4:
				case GLFW_KEY_KP_5:
				case GLFW_KEY_KP_6:
				case GLFW_KEY_KP_7:
				case GLFW_KEY_KP_8:
				case GLFW_KEY_KP_9:
				case GLFW_KEY_NUM_LOCK:
				case GLFW_KEY_KP_ADD:
				case GLFW_KEY_KP_DECIMAL:
				case GLFW_KEY_KP_DIVIDE:
				case GLFW_KEY_KP_ENTER:
				case GLFW_KEY_KP_EQUAL:
				case GLFW_KEY_KP_MULTIPLY:
				case GLFW_KEY_KP_SUBTRACT:
					modifiers |= EVENTFLAG_IS_KEY_PAD;
					break;

				default: break;
			}

			return modifiers;
		}
	}
}