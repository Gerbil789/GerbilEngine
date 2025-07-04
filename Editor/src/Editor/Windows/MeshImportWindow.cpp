#include "enginepch.h"
#include "MeshImportWindow.h"

namespace Editor
{
	void MeshImportWindow::SetScene(void* scene)
	{
		m_Scene = (aiScene*)scene;

		int meshCount = m_Scene->mNumMeshes;
		int materialCount = m_Scene->mNumMaterials;

	}
	void MeshImportWindow::OnUpdate(Engine::Timestep ts)
	{
		if (!m_IsVisible) { return; }
		if (!m_Scene) 
		{ 
			LOG_ERROR("No scene to import");
			return; 
		}

		ImGui::Begin("Mesh Import");

		ImGui::Text("File: %s", m_Scene->mRootNode->mName.C_Str());


		ImGui::End();
	}
}