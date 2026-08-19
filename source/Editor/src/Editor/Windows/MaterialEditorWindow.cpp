#include "MaterialEditorWindow.h"
#include "Editor/Windows/Utility/Property.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Graphics/Material.h"
#include "Editor/Core/SelectionManager.h"
#include "Engine/Event/EventBus.h"
#include "Editor/Core/EditorEvent.h"
#include <imgui.h>

namespace Editor
{
	namespace
	{
		//TODO: dont use pointer, use id
		Engine::MaterialAsset* m_Material = nullptr;
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

		const std::string& materialName = Engine::AssetManager::GetAssetPath(m_Material->id).stem().string();
		ImGui::Text("Material: %s", materialName.c_str());

		const std::string& shaderName = Engine::AssetManager::GetAssetPath(m_Material->GetShader().id).stem().string();


		if (ImGui::BeginCombo("##Shader", shaderName.c_str()))
		{
			for (const auto& [id, record] : Engine::AssetManager::GetAssetRegistry().GetAllRecords())
			{
				if (record.type != Engine::AssetType::Shader) continue;

				if (static_cast<uint64_t>(id) <= 1000) continue; // skip built-in shaders

				const std::string& name = record.path.stem().string();

				if (ImGui::Selectable(name.c_str()))
				{
					// Create your strongly typed handle directly from the Uuid
					m_Material->SetShader(Engine::Shader{ id });
				}
			}

			ImGui::EndCombo();
		}

		ImGui::Separator();

		if (ImGui::BeginTable("MaterialProperties", 2, ImGuiTableFlags_SizingStretchProp))
		{
			ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 100.0f);
			ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

			Engine::ShaderAsset& shader = Engine::AssetManager::GetAsset<Engine::ShaderAsset>(m_Material->GetShader());
			auto bindings = shader.GetMaterialBindings();

			for (auto& binding : bindings)
			{
				if (std::holds_alternative<Engine::BufferBinding>(binding.data))
				{
					const auto& bufferBinding = std::get<Engine::BufferBinding>(binding.data);

					for (auto& param : bufferBinding.parameters)
					{
						if (param.name[0] == '_') continue;

						Engine::MaterialValue variantValue = m_Material->GetParameterVariant(param.name);

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

				if (std::holds_alternative<Engine::TextureBinding>(binding.data))
				{
					Engine::Texture2D texture = m_Material->GetTexture(binding.name);
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
					m_Material->SetTextureFilter(static_cast<Engine::TextureFilter>(currentFilter));
				}

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::TextUnformatted("Texture Wrap");
				ImGui::TableSetColumnIndex(1);
				const char* wrapOptions[] = { "Repeat", "Clamp", "Mirror" };
				int currentWrap = static_cast<int>(m_Material->GetTextureWrap());
				if (ImGui::Combo("##TextureWrap", &currentWrap, wrapOptions, IM_ARRAYSIZE(wrapOptions)))
				{
					m_Material->SetTextureWrap(static_cast<Engine::TextureWrap>(currentWrap));
				}
			}

			ImGui::EndTable();
		}

		ImGui::End();
	}

	void MaterialEditorWindow::Initialize()
	{
		Engine::EventBus::Subscribe<SelectionChangedEvent>([](const SelectionChangedEvent& e)
			{
				if (e.context != SelectionContext::Asset) return false;;

				auto type = Engine::AssetManager::GetAssetType(e.id);
				if (type == Engine::AssetType::Material)
				{
					m_Material = &Engine::AssetManager::GetAsset<Engine::MaterialAsset>(Engine::Material{ e.id });
				}
				return false;
			});
	}
}