#include "ViewportWindow.h"
#include "Engine/Core/Application.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Math/Math.h"
#include "Engine/Core/Core.h"
#include "../EditorApp.h"
#include "../EditorServiceRegistry.h"
#include "imgui/imgui.h"
#include "ImGuizmo/ImGuizmo.h"
#include <glm/gtc/type_ptr.hpp>

namespace Engine
{
	ViewportWindow::ViewportWindow(EditorContext* context) : EditorWindow(context)
	{
		SceneManager::RegisterObserver(this);

		m_SceneController = EditorServiceRegistry::Get<SceneController>();

		//create editor frame buffer
		FrameBufferSpecification editorFrameBufferSpecification;
		editorFrameBufferSpecification.Attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::DEPTH24STENCIL8 };
		editorFrameBufferSpecification.Width = 1280;
		editorFrameBufferSpecification.Height = 720;
		m_EditorFrameBuffer = FrameBuffer::Create(editorFrameBufferSpecification);

		m_EditorCamera = CreateRef<EditorCamera>(30.0f, 1.778f, 0.1f, 1000.0f); //TODO: what are these values? must it be there?
	}

	ViewportWindow::~ViewportWindow()
	{
		SceneManager::UnregisterObserver(this);
	}

	void ViewportWindow::OnSceneChanged(Ref<Scene> newScene)
	{
		m_Scene = newScene;
	}

	void ViewportWindow::OnUpdate(Timestep ts)
	{
		// resize viewport 
		m_Scene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y); //TODO: must be called every frame?
		FrameBufferSpecification spec = m_EditorFrameBuffer->GetSpecification();
		if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_EditorFrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCamera->SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
		}


		//bind frame buffer
		m_EditorFrameBuffer->Bind();

		//clear frame buffer
		RenderCommand::Clear();
		m_EditorFrameBuffer->ClearAttachment(1, -1); //clear ID attachment

		//update scene
		m_EditorCamera->OnUpdate(ts);
		m_Scene->OnUpdate(ts, *m_EditorCamera);




		//get mouse position
		auto [mx, my] = ImGui::GetMousePos();

		//convert to viewport space
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;

		int mouseX = (int)mx;
		int mouseY = (int)my;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int pixelData = m_EditorFrameBuffer->ReadPixel(1, mouseX, mouseY);
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity{ (entt::entity)pixelData, m_Scene.get() };
		}

		//unbind frame buffer
		m_EditorFrameBuffer->Unbind();
	}

	void ViewportWindow::OnImGuiRender()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0)); // Remove padding
		ImGui::Begin("Viewport");

		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();

		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();

		auto editorApp = static_cast<EditorApp*>(&Engine::Application::Get()); //TODO: dont get it every cycle, store it somewhere
		editorApp->GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportSize.x, viewportSize.y };
		uint32_t textureID = m_EditorFrameBuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)textureID, ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* droppedPath = (const wchar_t*)payload->Data;
				std::filesystem::path path(droppedPath);

				LOG_INFO("Dropped file: {0}", path);

				//open scene
				if (path.extension() == ".scene")
				{
					SceneManager::LoadScene(path);
				}
			}
			ImGui::EndDragDropTarget();
		}


		//gizmo
		Entity selectedEntity = m_SceneController->GetSelectedEntity();

		if (selectedEntity && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

			const glm::mat4& cameraProjection = m_EditorCamera->GetProjection();
			glm::mat4 cameraView = m_EditorCamera->GetViewMatrix();

			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = 0.5f; // Snap to 0.5m for translation/scale
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f; // Snap to 45 degrees for rotation

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::MODE::LOCAL, glm::value_ptr(transform), nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				glm::vec3 originalRotation = tc.Rotation;
				glm::vec3 deltaRotation = glm::degrees(rotation) - originalRotation;

				tc.Position = translation;
				tc.Rotation += deltaRotation; // to prevent gimbal lock
				tc.Scale = scale;
			}
		}

		ImGui::End();
		ImGui::PopStyleVar(); // Restore padding
	}

	void ViewportWindow::OnEvent(Event& e)
	{
		if (m_ViewportFocused || m_ViewportHovered)
		{
			m_EditorCamera->OnEvent(e);
		}

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(ENGINE_BIND_EVENT_FN(ViewportWindow::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(ENGINE_BIND_EVENT_FN(ViewportWindow::OnMouseButtonPressed));
	}

	bool ViewportWindow::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetRepeatCount() > 0)
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

		switch (e.GetKeyCode())
		{
		case Key::Q:
			m_GizmoType = -1; // Disable gizmo
			break;

		case Key::W:
			m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;

		case Key::E:
			m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			break;

		case Key::R:
			m_GizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		}
	}

	bool ViewportWindow::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if (!m_HoveredEntity) return false;

			if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
			{
				m_SceneController->SelectEntity(m_HoveredEntity);
			}
		}
		return false;
	}


}