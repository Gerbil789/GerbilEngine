#include "enginepch.h"
#include "MaterialWindow.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Scene/SceneManager.h"
#include "../Elements/Elements.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui_internal.h>
#include <filesystem>

namespace Engine 
{
	MaterialWindow::MaterialWindow(EditorContext* context) : EditorWindow(context)
	{
		std::string path = "resources/shaders";

		for(auto& p: std::filesystem::directory_iterator(path))
		{
			m_ShaderPaths.emplace_back(p.path());
		}
	}

	MaterialWindow::~MaterialWindow()
	{

	}

	void MaterialWindow::OnImGuiRender() 
	{
		auto scene = SceneManager::GetCurrentScene();
		if (scene) 
		{
			//TODO: hold selected material reference
			//m_Material = scene->GetSelectedMaterial();
		}
		

		ImGui::Begin("Material");
		
		if(!m_Material)
		{
			ImGui::Text("No material selected");
			ImGui::End();
			return;
		}

		ImGui::Text(m_Material->GetFilePath().filename().string().c_str());


		float availWidth = glm::max(ImGui::GetContentRegionAvail().x - 32 - 100, 100.0f);
		ImGui::Columns(3, "mat_body", false);
		ImGui::SetColumnWidth(0, 32);
		ImGui::SetColumnWidth(1, 100);
		ImGui::SetColumnWidth(2, availWidth);

		/*ImGui::NextColumn();
		ImGui::Text("Shader");
		ImGui::NextColumn();

		//TODO: this is temporary shader selection...
		std::vector<std::filesystem::path> shaders{ "resources/shaders/phong.shader", "resources/shaders/PBR.shader" };
		auto currentShader = m_Material->GetShader()->GetFilePath();
		int index = 0;
		for (int i = 0; i < shaders.size(); i++)
		{
			if (shaders[i] == currentShader)
			{
				index = i;
				break;
			}
		}

		if (UI::EnumField("Shader", index, shaders)) 
		{
			m_Material->SetShader(AssetManager::GetAsset<Shader>(shaders[index]));
			m_Material->SetModified(true);
		}

		ImGui::NextColumn();*/

		ImGui::Separator();

		Ref<Shader> shader = m_Material->GetShader();
		auto properties = shader->GetMaterialBufferLayout();

		for (auto& prop : properties)
		{
			std::string name = prop.Name;

			ImGui::NextColumn();
			ImGui::Text(name.c_str());
			ImGui::NextColumn();

			if (prop.Type == ShaderDataType::Float)
			{
				float value = m_Material->GetProperty<float>(name);
				if (UI::FloatField("##float", value))
				{
					m_Material->SetProperty(prop.Name, value);
					m_Material->SetModified(true);
				}
			}
			else if (prop.Type == ShaderDataType::Int)
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

				


			}
			ImGui::NextColumn();
		}
		
		ImGui::Columns(1);
		ImGui::End();
	}
	bool MaterialWindow::IsColorProperty(const std::string& name)
	{
		const std::string color = "Color";
		if (name.find(color) != std::string::npos)
		{
			return true;
		}
		return false;
	}
}


