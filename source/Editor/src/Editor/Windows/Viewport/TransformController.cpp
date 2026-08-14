#include "Editor/Windows/Viewport/TransformController.h"
#include "Editor/Core/EditorContext.h"
#include "Editor/Core/SelectionManager.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Engine/Core/Input.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Components.h"
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
				if (Editor::editorContext.editorMode == EditorMode::Play) return false;;

				if (e.key == Engine::Key::Q) gizmoType = static_cast<ImGuizmo::OPERATION>(0);
				if (e.key == Engine::Key::W) gizmoType = ImGuizmo::OPERATION::TRANSLATE;
				if (e.key == Engine::Key::E) gizmoType = ImGuizmo::OPERATION::ROTATE;
				if (e.key == Engine::Key::R) gizmoType = ImGuizmo::OPERATION::SCALE;

				return false;
			});
	}

	void TransformController::DrawGizmo(Engine::Scene& scene, float x, float y, float width, float height)
	{
		if (Editor::editorContext.editorMode == EditorMode::Play) return;
		if (gizmoType == 0) return;

		Engine::Uuid selectedId = SelectionManager::Entities.GetPrimary();
		if (!selectedId) return;

		entt::registry& registry = scene.GetRegistry();

		Engine::Entity selectedEntity = scene.GetEntity(selectedId);

		if (!selectedEntity.HasComponent<Engine::WorldTransformComponent>()) return;

		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(x, y, width, height);

		entt::entity cameraEntity = scene.GetActiveCamera();

		auto& cc = registry.get<Engine::CameraComponent>(cameraEntity);

		const glm::mat4& cameraProjection = cc.projectionMatrix;
		glm::mat4 cameraView = cc.viewMatrix;

		auto& wtc = selectedEntity.GetComponent<Engine::WorldTransformComponent>();
		glm::mat4 worldTransform = wtc.worldMatrix;

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
				Engine::Entity entity = scene.GetEntity(id);
				auto& wc = entity.GetComponent<Engine::WorldTransformComponent>();
				m_InitialWorldTransforms[static_cast<entt::entity>(entity.GetHandle())] = wc.worldMatrix;
			}

			m_InitialPrimaryWorld = m_InitialWorldTransforms[static_cast<entt::entity>(selectedEntity.GetHandle())];
		}

		if (isUsing)
		{
			glm::mat4 newPrimaryWorld = worldTransform;
			glm::mat4 delta = newPrimaryWorld * glm::inverse(m_InitialPrimaryWorld);

			for (Engine::Uuid id : SelectionManager::Entities.GetAll())
			{
				Engine::Entity entity = scene.GetEntity(id);

				glm::mat4 originalWorld = m_InitialWorldTransforms[static_cast<entt::entity>(entity.GetHandle())];
				glm::mat4 newWorld = delta * originalWorld;

				glm::mat4 parentWorld = glm::mat4{ 1.0f };

				if (entity.HasComponent<Engine::HierarchyComponent>())
				{
					auto& hc = entity.GetComponent<Engine::HierarchyComponent>();

					if (hc.parent != entt::null)
					{
						Engine::Entity parentEntity = Engine::Entity(hc.parent, &scene);
						const auto& parentWTC = parentEntity.GetComponent<Engine::WorldTransformComponent>();
						parentWorld = parentWTC.worldMatrix;
					}
				}

				glm::mat4 newLocal = glm::inverse(parentWorld) * newWorld;
				glm::vec3 rot;
				glm::vec3 trans, scale;
				ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(newLocal), glm::value_ptr(trans), glm::value_ptr(rot), glm::value_ptr(scale));

				auto& tc = entity.GetComponent<Engine::TransformComponent>();
				tc.position = trans;
				tc.rotation = rot;
				tc.scale = scale;
				entity.AddTag<Engine::TransformDirty>();
			}
		}

		if (!isUsing && m_GizmoPreviouslyUsed)
		{
			auto selection = SelectionManager::Entities.GetAll();

			std::vector<Engine::TransformComponent> before, after;

			for (auto& [entity, initialWorld] : m_InitialWorldTransforms)
			{
				auto& hc = registry.get<Engine::HierarchyComponent>(static_cast<entt::entity>(entity));
				{
					glm::mat4 parentWorld = glm::mat4{ 1.0f };
					if (hc.parent != entt::null)
					{
						Engine::Entity parentEntity = Engine::Entity(hc.parent, &scene);
						const auto& parentWTC = parentEntity.GetComponent<Engine::WorldTransformComponent>();
						parentWorld = parentWTC.worldMatrix;
					}
					glm::mat4 initialLocal = glm::inverse(parentWorld) * initialWorld;
					glm::vec3 rot;
					glm::vec3 trans, scale;
					ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(initialLocal), glm::value_ptr(trans), glm::value_ptr(rot), glm::value_ptr(scale));
					before.push_back({ trans, rot, scale });
				}
			}

			std::vector<Engine::Entity> entities;
			entities.reserve(selection.size());

			for (Engine::Uuid id : selection)
			{
				Engine::Entity entity = scene.GetEntity(id);
				entities.push_back(entity);
				Engine::TransformComponent afterData = entity.GetComponent<Engine::TransformComponent>();
				after.push_back(afterData);
			}

			EditorCommandManager::ModifyComponents<Engine::TransformComponent>(entities, before, after);
		}

		m_GizmoPreviouslyUsed = isUsing;
	}

	bool TransformController::IsGizmoOver() const
	{
		return ImGuizmo::IsOver();
	}
}