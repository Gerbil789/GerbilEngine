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
#include "Engine/Scene/Scene.h"

namespace Editor
{
	void DrawEntityNode(Engine::Entity entity)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DrawLinesToNodes | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

		Engine::Uuid id = entity.GetComponent<Engine::IdentityComponent>().id;
		bool selected = SelectionManager::Entities.IsSelected(id);

		if (selected)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		ImGui::PushID(entity.GetHandle());

		const std::string& name = entity.GetComponent<Engine::NameComponent>().name;
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
			ImGui::SetDragDropPayload("ENTITY", &entity, sizeof(Engine::Entity));
			ImGui::Text("%s", name.c_str());
			ImGui::EndDragDropSource();
		}

		// Drop target
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
			{
				Engine::Entity dropped = *(Engine::Entity*)payload->Data;
				auto& childTC = dropped.GetComponent<Engine::TransformComponent>();
				childTC.parent = entity;
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete"))
			{
				EditorCommandManager::DeleteEntity(entity);
			}
			ImGui::EndPopup();
		}

		if (opened)
		{
		/*	auto view = registry.view<Engine::TransformComponent>();

			int i = 0;
			for (auto [childEntity, transform] : view.each())
			{
				if (transform.parent == entity)
				{
					DrawReorderDropTarget(entity, i);
					DrawEntityNode(childEntity);
					i++;
				}
			}
			DrawReorderDropTarget(entity, i);*/

			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	void DrawReorderDropTarget(Engine::Entity parent, size_t index)
	{
		ImGui::PushID((int)index);
		ImGui::Selectable("##DropTarget", false, ImGuiSelectableFlags_SpanAllColumns, ImVec2(0, 1));

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
			{
				Engine::Entity dropped = *(Engine::Entity*)payload->Data;
				auto& childTC = dropped.GetComponent<Engine::TransformComponent>();
				childTC.parent = parent;
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::PopID();
	}


	void SceneHierarchyWindow::Draw()
	{
		ScopedStyle style
		{
			{ ImGuiStyleVar_WindowPadding, { 0, 0 } },
			{ ImGuiStyleVar_ItemSpacing, { 0, 0 } }
		};

		ImGui::Begin("Scene Hierarchy");

		Engine::Scene& scene = Engine::AssetManager::GetAsset<Engine::Scene>(Engine::SceneManager::GetActiveScene());
		entt::registry& registry = scene.GetRegistry();

		auto view = registry.view<Engine::TransformComponent>();

		int i = 0;
		for (auto [entity, transform] : view.each())
		{
			if (transform.parent.IsValid()) continue;

			DrawReorderDropTarget(Engine::Entity{}, i);
			DrawEntityNode(Engine::Entity{ static_cast<uint32_t>(entity), &scene });
			i++;
		}

		DrawReorderDropTarget(Engine::Entity{}, i);

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
}