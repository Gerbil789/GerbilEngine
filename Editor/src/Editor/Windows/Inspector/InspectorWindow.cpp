#include "enginepch.h"
#include "InspectorWindow.h"
#include "Engine/Asset/AssetManager.h"
#include "Editor/Core/EditorContext.h"
#include <imgui.h>

namespace Editor
{
	void InspectorWindow::OnUpdate(Engine::Timestep ts)
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
			auto type = Engine::AssetManager::GetAssetType(selection.AssetID);

			if (type == Engine::AssetType::Material)
			{
				m_MaterialInspector.Draw(Engine::AssetManager::GetAsset<Engine::Material>(selection.AssetID));
			}
			else
			{
				ImGui::Text("Asset selected");
			}
			break;
		}


		default:
			ImGui::Text("Nothing selected");

		}

		ImGui::End();
	}
}