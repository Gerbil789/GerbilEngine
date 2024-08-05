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
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_Context->DeselectEntity();
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");
	
		if (m_Context)
		{
			m_Context->m_Registry.view<NameComponent>().each([&](auto entityId, auto tc)
			{
				Entity entity{ entityId, m_Context.get() };
				DrawEntityNode(entity);
			});
		}

		if(ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		{
			m_Context->DeselectEntity();
		}


		if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				Entity entity = m_Context->CreateEntity("Empty Entity");
				m_Context->SelectEntity(entity);
			}

			ImGui::EndPopup();
		}
	
		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& name = entity.GetComponent<NameComponent>().Name;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ((m_Context->IsEntitySelected(entity)) ? ImGuiTreeNodeFlags_Selected : 0);
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool expanded =  ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, name.c_str());

		if(ImGui::IsItemClicked())
		{
			m_Context->SelectEntity(entity);
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
			m_Context->DestroyEntity(entity);
			if(m_Context->IsEntitySelected(entity))
			{
				m_Context->DeselectEntity();
			}
		}
	}
}