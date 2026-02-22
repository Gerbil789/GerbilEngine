#include "Property.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Graphics/Texture.h"
#include "Engine/Audio/AudioClip.h"
#include <glm/gtc/type_ptr.hpp>
#include <array>

namespace Editor
{
	EditResult TextureField(const std::string& label, Engine::Texture2D*& texture)
	{
		EditResult result;
		const ImVec2 buttonSize = ImVec2(64, 64);

		ImGui::PushID(label.c_str());

		if (texture == nullptr) 
		{
			ImGui::Button("empty", buttonSize);
		}
		else 
		{
			ImGui::ImageButton(label.c_str(), (ImTextureID)(intptr_t)(WGPUTextureView)texture->GetTextureView(), buttonSize);
		}

		if(texture)
		{
			if (ImGui::BeginDragDropSource())
			{
				Engine::Uuid uuid = texture->id;
				ImGui::SetDragDropPayload("UUID", &uuid, sizeof(uuid));
				ImGui::Text("%llu", static_cast<unsigned long long>((uint64_t)texture->id));
				ImGui::EndDragDropSource();
			}
		}


		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("UUID"))
			{
				Engine::Uuid droppedUUID = *static_cast<const Engine::Uuid*>(payload->Data);
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

	EditResult AudioClipField(const std::string& label, Engine::AudioClip*& audioClip)
	{
		EditResult result;

		ImGui::PushID(label.c_str());

		std::string buttonText = audioClip != nullptr ? Engine::AssetManager::GetAssetName(audioClip->id) : "##Clip";

		ImGui::Button(buttonText.c_str(), ImVec2(-FLT_MIN, 0));

		if (audioClip)
		{
			if (ImGui::BeginDragDropSource())
			{
				Engine::Uuid uuid = audioClip->id;
				ImGui::SetDragDropPayload("UUID", &uuid, sizeof(uuid));
				ImGui::Text("%llu", static_cast<unsigned long long>((uint64_t)audioClip->id));
				ImGui::EndDragDropSource();
			}
		}


		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("UUID"))
			{
				Engine::Uuid droppedUUID = *static_cast<const Engine::Uuid*>(payload->Data);
				if (Engine::AssetManager::GetAssetType(droppedUUID) == Engine::AssetType::Audio)
				{
					audioClip = Engine::AssetManager::GetAsset<Engine::AudioClip>(droppedUUID);
					result.changed = true;
				}

			}
			ImGui::EndDragDropTarget();
		}

