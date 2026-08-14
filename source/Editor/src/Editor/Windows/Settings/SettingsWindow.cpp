#include "SettingsWindow.h"
#include "Editor/Core/EditorContext.h"
#include "Editor/Windows/Utility/Property.h"
#include "Editor/Windows/Viewport/ViewportWindow.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Graphics/RenderPass/RenderPassRegistry.h"
#include "Engine/Graphics/Texture/TextureCube.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Graphics/RenderPass/ShadowPass.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Core/Resources.h"
#include "Editor/Core/EditorSettings.h"
#include <imgui.h>

namespace Editor
{
	void SettingsWindow::Draw()
	{
		Engine::Scene& scene = Engine::AssetManager::GetAsset<Engine::Scene>(Engine::SceneManager::GetActiveScene());

		ImGui::Begin("Settings");

		if (ImGui::CollapsingHeader("Editor"))
		{
			PropertyTable table;

			if (PropertyField("Wireframe color", Editor::editorContext.settings.wireframeColor, { .mode = Editor::DisplayMode::Color }).changed)
			{
				auto wireframePass = Engine::RenderPassRegistry::GetPass(Engine::RenderPassType::Wireframe);
				if (wireframePass)
				{
					static_cast<Engine::WireframePass*>(wireframePass)->SetColor(Editor::editorContext.settings.wireframeColor);
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

			Engine::Uuid id = scene.GetEnvironmentTexture();

			if (AssetField("Environment", id, Engine::AssetType::Texture).changed)
			{
				if(!id) { id = RESOURCES::TEXTURE::HDR; }

				scene.SetEnvironmentTexture(id);
				Editor::editorContext.renderer.SetEnvironmentTexture(id);
			}
		}

		if (ImGui::CollapsingHeader("Shadows"))
		{
			PropertyTable table;

			PropertyField("Lambda", Engine::ShadowPass::s_Lambda, { .min = 0.0f, .max = 1.0f, .step = 0.01f });


			entt::registry& registry = scene.GetRegistry();

			auto view = registry.view<Engine::CameraComponent, Engine::EditorTag>();


			entt::entity editorCameraEntity = view.front();
			auto& cc = registry.get<Engine::CameraComponent>(editorCameraEntity);

			PropertyField("Near", cc.perspective.nearClip, { .min = 0.01f, .max = cc.perspective.farClip, .step = 0.01f });
			PropertyField("Far", cc.perspective.farClip, { .min = cc.perspective.nearClip, .max = 1000.0f, .step = 0.01f });
		}
		ImGui::End();
	}
}


