#pragma once

#include "Editor/Windows/IEditorWindow.h"

namespace Engine { class Material; }

namespace Editor
{
	class MaterialEditorWindow : public IEditorWindow
	{
	public:
		void Draw() override;
		static void SetMaterial(Engine::Material* material);
	};
}