#include "Widgets.h"
#include "Engine/Asset/AssetManager.h"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Editor::Widget
{
	WidgetResult TextureField(const char* label, Engine::Texture2D*& texture)
	{
		WidgetResult result;
		const ImVec2 buttonSize = ImVec2(64, 64);

		ImGui::PushID(label);

		if (texture == nullptr) 
		{
			ImGui::Button("empty", buttonSize);
		}
		else 
		{
			ImGui::ImageButton(label, (ImTextureID)(intptr_t)(WGPUTextureView)texture->GetTextureView(), buttonSize);
		}

		if(texture)
		{
			if (ImGui::BeginDragDropSource())
			{
				Engine::UUID uuid = texture->id;
				ImGui::SetDragDropPayload("UUID", &uuid, sizeof(uuid));
				ImGui::Text("%llu", static_cast<unsigned long long>((uint64_t)texture->id));
				ImGui::EndDragDropSource();
			}
		}


		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("UUID"))
			{
				Engine::UUID droppedUUID = *static_cast<const Engine::UUID*>(payload->Data);
				if (Engine::AssetManager::GetAssetType(droppedUUID) == Engine::AssetType::Texture2D)
				{
					texture = Engine::AssetManager::GetAsset<Engine::Texture2D>(droppedUUID);
					result.changed = true;
				}

			}
			ImGui::EndDragDropTarget();
		}

		if (texture && ImGui::BeginPopupContextItem("TextureOptions"))
		{
			if (ImGui::MenuItem("Remove Texture"))
			{
				texture = nullptr;
				result.changed = true;
			}
			ImGui::EndPopup();
		}

		result.active = ImGui::IsItemActive();
		result.started = ImGui::IsItemActivated();
		result.finished = ImGui::IsItemDeactivatedAfterEdit();

		ImGui::PopID();
		return result;
	}

	WidgetResult IntField(const char* label, int& value)
	{
		WidgetResult result;

		ImGui::PushID(label);

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragInt("##input", &value, 1.0f, 0, 0, "%d", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat))
		{
			result.changed = true;
		}

		result.active = ImGui::IsItemActive();
		result.started = ImGui::IsItemActivated();
		result.finished = ImGui::IsItemDeactivatedAfterEdit();

		ImGui::PopID();
		return result;

	}

	WidgetResult FloatField(const char* label, float& value)
	{
		WidgetResult result;

		ImGui::PushID(label);

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragFloat(("##" + std::string(label)).c_str(), &value, 0.05f, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat))
		{
			result.changed = true;
		}

		result.active = ImGui::IsItemActive();
		result.started = ImGui::IsItemActivated();
		result.finished = ImGui::IsItemDeactivatedAfterEdit();
		
		ImGui::PopID();
		return result;
	}

	WidgetResult FloatSliderField(const char* label, float& value, float min, float max)
	{
		WidgetResult result;
		ImGui::PushID(label);

		float fullWidth = ImGui::GetContentRegionAvail().x;
		float inputWidth = 70.0f;
		float sliderWidth = fullWidth - inputWidth - ImGui::GetStyle().ItemSpacing.x;

		ImGui::SetNextItemWidth(sliderWidth);
		if (ImGui::SliderFloat("##slider", &value, min, max, "", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_NoInput))
		{
			result.changed = true;
		}

		ImGui::SameLine();

		ImGui::SetNextItemWidth(inputWidth);
		if (ImGui::InputFloat("##input", &value, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AlwaysOverwrite | ImGuiInputTextFlags_ParseEmptyRefVal))
		{
			value = glm::clamp(value, min, max);
			result.changed = true;
		}

		result.active = ImGui::IsItemActive();
		result.started = ImGui::IsItemActivated();
		result.finished = ImGui::IsItemDeactivatedAfterEdit();

		ImGui::PopID();
		return result;
	}

	WidgetResult Vec2Field(const char* label, glm::vec2& value)
	{
		WidgetResult result;
		ImGui::PushID(label);

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

	WidgetResult Vec3Field(const char* label, glm::vec3& value)
	{
		WidgetResult result;
		ImGui::PushID(label);

		float itemSpacing = ImGui::GetStyle().ItemSpacing.x;
		float fullWidth = ImGui::GetContentRegionAvail().x - itemSpacing;

		float labelWidth = ImGui::CalcTextSize("X").x;       
		float inputWidth = (fullWidth - (labelWidth * 3) - (itemSpacing * 4)) / 3.0f;

		auto handleFloat = [&](const char* id, float& v)
			{
				ImGui::Text("%s", id);
				ImGui::SameLine();
				ImGui::PushItemWidth(inputWidth);
				result.changed = ImGui::DragFloat(("##" + std::string(id)).c_str(), &v, 0.1f);
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

	bool FloatSliderControl(const char* label, float& value, float min, float max)
	{
		bool valueChanged = false;
		ImGui::PushID(label);
		ImGui::PushItemWidth(-1);
		if (ImGui::SliderFloat("##value", &value, min, max))
		{
			valueChanged = true;
		}
		ImGui::PopItemWidth();

		ImGui::PopID();
		return valueChanged;
	}

	WidgetResult EnumField(const char* label, int& value, const std::vector<std::string>& options)
	{
		WidgetResult result;
		ImGui::PushID(label);

		if (ImGui::BeginCombo("##value", options[value].c_str()))
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
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		result.active = ImGui::IsItemActive();
		result.started = ImGui::IsItemActivated();
		result.finished = ImGui::IsItemDeactivatedAfterEdit();

		ImGui::PopID();
		return result;
	}

	WidgetResult EnumField(const char* label, int& value, const std::vector<std::filesystem::path>& options)
	{
		WidgetResult result;
		ImGui::PushID(label);

		if (ImGui::BeginCombo("##value", options[value].stem().string().c_str()))
		{
			for (int i = 0; i < static_cast<int>(options.size()); i++)
			{
				bool isSelected = value == i;
				if (ImGui::Selectable(options[i].stem().string().c_str(), isSelected))
				{
					value = i;
					result.changed = true;
				}
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		result.active = ImGui::IsItemActive();
		result.started = ImGui::IsItemActivated();
		result.finished = ImGui::IsItemDeactivatedAfterEdit();

		ImGui::PopID();
		return result;
	}

	WidgetResult ColorField(const char* label, glm::vec4& color)
	{
		WidgetResult result;

		ImGui::PushID(label);
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

	WidgetResult ColorField(const char* label, glm::vec3& color)
	{
		WidgetResult result;

		ImGui::PushID(label);
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
}
