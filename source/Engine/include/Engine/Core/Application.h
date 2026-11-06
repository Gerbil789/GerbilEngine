#pragma once

#include "Engine/Core/Window.h"
#include "Engine/Graphics/Renderer/Renderer.h"

namespace engine
{
	class Application
	{
	public:
		struct Configuration
		{
			std::filesystem::path projectPath;
			engine::WindowSpecification windowSpec;
			engine::SceneHandle startupScene;
		};

	public:
		Application(const Configuration& config);
		~Application();

		void Update();
		void Draw();

	public:
		static engine::Window s_window;
		static engine::Renderer s_renderer;
		static bool s_running;
	};

}

