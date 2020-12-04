#pragma once

#include <mainframe/cef/cefrenderframe.h>
#include <mainframe/render/color.h>
#include <mainframe/render/texture.h>

#include <include/cef_render_handler.h>

namespace mainframe {
	namespace cef_ {
		class CefRenderer : public CefRenderHandler {
			std::map<int, std::shared_ptr<CefRenderFrame>> frames;

		public:
			mainframe::render::Texture dragTexture;
			bool isDragging;

			virtual void GetViewRect(CefRefPtr<::CefBrowser> browser, CefRect& rect) override;
			virtual void OnPaint(CefRefPtr<::CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer, int width, int height) override;
			//virtual void OnCursorChange(CefRefPtr<::CefBrowser> browser, CefCursorHandle cursor, CursorType type, const CefCursorInfo& custom_cursor_info) override;
			virtual bool StartDragging(CefRefPtr<::CefBrowser> browser, CefRefPtr<CefDragData> drag_data, DragOperationsMask allowed_ops, int x, int y) override;
			virtual void UpdateDragCursor(CefRefPtr<::CefBrowser> browser, DragOperation operation) override;

			void stopDragging();

			void createFrame(int id);
			std::shared_ptr<CefRenderFrame> getFrame(int id);

			IMPLEMENT_REFCOUNTING(CefRenderer);
		};
	}
}
