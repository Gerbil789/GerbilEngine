#include "SceneHierarchyPanel.h"
#include "Engine/Scene/Components.h"
#include <imgui/imgui.h>


namespace Engine
{
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");


		if (m_Context)
		{
			m_Context->m_Registry.view<TagComponent>().each([&](auto entityId, auto tc)
			{
				Entity entity{ entityId, m_Context.get() };
				DrawEntityNode(entity);
			});
		}

		

		ImGui::End();
	}
	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0);
		bool expanded =  ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

		if(ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		if (expanded)
		{
			ImGui::TreePop();
		}
	}
}