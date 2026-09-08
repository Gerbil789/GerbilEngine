#include "Editor/Windows/Viewport/TransformController.h"
#include "Editor/Core/EditorContext.h"
#include "Editor/Core/SelectionManager.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/Scene.h"
#include "Engine/Core/Components.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Event/KeyEvent.h"
#include <imgui.h>
#include <ImGuizmo.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace editor
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
		ImGuizmo::SetOrthographic(false);

		engine::EventBus::Subscribe<engine::KeyPressedEvent>([](const engine::KeyPressedEvent& e)
			{
				if (editor::editorContext.editorMode == EditorMode::Play) return false;;

				if (e.key == engine::Key::Q) gizmoType = static_cast<ImGuizmo::OPERATION>(0);
				if (e.key == engine::Key::W) gizmoType = ImGuizmo::OPERATION::TRANSLATE;
				if (e.key == engine::Key::E) gizmoType = ImGuizmo::OPERATION::ROTATE;
				if (e.key == engine::Key::R) gizmoType = ImGuizmo::OPERATION::SCALE;

				return false;
			});
	}

	void TransformController::DrawGizmo(engine::SceneAsset& scene, float x, float y, float width, float height)
	{
		if (editor::editorContext.editorMode == EditorMode::Play) return;
		if (gizmoType == 0) return;

		engine::Uuid selectedId = SelectionManager::Entities.GetPrimary();
		if (!selectedId) return;

		entt::registry& registry = scene.GetRegistry();

		entt::entity selectedEntity = scene.GetEntityHandle(selectedId);

		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(x, y, width, height);

		auto& cc = editor::editorContext.camera;

		glm::mat4 cameraProjection = cc.projectionMatrix;
		glm::mat4 cameraView = cc.viewMatrix;

		engine::WorldTransformComponent& wtc = registry.get<engine::WorldTransformComponent>(selectedEntity);
		glm::mat4& worldTransform = wtc.worldMatrix;

		glm::vec3 snap;
		float* snapPtr = nullptr;

		if (engine::Input::IsKeyDown(engine::Key::LeftControl))
		{
			snap = (gizmoType == ImGuizmo::OPERATION::ROTATE) ? glm::vec3{ 45.0f, 45.0f, 45.0f } : glm::vec3{ 0.5f, 0.5f, 0.5f };
			snapPtr = glm::value_ptr(snap);
		}

		ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), gizmoType, ImGuizmo::MODE::LOCAL, glm::value_ptr(worldTransform), nullptr, snapPtr);

		bool isUsing = ImGuizmo::IsUsing();

		if (isUsing && !m_GizmoPreviouslyUsed)
		{
			m_InitialWorldTransforms.clear();

			const std::vector<engine::Uuid>& selection = SelectionManager::Entities.GetAll();

			for (engine::Uuid id : selection)
			{
				engine::Entity entity = scene.GetEntity(id);
				auto& wc = entity.GetComponent<engine::WorldTransformComponent>();
				m_InitialWorldTransforms[static_cast<entt::entity>(entity.GetHandle())] = wc.worldMatrix;
			}

			m_InitialPrimaryWorld = m_InitialWorldTransforms[static_cast<entt::entity>(selectedEntity)];
		}

		if (isUsing)
		{
			glm::mat4 newPrimaryWorld = worldTransform;
			glm::mat4 delta = newPrimaryWorld * glm::inverse(m_InitialPrimaryWorld);

			for (engine::Uuid id : SelectionManager::Entities.GetAll())
			{
				engine::Entity entity = scene.GetEntity(id);

				glm::mat4 originalWorld = m_InitialWorldTransforms[static_cast<entt::entity>(entity.GetHandle())];
				glm::mat4 newWorld = delta * originalWorld;

				glm::mat4 parentWorld = glm::mat4{ 1.0f };

				if (entity.HasComponent<engine::HierarchyComponent>())
				{
					auto& hc = entity.GetComponent<engine::HierarchyComponent>();

					if (hc.parent != entt::null)
					{
						engine::Entity parentEntity = engine::Entity(hc.parent, &scene);
						const auto& parentWTC = parentEntity.GetComponent<engine::WorldTransformComponent>();
						parentWorld = parentWTC.worldMatrix;
					}
				}

				glm::mat4 newLocal = glm::inverse(parentWorld) * newWorld;
				glm::vec3 trans, rot, scale;
				ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(newLocal), glm::value_ptr(trans), glm::value_ptr(rot), glm::value_ptr(scale));

				auto& tc = entity.GetComponent<engine::TransformComponent>();
				tc.position = trans;
				tc.rotation = glm::radians(rot);
				tc.scale = scale;
				entity.AddTag<engine::TransformDirty>();
			}
		}

		if (!isUsing && m_GizmoPreviouslyUsed)
		{
			auto selection = SelectionManager::Entities.GetAll();

			std::vector<engine::TransformComponent> before, after;

			for (auto& [entity, initialWorld] : m_InitialWorldTransforms)
			{
				auto& hc = registry.get<engine::HierarchyComponent>(static_cast<entt::entity>(entity));
				{
					glm::mat4 parentWorld = glm::mat4{ 1.0f };
					if (hc.parent != entt::null)
					{
						engine::Entity parentEntity = engine::Entity(hc.parent, &scene);
						const auto& parentWTC = parentEntity.GetComponent<engine::WorldTransformComponent>();
						parentWorld = parentWTC.worldMatrix;
					}
					glm::mat4 initialLocal = glm::inverse(parentWorld) * initialWorld;
					glm::vec3 trans, rot, scale;
					ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(initialLocal), glm::value_ptr(trans), glm::value_ptr(rot), glm::value_ptr(scale));
					before.push_back({ trans, glm::radians(rot), scale });
				}
			}

			std::vector<engine::Entity> entities;
			entities.reserve(selection.size());

			for (engine::Uuid id : selection)
			{
				engine::Entity entity = scene.GetEntity(id);
				entities.push_back(entity);
				engine::TransformComponent afterData = entity.GetComponent<engine::TransformComponent>();
				after.push_back(afterData);
			}

			EditorCommandManager::ModifyComponents<engine::TransformComponent>(entities, before, after);
		}

		m_GizmoPreviouslyUsed = isUsing;
	}

	bool TransformController::IsGizmoOver() const
	{
		return ImGuizmo::IsOver();
	}
}