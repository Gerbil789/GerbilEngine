#include "Editor/Windows/Viewport/TransformController.h"
#include "Editor/Core/EditorContext.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/SceneManager.h"
#include "Engine/Core/Components.h"
#include <imgui.h>
#include <ImGuizmo.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace editor
{
	namespace
	{
		bool m_gizmoPreviouslyUsed = false;

		std::unordered_map<entt::entity, glm::mat4> m_initialWorldTransforms;
		glm::mat4 m_initialPrimaryWorld = glm::mat4(1.0f);

		ImGuizmo::OPERATION gizmoType = ImGuizmo::OPERATION::TRANSLATE;
	}

	void TransformController::Initialize()
	{
		ImGuizmo::AllowAxisFlip(true);
		ImGuizmo::SetGizmoSizeClipSpace(0.15f);
		ImGuizmo::SetOrthographic(false);
	}

	void TransformController::DrawGizmo(float x, float y, float width, float height)
	{
		if (editor::context::editorMode == EditorMode::Play) return;

		if(engine::input::IsKeyReleased(engine::Key::Q)) gizmoType = static_cast<ImGuizmo::OPERATION>(0);
		if(engine::input::IsKeyReleased(engine::Key::W)) gizmoType = ImGuizmo::OPERATION::TRANSLATE;
		if(engine::input::IsKeyReleased(engine::Key::E)) gizmoType = ImGuizmo::OPERATION::ROTATE;
		if(engine::input::IsKeyReleased(engine::Key::R)) gizmoType = ImGuizmo::OPERATION::SCALE;

		if (gizmoType == 0) return;
		if(editor::context::selectedEntity == entt::null) return;

		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(x, y, width, height);


		glm::mat4 cameraProjection = editor::context::camera.projectionMatrix;
		glm::mat4 cameraView = editor::context::camera.viewMatrix;

		engine::Scene& scene = engine::SceneManager::GetActiveScene().Get();

		engine::component::WorldTransform& wtc = scene.registry.get<engine::component::WorldTransform>(editor::context::selectedEntity);
		glm::mat4& worldTransform = wtc.worldMatrix;

		glm::vec3 snap;
		float* snapPtr = nullptr;

		if (engine::input::IsKeyDown(engine::Key::LeftControl))
		{
			snap = (gizmoType == ImGuizmo::OPERATION::ROTATE) ? glm::vec3{ 45.0f, 45.0f, 45.0f } : glm::vec3{ 0.5f, 0.5f, 0.5f };
			snapPtr = glm::value_ptr(snap);
		}

		ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), gizmoType, ImGuizmo::MODE::LOCAL, glm::value_ptr(worldTransform), nullptr, snapPtr);

		bool isUsing = ImGuizmo::IsUsing();

		if (isUsing && !m_gizmoPreviouslyUsed)
		{
			m_initialWorldTransforms.clear();

			// const std::vector<engine::Uuid>& selection = SelectionManager::Entities.GetAll();

			// for (engine::Uuid id : selection)
			// {
			// 	engine::Entity entity = scene.GetEntity(id);
			// 	auto& wc = entity.GetComponent<engine::WorldTransformComponent>();
			// 	m_initialWorldTransforms[static_cast<entt::entity>(entity.GetHandle())] = wc.worldMatrix;
			// }

			m_initialPrimaryWorld = m_initialWorldTransforms[editor::context::selectedEntity];
		}

		if (isUsing)
		{
			glm::mat4 newPrimaryWorld = worldTransform;
			//glm::mat4 delta = newPrimaryWorld * glm::inverse(m_initialPrimaryWorld);

			// for (engine::Uuid id : SelectionManager::Entities.GetAll())
			// {
			// 	engine::Entity entity = scene.GetEntity(id);

			// 	glm::mat4 originalWorld = m_initialWorldTransforms[static_cast<entt::entity>(entity.GetHandle())];
			// 	glm::mat4 newWorld = delta * originalWorld;

			// 	glm::mat4 parentWorld = glm::mat4{ 1.0f };

			// 	if (entity.HasComponent<engine::HierarchyComponent>())
			// 	{
			// 		auto& hc = entity.GetComponent<engine::HierarchyComponent>();

			// 		if (hc.parent != entt::null)
			// 		{
			// 			engine::Entity parentEntity = engine::Entity(hc.parent, &scene);
			// 			const auto& parentWTC = parentEntity.GetComponent<engine::WorldTransformComponent>();
			// 			parentWorld = parentWTC.worldMatrix;
			// 		}
			// 	}

			// 	glm::mat4 newLocal = glm::inverse(parentWorld) * newWorld;
			// 	glm::vec3 trans, rot, scale;
			// 	ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(newLocal), glm::value_ptr(trans), glm::value_ptr(rot), glm::value_ptr(scale));

			// 	auto& tc = entity.GetComponent<engine::TransformComponent>();
			// 	tc.position = trans;
			// 	tc.rotation = glm::radians(rot);
			// 	tc.scale = scale;
			// 	entity.AddTag<engine::TransformDirty>();
			// }
		}

		if (!isUsing && m_gizmoPreviouslyUsed)
		{
			// auto selection = SelectionManager::Entities.GetAll();

			std::vector<engine::component::Transform> before, after;

			for (auto& [entity, initialWorld] : m_initialWorldTransforms)
			{
				auto& hc = scene.registry.get<engine::component::Hierarchy>(static_cast<entt::entity>(entity));
				{
					glm::mat4 parentWorld = glm::mat4{ 1.0f };
					if (hc.parent != entt::null)
					{
						const auto& parentWTC = scene.registry.get<engine::component::WorldTransform>(hc.parent);
						parentWorld = parentWTC.worldMatrix;
					}
					glm::mat4 initialLocal = glm::inverse(parentWorld) * initialWorld;
					glm::vec3 trans, rot, scale;
					ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(initialLocal), glm::value_ptr(trans), glm::value_ptr(rot), glm::value_ptr(scale));
					before.push_back({ trans, glm::radians(rot), scale });
				}
			}

			std::vector<entt::entity> entities;
			// entities.reserve(selection.size());

			// for (engine::Uuid id : selection)
			// {
			// 	engine::Entity entity = scene.GetEntity(id);
			// 	entities.push_back(entity);
			// 	engine::TransformComponent afterData = entity.GetComponent<engine::TransformComponent>();
			// 	after.push_back(afterData);
			// }

			EditorCommandManager::ModifyComponents<engine::component::Transform>(entities, before, after);
		}

		m_gizmoPreviouslyUsed = isUsing;
	}

	bool TransformController::IsGizmoOver() const
	{
		return ImGuizmo::IsOver();
	}
}