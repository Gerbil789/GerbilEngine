#include "enginepch.h"
#include "InspectorPanel.h"
#include "Engine/Scene/Components.h"
#include "../Elements/Elements.h"
#include "Engine/Core/AssetManager.h"
#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui_internal.h>
#include <filesystem>


namespace Engine
{
	InspectorPanel::InspectorPanel()
	{
		SceneManager::AddObserver(this);
	}

	InspectorPanel::~InspectorPanel()
	{
		SceneManager::RemoveObserver(this);
	}

	void InspectorPanel::OnSceneChanged()
	{
		m_Scene = SceneManager::GetCurrentScene();
	}

	void InspectorPanel::OnImGuiRender()
	{
		ImGui::Begin("Inspector");
		Entity entity = m_Scene->GetSelectedEntity();
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

		if (!entity.HasComponent<T>()) { return; }

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
					component.Material = AssetManager::LoadAsset<Material>("temp");
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
							component.Texture = AssetManager::LoadAsset<Texture2D>(texturePath.string());
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

				UI::BoolControl("Primary", component.Main);
				UI::BoolControl("Fixed Aspect Ratio", component.FixedAspectRatio);

				const char* projectionTypeString[] = { "Perspective", "Orthographic" };
				const char* currentProjectionTypeString = projectionTypeString[(int)camera.GetProjectionType()];

				SceneCamera::ProjectionType projectionType = camera.GetProjectionType();
				if (UI::EnumControl("Projection", (int&)projectionType, projectionTypeString, 2))
				{
					camera.SetProjectionType((SceneCamera::ProjectionType)projectionType);
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					float perspectiveVerticalFOV = glm::degrees(camera.GetPerspectiveVerticalFOV());
					if (UI::FloatControl("Vertical FOV", perspectiveVerticalFOV)) 
					{
						camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFOV));
					}

					float perspectiveNearClip = camera.GetPerspectiveNearClip();
					if (UI::FloatControl("Near Clip", perspectiveNearClip)) 
					{
						camera.SetPerspectiveNearClip(perspectiveNearClip);
					}

					float perspectiveFarClip = camera.GetPerspectiveFarClip();
					if (UI::FloatControl("Far Clip", perspectiveFarClip)) 
					{
						camera.SetPerspectiveFarClip(perspectiveFarClip); 
					}
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float orthographicSize = camera.GetOrthographicSize();
					if(UI::FloatControl("Size", orthographicSize))
					{
						camera.SetOrthographicSize(orthographicSize);
					}

					float orthographicNearClip = camera.GetOrthographicNearClip();
					if (UI::FloatControl("Near Clip", orthographicNearClip)) 
					{
						camera.SetOrthographicNearClip(orthographicNearClip);
					}
						
					float orthographicFarClip = camera.GetOrthographicFarClip();
					if(UI::FloatControl("Far Clip", orthographicFarClip))
					{
						camera.SetOrthographicFarClip(orthographicFarClip);
					}
				}
			});

		DrawComponent<LightComponent>("Light", entity, [](auto& component)
			{
				UI::ColorControl(component.Color);
				UI::FloatControl("Intensity", component.Intensity, 1.0f);
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

			if (ImGui::MenuItem("Light"))
			{
				entity.AddComponent<LightComponent>();
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
}