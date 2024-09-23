#pragma once

#include "EditorWindowBase.h"
#include "Engine/Core/Core.h"
#include <assimp/scene.h>

namespace Engine
{
	struct ImportSettings 
	{
		bool ImportMaterials = true;
		bool ImportTextures = true;
	};

	class MeshImportWindow : public EditorWindowBase
	{
	public:
		MeshImportWindow() {}
		~MeshImportWindow() = default;

		void SetScene(void* scene);


		virtual void OnImGuiRender() override;

	private:
		aiScene* m_Scene = nullptr;
	};
}