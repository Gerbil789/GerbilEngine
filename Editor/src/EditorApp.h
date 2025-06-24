#include "Engine/Core/Application.h"
#include "ImGui/ImGuiLayer.h"
#include "EditorLayer.h"

class EditorApp : public Engine::Application
{
public:
	EditorApp();
	~EditorApp();

	void Run() override;

	Engine::ImGuiLayer* GetImGuiLayer() { return m_EditorLayer->m_ImGuiLayer; }
private:
	Engine::EditorLayer* m_EditorLayer;
};

