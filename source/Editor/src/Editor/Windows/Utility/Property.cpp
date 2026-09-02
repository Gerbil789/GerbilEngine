#include "Property.h"
#include "Engine/Asset/AssetHandle.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Graphics/Texture/TextureCube.h"	
#include "Engine/Audio/AudioClip.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include <glm/gtc/type_ptr.hpp>
#include <array>

namespace editor
{
	EditResult EnumField(std::string_view label, int& value, const std::vector<std::string>& options)
	{
		EditResult result;
		PropertyRow row(label);

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

		return result;
	}
}
