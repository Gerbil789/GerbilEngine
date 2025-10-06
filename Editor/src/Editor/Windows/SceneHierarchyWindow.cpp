#include "enginepch.h"
#include "SceneHierarchyWindow.h"
#include "Editor/Components/Components.h"
#include "Engine/Scene/Components.h"
#include "Editor/Components/ScopedStyle.h"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui_internal.h>
#include "Editor/Session/EditorSessionManager.h"
#include "Editor/Command/SceneCommands.h"
#include "Engine/Core/Input.h"

namespace Editor
{
	using namespace Engine;

	SceneHierarchyWindow::SceneHierarchyWindow()
	{
		SceneManager::RegisterOnSceneChanged([this](Scene* scene) {m_Scene = scene; });
	}

	void SceneHierarchyWindow::OnUpdate(Engine::Timestep ts)
	{
		ScopedStyle style({
			{ ImGuiStyleVar_WindowPadding, { 0, 0 } },
			{ ImGuiStyleVar_ItemSpacing, { 0, 0 } }
		});

		ImGui::Begin("Scene Hierarchy");

		ImGuiMultiSelectFlags selectFlags = ImGuiMultiSelectFlags_None;
		ImGui::BeginMultiSelect(selectFlags);

		if (!m_Scene)
		{
			ImGui::End();
			return;
		}

		auto session = EditorSessionManager::Get().GetSceneSession(); //TODO: store session

		const auto& entities = m_Scene->GetEntities();

		for(size_t i = 0; i < entities.size(); ++i)
		{
			Entity entity = entities[i];
			if(entity.GetComponent<TransformComponent>().Parent == entt::null)
			{
				DrawReorderDropTarget(Engine::Entity::Null(), i);
				DrawEntityNode(entity);
			}
		}
		DrawReorderDropTarget(Engine::Entity::Null(), entities.size());

		// Deselect on empty space click
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
		{
			session->ClearSelection();
		}

		// Right-click context menu
		if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				session->CreateEntity("Empty");
			}
			ImGui::EndPopup();
		}

		ImGui::EndMultiSelect();
		ImGui::End();
	}

	void SceneHierarchyWindow::DrawEntityNode(Engine::Entity entity)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DrawLinesToNodes | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

		// If entity has no children, mark as leaf
		if (entity.GetChildren().size() == 0)
		{
			flags |= ImGuiTreeNodeFlags_Leaf;
		}

		auto session = EditorSessionManager::Get().GetSceneSession(); // TODO: store session
		bool selected = session->IsEntitySelected(entity);

		if (selected)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		ImGui::PushID((uint32_t)entity);

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, entity.GetName().c_str());

		// Handle selection
		if (ImGui::IsItemClicked())
		{
			bool additive = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::LeftShift);
			session->SelectEntity(entity, additive);
		}

		// Drag source
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("ENTITY", &entity, sizeof(Engine::Entity));
			ImGui::Text("%s", entity.GetName().c_str());
			ImGui::EndDragDropSource();
		}

		// Drop target
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
			{
				Engine::Entity dropped = *(Engine::Entity*)payload->Data;
				dropped.SetParent(entity);
			}
			ImGui::EndDragDropTarget();
		}

		bool entityDeleted = false;
		if(ImGui::BeginPopupContextItem())
		{
			if(ImGui::MenuItem("Delete Entity"))
			{
				entityDeleted = true;
			}
			ImGui::EndPopup();
		}

		if (opened)
		{
			entt::entity child = entity.GetComponent<TransformComponent>().FirstChild;
			size_t i = 0;

			while (child != entt::null)
			{
				Engine::Entity childEntity = { child, m_Scene };
				DrawReorderDropTarget(entity, i);
				DrawEntityNode(childEntity);
				child = childEntity.GetComponent<TransformComponent>().NextSibling;
				i++;
			}
			DrawReorderDropTarget(entity, i);

			ImGui::TreePop();
		}

		if (entityDeleted) //TODO: is delayed deletion necessary?
		{
			entity.Destroy();
		}

		ImGui::PopID();
	}

	void SceneHierarchyWindow::DrawReorderDropTarget(Engine::Entity parent, size_t index)
	{
		ImGui::PushID((int)index);
		ImGui::Selectable("##DropTarget", false, ImGuiSelectableFlags_SpanAllColumns, ImVec2(0, 1));

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
			{
				entt::entity dropped = *(entt::entity*)payload->Data;
				Entity droppedEntity = { dropped, m_Scene };
				droppedEntity.SetParent(parent);
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::PopID();
	}

}