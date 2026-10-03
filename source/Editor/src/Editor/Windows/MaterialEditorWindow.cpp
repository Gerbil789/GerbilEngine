#include "MaterialEditorWindow.h"
#include "Editor/Windows/Utility/Property.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Event/EventBus.h"
#include "Editor/Core/EditorEvent.h"
#include <imgui.h>

namespace editor
{
	namespace
	{
		engine::MaterialInstance m_MaterialInstance{};
	}

	void MaterialEditorWindow::Draw()
	{
		ImGui::Begin("Material Instance");

		if (!m_MaterialInstance)
		{
			ImGui::Text("No material selected");
			ImGui::End();
			return;
		}

		const std::string& materialName = engine::AssetRegistry::GetPath(m_MaterialInstance).stem().string();
		ImGui::Text("Material: %s", materialName.c_str());

		if (ImGui::BeginTable("MaterialProperties", 2, ImGuiTableFlags_SizingStretchProp))
		{
			ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 100.0f);
			ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);


			engine::MaterialInstanceData& instanceData = m_MaterialInstance.Get();
			engine::Material material = instanceData.material;
			engine::Shader shader = material.GetShader();

			auto bindings = shader.GetMaterialBindings();

			for (auto& binding : bindings)
			{
				if (std::holds_alternative<engine::BufferBinding>(binding.data))
				{
					const auto& bufferBinding = std::get<engine::BufferBinding>(binding.data);

					for (auto& param : bufferBinding.parameters)
					{
						if (param.name[0] == '_') continue;

						std::visit([&](auto& defaultValueVariant)
              {
                using T = std::decay_t<decltype(defaultValueVariant)>;

                DisplayMode mode = DisplayMode::Default;
                if (param.isColor)
                {
                  mode = DisplayMode::Color;
                }

                T value{};
                std::memcpy(&value, &instanceData.uniformBuffer + param.offset, sizeof(T));

                if (PropertyField<T>(param.name.c_str(), value, { .mode = mode }).changed)
                {
                  std::memcpy(&instanceData.uniformBuffer + param.offset, &value, sizeof(T));
                }
              }, param.defaultValue);
					}
				}
				else if (std::holds_alternative<engine::TextureBinding>(binding.data))
				{
					engine::Texture2D texture = instanceData.textures.at(binding.binding);
					if (AssetField(binding.name, texture).changed)
					{
						instanceData.textures[binding.binding] = texture;
					}
				}
			}

			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				ImGui::TextUnformatted("Texture Filter");
				ImGui::TableSetColumnIndex(1);
				const char* filterOptions[] = { "Nearest", "Bilinear", "Trilinear" };
				int currentFilter = static_cast<int>(instanceData.textureFilter);
				if (ImGui::Combo("##TextureFilter", &currentFilter, filterOptions, IM_ARRAYSIZE(filterOptions)))
				{
					instanceData.textureFilter = static_cast<engine::TextureFilter>(currentFilter);
				}

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::TextUnformatted("Texture Wrap");
				ImGui::TableSetColumnIndex(1);
				const char* wrapOptions[] = { "Repeat", "Clamp", "Mirror" };
				int currentWrap = static_cast<int>(instanceData.textureWrap);
				if (ImGui::Combo("##TextureWrap", &currentWrap, wrapOptions, IM_ARRAYSIZE(wrapOptions)))
				{
					instanceData.textureWrap = static_cast<engine::TextureWrap>(currentWrap);
				}
			}

			ImGui::EndTable();
		}

		ImGui::End();
	}

	void MaterialEditorWindow::Initialize()
	{
		engine::EventBus::Subscribe<SelectAssetEvent>([](const SelectAssetEvent& e)
			{
				engine::AssetType type = engine::AssetRegistry::GetAssetType(e.id);
				if(type != engine::AssetType::MaterialInstance) return false;
				m_MaterialInstance = engine::MaterialInstance{e.id};
				return false;
			});
	}
}