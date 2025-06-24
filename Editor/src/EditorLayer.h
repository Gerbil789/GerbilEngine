#pragma once

#include "EditorContext.h"
#include "Services/SceneController.h"
#include "ImGui/ImGuiLayer.h"

namespace Engine 
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(Timestep ts) override;
		void OnEvent(Event& e) override;
		void OnImGuiRender();


		Engine::ImGuiLayer* m_ImGuiLayer;
	private:
		Scope<SceneController> m_SceneController;
		Ref<EditorContext> m_Context;

	};
}