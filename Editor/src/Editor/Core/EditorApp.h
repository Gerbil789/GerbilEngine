#pragma once

#include "Engine/Core/Application.h"

namespace Editor
{
	class EditorApp : public Engine::Application
	{
	public:
		EditorApp(const Engine::ApplicationSpecification& specification);
		~EditorApp();

		void Run() override;
		void OnEvent(Engine::Event& e) override;
	};

	EditorApp CreateApp(Engine::ApplicationCommandLineArgs args);
}
