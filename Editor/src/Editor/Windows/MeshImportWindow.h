#pragma once

#include "EditorWindow.h"
#include "Engine/Core/Core.h"


namespace Editor
{
	//TODO: fix this whole window, it is a mess right now
	struct ImportSettings 
	{
		bool ImportMaterials = true;
		bool ImportTextures = true;
	};

	class MeshImportWindow : public EditorWindow
	{
	public:
		MeshImportWindow(EditorWindowManager* context) : EditorWindow(context) {}
		~MeshImportWindow() = default;

		void SetScene(void* scene);


		void OnUpdate(Engine::Timestep ts) override;
	};
}