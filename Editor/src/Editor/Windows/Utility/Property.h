#pragma once

#include <string>
#include <imgui.h>
#include <glm/glm.hpp>
#include <limits>

namespace Engine { class Texture2D; class AudioClip; }

namespace Editor
{
	constexpr float fltMin = std::numeric_limits<float>::lowest();
	constexpr float fltMax = std::numeric_limits<float>::max();
	constexpr int intMin = std::numeric_limits<int>::lowest();
	constexpr int intMax = std::numeric_limits<int>::max();

	struct EditResult
	{
		bool changed = false;		// value changed this frame
		bool active = false;		// currently being edited
		bool started = false;		// first frame of interaction
		bool finished = false;	// released after edit

		EditResult& operator |= (const EditResult& other)
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
			static_assert(std::is_invocable_v<Fn, void*>, "Accept() requires callable(void*)");

			if (!active) return;

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(type))
			{
				std::forward<Fn>(fn)(payload->Data);
			}
		}

	private:
		bool active = false;
	};

	EditResult TextureField(const std::string& label, Engine::Texture2D*& texture);
	EditResult AudioClipField(const std::string& label, Engine::AudioClip*& audioClip);
	EditResult IntField(const std::string& label, int& value, int min = intMin, int max = intMax);
	EditResult FloatField(const std::string& label, float& value, float min = fltMin, float max = fltMax, float speed = 0.05f);
	EditResult FloatSliderField(const std::string& label, float& value, float min = 0, float max = 1);
	EditResult Vec2Field(const std::string& label, glm::vec2& value);
	EditResult Vec3Field(const std::string& label, glm::vec3& value, float min = fltMin, float max = fltMax, float speed = 0.01f);
	EditResult BoolField(const std::string& label, bool& value);
	EditResult ColorField(const std::string& label, glm::vec4& color);
	EditResult ColorField(const std::string& label, glm::vec3& color);
	EditResult EnumField(const std::string& label, int& value, const std::vector<std::string>& options);
	EditResult TextField(const std::string& label, std::string& text);
}