#include "InspectorWindow.h"
#include "Engine/Asset/AssetManager.h"
#include "Editor/Core/SelectionManager.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Audio/AudioClip.h"
#include <imgui.h>

namespace Editor
{
	namespace
	{
		Engine::Uuid m_SelectedEntityId = Engine::Uuid{ 0 };
		Engine::Uuid m_SelectedAssetId = Engine::Uuid{ 0 };
	}

	void InspectorWindow::Initialize()
	{
		SelectionManager::Subscribe([this](const std::vector<SelectionEntry>& selections) 
			{ 
				auto id = selections.empty() ? Engine::Uuid{ 0 } : selections.back().id;
				if (!id) 
				{
					m_SelectedEntityId = Engine::Uuid{ 0 };
					m_SelectedAssetId = Engine::Uuid{ 0 };
					return; 
				}

				if (selections.back().Is(SelectionType::Entity))
				{
					m_SelectedEntityId = id;
					m_SelectedAssetId = Engine::Uuid{ 0 };
				}
				else if (selections.back().Is(SelectionType::Asset))
				{
					m_SelectedAssetId = id;
					m_SelectedEntityId = Engine::Uuid{ 0 };
				}
			});
	}

	void InspectorWindow::Draw()
	{
		ImGui::Begin("Inspector");

		if(m_SelectedEntityId)
		{
			m_EntityInspector.Draw(m_SelectedEntityId);
		}
		else if(m_SelectedAssetId)
		{
			DrawAssetPanel(m_SelectedAssetId);
		}
		else
		{
			ImGui::Text("Nothing selected");
		}

		ImGui::End();
	}

	void InspectorWindow::DrawAssetPanel(Engine::Uuid assetID)
	{
		auto type = Engine::AssetManager::GetAssetRegistry().GetType(assetID);

		switch (type)
		{
		case Engine::AssetType::Texture2D:
		{
			ImGui::Text("Texture2D selected");
			break;
		}

		case Engine::AssetType::Shader:
		{
			m_ShaderInspector.Draw(&(Engine::AssetManager::GetAsset<Engine::Shader>(assetID)));
			break;
		}
		case Engine::AssetType::Audio:
		{
			m_AudioInspector.Draw(&(Engine::AssetManager::GetAsset<Engine::AudioClip>(assetID)));
			break;
		}
		default:
		{
			ImGui::Text("View not implemented :)");
		}
		}
	}
}