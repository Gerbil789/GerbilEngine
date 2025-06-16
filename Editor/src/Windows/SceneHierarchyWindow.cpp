#include "enginepch.h"
#include "../Elements/Elements.h"
#include "SceneHierarchyWindow.h"
#include "Engine/Scene/Components.h"
#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui_internal.h>
#include <filesystem>

namespace Engine
{
	SceneHierarchyWindow::SceneHierarchyWindow(EditorContext* context) : EditorWindow(context)
	{
		SceneManager::RegisterObserver(this);
	}

	SceneHierarchyWindow::~SceneHierarchyWindow()
	{
		SceneManager::UnregisterObserver(this);
	}

	void SceneHierarchyWindow::OnSceneChanged(Ref<Scene> newScene)
	{
		m_Scene = newScene;
	}

	void SceneHierarchyWindow::OnImGuiRender()
	{
		// Save the current style
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec2 oldWindowPadding = style.WindowPadding;
		float oldIndentSpacing = style.IndentSpacing;

		// Set window padding to zero
		style.WindowPadding = ImVec2(0, 0);

		ImGui::Begin("Scene Hierarchy");
		if (!m_Scene) 
		{ 
			ImGui::End(); 
			return; 
		}

		for(auto& entity : m_Scene->GetEntitiesOrdered())
		{
			DrawEntityNode(entity);
		}

		if(ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		{
			m_Scene->DeselectEntity();
		}


		if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				Entity entity = m_Scene->CreateEntity("Empty Entity");
				m_Scene->SelectEntity(entity);
			}

			ImGui::EndPopup();
		}
	
		ImGui::End();

		style.WindowPadding = oldWindowPadding;
		style.IndentSpacing = oldIndentSpacing;
	}

	void SceneHierarchyWindow::DrawEntityNode(Entity entity)
	{
		auto& name = entity.GetComponent<NameComponent>().Name;

		// Save the current style
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec2 oldPadding = style.FramePadding;
		ImVec2 oldItemSpacing = style.ItemSpacing;

		// Set padding and spacing to zero
		style.FramePadding = ImVec2(0, 0);
		style.ItemSpacing = ImVec2(0, 0);

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ((m_Scene->IsEntitySelected(entity)) ? ImGuiTreeNodeFlags_Selected : 0);
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool expanded =  ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, name.c_str());


		style.FramePadding = oldPadding;
		style.ItemSpacing = oldItemSpacing;

		if(ImGui::IsItemClicked())
		{
			m_Scene->SelectEntity(entity);
		}

		// Handle dragging and dropping
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			ImGui::SetDragDropPayload("DND_ENTITY", &entity, sizeof(Entity));
			ImGui::Text("%s", name.c_str());
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_ENTITY"))
			{
				Entity* sourceEntity = (Entity*)payload->Data;
				m_Scene->ReorderEntity(*sourceEntity, entity);
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

		if (expanded)
		{
			ImGui::TreePop();
		}

		if(entityDeleted)
		{
			m_Scene->DestroyEntity(entity);
			if(m_Scene->IsEntitySelected(entity))
			{
				m_Scene->DeselectEntity();
			}
		}
	}
}