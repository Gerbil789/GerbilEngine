#pragma once

#ifdef ENGINE_PLATFORM_WINDOWS

extern Engine::Application* Engine::CreateApplication();

int main(int argc, char** argv)
{
	Engine::Log::Init();
	
	ENGINE_LOG_WARNING("Initialized Log!");
	int a = 5;
	LOG_TRACE("Hello! {0}", a);
	LOG_INFO("Hello! {0}", 8);
	LOG_WARNING("Hello! {0}", "BRUH");
	LOG_ERROR("Hello! {0}", a);

	auto app = Engine::CreateApplication();
	app->Run();
	delete app;
}


#else
	#error Engine only supports Windows!
#endif