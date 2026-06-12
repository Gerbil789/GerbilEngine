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

	EditResult IntField(std::string_view label, int& value, int min, int max)
	{
		EditResult result;

		ImGui::PushID(label.data());

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragInt("##input", &value, 1.0f, min, max, "%d", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat))
		{
			result.changed = true;
		}

		result.active = ImGui::IsItemActive();
		result.started = ImGui::IsItemActivated();
		result.finished = ImGui::IsItemDeactivatedAfterEdit();

		ImGui::PopID();
		return result;

	}

	EditResult FloatField(std::string_view label, float& value, float min, float max, float speed)
	{
		EditResult result;

		ImGui::PushID(label.data());

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragFloat(("##" + std::string(label)).c_str(), &value, speed, min, max, "%.2f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat))
		{
			result.changed = true;
		}

		result.active = ImGui::IsItemActive();
		result.started = ImGui::IsItemActivated();
		result.finished = ImGui::IsItemDeactivatedAfterEdit();
		
		ImGui::PopID();
		return result;
	}

	EditResult FloatSliderField(std::string_view label, float& value, float min, float max)
	{
		EditResult result;
		ImGui::PushID(label.data());

		float fullWidth = ImGui::GetContentRegionAvail().x;
		float inputWidth = 70.0f;
		float sliderWidth = fullWidth - inputWidth - ImGui::GetStyle().ItemSpacing.x;

		ImGui::SetNextItemWidth(sliderWidth);
		if (ImGui::SliderFloat("##slider", &value, min, max, "%.3f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_NoInput))
		{
			result.changed = true;
		}

		result.active = ImGui::IsItemActive();
		result.started = ImGui::IsItemActivated();
		result.finished = ImGui::IsItemDeactivatedAfterEdit();

		ImGui::PopID();
		return result;
	}

	EditResult Vec2Field(std::string_view label, glm::vec2& value)
	{
		EditResult result;
		ImGui::PushID(label.data());

		//TODO: this is not perfectly aligned with other fields...
		//float fieldWidth = ImGui::GetContentRegionAvail().x / 2.0f - ImGui::GetStyle().ItemSpacing.x - 14.0f;
		float availableWidth = ImGui::GetContentRegionAvail().x;
		float itemSpacing = ImGui::GetStyle().ItemSpacing.x;
		float totalSpacing = itemSpacing * 3;
		float fieldWidth = (availableWidth - totalSpacing) / 2.0f; 

		ImGui::Text("X");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(fieldWidth);
		if (ImGui::DragFloat("##X", &value.x, 0.05f, 0.0f, 0.0f, "%.2f"))
		{
			result.changed = true;
		}

		ImGui::SameLine();
		ImGui::Text("Y");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(fieldWidth);
		if (ImGui::DragFloat("##Y", &value.y, 0.05f, 0.0f, 0.0f, "%.2f"))
		{
			result.changed = true;
		}

		result.active |= ImGui::IsItemActive();
		result.started |= ImGui::IsItemActivated();
		result.finished |= ImGui::IsItemDeactivatedAfterEdit();
		
		ImGui::PopID();
		return result;
	}

	EditResult Vec3Field(std::string_view label, glm::vec3& value, float min, float max, float speed)
	{
		EditResult result;
		ImGui::PushID(label.data());

		float itemSpacing = ImGui::GetStyle().ItemSpacing.x;
		float fullWidth = ImGui::GetContentRegionAvail().x - itemSpacing;

		float labelWidth = ImGui::CalcTextSize("X").x;       
		float inputWidth = (fullWidth - (labelWidth * 3) - (itemSpacing * 4)) / 3.0f;

		auto handleFloat = [&](const std::string& id, float& v)
			{
				ImGui::TextUnformatted(id.c_str());
				ImGui::SameLine();
				ImGui::PushItemWidth(inputWidth);
				result.changed = ImGui::DragFloat(("##" + id).c_str(), &v, speed, min, max, "%.2f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat);
				ImGui::PopItemWidth();

				result.active |= ImGui::IsItemActive();
				result.started |= ImGui::IsItemActivated();
				result.finished |= ImGui::IsItemDeactivatedAfterEdit();
			};

		handleFloat("X", value.x);
		ImGui::SameLine();
		handleFloat("Y", value.y);
		ImGui::SameLine();
		handleFloat("Z", value.z);

		ImGui::PopID();
		return result;
	}

	EditResult BoolField(std::string_view label, bool& value)
	{
		EditResult result;
		ImGui::PushID(label.data());
		if (ImGui::Checkbox("##checkbox", &value))
		{
			result.changed = true;
		}
		result.active = ImGui::IsItemActive();
		result.started = ImGui::IsItemActivated();
		result.finished = ImGui::IsItemDeactivatedAfterEdit();
		ImGui::PopID();
		return result;
	}

	bool FloatSliderControl(std::string_view label, float& value, float min, float max)
	{
		bool valueChanged = false;
		ImGui::PushID(label.data());
		ImGui::PushItemWidth(-1);
		if (ImGui::SliderFloat("##value", &value, min, max))
		{
			valueChanged = true;
		}
		ImGui::PopItemWidth();

		ImGui::PopID();
		return valueChanged;
	}

	EditResult EnumField(std::string_view label, int& value, const std::vector<std::string>& options)
	{
		EditResult result;
		ImGui::PushID(label.data());

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

		ImGui::PopID();
		return result;
	}

	EditResult ColorField(std::string_view label, glm::vec4& color)
	{
		EditResult result;

		ImGui::PushID(label.data());
		ImGui::BeginGroup(); 

		if (ImGui::ColorButton("##ColorPreview", ImVec4(color.r, color.g, color.b, color.a), ImGuiColorEditFlags_NoPicker, ImVec2(ImGui::GetContentRegionAvail().x, 24.0f)))
		{
			ImGui::OpenPopup("##ColorPicker");
		}

		if (ImGui::BeginPopup("##ColorPicker"))
		{
			result.changed |= ImGui::ColorPicker4("##Picker", glm::value_ptr(color), ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_AlphaBar);
			ImGui::EndPopup();
		}

		result.active = ImGui::IsItemActive();
		result.started = ImGui::IsItemActivated();
		result.finished = ImGui::IsItemDeactivatedAfterEdit();

		ImGui::EndGroup();
		ImGui::PopID();

		return result;
	}

	EditResult ColorField(std::string_view label, glm::vec3& color)
	{
		EditResult result;

		ImGui::PushID(label.data());
		ImGui::BeginGroup();  // Group the controls to keep them together


		if (ImGui::ColorButton("##ColorPreview", ImVec4(color.r, color.g, color.b, 1.0f), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoAlpha, ImVec2(ImGui::GetContentRegionAvail().x, 24.0f)))
		{
			ImGui::OpenPopup("##ColorPicker");
		}

		if (ImGui::BeginPopup("##ColorPicker"))
		{
			result.changed |= ImGui::ColorPicker3("##Picker", glm::value_ptr(color), ImGuiColorEditFlags_DisplayRGB);
			ImGui::EndPopup();
		}

		ImGui::EndGroup();

		result.active = ImGui::IsItemActive();
		result.started = ImGui::IsItemActivated();
		result.finished = ImGui::IsItemDeactivatedAfterEdit();

		ImGui::PopID();

		return result;
	}

	EditResult TextField(std::string_view label, std::string& text)
	{
		std::array<char, 256> buffer{};

		std::snprintf(buffer.data(), buffer.size(), "%s", text.c_str());
		//std::strncpy(buffer.data(), text.c_str(), buffer.size());
		//buffer[buffer.size() - 1] = '\0'; // Ensure null-termination

		auto formattedLabel = std::format("##{}", label);

		EditResult result;
		ImGui::PushID(formattedLabel.c_str());
		ImGui::PushItemWidth(-1);

		if (buffer[0] == '\0')
		{
			std::snprintf(buffer.data(), buffer.size(), "%s", text.c_str());
		}

		if (ImGui::InputText(formattedLabel.c_str(), buffer.data(), buffer.size()))
		{ 
			text = buffer.data(); 
			result.changed = true;
		}

		result.active = ImGui::IsItemActive();
		result.started = ImGui::IsItemActivated();
		result.finished = ImGui::IsItemDeactivatedAfterEdit();

		ImGui::PopItemWidth();
		ImGui::PopID();
		return result;
	}
}
