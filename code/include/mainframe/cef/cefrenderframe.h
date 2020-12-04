#pragma once

#include <include/cef_render_handler.h>
#include <mainframe/math/vector2.h>
#include <vector>

namespace mainframe {
	namespace cef_ {
		class CefRenderFrame {
			unsigned int handle = -1;
			std::vector<int> buffer;
			mainframe::math::Vector2i size = {1, 1};

		public:
			CefRenderFrame();
			~CefRenderFrame();

			bool hitTest(const mainframe::math::Vector2i& location);
			void updateBuffer(const mainframe::math::Vector2i& buffSize, const std::vector<int>& buff);
			void setSize(const mainframe::math::Vector2i& viewSize);

			unsigned int getGLHandle();
			::CefRect getRect();
			mainframe::math::Vector2i getSize();
		};
	}
}
