#pragma once

#ifdef IV_PLATFORM_WINDOWS

extern Ivory::Application* Ivory::create_application();

int main(int argc, char** argv) {
	Ivory::Log::init();

	auto app = Ivory::create_application();
	app->run();
	delete app;
}

#endif