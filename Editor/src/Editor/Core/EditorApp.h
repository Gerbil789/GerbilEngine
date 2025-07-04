#include "Engine/Core/Application.h"
#include "Editor/Core/EditorLayer.h"
#include "Shared/Project.h"

namespace Editor
{
	class EditorApp : public Engine::Application
	{
	public:
		EditorApp(std::filesystem::path projectPath);
		~EditorApp() = default;

		void Run() override;

		const Project& GetProject() const { return m_Project; }

	private:
		Project m_Project;

	};
}

