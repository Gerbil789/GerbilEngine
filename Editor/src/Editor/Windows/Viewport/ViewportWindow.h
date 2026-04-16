#pragma once

#include "Editor/Windows/IEditorWindow.h"
#include "Engine/Graphics/Renderer/Renderer.h"

namespace Editor
{
	class ViewportWindow : public IEditorWindow
	{
	public:

		void Initialize() override;
		void Draw() override;

		static void UpdateViewportSize();
		
		static Engine::Renderer& GetRenderer();
	};
}