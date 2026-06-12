#include "SettingsWindow.h"
#include "Editor/Core/EditorContext.h"
#include "Editor/Windows/Utility/Property.h"
#include "Editor/Windows/Viewport/ViewportWindow.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Graphics/Camera.h"
#include "Engine/Graphics/RenderPass/RenderPassRegistry.h"
#include "Engine/Graphics/Texture/TextureCube.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Graphics/RenderPass/ShadowPass.h"
#include "Engine/Scene/SceneManager.h"
#include <imgui.h>

namespace Editor
{
	//namespace
	//{
	//	ImGuiTextFilter m_Filter;
	//}

	void SettingsWindow::Draw()
	{
		ImGui::Begin("Settings");

		//m_Filter.Draw("##SearchSettings");

		ImGui::Separator();
		ImGui::Spacing();

		if (ImGui::CollapsingHeader("Editor"))
		{

			PropertyTable table;

			//{
			//	PropertyRow row("Show Grid");
			//	BoolField("Show Grid", EditorSettings::showGrid);
			//}

			{
				PropertyRow row("Wireframe color");

				if (ColorField("Wireframe color", EditorSettings::wireframeColor).changed)
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
				//PropertyRow row("Environment");

				Engine::Scene& scene = Engine::SceneManager::GetActiveScene();
				Engine::Uuid id = scene.GetEnvironmentTexture();

				if (AssetField("Environment", id, Engine::AssetType::Texture2D).changed)
				{
					scene.SetEnvironmentTexture(id);
					EditorContext::renderer.SetEnvironmentTexture(id);
				}
			}
		}


		if (ImGui::CollapsingHeader("Shadows"))
		{
			PropertyTable table;

			{
				PropertyRow row("Lambda");
				FloatField("Lambda", Engine::ShadowPass::s_Lambda, 0.0f, 1.0f, 0.01f);
			}

			Engine::Camera& camera = EditorContext::editorCamera;

			{
				PropertyRow row("Near");
				float near = camera.GetPerspectiveNear();
				if (FloatField("Shadow Near", near, 0.01f, camera.GetPerspectiveFar()).changed)
				{
					camera.SetPerspectiveNear(near);
				}
			}

			{
				PropertyRow row("Far");
				float far = camera.GetPerspectiveFar();
				if (FloatField("Shadow Far", far, camera.GetPerspectiveNear(), 1000.0f).changed)
				{
					camera.SetPerspectiveFar(far);
				}

			}
		}
		ImGui::End();
	}
}


