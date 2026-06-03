#include "MaterialEditorWindow.h"
#include "Editor/Windows/Utility/Property.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Graphics/Material.h"
#include "Editor/Core/SelectionManager.h"
#include <imgui.h>

namespace Editor
{
	namespace
	{
		Engine::Material* m_Material = nullptr;
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


		Engine::Uuid currentShaderId = m_Material->GetShader();
		const Engine::Shader& currentShader = Engine::AssetManager::GetAsset<Engine::Shader>(currentShaderId);


		if (ImGui::BeginTable("MaterialHeader", 2, ImGuiTableFlags_SizingStretchProp))
		{
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Shader", ImGuiTableColumnFlags_WidthFixed, 120.0f);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); // Name column

			if (TextField("MaterialName", m_Material->EditorOnly.name).finished)
			{
				//record.path = record.path.parent_path() / (name + record.path.extension().string());
				//TODO: SAVE CHANGE
			}

			ImGui::TableSetColumnIndex(1); // Shader column
			ImGui::SetNextItemWidth(-FLT_MIN);



			if (ImGui::BeginCombo("##Shader", currentShader.EditorOnly.name.c_str()))
			{
				auto shaderRecords = Engine::AssetManager::GetAssetRegistry().GetRecords(Engine::AssetType::Shader);

				for (auto* record : shaderRecords)
				{
					Engine::Shader& shader = Engine::AssetManager::GetAsset<Engine::Shader>(record->id);

					bool isSelected = (record->id == currentShaderId);
					if (ImGui::Selectable(shader.EditorOnly.name.c_str(), isSelected))
					{
						m_Material->SetShader(record->id);
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}

			ImGui::EndTable();
		}

		ImGui::Separator();

		if (ImGui::BeginTable("MaterialProperties", 2, ImGuiTableFlags_SizingStretchProp))
		{
			ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 100.0f);
			ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

			auto bindings = currentShader.GetMaterialBindings();

			for (auto& binding : bindings)
			{
				if (binding.type == Engine::BindingType::UniformBuffer)
				{
					for (auto& param : binding.parameters)
					{
						if (param.name[0] == '_') continue;

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::TextUnformatted(param.name.c_str());
						ImGui::TableSetColumnIndex(1);

						Engine::MaterialValue variantValue = m_Material->GetParameterVariant(param.name);

						std::visit([&](auto& arg)
							{
								using T = std::decay_t<decltype(arg)>;

								// 3. Compile-time branching. The compiler strips out the blocks that don't match!
								if constexpr (std::is_same_v<T, float>)
								{
									if (FloatField(param.name.c_str(), arg).changed)
										m_Material->SetParameter(param.name, arg);
								}
								else if constexpr (std::is_same_v<T, glm::vec2>)
								{
									if (Vec2Field(param.name.c_str(), arg).changed)
										m_Material->SetParameter(param.name, arg);
								}
								else if constexpr (std::is_same_v<T, glm::vec4>)
								{
									if (ColorField(param.name.c_str(), arg).changed)
										m_Material->SetParameter(param.name, arg);
								}
								else
								{
									ImGui::TextUnformatted("<Unsupported Type>");
								}

							}, variantValue); // Pass the variant into the visitor
					}
				}

				if (binding.type == Engine::BindingType::Texture2D)
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::TextUnformatted(binding.name.c_str());

					ImGui::TableSetColumnIndex(1);

					Engine::Uuid id = m_Material->GetTexture(binding.name);
					//Engine::Texture2D* texture = Engine::AssetManager::GetAsset<Engine::Texture2D>(id);

					if (TextureField(binding.name, id).changed)
					{
						m_Material->SetTexture(binding.name, id);
					}

				}
			}

			{
				ImGui::Separator();

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
		SelectionManager::Subscribe([](const std::vector<SelectionEntry>& selections)
		{
			for (const auto& entry : selections)
			{
				if (entry.type == SelectionType::Asset)
				{
					if (Engine::AssetManager::GetAssetRegistry().GetType(entry.id) == Engine::AssetType::Material)
					{
						m_Material = &(Engine::AssetManager::GetAsset<Engine::Material>(entry.id));
						return;
					}
				}
			}
			});
	}

	void MaterialEditorWindow::SetMaterial(Engine::Material& material)
	{
		m_Material = &material;
	}


}

