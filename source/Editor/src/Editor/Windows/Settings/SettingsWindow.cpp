#include "SettingsWindow.h"
#include "Editor/Core/EditorContext.h"
#include "Editor/Windows/Utility/Property.h"
#include "Editor/Windows/Viewport/ViewportWindow.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Graphics/RenderPass/RenderPassRegistry.h"
#include "Engine/Graphics/Texture/TextureCube.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Graphics/RenderPass/ShadowPass.h"
#include "Engine/Core/SceneManager.h"
#include "Engine/Core/Scene.h"
#include "Engine/Asset/Resources.h"
#include "Editor/Core/EditorSettings.h"
#include <imgui.h>

namespace editor
{
	void SettingsWindow::Draw()
	{
		engine::SceneAsset& scene = engine::AssetManager::GetAsset<engine::SceneAsset>(engine::SceneManager::GetActiveScene());

		ImGui::Begin("Settings");

		if (ImGui::CollapsingHeader("Editor"))
		{
			PropertyTable table;

			if (PropertyField("Wireframe color", editor::editorContext.settings.wireframeColor, { .mode = editor::DisplayMode::Color }).changed)
			{
				auto wireframePass = engine::RenderPassRegistry::GetPass(engine::RenderPassType::Wireframe);
				if (wireframePass)
				{
					static_cast<engine::WireframePass*>(wireframePass)->SetColor(editor::editorContext.settings.wireframeColor);
				}
			}

			if (ImGui::Button("Reset default editor layout"))
			{
				ImGui::LoadIniSettingsFromDisk("resources/Editor/layouts/default.ini");
			}

			if (ImGui::Button("Save current layout"))
			{
				ImGui::SaveIniSettingsToDisk("resources/Editor/layouts/default.ini");
			}
		}

		if (ImGui::CollapsingHeader("Render"))
		{
			PropertyTable table;

			engine::Texture2D texture = scene.GetEnvironmentTexture();

			if (AssetField("Environment", texture).changed)
			{
				if(!texture) { texture = RESOURCES::TEXTURE::HDR; }
				scene.SetEnvironmentTexture(texture);
				editor::editorContext.renderer.SetEnvironmentTexture(texture);
			}
		}

		if (ImGui::CollapsingHeader("Shadows"))
		{
			PropertyTable table;

			PropertyField("Lambda", engine::ShadowPass::s_Lambda, { .min = 0.0f, .max = 1.0f, .step = 0.01f });


			entt::registry& registry = scene.GetRegistry();

			auto view = registry.view<engine::CameraComponent, engine::EditorTag>();


			entt::entity editorCameraEntity = view.front();
			auto& cc = registry.get<engine::CameraComponent>(editorCameraEntity);

			PropertyField("Near", cc.perspective.nearClip, { .min = 0.01f, .max = cc.perspective.farClip, .step = 0.01f });
			PropertyField("Far", cc.perspective.farClip, { .min = cc.perspective.nearClip, .max = 1000.0f, .step = 0.01f });
		}
		ImGui::End();
	}
}


