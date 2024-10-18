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
	MaterialWindow::MaterialWindow()
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
			m_Material = scene->GetSelectedMaterial();
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

		ImGui::NextColumn();
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

		ImGui::NextColumn();

		ImGui::Separator();

		std::array<const char*, 2> colorProperties = { "Color", "Albedo" };

		auto properties = m_Material->GetProperties();

		for (auto& [name, property] : properties)
		{
			ImGui::NextColumn();
			ImGui::Text(name.c_str());
			ImGui::NextColumn();

			if (std::holds_alternative<float>(property))
			{
				float value = std::get<float>(property);
				if (UI::FloatField("##float", value))
				{
					m_Material->SetProperty(name, value);
					m_Material->SetModified(true);
				}
			}
			else if (std::holds_alternative<int>(property))
			{
				int value = std::get<int>(property);
				if (UI::IntField("##int", value))
				{
					m_Material->SetProperty(name, value);
					m_Material->SetModified(true);
				}
			}
			else if (std::holds_alternative<glm::vec2>(property))
			{
				glm::vec2 value = std::get<glm::vec2>(property);
				if (UI::Vec2Field("##vec2", value))
				{
					m_Material->SetProperty(name, value);
					m_Material->SetModified(true);
				}
			}
			else if (std::holds_alternative<glm::vec3>(property))
			{
				glm::vec3 value = std::get<glm::vec3>(property);
				bool isColor = false;
				for (auto& colorProp : colorProperties)
				{
					if (name == colorProp)
					{
						isColor = true;
						break;
					}
				}

				if (isColor) 
				{
					if (UI::ColorField("##color", value))
					{
						m_Material->SetProperty(name, value);
						m_Material->SetModified(true);
					}
				}
				else 
				{
					if (UI::Vec3Field("##vec3", value))
					{
						m_Material->SetProperty(name, value);
						m_Material->SetModified(true);
					}
				}


			
			}
		}
			/*else if (std::holds_alternative<glm::vec4>(property))
			{
				glm::vec4 value = std::get<glm::vec4>(property);
				if (UI::Vec4Field("##vec4", value))
				{
					m_Material->SetProperty(name, value);
					m_Material->SetModified(true);
				}
			}*/
			//else if (std::holds_alternative<std::shared_ptr<Texture2D>>(property))
			//{
			//	auto value = std::get<std::shared_ptr<Texture2D>>(property);
			//	if (UI::TextureField("##texture", value))
			//	{
			//		m_Material->SetProperty(name, value);
			//		m_Material->SetModified(true);
			//	}
			//}

			ImGui::NextColumn();


		/*if(UI::TextureField("albedo", m_Material->GetColorTexture())) { m_Material->SetModified(true); }
		ImGui::NextColumn();
		ImGui::Text("Albedo");
		ImGui::NextColumn();
		if(UI::ColorField("##color", m_Material->GetColor())) { m_Material->SetModified(true); }
		ImGui::NextColumn();

		if(UI::TextureField("metallic", m_Material->GetMetallicTexture())) { m_Material->SetModified(true); }
		ImGui::NextColumn();
		ImGui::Text("Metallic");
		ImGui::NextColumn();
		ImGui::BeginDisabled(m_Material->GetMetallicTexture() != nullptr);
		if(UI::FloatSliderField("##metallic", m_Material->GetMetallic())) { m_Material->SetModified(true); }
		ImGui::EndDisabled();
		ImGui::NextColumn();

		if(UI::TextureField("roughness", m_Material->GetRoughnessTexture())) { m_Material->SetModified(true); }
		ImGui::NextColumn();
		ImGui::Text("Roughness");
		ImGui::NextColumn();
		ImGui::BeginDisabled(m_Material->GetRoughnessTexture() != nullptr);
		if(UI::FloatSliderField("##roughness", m_Material->GetRoughness())) { m_Material->SetModified(true); }
		ImGui::EndDisabled();
		ImGui::NextColumn();

		if(UI::TextureField("normal", m_Material->GetNormalTexture())) { m_Material->SetModified(true); }
		ImGui::NextColumn();
		ImGui::Text("Normal");
		ImGui::NextColumn();
		ImGui::NextColumn();

		if(UI::TextureField("height", m_Material->GetHeightTexture())) { m_Material->SetModified(true); }
		ImGui::NextColumn();
		ImGui::Text("Height");
		ImGui::NextColumn();
		ImGui::NextColumn();

		if(UI::TextureField("occlusion", m_Material->GetAmbientTexture())) { m_Material->SetModified(true); }
		ImGui::NextColumn();
		ImGui::Text("Occlusion");
		ImGui::NextColumn();
		ImGui::NextColumn();

		if (UI::TextureField("emission", m_Material->GetEmissionTexture())) { m_Material->SetModified(true); }
		ImGui::NextColumn();
		ImGui::Text("Emission");
		ImGui::NextColumn();
		if (UI::ColorField("##emission", m_Material->GetEmissionColor())) { m_Material->SetModified(true); }
		ImGui::NextColumn();

		ImGui::NextColumn();
		ImGui::Text("    Strenght");
		ImGui::NextColumn();
		if (UI::FloatField("##emission", m_Material->GetEmmissionStrength())) { m_Material->SetModified(true); }
		ImGui::NextColumn();

		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::NextColumn();

		ImGui::NextColumn();
		ImGui::Text("Tiling");
		ImGui::NextColumn();
		if (UI::Vec2Field("Tiling", m_Material->GetTiling())) { m_Material->SetModified(true); }
		ImGui::NextColumn();

		ImGui::NextColumn();
		ImGui::Text("Offset");
		ImGui::NextColumn();
		if (UI::Vec2Field("Offset", m_Material->GetOffset())) { m_Material->SetModified(true); }
		ImGui::NextColumn();*/


		ImGui::Columns(1);
		ImGui::End();
	}
}


