#include <Engine.h>
#include <Engine/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Engine 
{
	class Editor : public Application
	{
	public:
		Editor() : Application("Gerbil Editor")
		{
			PushLayer(new EditorLayer());
		}

		~Editor()
		{

		}
	};

	Application* Engine::CreateApplication()
	{
		return new Editor();
	}
}