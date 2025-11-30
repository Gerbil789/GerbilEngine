#include "enginepch.h"
#include "InspectorWindow.h"
#include "Engine/Asset/AssetManager.h"
#include "Editor/Core/EditorContext.h"
#include <imgui.h>

namespace Editor
{
	void InspectorWindow::OnUpdate()
	{
		ImGui::Begin("Inspector");

		auto selection = EditorContext::GetActiveSelection();

		switch (selection.Type)
		{
		case SelectionType::Entity:
		{
			m_EntityInspector.Draw(selection.Entity);
			break;
		}

		case SelectionType::Asset:
		{
			DrawAssetPanel(selection.AssetID);
			break;
		}

		default:
			ImGui::Text("Nothing selected");

		}

		ImGui::End();
	}

	void InspectorWindow::DrawAssetPanel(Engine::UUID assetID)
	{
		auto type = Engine::AssetManager::GetAssetType(assetID);

		switch (type)
		{
		case Engine::AssetType::Texture2D:
		{
			ImGui::Text("Texture2D selected");
			break;
		}

		case Engine::AssetType::Shader:
		{
			m_ShaderInspector.Draw(Engine::AssetManager::GetAsset<Engine::Shader>(assetID));
			break;
		}

		case Engine::AssetType::Material:
		{
			m_MaterialInspector.Draw(Engine::AssetManager::GetAsset<Engine::Material>(assetID));
			break;
		}


		default:
		{
			ImGui::Text("Unsupported asset type selected");
		}
		}
	}
}