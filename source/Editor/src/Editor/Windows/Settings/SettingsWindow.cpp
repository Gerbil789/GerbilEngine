#include "SettingsWindow.h"
#include "Editor/Core/EditorContext.h"
#include "Editor/Windows/Utility/Property.h"
#include "Engine/Core/Context.h"
#include "Engine/Core/Application.h"
#include <imgui.h>

namespace editor
{
	void SettingsWindow::Draw()
	{
		engine::Scene& scene = engine::Context::activeSceneHandle.Get();

		ImGui::Begin("Settings");

		if (ImGui::CollapsingHeader("Editor"))
		{
			PropertyTable table;

			/*if (PropertyField("Wireframe color", editor::editorContext.settings.wireframeColor, { .mode = editor::DisplayMode::Color }).changed)
			{
				auto wireframePass = engine::RenderPassRegistry::GetPass(engine::RenderPassType::Wireframe);
				if (wireframePass)
				{
					static_cast<engine::WireframePass*>(wireframePass)->SetColor(editor::editorContext.settings.wireframeColor);
				}
			}*/

			if (ImGui::Button("Reset default editor layout"))
			{
				ImGui::LoadIniSettingsFromDisk("resources/layouts/default.ini");
			}

			if (ImGui::Button("Save current layout"))
			{
				ImGui::SaveIniSettingsToDisk("resources/layouts/default.ini");
			}
		}

		if (ImGui::CollapsingHeader("Render"))
		{
			PropertyTable table;

			engine::TextureHandle texture = scene.environmentTexture; //TODO: unnecessary variable?

			if (AssetField("Environment", texture, engine::texture::HDR).changed)
			{
				scene.environmentTexture = texture;
				engine::Application::s_renderer.SetEnvironment(texture); // TODO: i dont like it
			}
		}

		if (ImGui::CollapsingHeader("Shadows"))
		{
			PropertyTable table;

			//PropertyField("Lambda", engine::ShadowPass::s_lambda, { .min = 0.0f, .max = 1.0f, .step = 0.01f });
			auto& camera = editor::context::camera;

			PropertyField("Near", camera.perspective.nearClip, { .min = 0.01f, .max = camera.perspective.farClip, .step = 0.01f });
			PropertyField("Far", camera.perspective.farClip, { .min = camera.perspective.nearClip, .max = 1000.0f, .step = 0.01f });
		}
		ImGui::End();
	}
}


