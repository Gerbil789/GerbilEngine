#pragma once

#include "Engine/Core/Layer.h"

namespace Editor
{
	class ImGuiLayer : public Engine::Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Engine::Event& e) override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }
		static void ResetLayout();

	private:
		bool m_BlockEvents = true;
	};
}