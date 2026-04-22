#include "SettingsWindow.h"
#include "Editor/Core/EditorSettings.h"
#include "Editor/Windows/Utility/Property.h"
#include "Editor/Windows/Viewport/ViewportWindow.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Graphics/Camera.h"
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
				BoolField("Show Grid", g_EditorSettings.showGrid);
			}



			ImGui::SeparatorText("Wireframe");

			{
				PropertyRow row("Color");
				ColorField("Wireframe Color", g_EditorSettings.wireframeColor);
			}

			{
				PropertyRow row("Thickness");
				FloatField("Wireframe Thickness", g_EditorSettings.wireframeThickness, 0.1f, 10.0f);
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

				auto renderer = *Engine::g_Renderer;

				auto envTexture = renderer.GetSkyboxCubemap();

				if (CubeMapField("Environment Texture", envTexture).changed)
				{
					renderer.SetSkyboxCubemap(envTexture);
				}
			}

			//TODO: Shadow settings
		}



		ImGui::End();
	}
}


