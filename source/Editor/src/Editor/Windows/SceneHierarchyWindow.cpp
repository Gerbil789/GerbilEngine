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
	void DrawEntityNode(engine::Scene& scene, entt::entity entity)
	{
		if(entity == entt::null) return;

		const std::string& name = scene.registry.get<engine::component::Name>(entity).name;

		auto& rc = scene.registry.get<engine::component::Relationship>(entity);

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DrawLinesToNodes | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		if (rc.firstChild == entt::null) flags |= ImGuiTreeNodeFlags_Leaf;

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

				if (dropped == entity) return;

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
					scene.AttachChild(entity, dropped);
					scene.registry.emplace_or_replace<engine::tag::TransformDirty>(dropped);
				}
			}
			ImGui::EndDragDropTarget();
		}

		// context menu
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Create Empty"))
			{
				scene.CreateEntity("Empty", entity);
			}

			if (ImGui::MenuItem("Create Image"))
			{
				entt::entity newEntity = scene.CreateEntity("Image", entity);
				scene.registry.emplace<engine::component::RectTransform>(newEntity);
				scene.registry.emplace<engine::component::Image>(newEntity);
				scene.registry.emplace<engine::tag::LayoutDirty>(newEntity);
			}

			if (ImGui::MenuItem("Delete"))
			{
				editor::context::DeselectEntity(entity);
				scene.DestroyEntity(entity);
			}

			ImGui::EndPopup();
		}

		if (opened)
		{
			DrawEntityNode(scene, rc.firstChild);
			ImGui::TreePop();
		}

		DrawEntityNode(scene, rc.nextSibling);

		ImGui::PopID();
	}

	void SceneHierarchyWindow::Draw()
	{
		ScopedStyle style
		{
			{ ImGuiStyleVar_WindowPadding,{ 0, 0 } },
			{ ImGuiStyleVar_ItemSpacing,{ 0, 0 } }
		};

		ImGui::Begin("Scene Hierarchy");

		engine::Scene& scene = engine::Context::activeSceneHandle.Get();

		DrawEntityNode(scene, scene.firstRoot);

		// context menu
		if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create Entity"))
			{
				scene.CreateEntity("Empty");
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
				scene.AttachRoot(dropped);
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::End();
	}
}