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
			m_Material = scene->GetSelectedMaterial();
		}
		

		ImGui::Begin("Material");
		
		if(!m_Material)
		{
			ImGui::Text("No material selected");
			ImGui::End();
			return;
		}


		//UI::EnumControl("Shader", *0, stringPaths);
		std::string materialName = m_Material->GetFilePath().filename().string();
		ImGui::Text(materialName.c_str());

		std::string shaderName = m_Material->shader->GetFilePath().filename().string();
		ImGui::Text(shaderName.c_str());
		ImGui::Separator();


		UI::TextureControl("Albedo", m_Material->colorTexture);
		ImGui::SameLine();
		UI::ColorControl( m_Material->color);

		UI::TextureControl("Metallic", m_Material->metallicTexture);
		ImGui::SameLine();
		UI::FloatSliderControl("##matealic", m_Material->metallic);

		UI::TextureControl("Roughness", m_Material->roughnessTexture);
		ImGui::SameLine();
		UI::FloatSliderControl("##roughness", m_Material->roughness);

		UI::TextureControl("Normal", m_Material->normalTexture);
		UI::TextureControl("Height", m_Material->heightTexture);
		UI::TextureControl("Occlusion", m_Material->occlusionTexture);

		ImGui::Separator();
		UI::Vec2Control("Tiling", m_Material->tiling, 1.0f);
		UI::Vec2Control("Offset", m_Material->offset);
	

		ImGui::End();
	}
}


