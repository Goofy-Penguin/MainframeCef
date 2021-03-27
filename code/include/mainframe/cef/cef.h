#pragma once

#include <mainframe/cef/cefapp.h>

namespace mainframe {
	namespace cef_ {
		class CefEngine {
			bool inited = false;
			::CefRefPtr<CefApp> app;

		public:
			static CefEngine& instance();
			static CefRefPtr<CefClient> client();

			bool init();
			void tick();
			void shutdown();

			::CefRefPtr<CefApp> getApp();

			~CefEngine();
		};
	}
}