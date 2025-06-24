#pragma once

#include "EditorWindow.h"
#include "Engine/Core/Core.h"
#include <assimp/scene.h>

namespace Editor
{
	struct ImportSettings 
	{
		bool ImportMaterials = true;
		bool ImportTextures = true;
	};

	class MeshImportWindow : public EditorWindow
	{
	public:
		MeshImportWindow(EditorContext* context) : EditorWindow(context) {}
		~MeshImportWindow() = default;

		void SetScene(void* scene);


		virtual void OnImGuiRender() override;

	private:
		aiScene* m_Scene = nullptr;
	};
}