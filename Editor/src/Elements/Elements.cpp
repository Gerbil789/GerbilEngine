#include "Elements.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Engine
{
	namespace UI
	{
		void IntControl(const char* label, int& value, int resetValue)
		{

		}

		void Vec2IntControl(const char* label, glm::ivec2& values, int resetValue)
		{

		}

		void Vec3IntControl(const char* label, glm::ivec3& values, int resetValue)
		{

		}

		void Vec4IntControl(const char* label, glm::ivec4& values, int resetValue)
		{

		}


		void FloatControl(const char* label, float& value, float resetValue)
		{

		}

		void Vec2Control(const char* label, glm::vec2& values, float resetValue)
		{

		}

		void Vec3Control(const char* label, glm::vec3& values, float resetValue)
		{
			const ImGuiTableFlags tableFlags = {};

			ImGui::PushID(label);
			ImGui::Columns(2, (const char*)0, false);

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 5 });

			// Label
			ImGui::Text(label);
			ImGui::NextColumn();

			ImGui::SetColumnOffset(1, 100);
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
			
		}

		void BoolControl(const char* label, bool& value)
		{
			ImGui::PushID(label);
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, 200);
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

		void StringControl(const char* label, std::string& value, const char* resetValue)
		{
		
		}

	}
}
