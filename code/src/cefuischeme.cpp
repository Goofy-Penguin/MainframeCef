#include <mainframe/cef/cefuischeme.h>
#include <include/cef_parser.h>
#include <include/wrapper/cef_helpers.h>

#include <filesystem>
#include <fmt/format.h>

namespace mainframe::cef_ {
    bool CefUIScheme::Open(CefRefPtr<CefRequest> request, bool &handle_request, CefRefPtr<CefCallback> callback) {
        DCHECK(!CefCurrentlyOn(TID_UI) && !CefCurrentlyOn(TID_IO));
        handle_request = true;

        std::string url = request->GetURL();

        auto extPos = url.find_last_of('.');
        if (extPos == std::string::npos) {
            callback->Cancel();
            return false;
        }

        std::string ext = url.substr(extPos + 1);
        mime_type_ = CefGetMimeType(ext);
        if(mime_type_.empty()){
            mime_type_ = "application/x-binary";
        }

        std::string path = std::filesystem::current_path().string();
        url.erase(0, 10); // Remove the http/ias protocol

        if(mime_type_ == "text/html") {
            injectCSS("global.css");
            injectCSS("components/index.css");
            injectJSFile("components/index.js");
        }

        if (!buffer.writeFromFile(path + url)) {
            callback->Cancel();
            return false;
        }

        buffer.seek(buffer.begin());
        callback->Continue();
        return true;
    }

    void CefUIScheme::injectCSS(const std::string& cssUrl) {
        auto script = fmt::format("<link href='http://ias/content/ui/{0}' rel='stylesheet' />", cssUrl);
        buffer.write(script, false);
    }

    void CefUIScheme::injectJSFile(const std::string& jsUrl) {
        auto script = fmt::format("<script src='http://ias/content/ui/{0}' type='module'></script>", jsUrl);
        buffer.write(script, false);
    }

    void CefUIScheme::GetResponseHeaders(CefRefPtr<CefResponse> response, int64 &response_length, CefString &redirectUrl) {
        CEF_REQUIRE_IO_THREAD();
        if(buffer.empty()) return;

        response->SetMimeType(mime_type_);
        response->SetStatus(200);
        response_length = buffer.size();
    }

    void CefUIScheme::Cancel() {
        CEF_REQUIRE_IO_THREAD();
    }

    bool CefUIScheme::Read(void *data_out, int bytes_to_read, int &bytes_read, CefRefPtr<CefResourceReadCallback> callback) {
        DCHECK(!CefCurrentlyOn(TID_UI) && !CefCurrentlyOn(TID_IO));

        auto toRead = std::min(static_cast<size_t>(bytes_to_read), buffer.size() - buffer.tell());
        if (toRead <= 0) {
            return false;
        }

        memcpy(data_out, buffer.data() + buffer.tell(), toRead);
        bytes_read = static_cast<int>(toRead);
        buffer.seek(buffer.tell() + toRead);

        return true;
    }
}