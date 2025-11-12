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

		float availWidth = glm::max(ImGui::GetContentRegionAvail().x - 32 - 100, 100.0f);
		ImGui::Columns(3, "mat_body", false);
		ImGui::SetColumnWidth(0, 32);
		ImGui::SetColumnWidth(1, 100);
		ImGui::SetColumnWidth(2, availWidth);

		ImGui::NextColumn();
		ImGui::Text("Shader");
		ImGui::NextColumn();

		ImGui::Text(shader->GetName().c_str());
		ImGui::NextColumn();

		ImGui::Separator();

		auto shaderSpec = shader->GetSpecification();
		auto bindings = GetMaterialBindings(shaderSpec);

		for (auto& binding : bindings)
		{
			if (binding.type == Engine::BindingType::UniformBuffer)
			{
				for (auto& param : binding.parameters)
				{
					ImGui::NextColumn();
					ImGui::Text(param.name.c_str());

					glm::vec4 value;
					auto data = material->GetUniformData();
					std::memcpy(&value, data.data() + param.offset, sizeof(glm::vec4));

					if (UI::ColorField(param.name.c_str(), value))
					{
						material->SetVec4(param.name, value);
					}


				}
			}
			if (binding.type == Engine::BindingType::Texture2D)
			{
				ImGui::NextColumn();
				ImGui::Text(binding.name.c_str());

				//float value = m_Material->GetProperty<float>(name);
				//if (UI::FloatField("##float", value))
				//{
				//	m_Material->SetProperty(prop.Name, value);
				//	m_Material->SetModified(true);
				//}
			}
			/*	else if (prop.Type == ShaderDataType::Int)
				{
					int value = m_Material->GetProperty<int>(name);
					if (UI::IntField("##int", value))
					{
						m_Material->SetProperty(prop.Name, value);
						m_Material->SetModified(true);
					}
				}
				else if (prop.Type == ShaderDataType::Float2)
				{
					glm::vec2 value = m_Material->GetProperty<glm::vec2>(name);
					if (UI::Vec2Field("##vec2", value))
					{
						m_Material->SetProperty(prop.Name, value);
						m_Material->SetModified(true);
					}
				}
				else if (prop.Type == ShaderDataType::Float3)
				{
					glm::vec3 value = m_Material->GetProperty<glm::vec3>(name);

					if (IsColorProperty(name))
					{
						if (UI::ColorField(name.c_str(), value))
						{
							m_Material->SetProperty(name, value);
							m_Material->SetModified(true);
						}
					}
					else
					{
						if (UI::Vec3Field(name.c_str(), value))
						{
							m_Material->SetProperty(name, value);
							m_Material->SetModified(true);
						}
					}
				}*/
			ImGui::NextColumn();
		}

		ImGui::Columns(1);
	}

}

