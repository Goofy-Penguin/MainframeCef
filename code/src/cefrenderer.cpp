#include <mainframe/cef/cefrenderer.h>

#include <include/cef_base.h>
#include <include/cef_app.h>
#include <include/cef_client.h>
#include <include/wrapper/cef_helpers.h>

#include <GL/glew.h>

namespace mainframe {
	namespace cef_ {
		void CefRenderer::GetViewRect(CefRefPtr<::CefBrowser> browser, CefRect& rect) {
			CEF_REQUIRE_UI_THREAD();

			// Never return an empty rectangle, cef does not like it
			auto frame = frames[browser->GetIdentifier()];
			if (frame == nullptr) {
				rect = CefRect(0, 0, 1, 1);
				return;
			}

			rect = frame->getRect();
		}

		void CefRenderer::createFrame(int browserId) {
			frames[browserId] = std::make_shared<CefRenderFrame>();
		}

		void CefRenderer::OnPaint(CefRefPtr<::CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer, int width, int height) {
			CEF_REQUIRE_UI_THREAD();
			if (width <= 0 || height <= 0) return;

			auto frame = frames[browser->GetIdentifier()];
			if (frame == nullptr) return;

			auto pixelbuff = reinterpret_cast<const int*>(buffer);
			frame->updateBuffer({width, height}, {pixelbuff, pixelbuff + width * height});
		}

		//void CefRenderer::OnCursorChange(CefRefPtr<::CefBrowser> browser, CefCursorHandle cursor, CursorType type, const CefCursorInfo& custom_cursor_info) {

		//}

		bool CefRenderer::StartDragging(CefRefPtr<::CefBrowser> browser, CefRefPtr<CefDragData> drag_data, DragOperationsMask allowed_ops, int x, int y) {
			return false;
		}

		void CefRenderer::UpdateDragCursor(CefRefPtr<::CefBrowser> browser, DragOperation operation) {

		}

		void CefRenderer::stopDragging() {

		}

		std::shared_ptr<CefRenderFrame> CefRenderer::getFrame(int id) {
			return frames[id];
		}
	}
}