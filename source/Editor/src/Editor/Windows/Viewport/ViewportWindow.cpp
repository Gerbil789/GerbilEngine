#include "ViewportWindow.h"
#include "Editor/Windows/Viewport/ViewportCameraController.h"
#include "Editor/Windows/Viewport/TransformController.h"
#include "Editor/Windows/Viewport/EditorPicker.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Editor/Command/ChangeEditorStateCommand.h"
#include "Editor/Windows/Utility/ScopedStyle.h"
#include "Engine/Core/SceneManager.h"
#include "Engine/Core/Scene.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Core/State.h"
#include "Engine/Core/Log.h"
#include "Engine/System/CameraSystem.h"
#include "Engine/System/TransformSystem.h"
#include <glm/glm.hpp>

namespace editor
{
	namespace
	{
		EditorPicker m_EntityPicker;
		ViewportCameraController m_CameraController;
		TransformController m_TransformController; //gizmo

		glm::vec2 m_ViewportBounds[2] = { {0.0f, 0.0f}, {0.0f, 0.0f} };
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
	}

	void ViewportWindow::Initialize()
	{
		m_CameraController.Initialize();
		m_TransformController.Initialize();
		m_EntityPicker.Initialize();


		const auto& tc = editor::editorContext.cameraTransform;
		const glm::mat4 localMatrix = engine::TransformSystem::CalculateLocalPositionMatrix(tc);
		engine::CameraSystem::UpdateCameraViewMatrix(editor::editorContext.camera, localMatrix);

	}

	static void UpdateViewportSize()
	{
		ImVec2 newSize = ImGui::GetContentRegionAvail();
		if (newSize.x == m_ViewportSize.x && newSize.y == m_ViewportSize.y) return;

		m_ViewportSize = { newSize.x, newSize.y };

		editor::editorContext.renderer.SetSize(m_ViewportSize.x, m_ViewportSize.y);

		ImVec2 viewportMinRegion = ImGui::GetWindowContentRegionMin();
		ImVec2 viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		ImVec2 viewportOffset = ImGui::GetWindowPos();

		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		if (m_ViewportSize.x <= 0.0f || m_ViewportSize.y <= 0.0f) return;

		wgpu::Extent3D size = { static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y), 1 };

		m_EntityPicker.Resize(size.width, size.height);

		// Color
		{
			wgpu::TextureDescriptor desc;
			desc.label = "RendererColorTexture";
			desc.dimension = wgpu::TextureDimension::e2D;
			desc.format = engine::GraphicsContext::GetSurfaceFormat();
			desc.size = size;
			desc.mipLevelCount = 1;
			desc.sampleCount = 1;
			desc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding;
			wgpu::Texture colorTexture = engine::GraphicsContext::GetDevice().CreateTexture(&desc);

			wgpu::TextureViewDescriptor view;
			view.label = "RendererColorTextureView";
			view.dimension = wgpu::TextureViewDimension::e2D;
			view.format = desc.format;
			view.baseMipLevel = 0;
			view.mipLevelCount = 1;
			view.baseArrayLayer = 0;
			view.arrayLayerCount = 1;
			editor::editorContext.renderer.SetColorTarget(colorTexture.CreateView(&view));
		}

		// Depth
		{
			// wgpu::TextureFormat format = wgpu::TextureFormat::Depth24Plus;

			// wgpu::TextureDescriptor desc;
			// desc.label = "RendererDepthTextureView";
			// desc.dimension = wgpu::TextureDimension::e2D;
			// desc.format = format;
			// desc.mipLevelCount = 1;
			// desc.sampleCount = 1;
			// desc.size = size;
			// desc.usage = wgpu::TextureUsage::RenderAttachment;
			// desc.viewFormatCount = 1;
			// desc.viewFormats = &format;
			// wgpu::Texture depthTexture = Engine::GraphicsContext::GetDevice().CreateTexture(&desc);

			// wgpu::TextureViewDescriptor view;
			// view.aspect = wgpu::TextureAspect::DepthOnly;
			// view.baseArrayLayer = 0;
			// view.arrayLayerCount = 1;
			// view.baseMipLevel = 0;
			// view.mipLevelCount = 1;
			// view.dimension = wgpu::TextureViewDimension::e2D;
			// view.format = wgpu::TextureFormat::Depth24Plus;

			// Editor::editorContext.renderer.SetDepthTarget(depthTexture.CreateView(&view));
		}

		engine::viewportState.width = m_ViewportSize.x;
		engine::viewportState.height = m_ViewportSize.y;

		engine::viewportState.positionX = m_ViewportBounds[0].x;
		engine::viewportState.positionY = m_ViewportBounds[0].y;


