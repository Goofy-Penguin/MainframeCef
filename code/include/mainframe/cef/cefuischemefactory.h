#pragma once

#include <include/cef_scheme.h>

namespace mainframe {
	namespace cef_ {
		class CefUISchemeFactory : public CefSchemeHandlerFactory {

		public:
            CefUISchemeFactory() {}

        	virtual CefRefPtr<CefResourceHandler> Create(
                CefRefPtr<CefBrowser> browser,
                CefRefPtr<CefFrame> frame,
                const CefString& scheme_name,
                CefRefPtr<CefRequest> request) override;

			IMPLEMENT_REFCOUNTING(CefUISchemeFactory);
			DISALLOW_COPY_AND_ASSIGN(CefUISchemeFactory);
		};
	}
}