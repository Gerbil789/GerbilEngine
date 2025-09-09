#include "Engine/Core/Application.h"
#include "Engine/Utils/Path.h"

namespace Editor
{
	class EditorApp : public Engine::Application
	{
	public:
		EditorApp(const Engine::ApplicationSpecification& specification);
		~EditorApp();

		void Run() override;
		void OnEvent(Engine::Event& e) override;
	};

	inline Engine::Application* CreateApp(Engine::ApplicationCommandLineArgs args)
	{
		Engine::ApplicationSpecification spec;
		spec.title = "Gerbil Editor";
		spec.workingDirectory = GetExecutableDir();
		spec.args = args;

		return new EditorApp(spec);
	}
}
