#include "TextureInspectorPanel.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include <imgui.h>
#include <string>
#include <format>

namespace editor
{
	void TextureInspectorPanel::Draw(engine::Texture2D texture)
	{
		const std::string& name = engine::AssetManager::GetAssetPath(texture.id).stem().string();
		const engine::Texture2DAsset& textureAsset = engine::AssetManager::GetAsset(texture);

		ImGui::TextUnformatted(std::format("Texture: {} ({}x{}))", name, textureAsset.GetWidth(), textureAsset.GetHeight()).c_str());

		ImVec2 available = ImGui::GetContentRegionAvail();

		float textureWidth = static_cast<float>(textureAsset.GetWidth());
		float textureHeight = static_cast<float>(textureAsset.GetHeight());

		float aspect = textureWidth / textureHeight;

		ImVec2 imageSize;

		imageSize.x = available.x;
		imageSize.y = imageSize.x / aspect;

		if (imageSize.y > available.y)
		{
			imageSize.y = available.y;
			imageSize.x = imageSize.y * aspect;
		}

		ImGui::Image(textureAsset.GetTextureView().Get(), imageSize);
	}
}