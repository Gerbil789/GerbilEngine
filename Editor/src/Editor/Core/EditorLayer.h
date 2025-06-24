#pragma once

#include "Editor/Core/EditorContext.h"
#include "Editor/Services/SceneController.h"
#include "Editor/Core/Core.h"
#include "Editor/ImGui/ImGuiLayer.h"

namespace Editor
{
	class EditorLayer : public Engine::Layer
	{
	public:
		EditorLayer();

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(Engine::Timestep ts) override;
		void OnEvent(Engine::Event& e) override;
		void OnImGuiRender();


		ImGuiLayer* m_ImGuiLayer;
	private:
		Scope<SceneController> m_SceneController;
		Ref<EditorContext> m_Context;

	};
}