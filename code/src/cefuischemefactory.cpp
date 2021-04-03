#include <mainframe/cef/cefuischemefactory.h>
#include <mainframe/cef/cefuischeme.h>
#include <include/wrapper/cef_helpers.h>

namespace mainframe {
    namespace cef_ {
        CefRefPtr<CefResourceHandler> CefUISchemeFactory::Create(CefRefPtr<CefBrowser> browser,
                                       CefRefPtr<CefFrame> frame,
                                       const CefString& scheme_name,
                                       CefRefPtr<CefRequest> request) {
            CEF_REQUIRE_IO_THREAD();
            return new CefUIScheme();
        }
    }
}