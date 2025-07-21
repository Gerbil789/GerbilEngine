#include "Engine/Core/Application.h"
#include "Engine/Core/Project.h"

namespace Editor
{
	class EditorApp : public Engine::Application
	{
	public:
		EditorApp(std::filesystem::path projectPath);
		~EditorApp();

		void Run() override;
		void OnEvent(Engine::Event& e) override;

		const Engine::Project& GetProject() const { return m_Project; }

	private:
		Engine::Project m_Project;
	};
}
