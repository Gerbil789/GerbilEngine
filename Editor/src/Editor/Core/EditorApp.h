#include "Engine/Core/Application.h"
#include "Editor/Core/EditorLayer.h"
#include "Editor/ImGui/ImGuiLayer.h"

namespace Editor
{
	class EditorApp : public Engine::Application
	{
	public:
		EditorApp();
		~EditorApp();

		void Run() override;

		ImGuiLayer* GetImGuiLayer() { return m_EditorLayer->m_ImGuiLayer; }
	private:
		EditorLayer* m_EditorLayer;
	};
}

