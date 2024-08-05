#include "enginepch.h"
#include "../Elements/Elements.h"
#include "InspectorPanel.h"
#include "Engine/Scene/Components.h"
#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui_internal.h>
#include <filesystem>

namespace Engine
{
	InspectorPanel::InspectorPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void InspectorPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}

	void InspectorPanel::OnImGuiRender()
	{
		ImGui::Begin("Inspector");
		Entity entity = m_Context->GetSelectedEntity();
		if (entity)
		{
			DrawComponents(entity);
			ImGui::Separator();
			DrawAddComponentButton(entity);
		}
		ImGui::End();
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction function)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 4));
			float lineHeigth = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();
			ImGui::SameLine(contentRegionAvailable.x - lineHeigth * 0.5f);
			if (ImGui::Button("+", ImVec2{ lineHeigth, lineHeigth }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

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
				function(component);
				ImGui::TreePop();
			}

			if (removeComponent)
			{
				entity.RemoveComponent<T>();
			}
		}

	}

	void InspectorPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<NameComponent>())
		{
			auto& name = entity.GetComponent<NameComponent>().Name;

			char buffer[64];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), name.c_str());

			ImGui::PushItemWidth(-1);
			ImGui::Checkbox("##Enabled", &entity.GetComponent<EnablingComponent>().Enabled);
			ImGui::SameLine();
			if (ImGui::InputText("##Name", buffer, sizeof(buffer))) { name = std::string(buffer); }
		}

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
			{
				UI::Vec3Control("Position", component.Position);
				UI::Vec3Control("Rotation", component.Rotation);
				UI::Vec3Control("Scale", component.Scale, 1.0f);
			});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
			{
				if (ImGui::Button("Material", ImVec2(100.0f, 0.0f)))
				{
					Ref<Material> material = CreateRef<Material>();
					material->shaderName = "Texture";
					material->texture = Texture2D::Create("assets/textures/gerbil.jpg");
					material->color = glm::vec4(0.8f, 0.2f, 0.3f, 1.0f);
					material->tiling = glm::vec2(2.0f, 2.0f);
					component.Material = material;
				}

				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

				ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path texturePath = path;
						if (texturePath.extension() == ".png" || texturePath.extension() == ".jpg")
						{
							component.Texture = Texture2D::Create(texturePath.string());
						}
						else {
							ENGINE_LOG_WARNING("Failed to load texture!");
						}
					}
					ImGui::EndDragDropTarget();
				}

				// Set the width of the window or column you are working within
				ImGui::PushItemWidth(-1);

				ImGui::Text("Tiling");
				ImGui::SameLine();
				ImGui::PushItemWidth(50);
				ImGui::DragFloat("x", &component.TilingFactor.x, 0.1f, 0.0f, 0.0f, "%.2f");

				ImGui::SameLine();
				ImGui::PushItemWidth(50);
				ImGui::DragFloat("y", &component.TilingFactor.y, 0.1f, 0.0f, 0.0f, "%.2f");

				ImGui::PopItemWidth();
				ImGui::PopItemWidth();
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

	void InspectorPanel::DrawAddComponentButton(Entity entity)
	{
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


		if (ImGui::BeginPopup("AddComponent"))
		{
			if (ImGui::MenuItem("Camera"))
			{
				entity.AddComponent<CameraComponent>();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Sprite Renderer"))
			{
				entity.AddComponent<SpriteRendererComponent>();
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
}