#include "MaterialEditorWindow.h"
#include "Editor/Windows/Utility/Property.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Graphics/Material.h"
#include "Editor/Core/SelectionManager.h"
#include "Engine/Event/EventBus.h"
#include "Editor/Core/EditorEvent.h"
#include <imgui.h>

namespace editor
{
	namespace
	{
		//TODO: dont use pointer, use id
		engine::MaterialAsset* m_Material = nullptr;
	}

	void MaterialEditorWindow::Draw()
	{
		ImGui::Begin("Material");

		if (!m_Material)
		{
			ImGui::Text("No material selected");
			ImGui::End();
			return;
		}

		const std::string& materialName = engine::AssetManager::GetAssetPath(m_Material->id).stem().string();
		ImGui::Text("Material: %s", materialName.c_str());

		const std::string& shaderName = engine::AssetManager::GetAssetPath(m_Material->GetShader().id).stem().string();


		if (ImGui::BeginCombo("##Shader", shaderName.c_str()))
		{
			for (const auto& [id, record] : engine::AssetManager::GetAssetRegistry().GetAllRecords())
			{
				if (record.type != engine::AssetType::Shader) continue;

				if (static_cast<uint64_t>(id) <= 1000) continue; // skip built-in shaders

				const std::string& name = record.path.stem().string();

				if (ImGui::Selectable(name.c_str()))
				{
					// Create your strongly typed handle directly from the Uuid
					m_Material->SetShader(engine::Shader{ id });
				}
			}

			ImGui::EndCombo();
		}

		ImGui::Separator();

		if (ImGui::BeginTable("MaterialProperties", 2, ImGuiTableFlags_SizingStretchProp))
		{
			ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 100.0f);
			ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

			engine::ShaderAsset& shader = engine::AssetManager::GetAsset<engine::ShaderAsset>(m_Material->GetShader());
			auto bindings = shader.GetMaterialBindings();

			for (auto& binding : bindings)
			{
				if (std::holds_alternative<engine::BufferBinding>(binding.data))
				{
					const auto& bufferBinding = std::get<engine::BufferBinding>(binding.data);

					for (auto& param : bufferBinding.parameters)
					{
						if (param.name[0] == '_') continue;

						engine::MaterialValue variantValue = m_Material->GetParameterVariant(param.name);

						std::visit([&](auto& arg)
							{
								using T = std::decay_t<decltype(arg)>;

								DisplayMode mode = DisplayMode::Default;
								if (param.isColor)
								{
									mode = DisplayMode::Color;
								}

								if (PropertyField<T>(param.name.c_str(), arg, { .mode = mode }).changed)
								{
									m_Material->SetParameter(param.name, arg);
								}
							}, variantValue);
					}
				}

				if (std::holds_alternative<engine::TextureBinding>(binding.data))
				{
					engine::Texture2D texture = m_Material->GetTexture(binding.name);
					if (AssetField(binding.name.c_str(), texture).changed)
					{
						m_Material->SetTexture(binding.name, texture);
					}
				}
			}

			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				ImGui::TextUnformatted("Texture Filter");
				ImGui::TableSetColumnIndex(1);
				const char* filterOptions[] = { "Nearest", "Bilinear", "Trilinear" };
				int currentFilter = static_cast<int>(m_Material->GetTextureFilter());
				if (ImGui::Combo("##TextureFilter", &currentFilter, filterOptions, IM_ARRAYSIZE(filterOptions)))
				{
					m_Material->SetTextureFilter(static_cast<engine::TextureFilter>(currentFilter));
				}

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::TextUnformatted("Texture Wrap");
				ImGui::TableSetColumnIndex(1);
				const char* wrapOptions[] = { "Repeat", "Clamp", "Mirror" };
				int currentWrap = static_cast<int>(m_Material->GetTextureWrap());
				if (ImGui::Combo("##TextureWrap", &currentWrap, wrapOptions, IM_ARRAYSIZE(wrapOptions)))
				{
					m_Material->SetTextureWrap(static_cast<engine::TextureWrap>(currentWrap));
				}
			}

			ImGui::EndTable();
		}

		ImGui::End();
	}

	void MaterialEditorWindow::Initialize()
	{
		engine::EventBus::Subscribe<SelectionChangedEvent>([](const SelectionChangedEvent& e)
			{
				if (e.context != SelectionContext::Asset) return false;;

				auto type = engine::AssetManager::GetAssetType(e.id);
				if (type == engine::AssetType::Material)
				{
					m_Material = &engine::AssetManager::GetAsset<engine::MaterialAsset>(engine::Material{ e.id });
				}
				return false;
			});
	}
}