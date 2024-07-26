#include "enginepch.h"

#include "SceneHierarchyPanel.h"
#include "Engine/Scene/Components.h"
#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui_internal.h>

namespace Engine
{
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
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

		if(ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		{
			m_SelectionContext = {};
		}


		if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				m_Context->CreateEntity("Empty Entity");
			}

			ImGui::EndPopup();
		}

		ImGui::End();



		ImGui::Begin("Inspector");
		if(m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);

			ImGui::Separator();

			ImGuiStyle& style = ImGui::GetStyle();
			style.FramePadding = ImVec2(20, 5);
			style.FrameRounding = 7.5f;

			float lineHeigth = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

			ImVec2 window_size = ImGui::GetWindowSize();
			ImVec2 button_size = ImVec2{ 200, 30 };

			// Calculate the center position
			float window_center_x = window_size.x / 2.0f;
			float button_center_x = button_size.x / 2.0f;

			// Set the cursor position to center the button
			ImGui::SetCursorPosX(window_center_x - button_center_x);

			//set Y offset
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
			


			if (ImGui::Button("Add Component", button_size))
			{
				ImGui::OpenPopup("AddComponent");
			}

			style.FramePadding = ImVec2(4, 4);
			style.FrameRounding = 0.0f;
	

			if(ImGui::BeginPopup("AddComponent"))
			{
				if(ImGui::MenuItem("Camera"))
				{
					m_SelectionContext.AddComponent<CameraComponent>();
					ImGui::CloseCurrentPopup();
				}

				if(ImGui::MenuItem("Sprite Renderer"))
				{
					m_SelectionContext.AddComponent<SpriteRendererComponent>();
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}
		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0);
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool expanded =  ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

		if(ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
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
			if(m_SelectionContext == entity)
			{
				m_SelectionContext = {};
			}
		}
	}

	static void DrawVec3Control(const char* label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label);

		ImGui::Columns(2, (const char*)0, false);
		ImGui::SetColumnWidth(0, columnWidth);
		


		ImGui::Text(label);

		ImGui::NextColumn();
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());


		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		

		ImGui::SameLine();
		ImGui::PushFont(boldFont);
		if (ImGui::Button("R", buttonSize))
		{
			values.x = resetValue;
			values.y = resetValue;
			values.z = resetValue;
		}
		ImGui::PopFont();

		ImGui::Columns(1);
		ImGui::PopID();
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction function)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		if(entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
			float lineHeigth = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();
			ImGui::SameLine(contentRegionAvailable.x - lineHeigth * 0.5f);
			if(ImGui::Button("+", ImVec2{ lineHeigth, lineHeigth }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}
	

			bool removeComponent = false;
			if(ImGui::BeginPopup("ComponentSettings"))
			{
				if(ImGui::MenuItem("Remove component"))
				{
					removeComponent = true;
				}
				ImGui::EndPopup();
			}

			if(open)
			{
				function(component);
				ImGui::TreePop();
			}

			if(removeComponent)
			{
				entity.RemoveComponent<T>();
			}
		}

	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if(entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[64];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());

			
			if(ImGui::InputText("Name", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
		{
			DrawVec3Control("Position", component.Position);
			DrawVec3Control("Rotation", component.Rotation);
			DrawVec3Control("Scale", component.Scale, 1.0f);
		});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
		});


		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
		{
			auto& camera = component.Camera;

			ImGui::Checkbox("Primary", &component.Main);
			ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);

			const char* projectionTypeString[] = { "Perspective", "Orthographic" };
			const char* currentProjectionTypeString = projectionTypeString[(int)camera.GetProjectionType()];
			if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
			{
				for (int i = 0; i < 2; i++)
				{
					bool isSelected = currentProjectionTypeString == projectionTypeString[i];
					if (ImGui::Selectable(projectionTypeString[i], isSelected))
					{
						currentProjectionTypeString = projectionTypeString[i];
						camera.SetProjectionType((SceneCamera::ProjectionType)i);
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}

			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
			{
				float perspectiveVerticalFOV = glm::degrees(camera.GetPerspectiveVerticalFOV());
				if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFOV))
					camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFOV));

				float perspectiveNearClip = camera.GetPerspectiveNearClip();
				if (ImGui::DragFloat("Near Clip", &perspectiveNearClip))
					camera.SetPerspectiveNearClip(perspectiveNearClip);

				float perspectiveFarClip = camera.GetPerspectiveFarClip();
				if (ImGui::DragFloat("Far Clip", &perspectiveFarClip))
					camera.SetPerspectiveFarClip(perspectiveFarClip);
			}

			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
			{
				float orthographicSize = camera.GetOrthographicSize();
				if (ImGui::DragFloat("Size", &orthographicSize))
					camera.SetOrthographicSize(orthographicSize);

				float orthographicNearClip = camera.GetOrthographicNearClip();
				if (ImGui::DragFloat("Near Clip", &orthographicNearClip))
					camera.SetOrthographicNearClip(orthographicNearClip);

				float orthographicFarClip = camera.GetOrthographicFarClip();
				if (ImGui::DragFloat("Far Clip", &orthographicFarClip))
					camera.SetOrthographicFarClip(orthographicFarClip);
			}
		});
		
	}
}