		float aspectRatio = m_ViewportSize.x / m_ViewportSize.y;
		engine::CameraSystem::UpdateCameraProjectionMatrix(editor::editorContext.camera, aspectRatio);
	}

	static void DrawOverlay(const ImVec2& imagePos, const ImVec2& size)
	{
		const float overlayHeight = 32.0f;

		ImGui::SetCursorPos(imagePos);

		ImGui::SetNextWindowBgAlpha(0.1f);

		ImGui::BeginChild("##OverlayUI",
			ImVec2(size.x, overlayHeight),
			false,
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoSavedSettings
		);

		{
			constexpr float buttonWidth = 60.0f;
			ImGui::SetCursorPos(ImVec2(size.x * 0.5f - buttonWidth * 0.5f, 4.0f));

			if (editor::editorContext.editorMode == EditorMode::Edit)
			{
				if (ImGui::Button("Play", ImVec2(buttonWidth, 0)))
				{
					EditorCommandManager::Enqueue(std::make_unique<ChangeEditorStateCommand>(EditorMode::Play));

					auto& scene = engine::AssetManager::GetAsset(engine::SceneManager::GetActiveScene());
					auto& registry = scene.GetRegistry();
					auto view = registry.view<engine::TransformComponent, engine::CameraComponent, engine::PrimaryCameraTag>();
					for(auto[entity, transform, camera] : view.each())
					{
						editor::editorContext.renderer.SetCamera(camera, transform);
						break;
					}
				}
			}
			else
			{
				if (ImGui::Button("Stop", ImVec2(buttonWidth, 0)))
				{
					EditorCommandManager::Enqueue(std::make_unique<ChangeEditorStateCommand>(EditorMode::Edit));
					editor::editorContext.renderer.SetCamera(editor::editorContext.camera, editor::editorContext.cameraTransform);
				}
			}
		}

		{
			constexpr float comboWidth = 120.0f;
			constexpr float margin = 8.0f;

			ImGui::SetCursorPos(ImVec2(size.x - comboWidth - margin, 4.0f));
			ImGui::SetNextItemWidth(comboWidth);

			if (ImGui::BeginCombo("##ViewportOptions", "Passes"))
			{
				auto flags = editor::editorContext.renderer.GetEnabledFlags();

				auto RenderPassToggle = [&](const char* label, engine::RenderPassType flag) {
					bool isEnabled = (flags & flag) != engine::RenderPassType::None;

					if (ImGui::Checkbox(label, &isEnabled))
					{
						if (isEnabled)
							editor::editorContext.renderer.EnableFlag(flag);
						else
							editor::editorContext.renderer.DisableFlag(flag);
					}
					};

				RenderPassToggle("Background", engine::RenderPassType::Background);
				RenderPassToggle("Opaque", engine::RenderPassType::Opaque);
				RenderPassToggle("Light", engine::RenderPassType::Light);
				RenderPassToggle("Shadow", engine::RenderPassType::Shadow);
				RenderPassToggle("Normal", engine::RenderPassType::Normal);
				RenderPassToggle("Wireframe", engine::RenderPassType::Wireframe);

				ImGui::EndCombo();
			}
		}

		ImGui::EndChild();
	}

	void ViewportWindow::Draw()
	{
		ScopedStyle style
		{
			{ ImGuiStyleVar_WindowPadding, ImVec2(0, 0) },
			{ ImGuiCol_WindowBg, ImVec4(0.9f, 0.2f, 1.0f, 1.0f) },
			{ ImGuiStyleVar_WindowBorderSize, 0.0f }
		};

		ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		UpdateViewportSize();
		m_CameraController.SetHovered(ImGui::IsWindowHovered());

		if(ImGui::IsKeyPressed(ImGuiKey_F, false))
		{
			engine::EventBus::Publish(FocusEntityEvent{ SelectionManager::Entities.GetPrimary() });
		}

		ImVec2 imagePos = ImGui::GetCursorPos();
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();

		engine::SceneAsset& scene = engine::AssetManager::GetAsset<engine::SceneAsset>(engine::SceneManager::GetActiveScene());
		editor::editorContext.renderer.RenderScene(scene);

		ImGui::Image(editor::editorContext.renderer.GetTextureView().Get(), viewportSize);

		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			if (editor::editorContext.editorMode == EditorMode::Edit && !m_TransformController.IsGizmoOver())
			{
				ImVec2 mousePos = ImGui::GetMousePos();

				const uint32_t mx = static_cast<uint32_t>(mousePos.x - m_ViewportBounds[0].x);
				const uint32_t my = static_cast<uint32_t>(mousePos.y - m_ViewportBounds[0].y);

				engine::Uuid id = m_EntityPicker.Pick(mx, my);
				bool additive = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_LeftShift);
				SelectionManager::Entities.Select(id, additive);
			}
		}

		DrawOverlay(imagePos, viewportSize);


		m_TransformController.DrawGizmo(scene, m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);
		ImGui::End();
	}
}