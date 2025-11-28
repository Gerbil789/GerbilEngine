#include "MaterialInspectorPanel.h"
#include "Engine/Renderer/Shader.h"
#include "Editor/Components/Components.h"
#include <imgui.h>

#include "Engine/Asset/AssetManager.h"

namespace Editor
{
	void MaterialInspectorPanel::Draw(Ref<Engine::Material> material)
	{
		if (!material)
		{
			ImGui::Text("No material selected");
			return;
		}

		Ref<Engine::Shader> shader = material->GetShader();
		auto shaderSpec = shader->GetSpecification();
		auto bindings = GetMaterialBindings(shaderSpec);



		static std::vector<Ref<Engine::Shader>> s_AllShaders;
		static std::vector<const char*> shaderNames;

		if (s_AllShaders.empty())
		{
			// Load or fetch from AssetManager
			s_AllShaders = Engine::AssetManager::GetAssetsOfType<Engine::Shader>(Engine::AssetType::Shader);

			shaderNames.clear();
			shaderNames.reserve(s_AllShaders.size());

			for (size_t i = 0; i < s_AllShaders.size(); i++)
			{
				shaderNames.push_back(s_AllShaders[i]->GetName().c_str());
			}
		}

		if (ImGui::BeginTable("MaterialHeader", 2, ImGuiTableFlags_SizingStretchProp))
		{
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Shader", ImGuiTableColumnFlags_WidthFixed, 120.0f);



			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);


			ImGui::Text(Engine::AssetManager::GetAssetMetadata(material->id)->path.stem().string().c_str());

			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(-FLT_MIN); // fill full column
			int currentShaderIndex = -1;
			for (size_t i = 0; i < s_AllShaders.size(); i++)
			{
				if (material->GetShader() == s_AllShaders[i])
					currentShaderIndex = (int)i;
			}

			if (ImGui::Combo("##Shader", &currentShaderIndex, shaderNames.data(), (int)shaderNames.size()))
			{
				material->SetShader(s_AllShaders[currentShaderIndex]);
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
							auto data = material->GetUniformData();
							memcpy(&value, data.data() + param.offset, sizeof(float));
							if (UI::FloatField(param.name.c_str(), value))
								material->SetFloat(param.name, value);
							break;
						}

						case Engine::ShaderValueType::Vec4:
						{
							glm::vec4 value;
							auto data = material->GetUniformData();
							memcpy(&value, data.data() + param.offset, sizeof(glm::vec4));
							if (UI::ColorField(param.name.c_str(), value))
								material->SetVec4(param.name, value);
							break;
						}
						}
					}
				}

				if (binding.type == Engine::BindingType::Texture2D)
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::TextUnformatted(binding.name.c_str());

					ImGui::TableSetColumnIndex(1);

					Ref<Engine::Texture2D> texture = material->GetTexture(binding.name);
					if (UI::TextureField(("##" + binding.name).c_str(), texture))
						material->SetTexture(binding.name, texture);
				}
			}

			ImGui::EndTable();
		}
	}
}

