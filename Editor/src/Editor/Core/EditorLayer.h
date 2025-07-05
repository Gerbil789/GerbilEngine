#pragma once

#include "Editor/Core/EditorWindowManager.h"
#include "Editor/Services/SceneController.h"
#include "Editor/Core/Core.h"

namespace Editor
{
	class EditorLayer : public Engine::Layer
	{
	public:
		EditorLayer();
		~EditorLayer();

		void OnEvent(Engine::Event& e) override;
		void OnUpdate(Engine::Timestep ts) override;

		void BlockEvents(bool block) { m_BlockEvents = block; }
		static void ResetLayout(); // TODO: is this good?

	private:
		void BeginFrame();
		void EndFrame();

	private:
		Scope<SceneController> m_SceneController;
		Ref<EditorWindowManager> m_WindowManager;

		bool m_BlockEvents = true;

	};
}