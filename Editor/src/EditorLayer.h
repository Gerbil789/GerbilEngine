#pragma once

#include "EditorContext.h"
#include "Controllers/SceneController.h"

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
		EditorContext m_Context;
		SceneController m_SceneController;
	};
}