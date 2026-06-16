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

			if (PropertyField("Wireframe color", EditorSettings::wireframeColor, { .mode = Editor::DisplayMode::Color }).changed)
			{
				auto wireframePass = Engine::RenderPassRegistry::GetPass(Engine::RenderPassType::Wireframe);
				if (wireframePass)
				{
					static_cast<Engine::WireframePass*>(wireframePass)->SetColor(EditorSettings::wireframeColor);
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

			Engine::Scene& scene = Engine::SceneManager::GetActiveScene();
			Engine::Uuid id = scene.GetEnvironmentTexture();

			if (AssetField("Environment", id, Engine::AssetType::Texture2D).changed)
			{
				scene.SetEnvironmentTexture(id);
				EditorContext::renderer.SetEnvironmentTexture(id);
			}
		}


		if (ImGui::CollapsingHeader("Shadows"))
		{
			PropertyTable table;

			PropertyField("Lambda", Engine::ShadowPass::s_Lambda, { .min = 0.0f, .max = 1.0f, .step = 0.01f });

			Engine::Camera& camera = EditorContext::editorCamera;

			float near = camera.GetPerspectiveNear();
			if (PropertyField("Near", near, { .min = 0.01f, .max = camera.GetPerspectiveFar(), .step = 0.01f }).changed)
			{
				camera.SetPerspectiveNear(near);
			}

			float far = camera.GetPerspectiveFar();
			if (PropertyField("Far", far, { .min = camera.GetPerspectiveNear(), .max = 1000.0f, .step = 0.01f }).changed)
			{
				camera.SetPerspectiveFar(far);
			}
		}
		ImGui::End();
	}
}


