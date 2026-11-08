#include "Editor/Windows/SceneHierarchyWindow.h"
#include "Editor/Core/EditorContext.h"
#include "Editor/Windows/Utility/ScopedStyle.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Engine/Core/Context.h"
#include "Engine/Core/Components.h"
#include "Engine/Core/Log.h"
#include <imgui.h>

namespace editor
{
	bool IsDescendant(entt::registry& registry, entt::entity ancestor, entt::entity entity)
	{
		while (entity != entt::null)
		{
			if (entity == ancestor)
			{
				return true;
			}
			entity = registry.get<engine::component::Hierarchy>(entity).parent;
		}
		return false;
	}

	void DrawEntityNode(engine::Scene& scene, entt::entity entity)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DrawLinesToNodes | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

		const std::string& name = scene.registry.get<engine::component::Name>(entity).name;

		auto& hc = scene.registry.get<engine::component::Hierarchy>(entity);
		if (hc.children.empty()) flags |= ImGuiTreeNodeFlags_Leaf;

		if (editor::context::currentSelection.IsSelected(entity))
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		ImGui::PushID(static_cast<int>(entity));

		bool opened = ImGui::TreeNodeEx(name.c_str(), flags);

		if (ImGui::IsItemClicked())
		{
			ImGuiIO& io = ImGui::GetIO();
  		bool additive = io.KeyCtrl || io.KeyShift;
			editor::context::SelectEntity(entity, additive);
		}

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("ENTITY", &entity, sizeof(entt::entity));
			ImGui::Text("%s", name.c_str());
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY", ImGuiDragDropFlags_AcceptBeforeDelivery | ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
			{
				entt::entity dropped = *(const entt::entity*)payload->Data;

				if (dropped != entity && !IsDescendant(scene.registry, dropped, entity))
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
						auto& droppedHC = scene.registry.get<engine::component::Hierarchy>(dropped);

						// detach from current parent
						if (droppedHC.parent != entt::null)
						{
							auto& oldParentHC = scene.registry.get<engine::component::Hierarchy>(droppedHC.parent);
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
								auto& parentHC = scene.registry.get<engine::component::Hierarchy>(targetParent);
								auto it = std::find(parentHC.children.begin(), parentHC.children.end(), entity);

								if (mode == DropMode::After && it != parentHC.children.end()) ++it;
								parentHC.children.insert(it, dropped);
							}
							else
							{
								// target is a root entity
								const auto& roots = scene.rootEntities;
								auto it = std::find(roots.begin(), roots.end(), entity);
								size_t index = (it != roots.end()) ? std::distance(roots.begin(), it) : roots.size();

								if (mode == DropMode::After) ++index;
								scene.InsertRootEntity(dropped, index);
							}
						}
						scene.registry.emplace_or_replace<engine::tag::TransformDirty>(dropped);
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
				scene.CreateEntity<engine::component::Transform, engine::component::WorldTransform>("Empty", entity);
			}

			if (ImGui::MenuItem("Create Image"))
			{
				scene.CreateEntity<engine::component::RectTransform, engine::component::Image, engine::tag::LayoutDirty>("Image", entity);
			}

			if (ImGui::MenuItem("Delete"))
			{
				scene.DestroyEntity(entity);
			}

			ImGui::EndPopup();
		}

		if (opened)
		{
			for (entt::entity child : hc.children)
			{
				DrawEntityNode(scene, child);
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

		engine::Scene& scene = engine::Context::activeSceneHandle.Get();

		for (entt::entity entity : scene.rootEntities)
		{
			DrawEntityNode(scene, entity);
		}

		// context menu
		if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create Entity"))
			{
				scene.CreateEntity<engine::component::Transform, engine::component::WorldTransform>("Empty");
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
				auto& droppedHC = scene.registry.get<engine::component::Hierarchy>(dropped);

				if (droppedHC.parent != entt::null)
				{
					auto& oldParentHC = scene.registry.get<engine::component::Hierarchy>(droppedHC.parent);
					std::erase(oldParentHC.children, dropped);
					droppedHC.parent = entt::null;

					scene.InsertRootEntity(dropped, scene.rootEntities.size());
					scene.registry.emplace_or_replace<engine::tag::TransformDirty>(dropped);
				}
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::End();
	}
}