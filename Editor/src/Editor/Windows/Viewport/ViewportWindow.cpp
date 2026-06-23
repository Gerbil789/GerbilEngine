#include "ViewportWindow.h"
#include "Editor/Windows/Viewport/ViewportCameraController.h"
#include "Editor/Windows/Viewport/TransformController.h"
#include "Editor/Windows/Viewport/EditorPicker.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Editor/Command/ChangeEditorStateCommand.h"
#include "Editor/Windows/Utility/ScopedStyle.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Core/State.h"
#include <glm/glm.hpp>

namespace Editor
{
	namespace
	{
		EditorPicker m_EntityPicker;
		ViewportCameraController m_CameraController;
		TransformController m_TransformController;

		glm::vec2 m_ViewportBounds[2] = { {0.0f, 0.0f}, {0.0f, 0.0f} };
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
	}

	void ViewportWindow::Initialize()
	{
		m_CameraController.Initialize();
		m_TransformController.Initialize();
		m_EntityPicker.Initialize();
	}

	static void UpdateViewportSize()
	{
		ImVec2 newSize = ImGui::GetContentRegionAvail();
		if (newSize.x == m_ViewportSize.x && newSize.y == m_ViewportSize.y) return;

		m_ViewportSize = { newSize.x, newSize.y };
		EditorContext::editorCamera.SetAspectRatio(m_ViewportSize.x / m_ViewportSize.y);

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
			desc.label = { "RendererColorTexture", WGPU_STRLEN };
			desc.dimension = wgpu::TextureDimension::_2D;
			desc.format = wgpu::TextureFormat::RGBA8Unorm;
			desc.size = size;
			desc.mipLevelCount = 1;
			desc.sampleCount = 1;
			desc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding;
			wgpu::Texture colorTexture = Engine::GraphicsContext::GetDevice().createTexture(desc);

			wgpu::TextureViewDescriptor view;
			view.label = { "RendererColorTextureView", WGPU_STRLEN };
			view.dimension = wgpu::TextureViewDimension::_2D;
			view.format = desc.format;
			view.baseMipLevel = 0;
			view.mipLevelCount = 1;
			view.baseArrayLayer = 0;
			view.arrayLayerCount = 1;
			EditorContext::renderer.SetColorTarget(colorTexture.createView(view));
		}

		// Depth
		{
			wgpu::TextureDescriptor desc;
			desc.label = { "RendererDepthTextureView", WGPU_STRLEN };
			desc.dimension = wgpu::TextureDimension::_2D;
			desc.format = wgpu::TextureFormat::Depth24Plus;
			desc.mipLevelCount = 1;
			desc.sampleCount = 1;
			desc.size = size;
			desc.usage = wgpu::TextureUsage::RenderAttachment;
			desc.viewFormatCount = 1;
			desc.viewFormats = &wgpu::TextureFormat::Depth24Plus;
			wgpu::Texture depthTexture = Engine::GraphicsContext::GetDevice().createTexture(desc);

			wgpu::TextureViewDescriptor view;
			view.aspect = wgpu::TextureAspect::DepthOnly;
			view.baseArrayLayer = 0;
			view.arrayLayerCount = 1;
			view.baseMipLevel = 0;
			view.mipLevelCount = 1;
			view.dimension = wgpu::TextureViewDimension::_2D;
			view.format = wgpu::TextureFormat::Depth24Plus;

			EditorContext::renderer.SetDepthTarget(depthTexture.createView(view));
		}

		Engine::viewportState.width = m_ViewportSize.x;
		Engine::viewportState.height = m_ViewportSize.y;

		Engine::viewportState.positionX = m_ViewportBounds[0].x;
		Engine::viewportState.positionY = m_ViewportBounds[0].y;
	}

	void DrawOverlay(const ImVec2& imagePos, const ImVec2& size)
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

			if (EditorContext::state == EditorState::Edit)
			{
				if (ImGui::Button("Play", ImVec2(buttonWidth, 0)))
				{
					EditorCommandManager::Enqueue(std::make_unique<ChangeEditorStateCommand>(EditorState::Play));
				}
			}
			else
			{
				if (ImGui::Button("Stop", ImVec2(buttonWidth, 0)))
				{
					EditorCommandManager::Enqueue(std::make_unique<ChangeEditorStateCommand>(EditorState::Edit));
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
				auto flags = EditorContext::renderer.GetEnabledFlags();

				auto RenderPassToggle = [&](const char* label, Engine::RenderPassType flag) {
					bool isEnabled = (flags & flag) != Engine::RenderPassType::None;

					if (ImGui::Checkbox(label, &isEnabled))
					{
						if (isEnabled)
							EditorContext::renderer.EnableFlag(flag);
						else
							EditorContext::renderer.DisableFlag(flag);
					}
					};

				RenderPassToggle("Background", Engine::RenderPassType::Background);
				RenderPassToggle("Opaque", Engine::RenderPassType::Opaque);
				RenderPassToggle("Light", Engine::RenderPassType::Light);
				RenderPassToggle("Shadow", Engine::RenderPassType::Shadow);
				RenderPassToggle("Normal", Engine::RenderPassType::Normal);
				RenderPassToggle("Wireframe", Engine::RenderPassType::Wireframe);

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

		ImVec2 imagePos = ImGui::GetCursorPos();
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();

		Engine::Scene& scene = Engine::AssetManager::GetAsset<Engine::Scene>(Engine::SceneManager::GetActiveScene());

		if (EditorContext::state == EditorState::Edit)
		{
			EditorContext::renderer.RenderScene(scene, EditorContext::editorCamera);
		}
		else
		{
			Engine::Camera* camera = scene.GetActiveCamera();
			if (camera)
			{
				camera->SetAspectRatio(m_ViewportSize.x / m_ViewportSize.y);
				EditorContext::renderer.RenderScene(scene, *camera);
			}
			else
			{
				EditorContext::renderer.RenderScene(scene, EditorContext::editorCamera);
			}
		}

		ImGui::Image(static_cast<WGPUTextureView>(EditorContext::renderer.GetTextureView()), viewportSize);

		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			if (EditorContext::state == EditorState::Edit && !m_TransformController.IsGizmoOver())
			{
				ImVec2 mousePos = ImGui::GetMousePos();

				const uint32_t mx = static_cast<uint32_t>(mousePos.x - m_ViewportBounds[0].x);
				const uint32_t my = static_cast<uint32_t>(mousePos.y - m_ViewportBounds[0].y);

				Engine::Uuid id = m_EntityPicker.Pick(mx, my);
				bool additive = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_LeftShift);
				SelectionManager::Entities.Select(id, additive);
			}
		}

		DrawOverlay(imagePos, viewportSize);
		m_TransformController.DrawGizmo(scene, m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);
		ImGui::End();
	}
}