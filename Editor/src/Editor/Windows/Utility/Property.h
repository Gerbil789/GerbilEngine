#pragma once

#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetRegistry.h"
#include <imgui.h>
#include <glm/glm.hpp>
#include <limits>

namespace Engine { enum class AssetType;}

namespace Editor
{
	namespace
	{
		constexpr float fltMin = std::numeric_limits<float>::lowest();
		constexpr float fltMax = std::numeric_limits<float>::max();
		constexpr int intMin = std::numeric_limits<int>::lowest();
		constexpr int intMax = std::numeric_limits<int>::max();
	}

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
		PropertyRow(std::string_view label)
		{
			ImGui::PushID(label.data(), label.data() + label.size());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::TextUnformatted(label.data(), label.data() + label.size());
			ImGui::TableSetColumnIndex(1);
		}

		~PropertyRow()
		{
			ImGui::PopID();
		}
	};

	struct DragDropSource
	{
		DragDropSource(std::string_view label, Engine::Uuid id)
		{
			if (!id) return;

			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("UUID", &id, sizeof(Engine::Uuid));
				ImGui::Text("%s", label.data());
				ImGui::EndDragDropSource();
			}
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
		bool AcceptAsset(Fn&& fn, Engine::AssetType expectedType)
		{
			static_assert(std::is_invocable_v<Fn, Engine::Uuid>, "Callback must take an Engine::Uuid");

			if (!active) return false;
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("UUID"))
			{
				assert(payload->DataSize == sizeof(Engine::Uuid) && "Drag drop payload size mismatch!");
				Engine::Uuid id = *static_cast<const Engine::Uuid*>(payload->Data); //TODO: better way to handle this?

				if (Engine::AssetManager::GetAssetRegistry().GetType(id) == expectedType)
				{
					std::forward<Fn>(fn)(id);
					return true;
				}
			}
			return false;
		}

	private:
		bool active = false;
	};

	struct PopupContextItem
	{
		PopupContextItem(const char* name = nullptr, ImGuiPopupFlags popupFlags = 1)
		{
			isOpen = ImGui::BeginPopupContextItem(name, popupFlags);
		}

		~PopupContextItem()
		{
			if (isOpen)
			{
				ImGui::EndPopup();
			}
		}

		explicit operator bool() const { return isOpen; }

	private:
		bool isOpen = false;
	};


	EditResult AssetField(std::string_view label, Engine::Uuid& id, Engine::AssetType type);

	EditResult IntField(std::string_view label, int& value, int min = intMin, int max = intMax);
	EditResult FloatField(std::string_view label, float& value, float min = fltMin, float max = fltMax, float speed = 0.05f);
	EditResult FloatSliderField(std::string_view label, float& value, float min = 0, float max = 1);
	EditResult Vec2Field(std::string_view label, glm::vec2& value);
	EditResult Vec3Field(std::string_view label, glm::vec3& value, float min = fltMin, float max = fltMax, float speed = 0.01f);
	EditResult BoolField(std::string_view label, bool& value);
	EditResult ColorField(std::string_view label, glm::vec4& color);
	EditResult ColorField(std::string_view label, glm::vec3& color);
	EditResult EnumField(std::string_view label, int& value, const std::vector<std::string>& options);
	EditResult TextField(std::string_view label, std::string& text);
}