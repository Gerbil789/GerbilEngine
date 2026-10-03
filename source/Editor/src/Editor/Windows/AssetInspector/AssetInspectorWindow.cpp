#include "Editor/Windows/AssetInspector/AssetInspectorWindow.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Audio/AudioClip.h"
#include "Engine/Event/EventBus.h"
#include "Editor/Core/EditorEvent.h"
#include "ShaderInspectorPanel.h"
#include "AudioInspectorPanel.h"
#include "TextureInspectorPanel.h"
#include <imgui.h>

namespace editor
{
	namespace
	{
		engine::Uuid m_Id;
		engine::AssetType m_Type = engine::AssetType::Unknown;

		TextureInspectorPanel m_TexturePanel;
		ShaderInspectorPanel m_ShaderPanel;
		AudioInspectorPanel m_AudioPanel;
	}

	void AssetInspectorWindow::Initialize()
	{
		engine::EventBus::Subscribe<SelectAssetEvent>([](const SelectAssetEvent& e)
			{
				m_Id = e.id;
				m_Type = engine::AssetRegistry::GetAssetType(m_Id);
				return false;
			});
	}

	void AssetInspectorWindow::Draw()
	{
		ImGui::Begin("Inspector");

		switch (m_Type)
		{
		case engine::AssetType::Texture:
			m_TexturePanel.Draw(engine::Texture2D{ m_Id });
			break;

		case engine::AssetType::Shader:
			m_ShaderPanel.Draw(engine::Shader{ m_Id });
			break;

		case engine::AssetType::AudioClip:
			m_AudioPanel.Draw(engine::AudioClip{ m_Id });
			break;

		default:
			ImGui::Text("Nothing selected");

		}

		ImGui::End();
	}
}