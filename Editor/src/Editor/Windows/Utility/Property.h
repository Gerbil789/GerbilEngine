#pragma once

#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetRegistry.h"
#include <imgui.h>
#include <glm/glm.hpp>
#include <limits>
#include <type_traits>
#include <glm/gtc/type_ptr.hpp>

namespace Engine { enum class AssetType; }

namespace Editor
{
	enum class DisplayMode
	{
		Default, // Standard drag inputs
		Slider,  // For floats/ints
		Color    // For vec3/vec4
	};

	struct FieldOptions
	{
		DisplayMode mode = DisplayMode::Default;
		float min = std::numeric_limits<float>::lowest();
		float max = std::numeric_limits<float>::max();
		float step = 0.1f;
		bool showLabel = true;
	};

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

	template <typename DrawFunc>
	EditResult DrawFieldWithBoilerplate(std::string_view label, bool showLabel, DrawFunc&& drawFunc)
	{
		EditResult result;
		std::optional<PropertyRow> row;

		if (showLabel)
		{
			// This draws the left column label and safely pushes the ImGui ID
			row.emplace(label);
		}
		else
		{
			// Bypass the row/table entirely, but we still need a unique ID for the widget!
			ImGui::PushID(label.data());
		}

		// Execute the type-specific ImGui drawing logic passed via lambda
		if (drawFunc())
		{
			result.changed = true;
		}

		result.active = ImGui::IsItemActive();
		result.started = ImGui::IsItemActivated();
		result.finished = ImGui::IsItemDeactivatedAfterEdit();

		if (!showLabel)
		{
			ImGui::PopID();
		}

		return result;
	}

	template<typename T>
	EditResult PropertyField(std::string_view label, T& value, const FieldOptions& options = {})
	{
		return DrawFieldWithBoilerplate(label, options.showLabel, [&]() {
			if constexpr (std::is_same_v<T, int>)
			{
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

				int v_min = options.min <= static_cast<float>(std::numeric_limits<int>::lowest()) ? std::numeric_limits<int>::lowest() : static_cast<int>(options.min);
				int v_max = options.max >= static_cast<float>(std::numeric_limits<int>::max()) ? std::numeric_limits<int>::max() : static_cast<int>(options.max);

				if (options.mode == DisplayMode::Slider) 
				{
					return ImGui::SliderInt("##input", &value, v_min, v_max);
				}
				else 
				{
					return ImGui::DragInt("##input", &value, options.step, v_min, v_max);
				}
			}
			else if constexpr (std::is_same_v<T, float>)
			{
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				if (options.mode == DisplayMode::Slider) 
				{
					return ImGui::SliderFloat("##input", &value, options.min, options.max);
				}
				else 
				{
					return ImGui::DragFloat("##input", &value, options.step, options.min, options.max);
				}
			}
			else if constexpr (std::is_same_v<T, bool>)
			{
				return ImGui::Checkbox("##checkbox", &value);
			}
			else if constexpr (std::is_same_v<T, glm::vec2>)
			{
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				return ImGui::DragFloat2("##input", glm::value_ptr(value), options.step);
			}
			else if constexpr (std::is_same_v<T, glm::vec3>)
			{
				if (options.mode == DisplayMode::Color)
				{
					bool changed = false;

					if (ImGui::ColorButton("##ColorPreview", ImVec4(value.r, value.g, value.b, 1.0f), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoAlpha, ImVec2(ImGui::GetContentRegionAvail().x, 24.0f)))
					{
						ImGui::OpenPopup("Popup");
					}

					if (ImGui::BeginPopup("Popup"))
					{
						changed = ImGui::ColorPicker3("##Picker", glm::value_ptr(value), ImGuiColorEditFlags_DisplayRGB);
						ImGui::EndPopup();
					}

					return changed;
				}
				else 
				{
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					return ImGui::DragFloat3("##input", glm::value_ptr(value), options.step);
				}
			}
			else if constexpr (std::is_same_v<T, glm::vec4>)
			{
				if (options.mode == DisplayMode::Color)
				{
					bool changed = false;

					if (ImGui::ColorButton("##ColorPreview", ImVec4(value.r, value.g, value.b, value.a), ImGuiColorEditFlags_NoPicker, ImVec2(ImGui::GetContentRegionAvail().x, 24.0f)))
					{
						ImGui::OpenPopup("Popup");
					}

					if (ImGui::BeginPopup("Popup"))
					{
						changed = ImGui::ColorPicker4("##Picker", glm::value_ptr(value), ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_AlphaBar);
						ImGui::EndPopup();
					}

					return changed;
				}
				else 
				{
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					return ImGui::DragFloat4("##input", glm::value_ptr(value), options.step);
				}
			}
			else if constexpr (std::is_same_v<T, std::string>)
			{
				std::array<char, 256> buffer{};
				std::snprintf(buffer.data(), buffer.size(), "%s", value.c_str());

				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				if (ImGui::InputText("##input", buffer.data(), buffer.size()))
				{
					value = buffer.data();
					return true;
				}
				return false;
			}
			else
			{
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Unsupported type");
				return false;
			}
			});
	}

	EditResult EnumField(std::string_view label, int& value, const std::vector<std::string>& options); //TODO: Update with static reflection in cpp26
}