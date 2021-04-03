#include <mainframe/cef/cefrenderframe.h>
#include <GL/glew.h>

namespace mainframe {
	namespace cef_ {
		CefRenderFrame::CefRenderFrame() {
			glGenTextures(1, &handle);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, handle);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			buffer = {0};
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, buffer.data());
		}

		CefRenderFrame::~CefRenderFrame() {
			glDeleteTextures(1, &handle);
		}

		bool CefRenderFrame::hitTest(const mainframe::math::Vector2i& location) {
			auto index = location.y * size.x + location.x;
			if (index < 0 || index >= buffer.size()) return false;

			auto col = buffer[index];
			auto ret = col & 0xFF000000;
			return ret != 0;
		}

		void CefRenderFrame::setSize(const mainframe::math::Vector2i& viewSize) {
			size = viewSize;
		}

		void CefRenderFrame::updateBuffer(const mainframe::math::Vector2i& buffSize, const std::vector<int>& buff) {
			buffer = buff;
			size = buffSize;

			glBindTexture(GL_TEXTURE_2D, handle);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, buffer.data());
		}

		::CefRect CefRenderFrame::getRect() {
			return {0, 0, size.x, size.y};
		}

		unsigned int CefRenderFrame::getGLHandle() {
			return handle;
		}
	}
}
