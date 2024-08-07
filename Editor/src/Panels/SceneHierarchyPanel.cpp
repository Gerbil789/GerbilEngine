#include "enginepch.h"
#include "../Elements/Elements.h"
#include "SceneHierarchyPanel.h"
#include "Engine/Scene/Components.h"
#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui_internal.h>
#include <filesystem>

namespace Engine
{
	SceneHierarchyPanel::SceneHierarchyPanel()
	{
		SceneManager::AddObserver(this);
	}

	SceneHierarchyPanel::~SceneHierarchyPanel()
	{
		SceneManager::RemoveObserver(this);
	}

	void SceneHierarchyPanel::OnSceneChanged()
	{
		m_Scene = SceneManager::GetCurrentScene();
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");
	
		if (m_Scene)
		{
			m_Scene->m_Registry.view<NameComponent>().each([&](auto entityId, auto tc)
			{
				Entity entity{ entityId, m_Scene.get() };
				DrawEntityNode(entity);
			});
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
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& name = entity.GetComponent<NameComponent>().Name;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ((m_Scene->IsEntitySelected(entity)) ? ImGuiTreeNodeFlags_Selected : 0);
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool expanded =  ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, name.c_str());

		if(ImGui::IsItemClicked())
		{
			m_Scene->SelectEntity(entity);
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