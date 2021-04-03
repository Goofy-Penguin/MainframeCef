#pragma once

#include <include/cef_resource_handler.h>
#include <mainframe/networking/packet.h>

namespace mainframe {
	namespace cef_ {
		class CefUIScheme : public CefResourceHandler {
            mainframe::networking::Packet buffer;
			std::string mime_type_;
		public:
        	CefUIScheme() {}

			std::string DumpRequestContents(CefRefPtr<CefRequest> request);

			virtual bool Open(CefRefPtr<CefRequest> request,bool& handle_request,CefRefPtr<CefCallback> callback) override;
			virtual void GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl) override;

			virtual void Cancel() override;
			virtual bool Read(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefResourceReadCallback> callback) override;

			IMPLEMENT_REFCOUNTING(CefUIScheme);
			DISALLOW_COPY_AND_ASSIGN(CefUIScheme);
		};
	}
}