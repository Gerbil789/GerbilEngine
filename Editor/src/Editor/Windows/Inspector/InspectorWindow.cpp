#include "InspectorWindow.h"
#include "Engine/Asset/AssetManager.h"
#include "Editor/Core/EditorContext.h"
#include <imgui.h>

namespace Editor
{
	void InspectorWindow::Draw()
	{
		ImGui::Begin("Inspector");

		auto& entities = EditorContext::Entities();
		auto& assets = EditorContext::Assets();

		if (!entities.Empty())
		{
			m_EntityInspector.Draw(entities.GetPrimary());
		}
		else if (!assets.Empty())
		{
			DrawAssetPanel(assets.GetPrimary());

		}
		else
		{
			ImGui::Text("Nothing selected");
		}

		ImGui::End();
	}

	void InspectorWindow::DrawAssetPanel(Engine::Uuid assetID)
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

		case Engine::AssetType::Audio:
		{
			m_AudioInspector.Draw(Engine::AssetManager::GetAsset<Engine::AudioClip>(assetID));
			break;
		}


		default:
		{
			ImGui::Text("Unsupported asset type selected");
		}
		}
	}
}