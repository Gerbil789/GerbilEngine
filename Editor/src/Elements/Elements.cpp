#include "Elements.h"
#include "Engine/Core/Core.h"
#include "Engine/Core/AssetManager.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>

namespace Engine::UI
{
	bool TextureField(const char* label, Ref<Texture2D>& texture)
	{
		bool modified = false;
		const ImVec2 buttonSize = ImVec2(24, 24);

		ImGui::PushID(label);

		if (texture == nullptr) 
		{
			ImGui::Button("", buttonSize);
		}
		else 
		{
			ImGui::ImageButton((void*)(intptr_t)texture->GetRendererID(), buttonSize, ImVec2(0, 1), ImVec2(1, 0), 0);
		}


		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* droppedPath = (const wchar_t*)payload->Data;
				std::filesystem::path path(droppedPath);

				if (path.extension() == ".png" || path.extension() == ".jpg" || path.extension() == ".bmp")
				{
					texture = AssetManager::GetAsset<Texture2D>(path);
					modified = true;
				}
			}
			ImGui::EndDragDropTarget();
		}

		if (texture && ImGui::BeginPopupContextItem("TextureOptions"))
		{
			if (ImGui::MenuItem("Remove Texture"))
			{
				texture = nullptr;
				modified = true;
			}
			ImGui::EndPopup();
		}

		ImGui::PopID();
		return modified;
	}
	void IntControl(const char* label, int& value, int resetValue)
	{
		ASSERT(false, "IntControl Not implemented")
	}

	void Vec2IntControl(const char* label, glm::ivec2& values, int resetValue)
	{
		ASSERT(false, "Vec2IntControl Not implemented")
	}

	void Vec3IntControl(const char* label, glm::ivec3& values, int resetValue)
	{
		ASSERT(false, "Vec3IntControl Not implemented")
	}

	void Vec4IntControl(const char* label, glm::ivec4& values, int resetValue)
	{
		ASSERT(false, "Vec4IntControl Not implemented")
	}


	bool FloatField(const char* label, float& value)
	{
		bool valueChanged = false;
		ImGui::PushID(label);

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragFloat("##input", &value, 0.05f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat))
		{
			valueChanged = true;
		}
		
		ImGui::PopID();
		return valueChanged;
	}

	bool FloatSliderField(const char* label, float& value, float min, float max)
	{
		bool valueChanged = false;
		ImGui::PushID(label);

		float fullWidth = ImGui::GetContentRegionAvail().x;
		float inputWidth = 70.0f;
		float sliderWidth = fullWidth - inputWidth - ImGui::GetStyle().ItemSpacing.x;

		ImGui::SetNextItemWidth(sliderWidth);
		if (ImGui::SliderFloat("##slider", &value, min, max, "", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_NoInput))
		{
			valueChanged = true;
		}

		ImGui::SameLine();

		ImGui::SetNextItemWidth(inputWidth);
		if (ImGui::InputFloat("##input", &value, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AlwaysOverwrite | ImGuiInputTextFlags_ParseEmptyRefVal))
		{
			value = glm::clamp(value, min, max);
			valueChanged = true;
		}

		ImGui::PopID();
		return valueChanged;
	}

	bool Vec2Field(const char* label, glm::vec2& values)
	{
		bool valueChanged = false;
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
		if (ImGui::DragFloat("##X", &values.x, 0.05f, 0.0f, 0.0f, "%.2f"))
		{
			valueChanged = true; 
		}

		ImGui::SameLine();
		ImGui::Text("Y");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(fieldWidth);
		if (ImGui::DragFloat("##Y", &values.y, 0.05f, 0.0f, 0.0f, "%.2f"))
		{
			valueChanged = true;
		}
		
		ImGui::PopID();
		return valueChanged;
	}

	void Vec3Control(const char* label, glm::vec3& values, float resetValue)
	{
		ImGui::PushID(label);
		ImGui::Columns(2, (const char*)0, false);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 5 });

		// Label
		ImGui::Text(label);
		ImGui::NextColumn();

		ImGui::SetColumnOffset(1, labelWidth);
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());

		// X component
		ImGui::Text("X");
		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// Y component
		ImGui::Text("Y");
		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// Z component
		ImGui::Text("Z");
		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// Reset button
		if (ImGui::Button("R", ImVec2(22, 22))) { values = glm::vec3(resetValue); }

		ImGui::PopStyleVar(2);
		ImGui::Columns(1);
		ImGui::PopID();
	}

	void Vec4Control(const char* label, glm::vec4& values, float resetValue)
	{
		ImGui::PushID(label);
		ImGui::Columns(2, (const char*)0, false);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 5 });

		// Label
		ImGui::Text(label);
		ImGui::NextColumn();

		ImGui::SetColumnOffset(1, labelWidth);
		ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());

		// X component
		ImGui::Text("X");
		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// Y component
		ImGui::Text("Y");
		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// Z component
		ImGui::Text("Z");
		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// W component
		ImGui::Text("W");
		ImGui::SameLine();
		ImGui::DragFloat("##W", &values.w, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// Reset button
		if (ImGui::Button("R", ImVec2(22, 22))) { values = glm::vec4(resetValue); }

		ImGui::PopStyleVar(2);
		ImGui::Columns(1);
		ImGui::PopID();
	}

	void BoolControl(const char* label, bool& value)
	{
		ImGui::PushID(label);
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, labelWidth);
		ImGui::Text(label);
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		if (ImGui::Checkbox("##value", &value))
		{
		}
		ImGui::PopItemWidth();
		ImGui::Columns(1);
		ImGui::PopID();
	}

	void Vec2BoolControl(const char* label, glm::bvec2& values)
	{
		ASSERT(false, "Vec2BoolControl Not implemented")
	}

	void Vec3BoolControl(const char* label, glm::bvec3& values)
	{
		ASSERT(false, "Vec3BoolControl Not implemented")
	}

	void Vec4BoolControl(const char* label, glm::bvec4& values)
	{
		ASSERT(false, "Vec4BoolControl Not implemented")
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

	void StringControl(const char* label, std::string& value, const char* resetValue)
	{
		ASSERT(false, "StringControl Not implemented")
	}

	bool EnumControl(const char* label, int& value, const std::vector<std::string>& options)
	{
		bool valueChanged = false;
		ImGui::PushID(label);
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, labelWidth);
		ImGui::Text(label);
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		if (ImGui::BeginCombo("##value", options[value].c_str()))
		{
			for (int i = 0; i < options.size(); i++)
			{
				bool isSelected = value == i;
				if (ImGui::Selectable(options[i].c_str(), isSelected))
				{
					value = i;
					valueChanged = true;
				}
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
		ImGui::Columns(1);
		ImGui::PopID();

		return valueChanged;
	}

	bool ColorField(const char* label, glm::vec4& color)
	{
		bool valueChanged = false;

		ImGui::PushID(label);
		ImGui::BeginGroup(); 

		if (ImGui::ColorButton("##ColorPreview", ImVec4(color.r, color.g, color.b, color.a), ImGuiColorEditFlags_NoPicker, ImVec2(ImGui::GetContentRegionAvail().x, 24.0f)))
		{
			ImGui::OpenPopup("##ColorPicker");
		}

		if (ImGui::BeginPopup("##ColorPicker"))
		{
			valueChanged |= ImGui::ColorPicker4("##Picker", glm::value_ptr(color), ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_AlphaBar);
			ImGui::EndPopup();
		}

		ImGui::EndGroup();
		ImGui::PopID();

		return valueChanged;
	}

	bool ColorField(const char* label, glm::vec3& color)
	{
		bool valueChanged = false;

		ImGui::PushID(label);
		ImGui::BeginGroup();  // Group the controls to keep them together


		if (ImGui::ColorButton("##ColorPreview", ImVec4(color.r, color.g, color.b, 1.0f), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoAlpha, ImVec2(ImGui::GetContentRegionAvail().x, 24.0f)))
		{
			ImGui::OpenPopup("##ColorPicker");
		}

		if (ImGui::BeginPopup("##ColorPicker"))
		{
			valueChanged |= ImGui::ColorPicker3("##Picker", glm::value_ptr(color), ImGuiColorEditFlags_DisplayRGB);
			ImGui::EndPopup();
		}

		ImGui::EndGroup();
		ImGui::PopID();

		return valueChanged;
	}

	

}
