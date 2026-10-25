#include "TextureInspectorPanel.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include <imgui.h>
#include <string>
#include <format>

namespace editor
{
	void TextureInspectorPanel::Draw(engine::Texture2D texture)
	{
		const std::string& name = engine::AssetRegistry::GetAssetPath(texture).stem().string();
		const engine::Texture2DData& textureData = texture.Get();

		ImGui::TextUnformatted(std::format("Texture: {} ({}x{}))", name, textureData.width, textureData.height).c_str());

		ImVec2 available = ImGui::GetContentRegionAvail();

		float textureWidth = static_cast<float>(textureData.width);
		float textureHeight = static_cast<float>(textureData.height);

		float aspect = textureWidth / textureHeight;

		ImVec2 imageSize;

		imageSize.x = available.x;
		imageSize.y = imageSize.x / aspect;

		if (imageSize.y > available.y)
		{
			imageSize.y = available.y;
			imageSize.x = imageSize.y * aspect;
		}

		ImGui::Image(textureData.textureView.Get(), imageSize);
	}
}