#pragma once

#include <include/cef_client.h>
#include <mainframe/ui/modifierkey.h>

namespace mainframe {
	namespace cef_ {
		static class Helper {
		public:
			static uint32_t mouseGlfwToCefEvent(CefBrowserHost::MouseButtonType btn);
			static int convertGlfwKeyToCef(int key);
			static int getCefKeyboardModifiers(int key, mainframe::ui::ModifierKey mods);
		};
	}
}
