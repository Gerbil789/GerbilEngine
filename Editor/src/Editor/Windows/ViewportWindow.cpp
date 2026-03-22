#include "ViewportWindow.h"

#include "Editor/Windows/Utility/ScopedStyle.h"
#include "Editor/Core/EditorSelection.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Editor/Command/TransformEntity.h"
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
#include "Engine/Graphics/RenderPass/ShadowPass.h"
#include "Editor/Core/EditorCameraController.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Editor/Core/EditorRuntime.h"

#include "Engine/Scene/Scene.h"
#include <glm/glm.hpp>
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>

//TODO: clean up this file

namespace Editor
{
	namespace
	{
		Engine::Scene* m_Scene = nullptr;

		glm::vec2 m_ViewportBounds[2] = { {0.0f, 0.0f}, {0.0f, 0.0f} };
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		Engine::Entity m_HoveredEntity;

		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;

		bool m_GizmoPreviouslyUsed = false;

		std::unordered_map<Engine::Entity, glm::mat4> m_InitialWorldTransforms;
		glm::mat4 m_InitialPrimaryWorld = glm::mat4(1.0f);

		static ImGuizmo::OPERATION gizmoType = ImGuizmo::OPERATION::TRANSLATE;

		static Engine::EntityIdPass* s_EntityIdPass = nullptr;
		static Engine::OpaquePass* s_OpaquePass = nullptr;
		static Engine::WireframePass* s_WireframePass = nullptr;
		static Engine::NormalPass* s_NormalPass = nullptr;
		static Engine::LightPass* s_LightPass = nullptr;
		static Engine::ShadowPass* s_ShadowPass = nullptr;

		static Engine::Renderer m_Renderer;
		static Engine::Camera* m_Camera = nullptr;

		static bool enableOpaquePass = true;
		static bool enableNormalPass = false;
		static bool enableWireframePass = false;
		static bool enableLightPass = false;
	}

