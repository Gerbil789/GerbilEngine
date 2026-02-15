#include "ViewportWindow.h"

#include "Editor/Windows/Utility/ScopedStyle.h"
#include "Editor/Core/EditorContext.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Editor/Command/TransformEntity.h"
#include "Engine/Core/Runtime.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Core/Input.h"
#include "Engine/Event/MouseEvent.h"

#include "Engine/Graphics/RenderPass/BackgroundPass.h"
#include "Engine/Graphics/RenderPass/OpaquePass.h"
#include "Engine/Graphics/RenderPass/WireframePass.h"
#include "Engine/Graphics/RenderPass/NormalPass.h"
#include "Engine/Graphics/RenderPass/EntityIdPass.h"
#include "Engine/Graphics/RenderPass/LightPass.h"

#include <imgui.h>
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace Editor
{
	static ImGuizmo::OPERATION gizmoType = ImGuizmo::OPERATION::TRANSLATE;

	static Engine::EntityIdPass* s_EntityIdPass = nullptr;
	static Engine::OpaquePass* s_OpaquePass = nullptr;
	static Engine::WireframePass* s_WireframePass = nullptr;
	static Engine::NormalPass* s_NormalPass = nullptr;
	static Engine::LightPass* s_LightPass = nullptr;

	ViewportWindow::ViewportWindow()
	{
		auto camera = &m_CameraController.GetCamera();
		camera->SetBackground(Engine::Camera::Background::Skybox);
		m_Renderer.SetCamera(camera);

		m_Renderer.AddPass(new Engine::BackgroundPass());

		s_OpaquePass = new Engine::OpaquePass();
		m_Renderer.AddPass(s_OpaquePass);

		s_EntityIdPass = new Engine::EntityIdPass();
		m_Renderer.AddPass(s_EntityIdPass);

		s_LightPass = new Engine::LightPass();
		m_Renderer.AddPass(s_LightPass);

		s_NormalPass = new Engine::NormalPass();
		s_NormalPass->m_Enabled = false;
		m_Renderer.AddPass(s_NormalPass);

		s_WireframePass = new Engine::WireframePass();
		s_WireframePass->m_Enabled = false;
		m_Renderer.AddPass(s_WireframePass);

		Engine::SceneManager::RegisterOnSceneChanged([this](Engine::Scene* scene)
			{
				m_Scene = scene;
				m_Renderer.SetScene(m_Scene);
			});
	}


	void ViewportWindow::Draw()
	{
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
		m_Renderer.RenderScene();

		ImVec2 mousePos = ImGui::GetMousePos();
		float mx = mousePos.x - m_ViewportBounds[0].x;
		float my = mousePos.y - m_ViewportBounds[0].y;

		if (mx >= 0 && my >= 0 && mx < (int)m_ViewportSize.x && my < (int)m_ViewportSize.y)
		{
			Engine::Uuid uuid = s_EntityIdPass->ReadPixel((uint32_t)mx, (uint32_t)my);
			if(uuid)
			{
				m_HoveredEntity = m_Scene->GetEntity(uuid);
			}
		}

		ImVec2 viewportMin = ImGui::GetCursorScreenPos();
		ImVec2 viewportSize = ImVec2(m_ViewportSize.x, m_ViewportSize.y);
		ImVec2 viewportMax = ImVec2(viewportMin.x + viewportSize.x, viewportMin.y + viewportSize.y);

		ImGui::Image(static_cast<WGPUTextureView>(m_Renderer.GetTextureView()), viewportSize);

		DrawGizmos();


		ImGui::SetCursorScreenPos(ImVec2(viewportMin.x + (-viewportMin.x + viewportMax.x) / 2.0f - 30.0f, viewportMin.y + 4.0f));

		ImGui::SetNextItemWidth(60.0f);
		if (ImGui::Button("Play"))
		{
			Engine::Runtime::Start();
		}

		ImGui::SetCursorScreenPos(ImVec2(viewportMax.x - 120.0f - 8.0f, viewportMin.y + 4.0f));

		ImGui::SetNextItemWidth(120.0f);

		if (ImGui::BeginCombo("##ViewportOptions", "Passes"))
		{
			ImGui::Checkbox("Opaque", &s_OpaquePass->m_Enabled);
			ImGui::Checkbox("Light", &s_LightPass->m_Enabled);
			ImGui::Checkbox("Normal", &s_NormalPass->m_Enabled);
			ImGui::Checkbox("Wireframe", &s_WireframePass->m_Enabled);
			//ImGui::Checkbox("Debug");
			ImGui::EndCombo();
		}

		ImGui::End();
	}


	void ViewportWindow::OnEvent(Engine::Event& e)
	{
		if (m_ViewportFocused || m_ViewportHovered)
		{
			m_CameraController.OnEvent(e);
		}

		Engine::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Engine::KeyPressedEvent>([this](auto e) {OnKeyPressed(e); });
		dispatcher.Dispatch<Engine::MouseButtonPressedEvent>([this](auto e) {OnMouseButtonPressed(e); });
	}

	void ViewportWindow::OnKeyPressed(Engine::KeyPressedEvent& e)
	{
		if (e.GetRepeatCount() > 0) return;

		switch (e.GetKey())
		{
		case Engine::Key::Q: gizmoType = (ImGuizmo::OPERATION)0; break;
		case Engine::Key::W: gizmoType = ImGuizmo::OPERATION::TRANSLATE; break;
		case Engine::Key::E: gizmoType = ImGuizmo::OPERATION::ROTATE; break;
		case Engine::Key::R: gizmoType = ImGuizmo::OPERATION::SCALE; break;
		}
	}

	void ViewportWindow::OnMouseButtonPressed(Engine::MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() != Engine::Mouse::ButtonLeft) return;
	
		if (m_ViewportHovered && !ImGuizmo::IsOver())
		{
			if (m_HoveredEntity)
			{
				EditorContext::Entities().Select(m_HoveredEntity, Engine::Input::IsKeyPressed(Engine::Key::LeftControl) || Engine::Input::IsKeyPressed(Engine::Key::LeftShift));
			}
			else
			{
				EditorContext::Entities().Clear();
			}
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
		if(gizmoType == 0)
		{
			return;
		}

		Engine::Entity selectedEntity = EditorContext::Entities().GetPrimary();

		if (!selectedEntity)
		{
			return;
		}

		ImGuizmo::SetDrawlist();

		ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

		const glm::mat4& cameraProjection = m_CameraController.GetCamera().GetProjectionMatrix();
		glm::mat4 cameraView = m_CameraController.GetCamera().GetViewMatrix();

		auto& transformComponent = selectedEntity.GetComponent<Engine::TransformComponent>();
		glm::mat4 worldTransform = transformComponent.GetWorldMatrix(m_Scene->Registry());

		float* snapValue = nullptr;
		if(Engine::Input::IsKeyPressed(Engine::Key::LeftControl))
		{
			static float snapTranslateScale[3] = { 0.5f, 0.5f, 0.5f };
			static float snapRotate[3] = { 45.0f, 45.0f, 45.0f };
			snapValue = (gizmoType == ImGuizmo::OPERATION::ROTATE) ? snapRotate : snapTranslateScale;
		}

		ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), gizmoType, ImGuizmo::MODE::LOCAL, glm::value_ptr(worldTransform), nullptr, snapValue);

		bool isUsing = ImGuizmo::IsUsing();

		if (isUsing && !m_GizmoPreviouslyUsed)
		{
			m_InitialWorldTransforms.clear();

			auto& selection = EditorContext::Entities().GetAll();

			for (auto entity : selection)
			{
				auto& tc = entity.GetComponent<Engine::TransformComponent>();
				m_InitialWorldTransforms[entity] = tc.GetWorldMatrix(m_Scene->Registry());
			}

			m_InitialPrimaryWorld = m_InitialWorldTransforms[selectedEntity];
		}

		if (isUsing)
		{

			glm::mat4 newPrimaryWorld = worldTransform;
			glm::mat4 delta = newPrimaryWorld * glm::inverse(m_InitialPrimaryWorld);

			for (auto entity : EditorContext::Entities().GetAll())
			{
				auto& tc = entity.GetComponent<Engine::TransformComponent>();

				glm::mat4 originalWorld = m_InitialWorldTransforms[entity];
				glm::mat4 newWorld = delta * originalWorld;

				glm::mat4 parentWorld = glm::mat4(1.0f);
				if (tc.parent != entt::null)
				{
					Engine::Entity parent{ tc.parent, &m_Scene->Registry() };
					parentWorld = parent.GetComponent<Engine::TransformComponent>().GetWorldMatrix(m_Scene->Registry());
				}

				glm::mat4 newLocal = glm::inverse(parentWorld) * newWorld;

				glm::vec3 skew;
				glm::vec4 perspective;
				glm::quat rot;
				glm::vec3 trans, scale;
				glm::decompose(newLocal, scale, rot, trans, skew, perspective);

				tc.position = trans;
				tc.rotation = glm::degrees(glm::eulerAngles(rot));
				tc.scale = scale;
			}
		}

		if (!isUsing && m_GizmoPreviouslyUsed)
		{
			auto& selection = EditorContext::Entities().GetAll();

			std::vector<TransformData> before, after;

			for (auto& [entity, initialWorld] : m_InitialWorldTransforms)
			{
				auto& tc = entity.GetComponent<Engine::TransformComponent>();
				{
					glm::mat4 parentWorld = glm::mat4(1.0f);
					if (tc.parent != entt::null)
					{
						Engine::Entity parent{ tc.parent, &m_Scene->Registry() };
						parentWorld = parent.GetComponent<Engine::TransformComponent>().GetWorldMatrix(m_Scene->Registry());
					}
					glm::mat4 initialLocal = glm::inverse(parentWorld) * initialWorld;
					glm::vec3 skew;
					glm::vec4 perspective;
					glm::quat rot;
					glm::vec3 trans, scale;
					glm::decompose(initialLocal, scale, rot, trans, skew, perspective);

					before.push_back({ trans, glm::degrees(glm::eulerAngles(rot)), scale });
				}
			}

			for (auto entity : selection)
			{
				auto& tc = entity.GetComponent<Engine::TransformComponent>();
				TransformData afterData;
				afterData.Position = tc.position;
				afterData.Rotation = tc.rotation;
				afterData.Scale = tc.scale;
				after.push_back(afterData);
			}

			EditorCommandManager::Enqueue(std::make_unique<TransformEntitiesCommand>(selection, before, after));

		}

		m_GizmoPreviouslyUsed = isUsing;
	}

}