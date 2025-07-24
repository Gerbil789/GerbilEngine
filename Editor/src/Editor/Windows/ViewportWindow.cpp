#include "ViewportWindow.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Core/Core.h"
#include "Engine/Core/Input.h"
#include "Editor/Core/EditorSceneController.h"
#include "Engine/Events/MouseEvent.h"
#include "Engine/Math/Math.h"
#include "Editor/Elements/Style.h"
#include <imgui.h>
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>

namespace Editor
{
	using namespace Engine;

	void ViewportWindow::OnUpdate(Timestep ts)
	{
		Scene* scene = SceneManager::GetActiveScene();
		if(scene != m_Scene)
		{
			m_Scene = scene;
			m_Renderer.SetScene(m_Scene);
			m_EntityIdRenderer.SetScene(m_Scene);
		}

		ScopedStyle style({
			{ ImGuiStyleVar_WindowPadding, ImVec2(0, 0) },
			{ ImGuiCol_WindowBg, ImVec4(0.9f, 0.2f, 1.0f, 1.0f) },
			{ ImGuiStyleVar_WindowBorderSize, 0.0f }
			});

		ImGui::Begin("Viewport");

		UpdateViewportSize();
		
		m_ViewportHovered = ImGui::IsWindowHovered();
		m_ViewportFocused = ImGui::IsWindowFocused();

		// Render scene
		m_Renderer.BeginScene(m_CameraController.GetCamera());
		m_Renderer.RenderScene();
		m_Renderer.EndScene();

		// Render entity IDs
		m_EntityIdRenderer.Render(); 

		ImVec2 mousePos = ImGui::GetMousePos();
		float mx = mousePos.x - m_ViewportBounds[0].x;
		float my = mousePos.y - m_ViewportBounds[0].y;

		if (mx >= 0 && my >= 0 && mx < (int)m_ViewportSize.x && my < (int)m_ViewportSize.y)
		{
			UUID uuid = m_EntityIdRenderer.ReadPixel(mx, my);
			m_HoveredEntity = m_Scene->GetEntityByUUID(uuid);
		}

		// Draw scene
		ImGui::Image((ImTextureID)(intptr_t)(WGPUTextureView)m_Renderer.GetTextureView(), ImVec2(m_ViewportSize.x, m_ViewportSize.y));


		//if (ImGui::BeginDragDropTarget())
		//{
		//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		//	{
		//		const wchar_t* droppedPath = (const wchar_t*)payload->Data;
		//		std::filesystem::path path(droppedPath);

		//		LOG_INFO("Dropped file: {0}", path);

		//		//open scene
		//		if (path.extension() == ".scene")
		//		{
		//			SceneManager::LoadScene(path);
		//		}
		//	}
		//	ImGui::EndDragDropTarget();
		//}

		DrawGizmos();
		ImGui::End();
	}


	void ViewportWindow::OnEvent(Event& e)
	{
		if (m_ViewportFocused || m_ViewportHovered)
		{
			m_CameraController.OnEvent(e);
		}

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(ENGINE_BIND_EVENT_FN(ViewportWindow::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(ENGINE_BIND_EVENT_FN(ViewportWindow::OnMouseButtonPressed));
	}

	bool ViewportWindow::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetRepeatCount() > 0) return false;

		switch (e.GetKey())
		{
		case Key::Q: m_GizmoType = -1; break; // Disable gizmo
		case Key::W: m_GizmoType = ImGuizmo::OPERATION::TRANSLATE; break;
		case Key::E: m_GizmoType = ImGuizmo::OPERATION::ROTATE; break;
		case Key::R: m_GizmoType = ImGuizmo::OPERATION::SCALE; break;
		}
	}

	bool ViewportWindow::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if (!m_HoveredEntity) return false;

			if (m_ViewportHovered && !ImGuizmo::IsOver())
			{
				EditorSceneController::SelectEntity(m_HoveredEntity);
			}
		}
		return false;
	}

	void ViewportWindow::UpdateViewportSize()
	{
		ImVec2 newSize = ImGui::GetContentRegionAvail();

		if (newSize.x != m_ViewportSize.x || newSize.y != m_ViewportSize.y)
		{
			m_ViewportSize = { newSize.x, newSize.y };
			m_CameraController.SetViewportSize(m_ViewportSize);

			if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f)
			{
				m_Renderer.Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
				m_EntityIdRenderer.Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			}
		}

		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();

		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };
	}

	void ViewportWindow::DrawGizmos()
	{
		Entity selectedEntity = EditorSceneController::GetSelectedEntity();

		if (selectedEntity && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

			const glm::mat4& cameraProjection = m_CameraController.GetCamera().GetProjectionMatrix();
			glm::mat4 cameraView = m_CameraController.GetCamera().GetViewMatrix();

			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetModelMatrix();

			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = 0.5f; // Snap to 0.5m for translation/scale
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE) snapValue = 45.0f; // Snap to 45 degrees for rotation

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::MODE::LOCAL, glm::value_ptr(transform), nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale); 

				tc.Position = translation;
				tc.Rotation = glm::degrees(rotation);
				tc.Scale = scale;
			}
		}
	}

}