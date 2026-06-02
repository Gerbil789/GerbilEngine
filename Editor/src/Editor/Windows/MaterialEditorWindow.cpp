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

		const Engine::Shader& currentShader = m_Material->GetShader();
		auto bindings = currentShader.GetBindings();

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

					bool isSelected = (&shader == &currentShader);
					if (ImGui::Selectable(shader.EditorOnly.name.c_str(), isSelected))
					{
						m_Material->SetShader(shader);
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

			for (auto& binding : bindings)
			{
				if (binding.type == Engine::BindingType::UniformBuffer)
				{
					for (auto& param : binding.parameters)
					{
						if (param.name[0] == '_')
						{
							continue;
						}

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::TextUnformatted(param.name.c_str());
						ImGui::TableSetColumnIndex(1);

						switch(param.type)
						{
						case Engine::ShaderValueType::Float:
						{
							float value;
							auto data = m_Material->GetUniformData();
							memcpy(&value, data.data() + param.offset, sizeof(float));
							if (FloatField(param.name.c_str(), value).changed)
								m_Material->SetFloat(param.name, value);
							break;
						}

						case Engine::ShaderValueType::Vec2:
						{
							glm::vec2 value;
							auto data = m_Material->GetUniformData();
							memcpy(&value, data.data() + param.offset, sizeof(glm::vec2));
							if (Vec2Field(param.name.c_str(), value).changed)
								m_Material->SetVec2(param.name, value);
							break;
						}

						case Engine::ShaderValueType::Vec4:
						{
							glm::vec4 value;
							auto data = m_Material->GetUniformData();
							memcpy(&value, data.data() + param.offset, sizeof(glm::vec4));
							if (ColorField(param.name.c_str(), value).changed)
								m_Material->SetVec4(param.name, value);
							break;
						}

						default:
							ImGui::TextUnformatted("<Unsupported Type>");
							break;
						}
					}
				}

				if (binding.type == Engine::BindingType::Texture2D)
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::TextUnformatted(binding.name.c_str());

					ImGui::TableSetColumnIndex(1);

					Engine::Texture2D* texture = m_Material->GetTexture(binding.name);
					if (TextureField(binding.name, texture).changed)
					{
						m_Material->SetTexture(binding.name, texture);
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

