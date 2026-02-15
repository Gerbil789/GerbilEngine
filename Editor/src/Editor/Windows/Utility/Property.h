#pragma once

#include "Engine/Graphics/Texture.h"
#include <string>
#include <imgui.h>
#include <glm/glm.hpp>

namespace Editor
{
	struct PropertyEditResult
	{
		bool changed = false;		// value changed this frame
		bool active = false;		// currently being edited
		bool started = false;		// first frame of interaction
		bool finished = false;	// released after edit

		PropertyEditResult& operator |= (const PropertyEditResult& other)
		{
			changed |= other.changed;
			active |= other.active;
			started |= other.started;
			finished |= other.finished;
			return *this;
		}
	};

	struct PropertyTable
	{
		bool open = false;

		PropertyTable(float labelWidth = 100.0f)
		{
			open = ImGui::BeginTable("table", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoBordersInBody);

			if (open)
			{
				ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, labelWidth);
				ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
			}
		}

		~PropertyTable()
		{
			if (open) { ImGui::EndTable(); }
		}

		explicit operator bool() const { return open; }
	};

	struct PropertyRow
	{
		PropertyRow(const char* label)
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::TextUnformatted(label);
			ImGui::TableSetColumnIndex(1);
		}
	};

	struct DragDropTarget
	{
		DragDropTarget()
		{
			active = ImGui::BeginDragDropTarget();
		}

		~DragDropTarget()
		{
			if (active) ImGui::EndDragDropTarget();
		}

		template<typename Fn>
		void Accept(const char* type, Fn&& fn)
		{
			if (!active) return;

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(type))
			{
				fn(payload->Data);
			}
		}

	private:
		bool active = false;
	};

	PropertyEditResult TextureField(const std::string& label, Engine::Texture2D*& texture);
	PropertyEditResult IntField(const std::string& label, int& value, int min = INT_MIN, int max = INT_MAX);
	PropertyEditResult FloatField(const std::string& label, float& value, float min = FLT_MIN, float max = FLT_MAX);
	PropertyEditResult FloatSliderField(const std::string& label, float& value, float min = FLT_MIN, float max = FLT_MAX);
	PropertyEditResult Vec2Field(const std::string& label, glm::vec2& value);
	PropertyEditResult Vec3Field(const std::string& label, glm::vec3& value);
	PropertyEditResult BoolField(const std::string& label, bool& value);
	PropertyEditResult ColorField(const std::string& label, glm::vec4& color);
	PropertyEditResult ColorField(const std::string& label, glm::vec3& color);
	PropertyEditResult EnumField(const std::string& label, int& value, const std::vector<std::string>& options);
	PropertyEditResult TextField(const std::string& label, std::string& text);
}