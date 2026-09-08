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
#include "Engine/Core/Log.h"
#include "Engine/System/CameraSystem.h"
#include "Engine/System/TransformSystem.h"
#include "Engine/Core/Application.h"
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

		wgpu::Texture m_Texture;
		wgpu::TextureView m_TextureView;
	}

	void ViewportWindow::Initialize()
	{
		m_CameraController.Initialize();
		m_TransformController.Initialize();
	}

	static void UpdateViewportSize()
	{
		ImVec2 newSize = ImGui::GetContentRegionAvail();
		if (newSize.x == m_ViewportSize.x && newSize.y == m_ViewportSize.y) return;

		m_ViewportSize = { newSize.x, newSize.y };

		engine::Application::s_Renderer.SetSize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));


		wgpu::TextureDescriptor texDesc;
		texDesc.label = "viewportTexture";
		texDesc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding;
		texDesc.dimension = wgpu::TextureDimension::e2D;
		texDesc.size = { static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y), 1 };
		texDesc.format = engine::GraphicsContext::GetSurfaceFormat();
		texDesc.mipLevelCount = 1;
		texDesc.sampleCount = 1;

		m_Texture = engine::GraphicsContext::GetDevice().CreateTexture(&texDesc);
		m_TextureView = m_Texture.CreateView();

		ImVec2 viewportMinRegion = ImGui::GetWindowContentRegionMin();
		ImVec2 viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		ImVec2 viewportOffset = ImGui::GetWindowPos();

		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		if (m_ViewportSize.x <= 0.0f || m_ViewportSize.y <= 0.0f) return;

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
				}
			}
			else
			{
				if (ImGui::Button("Stop", ImVec2(buttonWidth, 0)))
				{
					EditorCommandManager::Enqueue(std::make_unique<ChangeEditorStateCommand>(EditorMode::Edit));
				}
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

		auto& scene = engine::AssetManager::GetAsset(engine::SceneManager::GetActiveScene());


		if (editor::editorContext.editorMode == EditorMode::Edit)
		{
			engine::Application::s_Renderer.RenderScene(scene, editor::editorContext.camera, m_TextureView);
		}
		else
		{
			engine::Application::s_Renderer.RenderScene(scene, scene.GetPrimaryCamera(), m_TextureView);
		}

		ImGui::Image(m_TextureView.Get(), viewportSize);

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