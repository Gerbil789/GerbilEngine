#include "Editor/Windows/AssetInspector/AssetInspectorWindow.h"
#include "Editor/Core/EditorContext.h"
#include "Engine/Asset/AssetManager.h"
#include "ShaderInspectorPanel.h"
#include "AudioInspectorPanel.h"
#include "TextureInspectorPanel.h"
#include <imgui.h>

namespace editor
{
	namespace
	{
		TextureInspectorPanel m_texturePanel;
		ShaderInspectorPanel m_shaderPanel;
		AudioInspectorPanel m_audioPanel;
	}
	
	void AssetInspectorWindow::Draw()
	{
		ImGui::Begin("Inspector");

		editor::context::Asset asset = editor::context::selectedAsset;

		switch (asset.type)
		{
		case engine::AssetType::Texture:
			m_texturePanel.Draw(engine::TextureHandle{ asset.id });
			break;

		case engine::AssetType::Shader:
			m_shaderPanel.Draw(engine::ShaderHandle{ asset.id });
			break;

		case engine::AssetType::AudioClip:
			m_audioPanel.Draw(engine::AudioClipHandle{ asset.id });
			break;

		default:
			ImGui::Text("Nothing selected");

		}

		ImGui::End();
	}
}