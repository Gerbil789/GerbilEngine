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
#include "Engine/Event/EventBus.h"
#include "Editor/Core/EditorEvent.h"

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
			//Engine::EventBus::Get().Publish(FocusEntityEvent{ 0 });
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

		Engine::Uuid id = registry.get<Engine::IdentityComponent>(entity).id;
		bool selected = SelectionManager::Entities.IsSelected(id);

		if (selected)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		ImGui::PushID((uint32_t)entity);

		const std::string& name = registry.get<Engine::NameComponent>(entity).name;
		bool opened = ImGui::TreeNodeEx(name.c_str(), flags);

		// Handle selection
		if (ImGui::IsItemClicked())
		{
			bool additive = Engine::Input::IsKeyDown(Engine::Key::LeftControl) || Engine::Input::IsKeyDown(Engine::Key::LeftShift);
			SelectionManager::Entities.Select(id, additive);
		}

		// Drag source
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("ENTITY", &entity, sizeof(entt::entity));
			ImGui::Text("%s", name.c_str());
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

		if(ImGui::BeginPopupContextItem())
		{
			if(ImGui::MenuItem("Delete"))
			{
				EditorCommandManager::DeleteEntity(entity);
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