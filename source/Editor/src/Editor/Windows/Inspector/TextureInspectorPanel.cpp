#include "TextureInspectorPanel.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include <imgui.h>
#include <string>
#include <format>

namespace Editor
{
	void TextureInspectorPanel::Draw(Engine::Texture2D texture)
	{
		const std::string& name = Engine::AssetManager::GetAssetPath(texture.id).stem().string();
		const Engine::Texture2DAsset& textureAsset = Engine::AssetManager::GetAsset(texture);

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