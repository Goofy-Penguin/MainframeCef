#include <mainframe/game/engine.h>
#include <mainframe/game/window.h>
#include <mainframe/render/stencil.h>
#include <mainframe/ui/scene.h>
#include <fmt/printf.h>
#include <mainframe/cef/elms/webbrowser.h>
#include <mainframe/cef/cef.h>

using namespace mainframe::game;
using namespace mainframe::render;
using namespace mainframe::math;
using namespace mainframe::ui;

class GameTest : public Engine {
	Window& window;
	Stencil stencil;
	std::shared_ptr<Scene> scene = Scene::create();

public:
	virtual void init() override {
		scene->setWindow(window);

		auto browser = scene->createChild<mainframe::cef_::WebBrowser>();
		browser->setSize(window.getSize());
		browser->loadUrl("https://google.com");
	}

	virtual void draw() override {
		scene->draw(stencil);
		window.swapBuffer();
	}

	virtual void tick() override {
		Window::pollEvents();
	}

	virtual void quit() override {
		Engine::quit();
		window.close();
	}

	virtual void update() override {
		scene->update();
		mainframe::cef_::CefEngine::instance().tick();

		if (window.getShouldClose()) {
			quit();
		}
	}

	GameTest(Window& w) : window(w) {
		stencil.setWindowSize(w.getSize());
	}
};

int main(int argc, char* argv[]) {
	auto& cef = mainframe::cef_::CefEngine::instance();
	if (cef.init() != 0) {
		fmt::print("Failed to init CEF\n");
		return -1;
	}

	Window w;
	if (!w.create(1024, 1024, "mainframe.cef.catboard")) {
		fmt::print("Failed to create window\n");
		return -1;
	}

	GameTest e(w);
	e.setFPS(75);

	e.init();
	e.run();

	return 0;
}