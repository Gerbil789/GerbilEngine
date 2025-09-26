#include "Engine/Core/Application.h"
#include "Engine/Utils/Path.h"
#include "Engine/Utils/FileWatcher.h"

namespace Editor
{
	class EditorApp : public Engine::Application
	{
	public:
		EditorApp(const Engine::ApplicationSpecification& specification);
		void Shutdown();

		void Run() override;
		void OnEvent(Engine::Event& e) override;

	private:
		Scope<Engine::FileWatcher> m_FileWatcher;
	};

	inline EditorApp CreateApp(Engine::ApplicationCommandLineArgs args)
	{
		Engine::ApplicationSpecification spec;
		spec.title = "Gerbil Editor";
		spec.workingDirectory = GetExecutableDir();
		spec.args = args;

		return EditorApp(spec);
	}
}
