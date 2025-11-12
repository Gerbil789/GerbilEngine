#include "ViewportWindow.h"
#include "Engine/Core/Core.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Core/Input.h"
#include "Engine/Event/MouseEvent.h"
#include "Editor/Components/ScopedStyle.h"
#include "Engine/Asset/Importer/TextureImporter.h"
#include "Engine/Asset/AssetManager.h"
#include "Editor/Core/EditorContext.h"
#include "Editor/Command/CommandManager.h"
#include "Editor/Command/TransformEntity.h"
#include <imgui.h>
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace Editor
{
	using namespace Engine;

	static ImGuizmo::OPERATION gizmoType = ImGuizmo::OPERATION::TRANSLATE;

	ViewportWindow::ViewportWindow()
	{
		auto camera = &m_CameraController.GetCamera();
		camera->SetBackgroundType(Camera::BackgroundType::Skybox);
		m_Renderer.SetCamera(camera);

		SceneManager::RegisterOnSceneChanged([this](Scene* scene)
			{
				m_Scene = scene;
				m_Renderer.SetScene(m_Scene);
				m_EntityIdRenderer.SetScene(m_Scene);
			});
	}

	void ViewportWindow::OnUpdate(Timestep ts)
	{
		ScopedStyle style({
			{ ImGuiStyleVar_WindowPadding, ImVec2(0, 0) },
			{ ImGuiCol_WindowBg, ImVec4(0.9f, 0.2f, 1.0f, 1.0f) },
			{ ImGuiStyleVar_WindowBorderSize, 0.0f }
			});

		ImGui::Begin("Viewport");

		UpdateViewportSize(); //TODO: dont call every frame?
		
		m_ViewportHovered = ImGui::IsWindowHovered();
		m_ViewportFocused = ImGui::IsWindowFocused();

		// Render scene
		m_Renderer.BeginScene();
		m_Renderer.RenderScene();
		m_Renderer.EndScene();

		// Render entity IDs
		m_EntityIdRenderer.Render(); 

		ImVec2 mousePos = ImGui::GetMousePos();
		float mx = mousePos.x - m_ViewportBounds[0].x;
		float my = mousePos.y - m_ViewportBounds[0].y;

		if (mx >= 0 && my >= 0 && mx < (int)m_ViewportSize.x && my < (int)m_ViewportSize.y)
		{
			UUID uuid = m_EntityIdRenderer.ReadPixel((uint32_t)mx, (uint32_t)my);
			if(uuid.IsValid())
			{
				m_HoveredEntity = m_Scene->GetEntity(uuid);
			}
		}

		// Draw scene
		ImGui::Image((ImTextureID)(intptr_t)(WGPUTextureView)m_Renderer.GetTextureView(), ImVec2(m_ViewportSize.x, m_ViewportSize.y)); //TODO: too many casts?

		//if (ImGui::BeginDragDropTarget())
		//{
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
		dispatcher.Dispatch<KeyPressedEvent>([this](auto e) {OnKeyPressed(e); });
		dispatcher.Dispatch<MouseButtonPressedEvent>([this](auto e) {OnMouseButtonPressed(e); });
	}

	void ViewportWindow::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetRepeatCount() > 0) return;

		switch (e.GetKey())
		{
		case Key::Q: gizmoType = (ImGuizmo::OPERATION)0; break;
		case Key::W: gizmoType = ImGuizmo::OPERATION::TRANSLATE; break;
		case Key::E: gizmoType = ImGuizmo::OPERATION::ROTATE; break;
		case Key::R: gizmoType = ImGuizmo::OPERATION::SCALE; break;
		}
	}

	void ViewportWindow::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() != Mouse::ButtonLeft) return;
		if (!m_HoveredEntity) return;

		if (m_ViewportHovered && !ImGuizmo::IsOver())
		{
			EditorContext::SelectEntity(m_HoveredEntity);
		}
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
		if(!gizmoType)
		{
			return;
		}

		Entity selectedEntity = EditorContext::GetActiveSelection().Type == SelectionType::Entity ? EditorContext::GetActiveSelection().Entity : Entity{};

		if (!selectedEntity)
		{
			return;
		}

		ImGuizmo::SetDrawlist();

		ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

		const glm::mat4& cameraProjection = m_CameraController.GetCamera().GetProjectionMatrix();
		glm::mat4 cameraView = m_CameraController.GetCamera().GetViewMatrix();

		auto& tc = selectedEntity.GetComponent<TransformComponent>();
		glm::mat4 worldTransform = tc.GetWorldMatrix(m_Scene->GetRegistry());

		float* snapValue = nullptr;
		if(Input::IsKeyPressed(Key::LeftControl))
		{
			static float snapTranslateScale[3] = { 0.5f, 0.5f, 0.5f };
			static float snapRotate[3] = { 45.0f, 45.0f, 45.0f };

			if (gizmoType == ImGuizmo::OPERATION::ROTATE)
			{
				snapValue = snapRotate;
			}
			else
			{
				snapValue = snapTranslateScale;
			}
		}

		ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), gizmoType, ImGuizmo::MODE::LOCAL, glm::value_ptr(worldTransform), nullptr, snapValue);

		bool isUsing = ImGuizmo::IsUsing();

		if (isUsing && !m_GizmoPreviouslyUsed)
		{
			// Store initial state for undo
			m_InitialPos = tc.Position;
			m_InitialRot = tc.Rotation;
			m_InitialScale = tc.Scale;
		}

		if (isUsing)
		{
			glm::mat4 newWorld = worldTransform;

			glm::mat4 parentWorld = glm::mat4(1.0f);

			if (selectedEntity.GetComponent<TransformComponent>().Parent != entt::null)
			{
				Entity parent = { selectedEntity.GetComponent<TransformComponent>().Parent, m_Scene };
				parentWorld = parent.GetComponent<TransformComponent>().GetWorldMatrix(m_Scene->GetRegistry());
			}

			glm::mat4 newLocal = glm::inverse(parentWorld) * newWorld;

			glm::vec3 skew;
			glm::vec4 perspective;
			glm::quat rot;
			glm::vec3 trans, scale;
			glm::decompose(newLocal, scale, rot, trans, skew, perspective);

			tc.Position = trans;
			tc.Rotation = glm::degrees(glm::eulerAngles(rot));
			tc.Scale = scale;
		}

		if (!isUsing && m_GizmoPreviouslyUsed)
		{
			CommandManager::ExecuteCommand<TransformEntityCommand>(
				selectedEntity,
				m_InitialPos, m_InitialRot, m_InitialScale,
				tc.Position, tc.Rotation, tc.Scale
			);
		}

		m_GizmoPreviouslyUsed = isUsing;
	}

}