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

		void CefRenderer::generateDragTexture(const CefRefPtr<CefImage>& image) {
			int img_width, img_height;

			CefRefPtr<CefBinaryValue> bitmapData = image->GetAsBitmap(1, CEF_COLOR_TYPE_RGBA_8888, CEF_ALPHA_TYPE_POSTMULTIPLIED, img_width, img_height);
			if (bitmapData == nullptr) return;

			this->dragTexture.clear();
			this->dragTexture.resize({img_width, img_height});

			bitmapData->GetData(&this->dragTexture.data(), this->dragTexture.data().size(), 0);
			if (bitmapData == nullptr) return;

			this->dragTexture.bind();
			this->dragTexture.upload();

			this->dragTexture.data().clear();
		}

		void CefRenderer::OnPaint(CefRefPtr<::CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer, int width, int height) {
			CEF_REQUIRE_UI_THREAD();
			if (width <= 0 || height <= 0) return;

			auto frame = frames[browser->GetIdentifier()];
			if (frame == nullptr) return;

			auto pixelbuff = reinterpret_cast<const int*>(buffer);
			frame->updateBuffer({width, height}, {pixelbuff, pixelbuff + width * height});
		}

		/*void CefRenderer::OnCursorChange(CefRefPtr<::CefBrowser> browser, CefCursorHandle cursor, CursorType type, const CefCursorInfo& custom_cursor_info) {
			CEF_REQUIRE_UI_THREAD();

			int glCursor = GLFW_ARROW_CURSOR;

			switch (type) {
				case CT_POINTER: glCursor = GLFW_ARROW_CURSOR; break;
				case CT_HAND: glCursor = GLFW_HAND_CURSOR; break;
				case CT_IBEAM: glCursor = GLFW_IBEAM_CURSOR; break;
				case CT_CROSS: glCursor = GLFW_CROSSHAIR_CURSOR; break;
				case CT_COLUMNRESIZE: glCursor = GLFW_HRESIZE_CURSOR; break;
				case CT_ROWRESIZE: glCursor = GLFW_VRESIZE_CURSOR; break;
				default:
					break;
			}

			glfwSetCursor(core::getInstance().m_MainWindow.Handle, glfwCreateStandardCursor(glCursor));
		}*/

		bool CefRenderer::StartDragging(CefRefPtr<::CefBrowser> browser, CefRefPtr<CefDragData> drag_data, DragOperationsMask allowed_ops, int x, int y) {
			if (this->isDragging) return false;

			CefMouseEvent ev;
			ev.x = x;
			ev.y = y;
			ev.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;

			browser->GetHost()->DragTargetDragEnter(drag_data, ev, allowed_ops);
			/*if (drag_data->HasImage())
				this->generateDragTexture(drag_data->GetImage());*/
				
			this->isDragging = true;
			return true;
		}

		void CefRenderer::UpdateDragCursor(CefRefPtr<::CefBrowser> browser, DragOperation operation) {
			// TODO : Change the cursor to a dragging one
		}

		void CefRenderer::stopDragging() {
			if (!this->isDragging) return;

			this->dragTexture.clear();
			this->isDragging = false;
		}

		std::shared_ptr<CefRenderFrame> CefRenderer::getFrame(int id) {
			return frames[id];
		}
	}
}