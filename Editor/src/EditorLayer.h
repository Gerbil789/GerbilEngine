#pragma once

#include "EditorContext.h"
#include "Services/SceneController.h"

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
		void OnImGuiRender() override;

	private:
		Scope<SceneController> m_SceneController;

		Ref<EditorContext> m_Context;


	};
}