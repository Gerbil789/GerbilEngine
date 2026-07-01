#include "TransformController.h"
#include "Editor/Core/EditorContext.h"
#include "Editor/Core/SelectionManager.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Editor/Command/TransformEntity.h"
#include "Engine/Core/Input.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Event/KeyEvent.h"
#include <imgui.h>
#include <ImGuizmo.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Editor
{
	namespace
	{
		bool m_GizmoPreviouslyUsed = false;

		std::unordered_map<entt::entity, glm::mat4> m_InitialWorldTransforms;
		glm::mat4 m_InitialPrimaryWorld = glm::mat4(1.0f);

		ImGuizmo::OPERATION gizmoType = ImGuizmo::OPERATION::TRANSLATE;
	}

	void TransformController::Initialize()
	{
		ImGuizmo::AllowAxisFlip(true);
		ImGuizmo::SetGizmoSizeClipSpace(0.15f);

		Engine::EventBus::Subscribe<Engine::KeyPressedEvent>([](const Engine::KeyPressedEvent& e)
			{
				if (EditorContext::state == EditorState::Play) return false;;

				if (e.key == Engine::Key::Q) gizmoType = static_cast<ImGuizmo::OPERATION>(0);
				if (e.key == Engine::Key::W) gizmoType = ImGuizmo::OPERATION::TRANSLATE;
				if (e.key == Engine::Key::E) gizmoType = ImGuizmo::OPERATION::ROTATE;
				if (e.key == Engine::Key::R) gizmoType = ImGuizmo::OPERATION::SCALE;

				return false;
			});
	}

	void TransformController::DrawGizmo(Engine::Scene& scene, float x, float y, float width, float height)
	{
		if (EditorContext::state == EditorState::Play) return;
		if (gizmoType == 0) return;

		Engine::Uuid selectedId = SelectionManager::Entities.GetPrimary();
		if (!selectedId) return;

		entt::registry& registry = scene.GetRegistry();

		entt::entity selectedEntity = scene.GetEntity(selectedId);

		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(x, y, width, height);

		const glm::mat4& cameraProjection = EditorContext::editorCamera.GetProjectionMatrix();
		glm::mat4 cameraView = EditorContext::editorCamera.GetViewMatrix();

		auto& transformComponent = registry.get<Engine::TransformComponent>(selectedEntity);
		glm::mat4 worldTransform = transformComponent.worldMatrix;

		float* snapValue = nullptr;
		if (Engine::Input::IsKeyDown(Engine::Key::LeftControl))
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

			const std::vector<Engine::Uuid>& selection = SelectionManager::Entities.GetAll();

			for (Engine::Uuid id : selection)
			{
				entt::entity entity = scene.GetEntity(id);
				auto& tc = registry.get<Engine::TransformComponent>(entity);
				m_InitialWorldTransforms[entity] = tc.worldMatrix;
			}

			m_InitialPrimaryWorld = m_InitialWorldTransforms[selectedEntity];
		}

		if (isUsing)
		{
			glm::mat4 newPrimaryWorld = worldTransform;
			glm::mat4 delta = newPrimaryWorld * glm::inverse(m_InitialPrimaryWorld);

			for (Engine::Uuid id : SelectionManager::Entities.GetAll())
			{
				entt::entity entity = scene.GetEntity(id);
				auto& tc = registry.get<Engine::TransformComponent>(entity);

				glm::mat4 originalWorld = m_InitialWorldTransforms[entity];
				glm::mat4 newWorld = delta * originalWorld;

				glm::mat4 parentWorld = glm::mat4(1.0f);
				if (tc.parent != entt::null)
				{
					parentWorld = registry.get<Engine::TransformComponent>(tc.parent).worldMatrix;
				}

				glm::mat4 newLocal = glm::inverse(parentWorld) * newWorld;
				glm::vec3 rot;
				glm::vec3 trans, scale;
				ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(newLocal), glm::value_ptr(trans), glm::value_ptr(rot), glm::value_ptr(scale));

				tc.position = trans;
				tc.rotation = rot;
				tc.scale = scale;
				registry.patch<Engine::TransformComponent>(entity);
			}
		}

		if (!isUsing && m_GizmoPreviouslyUsed)
		{
			auto selection = SelectionManager::Entities.GetAll();

			std::vector<TransformData> before, after;

			for (auto& [entity, initialWorld] : m_InitialWorldTransforms)
			{
				auto& tc = registry.get<Engine::TransformComponent>(entity);
				{
					glm::mat4 parentWorld = glm::mat4(1.0f);
					if (tc.parent != entt::null)
					{
						parentWorld = registry.get<Engine::TransformComponent>(tc.parent).worldMatrix;
					}
					glm::mat4 initialLocal = glm::inverse(parentWorld) * initialWorld;
					glm::vec3 rot;
					glm::vec3 trans, scale;
					ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(initialLocal), glm::value_ptr(trans), glm::value_ptr(rot), glm::value_ptr(scale));
					before.push_back({ trans, rot, scale });
				}
			}

			std::vector<entt::entity> entities;
			entities.reserve(selection.size());

			for (Engine::Uuid id : selection)
			{
				entt::entity entity = scene.GetEntity(id);
				entities.push_back(entity);
				auto& tc = registry.get<Engine::TransformComponent>(entity);
				TransformData afterData;
				afterData.Position = tc.position;
				afterData.Rotation = tc.rotation;
				afterData.Scale = tc.scale;
				after.push_back(afterData);
			}

			EditorCommandManager::TransformEntities(entities, before, after);
		}

		m_GizmoPreviouslyUsed = isUsing;
	}

	bool TransformController::IsGizmoOver() const
	{
		return ImGuizmo::IsOver();
	}
}