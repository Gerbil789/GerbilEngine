#pragma once

#ifdef ENGINE_PLATFORM_WINDOWS

extern Engine::Application* Engine::CreateApplication();

int main(int argc, char** argv)
{
	Engine::Log::Init();

	//test logs
	ENGINE_LOG_ERROR("ERROR LOG!");
	ENGINE_LOG_WARNING("WARNING LOG!");
	ENGINE_LOG_INFO("INFO LOG!");
	ENGINE_LOG_TRACE("TRACE LOG!");
	
	LOG_ERROR("ERROR LOG!");
	LOG_WARNING("WARNING LOG!");
	LOG_INFO("INFO LOG!");
	LOG_TRACE("TRACE LOG!");

	auto app = Engine::CreateApplication();
	app->Run();
	delete app;
}


#else
	#error Engine only supports Windows!
#endif