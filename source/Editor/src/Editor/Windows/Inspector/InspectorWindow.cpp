#include "InspectorWindow.h"
#include "Engine/Asset/AssetManager.h"
#include "Editor/Core/SelectionManager.h"
#include "Engine/Audio/AudioClip.h"
#include "Engine/Event/EventBus.h"
#include "Editor/Core/EditorEvent.h"

#include "EntityInspectorPanel.h"
#include "ShaderInspectorPanel.h"
#include "AudioInspectorPanel.h"
#include "TextureInspectorPanel.h"

#include <imgui.h>

namespace editor
{
	namespace
	{
		engine::Uuid m_Id;
		SelectionContext m_Context;

		EntityInspectorPanel m_EntityInspector;
		ShaderInspectorPanel m_ShaderInspector;
		AudioInspectorPanel m_AudioInspector;
		TextureInspectorPanel m_TextureInspector;
	}

	void InspectorWindow::Initialize()
	{
		engine::EventBus::Subscribe<SelectionChangedEvent>([](const SelectionChangedEvent& e)
			{
				if(e.context == SelectionContext::Asset && engine::AssetManager::GetAssetType(e.id) == engine::AssetType::Material)
				{
					return false;
				}

				m_Context = e.context;
				m_Id = e.id;
				return false;
			});
	}

	void InspectorWindow::Draw()
	{
		ImGui::Begin("Inspector");

		if(!m_Id)
		{
			ImGui::Text("Nothing selected");
			ImGui::End();
			return;
		}

		if (m_Context == SelectionContext::Entity)
		{
			m_EntityInspector.Draw(m_Id);
		}
		else if (m_Context == SelectionContext::Asset)
		{
			auto type = engine::AssetManager::GetAssetType(m_Id);

			switch (type)
			{
			case engine::AssetType::Texture:
			{
				m_TextureInspector.Draw(engine::Texture2D{ m_Id });
				break;
			}

			case engine::AssetType::Shader:
			{
				m_ShaderInspector.Draw(engine::Shader{ m_Id });
				break;
			}
			case engine::AssetType::Audio:
			{
				m_AudioInspector.Draw(engine::AudioClip{ m_Id });
				break;
			}
			default:
			{
				ImGui::Text("View not implemented :)");
			}
			}
		}


		ImGui::End();
	}
}