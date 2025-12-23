#include "Engine/Core/Application.h"
#include "Engine/Utils/FileWatcher.h"

namespace Editor
{
	class EditorApp : public Engine::Application
	{
	public:
		EditorApp(const Engine::ApplicationSpecification& specification);
		~EditorApp();

		void Run() override;
		void OnEvent(Engine::Event& e) override;

	private:
		Scope<Engine::FileWatcher> m_FileWatcher;

	};

	EditorApp CreateApp(Engine::ApplicationCommandLineArgs args);
}
