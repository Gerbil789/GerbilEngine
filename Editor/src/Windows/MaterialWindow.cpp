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

		ImGui::Text(m_Material->GetFilePath().filename().string().c_str());
		ImGui::Text(m_Material->GetShader()->GetFilePath().filename().string().c_str());
		ImGui::Separator();

		ImGui::Columns(3, nullptr, false);
		ImGui::SetColumnWidth(0, 32);
		ImGui::SetColumnWidth(1, 100);
		ImGui::SetColumnWidth(2, 300);

		if(UI::TextureField("albedo", m_Material->GetColorTexture())) { m_Material->SetModified(true); }
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
		ImGui::NextColumn();


		ImGui::Columns(1);
		ImGui::End();
	}
}


