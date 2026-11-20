#include "settings_window.h"
#include "editor/core/editor_context.h"
#include "editor/windows/utility/property.h"
#include "engine/core/context.h"
#include "engine/core/application.h"
#include <imgui.h>

namespace editor
{
	void SettingsWindow::Draw()
	{
		engine::Scene& scene = engine::context::activeSceneHandle.Get();

		ImGui::Begin("Settings");

		if (ImGui::CollapsingHeader("Editor"))
		{
			PropertyTable table;

			auto& renderer = engine::Application::s_renderer;

			auto& normal = renderer.GetPass<engine::NormalPass>();
			PropertyField("Normal", normal.enabled);

			auto& wireframe = renderer.GetPass<engine::WireframePass>();
			PropertyField("Wireframe", wireframe.enabled);
			PropertyField("Color", wireframe.color, { .mode = editor::DisplayMode::Color });

			ImGui::Separator();

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

			if (assetField("Environment", texture, engine::texture::hdr).changed)
			{
				scene.environmentTexture = texture;
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


