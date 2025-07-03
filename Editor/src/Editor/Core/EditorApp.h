#include "Engine/Core/Application.h"
#include "Editor/Core/EditorLayer.h"

namespace Editor
{
	class EditorApp : public Engine::Application
	{
	public:
		EditorApp();
		~EditorApp() = default;

		void Run() override;
	};
}

