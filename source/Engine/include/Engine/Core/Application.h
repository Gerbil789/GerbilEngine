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
			engine::Scene startupScene;
		};

	public:
		Application(const Configuration& config);
		~Application();

		void Run();

	protected:
		void Update();
		void Draw();

	public:
		static engine::Window s_Window;
		static engine::Renderer s_Renderer;
		static bool s_Running;
	};

}

