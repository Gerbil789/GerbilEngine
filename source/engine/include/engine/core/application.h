#pragma once

#include "engine/core/window.h"
#include "engine/graphics/renderer/renderer.h"

namespace engine
{
	struct Application
	{
		inline static Window s_window;
		inline static Renderer<BackgroundPass, OpaquePass, NormalPass, WireframePass> s_renderer;
		inline static bool s_running = true;

		struct Configuration
		{
			std::filesystem::path projectPath;
			WindowSpecification windowSpec;
			SceneHandle startupScene;
		};

		Application(const Configuration& config);
		~Application();

		void Update();
		void Draw();
	};
}