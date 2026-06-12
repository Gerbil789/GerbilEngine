#include "InspectorWindow.h"
#include "Engine/Asset/AssetManager.h"
#include "Editor/Core/SelectionManager.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Audio/AudioClip.h"
#include "Engine/Event/EventBus.h"
#include "Editor/Core/EditorEvent.h"
#include <imgui.h>

namespace Editor
{
	namespace
	{
		Engine::Uuid m_Id{};
		SelectionContext m_Context{};

		EntityInspectorPanel m_EntityInspector;
		ShaderInspectorPanel m_ShaderInspector;
		AudioInspectorPanel m_AudioInspector;
	}

	void InspectorWindow::Initialize()
	{
		Engine::EventBus::Get().Subscribe<SelectionChangedEvent>([](const SelectionChangedEvent& e)
			{
				if(e.context == SelectionContext::Asset && Engine::AssetManager::GetAssetRegistry().GetType(e.id) == Engine::AssetType::Material)
				{
					return;
				}

				m_Context = e.context;
				m_Id = e.id;
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
			auto type = Engine::AssetManager::GetAssetRegistry().GetType(m_Id);

			switch (type)
			{
			case Engine::AssetType::Texture2D:
			{
				ImGui::Text("Texture2D selected");
				break;
			}

			case Engine::AssetType::Shader:
			{
				m_ShaderInspector.Draw(&(Engine::AssetManager::GetAsset<Engine::Shader>(m_Id)));
				break;
			}
			case Engine::AssetType::Audio:
			{
				m_AudioInspector.Draw(&(Engine::AssetManager::GetAsset<Engine::AudioClip>(m_Id)));
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