#pragma once

#include <mainframe/cef/cefapp.h>

namespace mainframe {
	namespace cef_ {
		class CefEngine {
			bool inited = false;
			::CefRefPtr<CefApp> app;

		public:
			static CefEngine& instance();

			int init();
			void tick();

			::CefRefPtr<CefApp> getApp();

			~CefEngine();
		};
	}
}