#include "enginepch.h"
#include "SceneHierarchyWindow.h"
#include "Editor/Elements/Elements.h"
#include "Engine/Scene/Components.h"
#include "Editor/Elements/Style.h"
#include "Editor/Core/EditorSceneController.h"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui_internal.h>
#include <filesystem>

namespace Editor
{
	using namespace Engine;

	void SceneHierarchyWindow::OnUpdate(Engine::Timestep ts)
	{
		ScopedStyle style({
			{ ImGuiStyleVar_WindowPadding, { 0, 0 } },
			{ ImGuiStyleVar_ItemSpacing, { 0, 0 } }
		});


		ImGui::Begin("Scene Hierarchy");

		m_Scene = SceneManager::GetActiveScene();
		if (!m_Scene)
		{
			ImGui::End();
			return;
		}

		auto& registry = m_Scene->m_Registry;
		auto& roots = m_Scene->GetRootEntities();

		for (size_t i = 0; i < roots.size(); ++i)
		{
			entt::entity root = roots[i];

			DrawReorderDropTarget(entt::null, i);
			DrawEntityNode(root);
		}
		DrawReorderDropTarget(entt::null, roots.size());

		// Handle mouse click to deselect entity
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		{
			EditorSceneController::DeselectEntity();
		}

		if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				Entity entity = m_Scene->CreateEntity("Empty Entity");
				EditorSceneController::SelectEntity(entity);
			}
			ImGui::EndPopup();
		}

		ImGui::End();
	}

	void SceneHierarchyWindow::DrawEntityNode(entt::entity entity)
	{
		auto& registry = m_Scene->m_Registry;
		Entity wrapper(entity, &registry);

		// Save the current style
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec2 oldPadding = style.FramePadding;
		ImVec2 oldItemSpacing = style.ItemSpacing;

		// Set padding and spacing to zero
		style.FramePadding = ImVec2(0, 0);
		style.ItemSpacing = ImVec2(0, 0);

		auto& name = wrapper.GetName();
		bool hasChildren = wrapper.HasChildren();

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

		// If entity has no children, mark as leaf
		if (!hasChildren)
			flags |= ImGuiTreeNodeFlags_Leaf;

		ImGui::PushID((uint32_t)wrapper);
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)wrapper, flags, name.c_str());


		style.FramePadding = oldPadding;
		style.ItemSpacing = oldItemSpacing;



		// Handle selection
		if (ImGui::IsItemClicked())
		{
			EditorSceneController::SelectEntity(wrapper);
		}

		// Drag source
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("ENTITY", &entity, sizeof(entt::entity));
			ImGui::Text("%s", wrapper.GetName().c_str());
			ImGui::EndDragDropSource();
		}

		// Drop target
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
			{
				entt::entity dropped = *(entt::entity*)payload->Data;
				if (dropped != entity)
				{
					m_Scene->SetParent(dropped, entity);
					m_Scene->RemoveRootEntity(dropped); // If dropped entity was root
				}
			}
			ImGui::EndDragDropTarget();
		}

		/*	bool entityDeleted = false;
			if(ImGui::BeginPopupContextItem())
			{
				if(ImGui::MenuItem("Delete Entity"))
				{
					entityDeleted = true;
				}
				ImGui::EndPopup();
			}*/

		if (opened)
		{
			entt::entity child = registry.get<HierarchyComponent>(entity).FirstChild;
			size_t i = 0;

			while (child != entt::null)
			{
				DrawReorderDropTarget(entity, i);
				DrawEntityNode(child);
				child = registry.get<HierarchyComponent>(child).NextSibling;
				i++;
			}
			//DrawReorderDropTarget(entity, i);

			ImGui::TreePop();
		}

		/*	if(entityDeleted)
			{
				m_Scene->DestroyEntity(entity);
				if(m_SceneController->IsEntitySelected(entity))
				{
					m_SceneController->DeselectEntity();
				}
			}*/

		ImGui::PopID();
	}
	void SceneHierarchyWindow::DrawReorderDropTarget(entt::entity parent, size_t index)
	{
		ImGui::PushID((int)index);
		ImGui::Selectable("##DropTarget", false, ImGuiSelectableFlags_SpanAllColumns, ImVec2(0, 1));

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
			{
				entt::entity dropped = *(entt::entity*)payload->Data;

				m_Scene->SetParent(dropped, parent);

				if (parent == entt::null)
				{
					m_Scene->RemoveRootEntity(dropped);
					m_Scene->AddRootEntity(dropped);
					m_Scene->ReorderRootEntity(dropped, index);
				}

			}
			ImGui::EndDragDropTarget();
		}
		ImGui::PopID();
	}

}