	void UpdateViewportSize()
	{
		ImVec2 newSize = ImGui::GetContentRegionAvail();

		if (newSize.x != m_ViewportSize.x || newSize.y != m_ViewportSize.y)
		{
			m_ViewportSize = { newSize.x, newSize.y };
			m_Camera->SetAspectRatio(m_ViewportSize.x / m_ViewportSize.y);

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

	void DrawGizmos()
	{
		if (gizmoType == 0)
		{
			return;
		}

		Engine::Entity selectedEntity = EditorSelection::Entities().GetPrimary();

		if (!selectedEntity)
		{
			return;
		}

		ImGuizmo::SetDrawlist();

		ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

		const glm::mat4& cameraProjection = m_Camera->GetProjectionMatrix();
		glm::mat4 cameraView = m_Camera->GetViewMatrix();

		auto& transformComponent = selectedEntity.Get<Engine::TransformComponent>();
		glm::mat4 worldTransform = transformComponent.GetWorldMatrix();

		float* snapValue = nullptr;
		if (Engine::Input::IsKeyDown(Engine::KeyCode::LeftControl))
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

			auto& selection = EditorSelection::Entities().GetAll();

			for (auto entity : selection)
			{
				auto& tc = entity.Get<Engine::TransformComponent>();
				m_InitialWorldTransforms[entity] = tc.GetWorldMatrix();
			}

			m_InitialPrimaryWorld = m_InitialWorldTransforms[selectedEntity];
		}

		if (isUsing)
		{

			glm::mat4 newPrimaryWorld = worldTransform;
			glm::mat4 delta = newPrimaryWorld * glm::inverse(m_InitialPrimaryWorld);

			for (auto entity : EditorSelection::Entities().GetAll())
			{
				auto& tc = entity.Get<Engine::TransformComponent>();

				glm::mat4 originalWorld = m_InitialWorldTransforms[entity];
				glm::mat4 newWorld = delta * originalWorld;

				glm::mat4 parentWorld = glm::mat4(1.0f);
				if (tc.parent)
				{
					parentWorld = tc.parent.Get<Engine::TransformComponent>().GetWorldMatrix();
				}

				glm::mat4 newLocal = glm::inverse(parentWorld) * newWorld;

				//glm::vec3 skew;
				//glm::vec4 perspective;
				glm::vec3 rot;
				glm::vec3 trans, scale;
				//glm::decompose(newLocal, scale, rot, trans, skew, perspective);


				ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(newLocal), glm::value_ptr(trans), glm::value_ptr(rot), glm::value_ptr(scale));

				tc.position = trans;
				tc.rotation = rot;
				tc.scale = scale;
			}
		}

		if (!isUsing && m_GizmoPreviouslyUsed)
		{
			auto& selection = EditorSelection::Entities().GetAll();

			std::vector<TransformData> before, after;

			for (auto& [entity, initialWorld] : m_InitialWorldTransforms)
			{
				auto& tc = entity.Get<Engine::TransformComponent>();
				{
					glm::mat4 parentWorld = glm::mat4(1.0f);
					if (tc.parent)
					{
						parentWorld = tc.parent.Get<Engine::TransformComponent>().GetWorldMatrix();
					}
					glm::mat4 initialLocal = glm::inverse(parentWorld) * initialWorld;
					//glm::vec3 skew;
					//glm::vec4 perspective;
					//glm::quat rot;
					//glm::vec3 trans, scale;
					//glm::decompose(initialLocal, scale, rot, trans, skew, perspective);

					//glm::vec3 skew;
//glm::vec4 perspective;
					glm::vec3 rot;
					glm::vec3 trans, scale;
					//glm::decompose(newLocal, scale, rot, trans, skew, perspective);


					ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(initialLocal), glm::value_ptr(trans), glm::value_ptr(rot), glm::value_ptr(scale));

					before.push_back({ trans, rot, scale });
				}
			}

			for (auto entity : selection)
			{
				auto& tc = entity.Get<Engine::TransformComponent>();
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

	void ViewportWindow::Initialize()
	{
		ImGuizmo::AllowAxisFlip(false);
		ImGuizmo::SetGizmoSizeClipSpace(0.15f);

		m_Camera = new Engine::Camera();
		m_Camera->SetBackground(Engine::Camera::Background::Skybox);
		m_Camera->SetPosition(glm::vec3(0.0f, 0.0f, -20.0f));

		SetCameraController(new EditorCameraController(m_Camera)); //TODO: this api is weird, make controller static?

		m_Renderer.SetCamera(m_Camera);

		m_Renderer.AddPass(new Engine::BackgroundPass());

		s_ShadowPass = new Engine::ShadowPass();
		m_Renderer.AddPass(s_ShadowPass);

		s_OpaquePass = new Engine::OpaquePass();
		m_Renderer.AddPass(s_OpaquePass);

		s_EntityIdPass = new Engine::EntityIdPass();
		m_Renderer.AddPass(s_EntityIdPass);

		s_LightPass = new Engine::LightPass();
		s_NormalPass = new Engine::NormalPass();
		s_WireframePass = new Engine::WireframePass();


		Engine::SceneManager::RegisterOnSceneChanged([this](Engine::Scene* scene)
			{
				m_Scene = scene;
				EditorSelection::Entities().Clear();
				m_Renderer.SetScene(m_Scene);

				if (EditorRuntime::GetState() == EditorState::Edit)
				{
					m_Renderer.SetCamera(m_Camera);
				}
				else if (EditorRuntime::GetState() == EditorState::Play)
				{
					auto cameraEntity = m_Scene->GetActiveCamera();
					if (cameraEntity)
					{
						m_Renderer.SetCamera(cameraEntity.Get<Engine::CameraComponent>().camera);
					}
				}

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


		if(EditorRuntime::GetState() == EditorState::Edit)
		{
			if (ImGui::Button("Play")) EditorRuntime::SetEditorState(EditorState::Play);
		}
		else
		{
			if (ImGui::Button("Stop")) EditorRuntime::SetEditorState(EditorState::Edit);
		}
	

		ImGui::SetCursorScreenPos(ImVec2(viewportMax.x - 120.0f - 8.0f, viewportMin.y + 4.0f));

		ImGui::SetNextItemWidth(120.0f);

		if (ImGui::BeginCombo("##ViewportOptions", "Passes"))
		{
			if(ImGui::Checkbox("Opaque", &enableOpaquePass))
			{
				enableOpaquePass ? m_Renderer.AddPass(s_OpaquePass) : m_Renderer.RemovePass(s_OpaquePass);
			}

			if (ImGui::Checkbox("Light", &enableLightPass))
			{
				enableLightPass ? m_Renderer.AddPass(s_LightPass) : m_Renderer.RemovePass(s_LightPass);
			}

			if (ImGui::Checkbox("Normal", &enableNormalPass))
			{
				enableNormalPass ? m_Renderer.AddPass(s_NormalPass) : m_Renderer.RemovePass(s_NormalPass);
			}

			if (ImGui::Checkbox("Wireframe", &enableWireframePass))
			{
				enableWireframePass ? m_Renderer.AddPass(s_WireframePass) : m_Renderer.RemovePass(s_WireframePass);
			}

			ImGui::EndCombo();
		}

		ImGui::End();

		if(Engine::Input::IsKeyPressedOnce(Engine::KeyCode::Q)) gizmoType = (ImGuizmo::OPERATION)0;
		if(Engine::Input::IsKeyPressedOnce(Engine::KeyCode::W)) gizmoType = ImGuizmo::OPERATION::TRANSLATE;
		if(Engine::Input::IsKeyPressedOnce(Engine::KeyCode::E)) gizmoType = ImGuizmo::OPERATION::ROTATE;
		if(Engine::Input::IsKeyPressedOnce(Engine::KeyCode::R)) gizmoType = ImGuizmo::OPERATION::SCALE;

		if(Engine::Input::IsMouseButtonPressed(Engine::MouseCode::ButtonLeft))
		{
			if (m_ViewportHovered && !ImGuizmo::IsOver())
			{
				if (m_HoveredEntity)
				{
					EditorSelection::Entities().Select(m_HoveredEntity, Engine::Input::IsKeyDown(Engine::KeyCode::LeftControl) || Engine::Input::IsKeyDown(Engine::KeyCode::LeftShift));
				}
				else
				{
					EditorSelection::Entities().Clear();
				}
			}
		}
	}
}