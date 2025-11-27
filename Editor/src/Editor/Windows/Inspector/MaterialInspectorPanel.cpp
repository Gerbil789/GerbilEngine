#include "MaterialInspectorPanel.h"
#include "Engine/Renderer/Shader.h"
#include "Editor/Components/Components.h"
#include <imgui.h>

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

		if (ImGui::BeginTable("MaterialTable", 2, ImGuiTableFlags_SizingStretchProp))
		{
			ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 100.0f);
			ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

			ImGui::TableHeadersRow();

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

						switch(param.type)
						{
						case Engine::ShaderValueType::Bool:
						{
							bool value;
							auto data = material->GetUniformData();
							memcpy(&value, data.data() + param.offset, sizeof(bool));
							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);
							ImGui::TextUnformatted(param.name.c_str());
							ImGui::TableSetColumnIndex(1);
							if (UI::IntField(param.name.c_str(), (int&)value))
								material->SetFloat(param.name, value ? 1.0f : 0.0f);
							break;
						}


						case Engine::ShaderValueType::Float:
						{
							float value;
							auto data = material->GetUniformData();
							memcpy(&value, data.data() + param.offset, sizeof(float));
							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);
							ImGui::TextUnformatted(param.name.c_str());
							ImGui::TableSetColumnIndex(1);
							if (UI::FloatField(param.name.c_str(), value))
								material->SetFloat(param.name, value);
							break;
						}

						case Engine::ShaderValueType::Vec4:
						{
							glm::vec4 value;
							auto data = material->GetUniformData();
							memcpy(&value, data.data() + param.offset, sizeof(glm::vec4));

							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);
							ImGui::TextUnformatted(param.name.c_str());

							ImGui::TableSetColumnIndex(1);
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

