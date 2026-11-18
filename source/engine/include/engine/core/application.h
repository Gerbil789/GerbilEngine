#pragma once

#include "engine/core/window.h"
#include "engine/graphics/renderer/renderer.h"

namespace engine
{
	struct Application
	{
		static engine::Window s_window;
		static engine::Renderer s_renderer;
		static bool s_running;

		struct Configuration
		{
			std::filesystem::path projectPath;
			engine::WindowSpecification windowSpec;
			engine::SceneHandle startupScene;
		};

		Application(const Configuration& config);
		~Application();

		void Update();
		void Draw();
	};
}