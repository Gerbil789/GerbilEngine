#include "Elements.h"
#include "Engine/Core/Core.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

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

		void Vec2Control(const char* label, glm::vec2& values, float resetValue)
		{
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
			ImGui::DragFloat("##X", &values.x, 0.1f);
			ImGui::PopItemWidth();
			ImGui::SameLine();

			// Y component
			ImGui::Text("Y");
			ImGui::SameLine();
			ImGui::DragFloat("##Y", &values.y, 0.1f);
			ImGui::PopItemWidth();
			ImGui::SameLine();

			// Reset button
			if (ImGui::Button("R", ImVec2(22, 22))) { values = glm::vec2(resetValue); }

			ImGui::PopStyleVar(2);
			ImGui::Columns(1);
			ImGui::PopID();

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

	}
}
