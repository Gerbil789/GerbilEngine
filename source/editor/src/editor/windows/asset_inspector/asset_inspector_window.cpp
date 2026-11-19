#include "editor/windows/asset_inspector/asset_inspector_window.h"
#include "editor/core/editor_context.h"
#include "engine/asset/asset_manager.h"
#include "shader_inspector_panel.h"
#include "audio_inspector_panel.h"
#include "texture_inspector_panel.h"
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

		editor::context::asset asset = editor::context::selectedasset;

		switch (asset.type)
		{
		case engine::AssetType::texture:
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