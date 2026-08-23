#include "Editor/Windows/SceneHierarchyWindow.h"
#include "Editor/Windows/Utility/ScopedStyle.h"
#include "Editor/Core/SelectionManager.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Core/SceneManager.h"
#include "Engine/Core/Scene.h"
#include "Engine/Core/Components.h"
#include "Engine/Core/Input.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Core/Log.h"
#include <imgui.h>

namespace Editor
{
	bool IsDescendant(entt::registry& registry, entt::entity ancestor, entt::entity entity)
	{
		while (entity != entt::null)
		{
			if (entity == ancestor)
			{
				return true;
			}
			entity = registry.get<Engine::HierarchyComponent>(entity).parent;
		}
		return false;
	}

	void DrawEntityNode(Engine::SceneAsset& scene, entt::registry& registry, entt::entity entity)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DrawLinesToNodes | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

		//TODO: delete this check
		if(!registry.any_of<Engine::IdentityComponent>(entity))
		{
			LOG_WARNING("Entity has no id!!!!");


			auto name = registry.get<Engine::NameComponent>(entity);
			return;
		}

		Engine::Uuid id = registry.get<Engine::IdentityComponent>(entity).id;
		const std::string& name = registry.get<Engine::NameComponent>(entity).name;

		auto& hc = registry.get<Engine::HierarchyComponent>(entity);
		if (hc.children.empty()) flags |= ImGuiTreeNodeFlags_Leaf;

		bool selected = SelectionManager::Entities.IsSelected(id);

		if (selected)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		ImGui::PushID(static_cast<int>(entity));

		bool opened = ImGui::TreeNodeEx(name.c_str(), flags);

		if (ImGui::IsItemClicked())
		{
			bool additive = Engine::Input::IsKeyDown(Engine::Key::LeftControl) || Engine::Input::IsKeyDown(Engine::Key::LeftShift);
			SelectionManager::Entities.Select(id, additive);
		}

		// drag source
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("ENTITY", &entity, sizeof(entt::entity));
			ImGui::Text("%s", name.c_str());
			ImGui::EndDragDropSource();
		}

		// drop target directly on the node
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY", ImGuiDragDropFlags_AcceptBeforeDelivery | ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
			{
				entt::entity dropped = *(const entt::entity*)payload->Data;

				if (dropped != entity && !IsDescendant(registry, dropped, entity))
				{
					ImVec2 mousePos = ImGui::GetMousePos();
					ImVec2 itemMin = ImGui::GetItemRectMin();
					ImVec2 itemMax = ImGui::GetItemRectMax();
					float itemHeight = itemMax.y - itemMin.y;

					// define thresholds (top 25%, middle 50%, bottom 25%)
					float threshold = itemHeight * 0.25f;

					enum class DropMode { Before, After, Child };
					DropMode mode = DropMode::Child;

					if (mousePos.y < itemMin.y + threshold) mode = DropMode::Before;
					else if (mousePos.y > itemMax.y - threshold) mode = DropMode::After;

					// Visual Feedback
					ImDrawList* drawList = ImGui::GetWindowDrawList();
					ImU32 highlightColor = IM_COL32(255, 255, 0, 255); // Yellow
					float lineThickness = 2.0f;

					if (mode == DropMode::Before)
					{
						drawList->AddLine(itemMin, ImVec2(itemMax.x, itemMin.y), highlightColor, lineThickness);
					}
					else if (mode == DropMode::After)
					{
						drawList->AddLine(ImVec2(itemMin.x, itemMax.y), itemMax, highlightColor, lineThickness);
					}
					else
					{
						drawList->AddRect(itemMin, itemMax, highlightColor, 0.0f, 0, 1.0f);
					}

					// actual Drop Processing
					if (ImGui::AcceptDragDropPayload("ENTITY"))
					{
						auto& droppedHC = registry.get<Engine::HierarchyComponent>(dropped);

						// detach from current parent
						if (droppedHC.parent != entt::null)
						{
							auto& oldParentHC = registry.get<Engine::HierarchyComponent>(droppedHC.parent);
							std::erase(oldParentHC.children, dropped);
						}
						else
						{
							scene.RemoveRootEntity(dropped);
						}

						// attach to new location
						if (mode == DropMode::Child)
						{
							droppedHC.parent = entity;
							hc.children.push_back(dropped);
						}
						else
						{
							entt::entity targetParent = hc.parent;
							droppedHC.parent = targetParent;

							if (targetParent != entt::null)
							{
								auto& parentHC = registry.get<Engine::HierarchyComponent>(targetParent);
								auto it = std::find(parentHC.children.begin(), parentHC.children.end(), entity);

								if (mode == DropMode::After && it != parentHC.children.end()) ++it;
								parentHC.children.insert(it, dropped);
							}
							else
							{
								// target is a root entity
								const auto& roots = scene.GetRootEntities();
								auto it = std::find(roots.begin(), roots.end(), entity);
								size_t index = (it != roots.end()) ? std::distance(roots.begin(), it) : roots.size();

								if (mode == DropMode::After) ++index;
								scene.InsertRootEntity(dropped, index);
							}
						}
						registry.emplace_or_replace<Engine::TransformDirty>(dropped);
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		// context menu
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Create Empty"))
			{
				EditorCommandManager::CreateEntity<Engine::TransformComponent, Engine::WorldTransformComponent>("Empty", entity);
			}

			if (ImGui::MenuItem("Create Image"))
			{
				EditorCommandManager::CreateEntity<Engine::UI::RectTransform, Engine::UI::Image, Engine::UI::LayoutDirtyTag>("Image", entity);
			}

			if (ImGui::MenuItem("Delete"))
			{
				EditorCommandManager::DeleteEntity(id);
			}

			ImGui::EndPopup();
		}

		if (opened)
		{
			for (entt::entity child : hc.children)
			{
				DrawEntityNode(scene, registry, child);
			}
			ImGui::TreePop();
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

		Engine::SceneAsset& scene = Engine::AssetManager::GetAsset(Engine::SceneManager::GetActiveScene());
		entt::registry& registry = scene.GetRegistry();

		const std::vector<entt::entity>& rootEntities = scene.GetRootEntities();

		for (entt::entity entity : rootEntities)
		{
			DrawEntityNode(scene, registry, entity);
		}

		// context menu
		if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create Entity"))
			{
				EditorCommandManager::CreateEntity("Empty");
			}
			ImGui::EndPopup();
		}

		// empty space drop target
		ImGui::Dummy(ImGui::GetContentRegionAvail());
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
			{
				entt::entity dropped = *(const entt::entity*)payload->Data;
				auto& droppedHC = registry.get<Engine::HierarchyComponent>(dropped);

				if (droppedHC.parent != entt::null)
				{
					auto& oldParentHC = registry.get<Engine::HierarchyComponent>(droppedHC.parent);
					std::erase(oldParentHC.children, dropped);
					droppedHC.parent = entt::null;


					scene.InsertRootEntity(dropped, scene.GetRootEntities().size());
					registry.emplace_or_replace<Engine::TransformDirty>(dropped);
				}
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::End();
	}
}