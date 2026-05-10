#include "SceneHierarchyWindow.h"
#include "Editor/Windows/Utility/Property.h"
#include "Engine/Scene/Components.h"
#include "Editor/Windows/Utility/ScopedStyle.h"
#include "Engine/Core/Input.h"
#include "Editor/Core/SelectionManager.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Engine/Scene/SceneManager.h"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include "Engine/Core/KeyCodes.h"
#include <imgui_internal.h>

namespace Editor
{
	void SceneHierarchyWindow::Draw()
	{
		ScopedStyle style
		{
			{ ImGuiStyleVar_WindowPadding, { 0, 0 } },
			{ ImGuiStyleVar_ItemSpacing, { 0, 0 } }
		};

		ImGui::Begin("Scene Hierarchy");

		Engine::Scene& scene = Engine::SceneManager::GetActiveScene();
		entt::registry& registry = scene.GetRegistry();

		auto view = registry.view<Engine::TransformComponent>();

		int i = 0;
		for(auto [entity, transform] : view.each())
		{
			if(transform.parent != entt::null) continue;

			DrawReorderDropTarget(registry, entt::null, i);
			DrawEntityNode(registry, entity);
			i++;
		}

		DrawReorderDropTarget(registry, entt::null, i);

		// deselect on empty space click
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
		{
			SelectionManager::Clear(SelectionType::Entity);
		}

		// right-click context menu
		if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				EditorCommandManager::CreateEntity("Empty");
			}
			ImGui::EndPopup();
		}

		ImGui::End();
	}

	void SceneHierarchyWindow::DrawEntityNode(entt::registry& registry, entt::entity entity)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DrawLinesToNodes | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

		//auto& tc = registry.get<Engine::TransformComponent>(entity);

		// If entity has no children, mark as leaf
		//if (tc.firstChild == entt::null)
		//{
		//	flags |= ImGuiTreeNodeFlags_Leaf;
		//}

		auto& ic = registry.get<Engine::IdentityComponent>(entity);
		bool selected = SelectionManager::IsSelected(SelectionType::Entity, ic.id);

		if (selected)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		ImGui::PushID((uint32_t)entity);

		auto& nc = registry.get<Engine::NameComponent>(entity);
		bool opened = ImGui::TreeNodeEx(nc.name.c_str(), flags);

		// Handle selection
		if (ImGui::IsItemClicked())
		{
			bool additive = Engine::Input::IsKeyDown(Engine::KeyCode::LeftControl) || Engine::Input::IsKeyDown(Engine::KeyCode::LeftShift);
			SelectionManager::Select(SelectionType::Entity, ic.id, additive);
		}

		// Drag source
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("ENTITY", &entity, sizeof(entt::entity));
			ImGui::Text("%s", nc.name.c_str());
			ImGui::EndDragDropSource();
		}

		// Drop target
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
			{
				entt::entity dropped = *(entt::entity*)payload->Data;
				auto& childTC = registry.get<Engine::TransformComponent>(dropped);
				childTC.parent = entity;
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
			ImGui::EndPopup();
		}

		if(opened)
		{
			auto view = registry.view<Engine::TransformComponent>();

			int i = 0;
			for(auto[childEntity, transform] : view.each())
			{
				if(transform.parent == entity)
				{
					DrawReorderDropTarget(registry, entity, i);
					DrawEntityNode(registry, childEntity);
					i++;
				}
			}
			DrawReorderDropTarget(registry, entity, i);

			ImGui::TreePop();
		}

		if(entityDeleted) //TODO: is delayed deletion necessary?
		{
			EditorCommandManager::DeleteEntity(entity); // <---- its already deferred in here 
		}

		ImGui::PopID();
	}

	void SceneHierarchyWindow::DrawReorderDropTarget(entt::registry& registry, entt::entity parent, size_t index)
	{
		ImGui::PushID((int)index);
		ImGui::Selectable("##DropTarget", false, ImGuiSelectableFlags_SpanAllColumns, ImVec2(0, 1));

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
			{
				entt::entity dropped = *(entt::entity*)payload->Data;
				auto& childTC = registry.get<Engine::TransformComponent>(dropped);
				childTC.parent = parent;

			}
			ImGui::EndDragDropTarget();
		}
		ImGui::PopID();
	}

}