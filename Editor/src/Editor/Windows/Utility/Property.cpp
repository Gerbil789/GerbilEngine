#include "Property.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Graphics/Texture/TextureCube.h"	
#include "Engine/Audio/AudioClip.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include <glm/gtc/type_ptr.hpp>
#include <array>

namespace Editor
{
	EditResult AssetField(std::string_view label, Engine::Uuid& id, Engine::AssetType type)
	{
		PropertyRow row(label);
		EditResult result;

		const std::string& assetName = Engine::AssetManager::GetAssetRegistry().GetRecord(id).GetName();

		bool isTexture = (type == Engine::AssetType::Texture2D);
		ImVec2 size = isTexture ? ImVec2(64, 64) : ImVec2(-FLT_MIN, 0);

		if (isTexture)
		{
			if (id)
			{
				const auto& texture = Engine::AssetManager::GetAsset<Engine::Texture2D>(id);
				auto textureView = (WGPUTextureView)texture.GetTextureView();
				result.changed = ImGui::ImageButton("##TexturePreview", (ImTextureID)(intptr_t)textureView, size);
			}
			else
			{
				result.changed = ImGui::ImageButton("##TexturePreviewBlank", nullptr, size);
			}
		}
		else
		{
			result.changed = ImGui::Button(assetName.c_str(), size);
		}


		result.active = ImGui::IsItemActive();
		result.started = ImGui::IsItemActivated();
		result.finished = ImGui::IsItemDeactivatedAfterEdit();

		DragDropSource dragSource(assetName, id);
		result.changed |= DragDropTarget{}.AcceptAsset([&id](Engine::Uuid newId) {id = newId; }, type);

		if (PopupContextItem contextMenu{ "AssetOptionsPopup" })
		{
			if (ImGui::MenuItem("Clear", nullptr, false, static_cast<bool>(id)))
			{
				id = Engine::Uuid{};
				result.changed = true;
			}
		}

		return result;
	}

	EditResult EnumField(std::string_view label, int& value, const std::vector<std::string>& options)
	{
		EditResult result;
		PropertyRow row(label);

		if (ImGui::BeginCombo("##value", value >= 0 ? options[value].c_str() : nullptr))
		{
			for (int i = 0; i < static_cast<int>(options.size()); i++)
			{
				bool isSelected = value == i;
				if (ImGui::Selectable(options[i].c_str(), isSelected))
				{
					value = i;
					result.changed = true;
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		result.active = ImGui::IsItemActive();
		result.started = ImGui::IsItemActivated();
		result.finished = ImGui::IsItemDeactivatedAfterEdit();

		return result;
	}
}
