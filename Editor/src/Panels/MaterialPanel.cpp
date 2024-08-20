#include "enginepch.h"
#include "MaterialPanel.h"
#include "Engine/Core/AssetManager.h"
#include "../Elements/Elements.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui_internal.h>
#include <filesystem>

namespace Engine 
{
	MaterialPanel::MaterialPanel()
	{

	}

	MaterialPanel::~MaterialPanel()
	{

	}

	void MaterialPanel::OnImGuiRender() 
	{
		ImGui::Begin("Material");
		
		if(!m_Material)
		{
			ImGui::Text("No material selected");
			ImGui::End();
			return;
		}

		ImGui::Text("Shader: %s", m_Material->shaderName.c_str());
		ImGui::Separator();

		ImGui::Text("Surface Type");
		ImGui::RadioButton("Opaque", (int*)&m_Material->surfaceType, (int)SurfaceType::Opaque);
		ImGui::SameLine();
		ImGui::RadioButton("Transparent", (int*)&m_Material->surfaceType, (int)SurfaceType::Transparent);
		ImGui::Separator();


		//ImGui::Text("Color");
		UI::TextureControl("Color", m_Material->colorTexture);
		ImGui::SameLine();
		UI::ColorControl( m_Material->color);

		//ImGui::Text("Metallic");
		UI::TextureControl("Metallic", m_Material->metallicTexture);
		ImGui::SameLine();
		UI::FloatSliderControl("##matealic", m_Material->metallic);

		//ImGui::Text("Roughness");
		UI::TextureControl("Roughness", m_Material->roughnessTexture);
		ImGui::SameLine();
		UI::FloatSliderControl("##roughness", m_Material->roughness);

		//ImGui::Text("Normal");
		UI::TextureControl("Normal", m_Material->normalTexture);

		ImGui::Separator();

		UI::Vec2Control("Tiling", m_Material->tiling, 1.0f);
		UI::Vec2Control("Offset", m_Material->offset);
	

		ImGui::End();
	}
}