		if (audioClip && ImGui::BeginPopupContextItem("Options"))
		{
			if (ImGui::MenuItem("Remove"))
			{
				audioClip = nullptr;
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

	EditResult IntField(const std::string& label, int& value, int min, int max)
	{
		EditResult result;

		ImGui::PushID(label.c_str());

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragInt("##input", &value, 1.0f, min, max, "%d", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat))
		{
			result.changed = true;
		}

		result.active = ImGui::IsItemActive();
		result.started = ImGui::IsItemActivated();
		result.finished = ImGui::IsItemDeactivatedAfterEdit();

		ImGui::PopID();
		return result;

	}

	EditResult FloatField(const std::string& label, float& value, float min, float max, float speed)
	{
		EditResult result;

		ImGui::PushID(label.c_str());

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragFloat(("##" + std::string(label)).c_str(), &value, speed, min, max, "%.2f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat))
		{
			result.changed = true;
		}

		result.active = ImGui::IsItemActive();
		result.started = ImGui::IsItemActivated();
		result.finished = ImGui::IsItemDeactivatedAfterEdit();
		
		ImGui::PopID();
		return result;
	}

	EditResult FloatSliderField(const std::string& label, float& value, float min, float max)
	{
		EditResult result;
		ImGui::PushID(label.c_str());

		float fullWidth = ImGui::GetContentRegionAvail().x;
		float inputWidth = 70.0f;
		float sliderWidth = fullWidth - inputWidth - ImGui::GetStyle().ItemSpacing.x;

		ImGui::SetNextItemWidth(sliderWidth);
		if (ImGui::SliderFloat("##slider", &value, min, max, "%.3f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_NoInput))
		{
			result.changed = true;
		}

		result.active = ImGui::IsItemActive();
		result.started = ImGui::IsItemActivated();
		result.finished = ImGui::IsItemDeactivatedAfterEdit();

		ImGui::PopID();
		return result;
	}

	EditResult Vec2Field(const std::string& label, glm::vec2& value)
	{
		EditResult result;
		ImGui::PushID(label.c_str());

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

	EditResult Vec3Field(const std::string& label, glm::vec3& value, float min, float max, float speed)
	{
		EditResult result;
		ImGui::PushID(label.c_str());

		float itemSpacing = ImGui::GetStyle().ItemSpacing.x;
		float fullWidth = ImGui::GetContentRegionAvail().x - itemSpacing;

		float labelWidth = ImGui::CalcTextSize("X").x;       
		float inputWidth = (fullWidth - (labelWidth * 3) - (itemSpacing * 4)) / 3.0f;

		auto handleFloat = [&](const std::string& id, float& v)
			{
				ImGui::TextUnformatted(id.c_str());
				ImGui::SameLine();
				ImGui::PushItemWidth(inputWidth);
				result.changed = ImGui::DragFloat(("##" + id).c_str(), &v, speed, min, max, "%.2f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat);
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

	EditResult BoolField(const std::string& label, bool& value)
	{
		EditResult result;
		ImGui::PushID(label.c_str());
		if (ImGui::Checkbox("##checkbox", &value))
		{
			result.changed = true;
		}
		result.active = ImGui::IsItemActive();
		result.started = ImGui::IsItemActivated();
		result.finished = ImGui::IsItemDeactivatedAfterEdit();
		ImGui::PopID();
		return result;
	}

	bool FloatSliderControl(const std::string& label, float& value, float min, float max)
	{
		bool valueChanged = false;
		ImGui::PushID(label.c_str());
		ImGui::PushItemWidth(-1);
		if (ImGui::SliderFloat("##value", &value, min, max))
		{
			valueChanged = true;
		}
		ImGui::PopItemWidth();

		ImGui::PopID();
		return valueChanged;
	}

	EditResult EnumField(const std::string& label, int& value, const std::vector<std::string>& options)
	{
		EditResult result;
		ImGui::PushID(label.c_str());

		if (ImGui::BeginCombo("##value", value >= 0 ? options[value].c_str() : nullptr))
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
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		result.active = ImGui::IsItemActive();
		result.started = ImGui::IsItemActivated();
		result.finished = ImGui::IsItemDeactivatedAfterEdit();

		ImGui::PopID();
		return result;
	}

	EditResult ColorField(const std::string& label, glm::vec4& color)
	{
		EditResult result;

		ImGui::PushID(label.c_str());
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

	EditResult ColorField(const std::string& label, glm::vec3& color)
	{
		EditResult result;

		ImGui::PushID(label.c_str());
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

	EditResult TextField(const std::string& label, std::string& text)
	{
		static std::array<char, 256> buffer{};

		EditResult result;
		ImGui::PushID(label.c_str());
		ImGui::PushItemWidth(-1);

		if (buffer[0] == '\0')
		{
			std::snprintf(buffer.data(), buffer.size(), "%s", text.c_str());
		}

		if (ImGui::InputText("##Text", buffer.data(), buffer.size())) 
		{ 
			text = buffer.data(); 
			result.changed = true;
		}

		result.active = ImGui::IsItemActive();
		result.started = ImGui::IsItemActivated();
		result.finished = ImGui::IsItemDeactivatedAfterEdit();
		ImGui::PopItemWidth();
		ImGui::PopID();
		return result;
	}
}
