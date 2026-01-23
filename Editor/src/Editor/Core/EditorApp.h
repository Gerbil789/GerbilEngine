#pragma once

#include "Engine/Core/Application.h"

namespace Editor
{
	class EditorApp : public Engine::Application
	{
	public:
		EditorApp(const Engine::ApplicationSpecification& specification);
		~EditorApp();

		virtual void Run() override;
		virtual void OnEvent(Engine::Event& e) override;

		void PlayGame(); //TODO: move somewhere else
	};

	EditorApp CreateApp(Engine::ApplicationCommandLineArgs args);
}