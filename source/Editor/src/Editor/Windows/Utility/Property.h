#pragma once

#include "Engine/Asset/AssetManager.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include <imgui.h>
#include <glm/glm.hpp>
#include <limits>
#include <type_traits>
#include <glm/gtc/type_ptr.hpp>

namespace engine { enum class AssetType; }

namespace editor
{
	enum class DisplayMode
	{
		Default, // Standard drag inputs
		Slider,  // For floats/ints
		Color,    // For vec3/vec4
		Multiline
	};

	struct FieldOptions
	{
		DisplayMode mode = DisplayMode::Default;
		float min = std::numeric_limits<float>::lowest();
		float max = std::numeric_limits<float>::max();
		float step = 0.1f;
		bool showLabel = true;
		bool useDegrees = false;
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

	private:
		bool open = false;
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

	template<typename T>
	struct DragDropSource
	{
		DragDropSource(const char* payloadId, const T& payloadData, std::string_view label = "")
		{
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload(payloadId, &payloadData, sizeof(engine::Uuid));
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
		bool AcceptAsset(engine::AssetType expectedType, Fn&& fn)
		{
			static_assert(std::is_invocable_v<Fn, engine::Uuid>, "Asset callback must take an Engine::Uuid");

			if (!active) return false;

			const ImGuiPayload* payload = ImGui::GetDragDropPayload();
			bool isValidAsset = false;

			if (payload && payload->IsDataType("UUID"))
			{
				engine::Uuid id = *static_cast<const engine::Uuid*>(payload->Data);
				isValidAsset = (engine::AssetManager::GetAssetType(id) == expectedType);
			}

			ImVec4 targetColor = isValidAsset ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f) : ImVec4(0.8f, 0.2f, 0.2f, 1.0f);

			ImGui::PushStyleColor(ImGuiCol_DragDropTarget, targetColor);
			const ImGuiPayload* droppedPayload = ImGui::AcceptDragDropPayload("UUID");
			ImGui::PopStyleColor();

			if (droppedPayload && isValidAsset)
			{
				engine::Uuid id = *static_cast<const engine::Uuid*>(droppedPayload->Data);
				std::forward<Fn>(fn)(id);
				return true;
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

	template <typename Asset>
	EditResult AssetField(std::string_view label, engine::AssetHandle<Asset>& handle)
	{
		PropertyRow row(label);
		EditResult result;

		constexpr engine::AssetType type = []() {
			if constexpr (std::is_same_v<Asset, engine::Texture2DAsset>) return engine::AssetType::Texture;
			else if constexpr (std::is_same_v<Asset, engine::MeshAsset>) return engine::AssetType::Mesh;
			else if constexpr (std::is_same_v<Asset, engine::ShaderAsset>) return engine::AssetType::Shader;
			else if constexpr (std::is_same_v<Asset, engine::MaterialAsset>) return engine::AssetType::Material;
			else if constexpr (std::is_same_v<Asset, engine::AudioClipAsset>) return engine::AssetType::Audio;
			else if constexpr (std::is_same_v<Asset, engine::SceneAsset>) return engine::AssetType::Scene;
			else return engine::AssetType::Unknown;
			}();

		const std::string& assetName = engine::AssetManager::GetAssetPath(handle.id).stem().string();

		if constexpr (std::is_same_v<Asset, engine::Texture2DAsset>)
		{
			if (handle)
			{
				const engine::Texture2DAsset& texture = engine::AssetManager::GetAsset(handle);
				result.changed = ImGui::ImageButton("##TexturePreview", (ImTextureID)(intptr_t)texture.GetTextureView().Get(), ImVec2(64, 64));
			}
			else
			{
				result.changed = ImGui::ImageButton("##TexturePreviewBlank", nullptr, ImVec2(64, 64));
			}
		}
		else
		{
			result.changed = ImGui::Button(assetName.c_str(), ImVec2(-FLT_MIN, 0));
		}

		result.active = ImGui::IsItemActive();
		result.started = ImGui::IsItemActivated();
		result.finished = ImGui::IsItemDeactivatedAfterEdit();

		DragDropSource<engine::Uuid>("UUID", handle.id, assetName);
		result.changed |= DragDropTarget{}.AcceptAsset(type, [&handle](engine::Uuid newId) { handle.id = newId; });

		if (PopupContextItem contextMenu{ "AssetOptionsPopup" })
		{
			if (ImGui::MenuItem("Clear", nullptr, false, static_cast<bool>(handle)))
			{
				handle = {};
				result.changed = true;
			}
		}

		if (result.changed)
		{
			engine::AssetManager::MarkAssetDirty(handle.id);
		}

		return result;
	}

	template <typename DrawFunc>
	EditResult DrawFieldWithBoilerplate(std::string_view label, bool showLabel, DrawFunc&& drawFunc)
	{
		EditResult result;
		std::optional<PropertyRow> row;

		if (showLabel)
		{
			row.emplace(label);
		}
		else
		{
			ImGui::PushID(label.data());
		}

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

					glm::vec3 displayValue = options.useDegrees ? glm::degrees(value) : value;
					bool changed = ImGui::DragFloat3("##input", glm::value_ptr(displayValue), options.step);

					if (changed)
					{
						value = options.useDegrees ? glm::radians(displayValue) : displayValue;
					}
					return changed;
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
			else if constexpr (std::is_same_v<T, glm::ivec2>)
			{
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				return ImGui::DragInt2("##input", glm::value_ptr(value), options.step);
			}
			else if constexpr (std::is_same_v<T, std::string>)
			{
				std::array<char, 2048> buffer{};
				std::snprintf(buffer.data(), buffer.size(), "%s", value.c_str());

				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

				if (options.mode == DisplayMode::Multiline)
				{
					// ImVec2(0, ...) uses the full available width. 
					// We set the height to exactly 4 lines of text.
					ImVec2 size(0.0f, ImGui::GetTextLineHeightWithSpacing() * 4.0f);

					if (ImGui::InputTextMultiline("##input_multiline", buffer.data(), buffer.size(), size))
					{
						value = buffer.data();
						return true;
					}
				}
				else
				{
					if (ImGui::InputText("##input", buffer.data(), buffer.size()))
					{
						value = buffer.data();
						return true;
					}
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