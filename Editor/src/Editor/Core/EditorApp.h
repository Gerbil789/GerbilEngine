#include "Engine/Core/Application.h"
#include "Editor/Core/EditorWindowManager.h"
#include "Editor/Services/SceneController.h"
#include "Shared/Project.h"

namespace Editor
{
	class EditorApp : public Engine::Application
	{
	public:
		EditorApp(std::filesystem::path projectPath);
		~EditorApp() = default;

		void Run() override;
		void OnEvent(Engine::Event& e) override;

		const Project& GetProject() const { return m_Project; }

	private:
		Project m_Project;

		Scope<Editor::SceneController> m_SceneController;
		Scope<Editor::EditorWindowManager> m_EditorWindowManager;
	};
}

