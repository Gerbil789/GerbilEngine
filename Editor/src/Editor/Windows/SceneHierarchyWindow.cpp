#include "SceneHierarchyWindow.h"
#include "Editor/Windows/Utility/Property.h"
#include "Engine/Scene/Components.h"
#include "Editor/Windows/Utility/ScopedStyle.h"
#include "Engine/Core/Input.h"
#include "Editor/Core/EditorContext.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Editor/Command/Entity/CreateEntity.h"
#include "Editor/Command/Entity/DeleteEntity.h"
#include "Editor/Command/Entity/RenameEntity.h"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui_internal.h>

namespace Editor
{
	using namespace Engine;

	SceneHierarchyWindow::SceneHierarchyWindow()
	{
		SceneManager::RegisterOnSceneChanged([this](Scene* scene) {m_Scene = scene; });
	}

	void SceneHierarchyWindow::Draw()
	{
		ScopedStyle style({
			{ ImGuiStyleVar_WindowPadding, { 0, 0 } },
			{ ImGuiStyleVar_ItemSpacing, { 0, 0 } }
		});

		ImGui::Begin("Scene Hierarchy");

		if (!m_Scene)
		{
			ImGui::End();
			return;
		}

		const auto& entities = m_Scene->GetEntities(true);

		for(size_t i = 0; i < entities.size(); ++i)
		{
			Entity entity = entities[i];
			if(entity.GetComponent<TransformComponent>().parent == entt::null)
			{
				DrawReorderDropTarget(Engine::Entity::Null(), i);
				DrawEntityNode(entity);
			}
		}
		DrawReorderDropTarget(Engine::Entity::Null(), entities.size());

		// deselect on empty space click
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
		{
			EditorContext::Entities().Clear();
		}

		// right-click context menu
		if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				EditorCommandManager::ExecuteCommand<CreateEntityCommand>("Empty");
			}
			ImGui::EndPopup();
		}

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

		bool selected = EditorContext::Entities().IsSelected(entity);

		if (selected)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		ImGui::PushID((uint32_t)entity);

		bool opened = ImGui::TreeNodeEx(entity.GetName().c_str(), flags);

		// Handle selection
		if (ImGui::IsItemClicked())
		{
			bool additive = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::LeftShift);
			EditorContext::Select(entity, additive);
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
			if(ImGui::MenuItem("Delete"))
			{
				entityDeleted = true;
			}

			if (ImGui::MenuItem("Rename"))
			{
				EditorCommandManager::ExecuteCommand<RenameEntityCommand>(entity, "BRUH");
			}
			ImGui::EndPopup();
		}

		if (opened)
		{
			entt::entity child = entity.GetComponent<TransformComponent>().firstChild;
			size_t i = 0;

			while (child != entt::null)
			{
				Engine::Entity childEntity{ child, &m_Scene->Registry() };
				DrawReorderDropTarget(entity, i);
				DrawEntityNode(childEntity);
				child = childEntity.GetComponent<TransformComponent>().nextSibling;
				i++;
			}
			DrawReorderDropTarget(entity, i);

			ImGui::TreePop();
		}

		if (entityDeleted) //TODO: is delayed deletion necessary?
		{
			EditorCommandManager::ExecuteCommand<DeleteEntityCommand>(entity);
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
				Entity droppedEntity{ dropped, &m_Scene->Registry() };
				droppedEntity.SetParent(parent);
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::PopID();
	}

}