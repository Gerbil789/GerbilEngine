#include "viewport_window.h"
#include "editor/core/editor_context.h"
#include "editor/windows/viewport/viewport_camera_controller.h"
#include "editor/windows/viewport/transform_controller.h"
#include "engine/core/command_manager.h"
#include "editor/windows/utility/scoped_style.h"
#include "engine/core/context.h"
#include "engine/graphics/graphics_context.h"
#include "engine/system/camera_system.h"
#include "engine/core/application.h"
#include "engine/math/raycast.h"
#include <glm/glm.hpp>

namespace editor
{
	namespace
	{
		ViewportCameraController m_cameraController;
		TransformController m_transformController; //gizmo

		glm::vec2 m_viewportBounds[2] = { { 0.0f, 0.0f },{ 0.0f, 0.0f } };
		glm::vec2 m_viewportSize = { 0.0f, 0.0f };

		wgpu::Texture m_texture;
		wgpu::TextureView m_textureView;
	}

	void ViewportWindow::Initialize()
	{
		m_transformController.Initialize();
	}

	static void UpdateviewportSize()
	{
		ImVec2 newSize = ImGui::GetContentRegionAvail();
		if (newSize.x == m_viewportSize.x && newSize.y == m_viewportSize.y) return;

		m_viewportSize = { newSize.x, newSize.y };

		engine::Application::s_renderer.SetSize(static_cast<uint32_t>(m_viewportSize.x), static_cast<uint32_t>(m_viewportSize.y));

		wgpu::TextureDescriptor texDesc;
		texDesc.label = "viewporttexture";
		texDesc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding;
		texDesc.dimension = wgpu::TextureDimension::e2D;
		texDesc.size = { static_cast<uint32_t>(m_viewportSize.x), static_cast<uint32_t>(m_viewportSize.y), 1 };
		texDesc.format = engine::graphicsContext::GetSurfaceFormat();
		texDesc.mipLevelCount = 1;
		texDesc.sampleCount = 1;

		m_texture = engine::graphicsContext::GetDevice().CreateTexture(&texDesc);
		m_textureView = m_texture.CreateView();

		ImVec2 viewportMinRegion = ImGui::GetWindowContentRegionMin();
		ImVec2 viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		ImVec2 viewportOffset = ImGui::GetWindowPos();

		m_viewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_viewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		if (m_viewportSize.x <= 0.0f || m_viewportSize.y <= 0.0f) return;

		float aspectRatio = m_viewportSize.x / m_viewportSize.y;
		engine::Camerasystem::UpdateCameraProjectionMatrix(editor::context::camera, aspectRatio);
	}

	static void DrawOverlay(const ImVec2& imagePos, const ImVec2& size)
	{
		const float overlayHeight = 32.0f;

		ImGui::SetCursorPos(imagePos);

		ImGui::SetNextWindowBgAlpha(0.1f);

		ImGui::BeginChild("##Overlayui",
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

			if (editor::context::editorMode == EditorMode::Edit)
			{
				if (ImGui::Button("Play", ImVec2(buttonWidth, 0)))
				{
					engine::CommandManager::Enqueue([](){
						editor::context::editorMode = EditorMode::Play;
					});
				}
			}
			else
			{
				if (ImGui::Button("Stop", ImVec2(buttonWidth, 0)))
				{
					engine::CommandManager::Enqueue([](){
						editor::context::editorMode = EditorMode::Edit;
						//engine::audio::StopAll();
						//engine::input::SetCursorMode(engine::input::CursorMode::Normal);
					});

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

		ImGui::Begin("viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		UpdateviewportSize();
		if (ImGui::IsWindowHovered())
		{
			m_cameraController.Update();
		}

		ImVec2 imagePos = ImGui::GetCursorPos();
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();

		const engine::Scene& scene = engine::context::activeSceneHandle.Get();
		engine::DrawList drawList = engine::DrawList::Create(scene.registry);
		const engine::component::Camera* camera = editor::context::editorMode == EditorMode::Edit ? 
		&editor::context::camera : scene.registry.try_get<engine::component::Camera>(scene.primaryCameraEntity);

		engine::Application::s_renderer.Render(drawList, camera, m_textureView);

		ImGui::Image(m_textureView.Get(), viewportSize);

		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			if (editor::context::editorMode == EditorMode::Edit && !m_transformController.IsGizmoOver())
			{
				ImVec2 mousePos = ImGui::GetMousePos();

				const float mx = mousePos.x - m_viewportBounds[0].x;
				const float my = mousePos.y - m_viewportBounds[0].y;

				// NDC coordinates
				float x = (mx / m_viewportSize.x) * 2.0f - 1.0f;
				float y = 1.0f - (my / m_viewportSize.y) * 2.0f;

				if (x >= -1.0f && x <= 1.0f && y >= -1.0f && y <= 1.0f)
				{
					glm::mat4 invViewProj = glm::inverse(camera->viewProjectionMatrix);

					glm::vec4 nearNdc(x, y, 0.0f, 1.0f);
					glm::vec4 farNdc(x, y, 1.0f, 1.0f);

					glm::vec4 nearWorld = invViewProj * nearNdc;
					nearWorld /= nearWorld.w;

					glm::vec4 farWorld = invViewProj * farNdc;
					farWorld /= farWorld.w;

					glm::vec3 origin = glm::vec3(nearWorld);
					glm::vec3 direction = glm::normalize(glm::vec3(farWorld) - origin);

					engine::RayHit hit = engine::Raycast(scene, { origin, direction });
					if (hit) editor::context::SelectEntity(hit.entity);
				}
			}
		}

		DrawOverlay(imagePos, viewportSize);

		m_transformController.DrawGizmo(m_viewportBounds[0].x, m_viewportBounds[0].y, m_viewportBounds[1].x - m_viewportBounds[0].x, m_viewportBounds[1].y - m_viewportBounds[0].y);
		ImGui::End();
	}
}