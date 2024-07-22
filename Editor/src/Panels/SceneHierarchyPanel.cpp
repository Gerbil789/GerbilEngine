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

			if(ImGui::Button("Add Component"))
			{
				ImGui::OpenPopup("AddComponent");
			}

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
		if (ImGui::Button("R", buttonSize))
		{
			values.x = resetValue;
			values.y = resetValue;
			values.z = resetValue;
		}

		ImGui::Columns(1);
		ImGui::PopID();
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

		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		
		if(entity.HasComponent<TransformComponent>())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
			bool open = ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), treeNodeFlags, "Transform");

			ImGui::SameLine(ImGui::GetWindowWidth() - 25.0f);
			if (ImGui::Button("+", ImVec2{ 20, 20 }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}
			ImGui::PopStyleVar();

			if(ImGui::BeginPopup("ComponentSettings"))
			{
				if(ImGui::MenuItem("Reset"))
				{
					auto& tc = entity.GetComponent<TransformComponent>();
					tc.Position = { 0.0f, 0.0f, 0.0f };
					tc.Rotation = { 0.0f, 0.0f, 0.0f };
					tc.Scale = { 1.0f, 1.0f, 1.0f };
				}
				ImGui::EndPopup();
			}

			if (open)
			{
				auto& tc = entity.GetComponent<TransformComponent>();

				DrawVec3Control("Position", tc.Position);

				//glm::vec3 rotation = glm::degrees(tc.Rotation);
				DrawVec3Control("Rotation", tc.Rotation);
				//tc.Rotation = glm::radians(rotation);

				DrawVec3Control("Scale", tc.Scale, 1.0f);

				ImGui::TreePop();
			}
		}


		if(entity.HasComponent<SpriteRendererComponent>())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
			bool open = ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(), treeNodeFlags, "Sprite Renderer");

			ImGui::SameLine(ImGui::GetWindowWidth() - 25.0f);
			if(ImGui::Button("+", ImVec2{ 20, 20 }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}
			ImGui::PopStyleVar();

			bool removeComponent = false;
			if(ImGui::BeginPopup("ComponentSettings"))
			{
				if(ImGui::MenuItem("Remove component"))
				{
					removeComponent = true;
				}
				ImGui::EndPopup();
			}
			if (open)
			{
				auto& color = entity.GetComponent<SpriteRendererComponent>().Color;
				ImGui::ColorEdit4("Color", glm::value_ptr(color));

				ImGui::TreePop();
			}

			if(removeComponent)
			{
				entity.RemoveComponent<SpriteRendererComponent>();
			}
			
		}


		if (entity.HasComponent<CameraComponent>())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
			bool open = ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), treeNodeFlags, "Camera");

			ImGui::SameLine(ImGui::GetWindowWidth() - 25.0f);
			if (ImGui::Button("+", ImVec2{ 20, 20 }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}
			ImGui::PopStyleVar();

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
				{
					removeComponent = true;
				}
				ImGui::EndPopup();
			}

			if (open)
			{
				auto& cameraComponent = entity.GetComponent<CameraComponent>();
				auto& camera = cameraComponent.Camera;

				ImGui::Checkbox("Main", &cameraComponent.Main);
				ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.FixedAspectRatio);


				const char* projectionTypeString[] = { "Perspective", "Orthographic" };
				const char* currentProjectionTypeString = projectionTypeString[(int)camera.GetProjectionType()];
				if (ImGui::BeginCombo("Projection", currentProjectionTypeString)) 
				{
					for(int i = 0; i < 2; i++)
					{
						bool isSelected = currentProjectionTypeString == projectionTypeString[i];
						if(ImGui::Selectable(projectionTypeString[i], isSelected))
						{
							currentProjectionTypeString = projectionTypeString[i];
							camera.SetProjectionType((SceneCamera::ProjectionType)i);
						}

						if(isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}

					ImGui::EndCombo();
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective) 
				{
					float verticalFOV = glm::degrees(camera.GetPerspectiveVerticalFOV());
					if (ImGui::DragFloat("Vertical FOV", &verticalFOV)) 
					{
						camera.SetPerspectiveVerticalFOV(glm::radians(verticalFOV));
					}

					float nearClip = camera.GetPerspectiveNearClip();
					if (ImGui::DragFloat("Near Clip", &nearClip)) 
					{
						camera.SetPerspectiveNearClip(nearClip);
					}

					float farClip = camera.GetPerspectiveFarClip();
					if (ImGui::DragFloat("Far Clip", &farClip)) 
					{
						camera.SetPerspectiveFarClip(farClip);
					}
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic) 
				{
					float size = camera.GetOrthographicSize();
					if (ImGui::DragFloat("Size", &size)) 
					{
						camera.SetOrthographicSize(size);
					}

					float nearClip = camera.GetOrthographicNearClip();
					if (ImGui::DragFloat("Near Clip", &nearClip)) 
					{
						camera.SetOrthographicNearClip(nearClip);
					}

					float farClip = camera.GetOrthographicFarClip();
					if (ImGui::DragFloat("Far Clip", &farClip)) 
					{
						camera.SetOrthographicFarClip(farClip);
					}
				}


			

				ImGui::TreePop();
			}

			if (removeComponent)
			{
				entity.RemoveComponent<CameraComponent>();
			}
		}
	}
}