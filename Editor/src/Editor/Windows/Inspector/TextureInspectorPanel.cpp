#include "TextureInspectorPanel.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include <imgui.h>
#include <string>
#include <format>

namespace Editor
{
	void TextureInspectorPanel::Draw(Engine::Uuid id)
	{
		const Engine::AssetRecord& record = Engine::AssetManager::GetAssetRegistry().GetRecord(id);
		const Engine::Texture2D& texture = Engine::AssetManager::GetAsset<Engine::Texture2D>(id);

		ImGui::TextUnformatted(std::format("Texture: {} ({}x{}))", record.GetName(), texture.GetWidth(), texture.GetHeight()).c_str());


		ImVec2 available = ImGui::GetContentRegionAvail();

		float textureWidth = static_cast<float>(texture.GetWidth());
		float textureHeight = static_cast<float>(texture.GetHeight());

		float aspect = textureWidth / textureHeight;

		ImVec2 imageSize;

		imageSize.x = available.x;
		imageSize.y = imageSize.x / aspect;

		if (imageSize.y > available.y)
		{
			imageSize.y = available.y;
			imageSize.x = imageSize.y * aspect;
		}

		ImGui::Image(static_cast<WGPUTextureView>(texture.GetTextureView()), imageSize);
	}
}