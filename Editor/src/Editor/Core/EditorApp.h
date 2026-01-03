#pragma once

#include "Engine/Core/Application.h"
#include "Engine/Core/GameInstance.h"

namespace Editor
{
	class EditorApp : public Engine::Application
	{
	public:
		EditorApp(const Engine::ApplicationSpecification& specification);
		~EditorApp();

		virtual void Run() override;
		virtual void OnEvent(Engine::Event& e) override;

		void PlayGame();

	private:
		std::unique_ptr<Engine::GameInstance> m_GameInstance;
	};

	EditorApp CreateApp(Engine::ApplicationCommandLineArgs args);
}
