#include "SettingsWindow.h"
#include "Editor/Core/EditorContext.h"
#include "Editor/Windows/Utility/Property.h"
#include "Editor/Windows/Viewport/ViewportWindow.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Graphics/Camera.h"
#include "Engine/Graphics/RenderPass/RenderPassRegistry.h"
#include "Engine/Graphics/Texture/TextureCube.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include <imgui.h>

namespace Editor
{
	namespace
	{
		ImGuiTextFilter m_Filter;
	}

	void SettingsWindow::Draw()
	{
		ImGui::Begin("Settings");

		//m_Filter.Draw("##SearchSettings");

		ImGui::Separator();
		ImGui::Spacing();

		if (ImGui::CollapsingHeader("Editor"))
		{

			PropertyTable table;

			{
				PropertyRow row("Show Grid");
				BoolField("Show Grid", EditorSettings::showGrid);
			}

			{
				PropertyRow row("Wireframe color");

				if(ColorField("Wireframe color", EditorSettings::wireframeColor).changed)
				{
					auto wireframePass = Engine::RenderPassRegistry::GetPass(Engine::RenderPassType::Wireframe);
					if (wireframePass)
					{
						static_cast<Engine::WireframePass*>(wireframePass)->SetColor(EditorSettings::wireframeColor);
					}
				}
			}



			if (ImGui::Button("Reset default editor layout"))
			{
				ImGui::LoadIniSettingsFromDisk("Resources/Editor/layouts/default.ini");
			}

			ImGui::SameLine();

			if (ImGui::Button("Save current layout"))
			{
				ImGui::SaveIniSettingsToDisk("Resources/Editor/layouts/default.ini");
			}
		}

		if (ImGui::CollapsingHeader("Render"))
		{
			PropertyTable table;

			{
				PropertyRow row("Environment");

				Engine::Texture2D* texture = &Engine::g_Renderer.GetRenderContext().environment.TextureHDR;

				if (TextureField("Environment Texture", texture).changed)
				{
					Engine::g_Renderer.SetEnvironmentTexture(*texture);
				}
			}

			//TODO: Shadow settings
		}



		ImGui::End();
	}
}


