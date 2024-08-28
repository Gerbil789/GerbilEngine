#include "Elements.h"
#include "Engine/Core/Core.h"

#include "Engine/Core/AssetManager.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>

namespace Engine
{
	namespace UI
	{
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


		bool FloatControl(const char* label, float& value, float resetValue)
		{
			bool valueChanged = false;
			ImGui::PushID(label);
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, labelWidth);
			ImGui::Text(label);
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);
			if (ImGui::DragFloat("##value", &value, 0.1f)) { valueChanged = true; }
			ImGui::PopItemWidth();
			ImGui::Columns(1);
			ImGui::PopID();

			return valueChanged;
		}

		bool Vec2Control(const char* label, glm::vec2& values, float resetValue)
		{ 
			bool valueChanged = false;
			ImGui::PushID(label);
			ImGui::Columns(2, (const char*)0, false);

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 5 });

			// Label
			ImGui::Text(label);
			ImGui::NextColumn();

			ImGui::SetColumnOffset(1, labelWidth);
			ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());

			// X component
			ImGui::Text("X");
			ImGui::SameLine();
			valueChanged |= ImGui::DragFloat("##X", &values.x, 0.1f);
			ImGui::PopItemWidth();
			ImGui::SameLine();

			// Y component
			ImGui::Text("Y");
			ImGui::SameLine();
			valueChanged |= ImGui::DragFloat("##Y", &values.y, 0.1f);
			ImGui::PopItemWidth();
			ImGui::SameLine();

			// Reset button
			if (ImGui::Button("R", ImVec2(22, 22))) 
			{ 
				values = glm::vec2(resetValue); 
				valueChanged = true;
			}

			ImGui::PopStyleVar(2);
			ImGui::Columns(1);
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

		bool EnumControl(const char* label, int& value, const char* options[], int optionCount)
		{
			bool valueChanged = false;
			ImGui::PushID(label);
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, labelWidth);
			ImGui::Text(label);
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);
			if (ImGui::BeginCombo("##value", options[value]))
			{
				for (int i = 0; i < optionCount; i++)
				{
					bool isSelected = value == i;
					if (ImGui::Selectable(options[i], isSelected))
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

		bool ColorControl(glm::vec4& color)
		{
			bool valueChanged = false;

			ImGui::BeginGroup();  // Group the controls to keep them together


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

			return valueChanged;
		}

		bool ColorControl(glm::vec3& color)
		{
			bool valueChanged = false;

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

			return valueChanged;
		}

		bool TextureControl(const char* label, Ref<Texture2D>& texture)
		{
			bool valueChanged = false;
			const ImVec2 buttonSize = ImVec2(24.0f, 24.0f);
			ImGui::PushID(label);

			ImGui::Text(label);
			ImGui::SameLine();
			ImGui::SetCursorPosX(labelWidth);

			if (texture)
			{
				ImGui::ImageButton((void*)(intptr_t)texture->GetRendererID(), buttonSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 }, 0);
			}
			else
			{
				ImGui::Button("##", buttonSize);
			}
			
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path texturePath = path;
					if (texturePath.extension() == ".png" || texturePath.extension() == ".jpg")
					{
						texture = AssetManager::GetAsset<Texture2D>(texturePath.string());
						valueChanged = true;
					}
				}
				ImGui::EndDragDropTarget();
			}

			if (texture)
			{
				if (ImGui::BeginPopupContextItem("TextureOptions"))
				{

					if (ImGui::MenuItem("Remove Texture"))
					{
						texture = nullptr;
						valueChanged = true;
					}
					ImGui::EndPopup();
				}

				if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
				{
					ImGui::OpenPopup("TextureOptions");
				}
			}

			ImGui::PopID();

			return valueChanged;
		}
	}
}
