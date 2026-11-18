#include "settings_window.h"
#include "editor/core/editor_context.h"
#include "editor/windows/utility/property.h"
#include "engine/core/context.h"
#include "engine/core/application.h"
#include <imgui.h>

namespace editor
{
	void settingsWindow::Draw()
	{
		engine::Scene& scene = engine::Context::activeSceneHandle.Get();

		ImGui::Begin("settings");

		if (ImGui::CollapsingHeader("editor"))
		{
			PropertyTable table;

			/*if (PropertyField("Wireframe color", editor::editorContext.settings.wireframeColor, { .mode = editor::DisplayMode::Color }).changed)
			{
				auto wireframepass = engine::RenderpassRegistry::Getpass(engine::RenderpassType::Wireframe);
				if (wireframepass)
				{
					static_cast<engine::Wireframepass*>(wireframepass)->SetColor(editor::editorContext.settings.wireframeColor);
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

			engine::TextureHandle texture = scene.environmenttexture; //TODO: unnecessary variable?

			if (assetField("Environment", texture, engine::texture::hdr).changed)
			{
				scene.environmenttexture = texture;
				engine::Application::s_renderer.SetEnvironment(texture); // TODO: i dont like it
			}
		}

		if (ImGui::CollapsingHeader("Shadows"))
		{
			PropertyTable table;

			//PropertyField("Lambda", engine::Shadowpass::s_lambda, { .min = 0.0f, .max = 1.0f, .step = 0.01f });
			auto& camera = editor::context::camera;

			PropertyField("Near", camera.perspective.nearClip, { .min = 0.01f, .max = camera.perspective.farClip, .step = 0.01f });
			PropertyField("Far", camera.perspective.farClip, { .min = camera.perspective.nearClip, .max = 1000.0f, .step = 0.01f });
		}
		ImGui::End();
	}
}


