#include "EntityInspectorPanel.h"
#include "Engine/Scene/Components.h"
#include "Editor/Components/Components.h"
#include "Engine/Asset/AssetManager.h"

#include "Editor/Command/CommandManager.h"
#include "Editor/Command/TransformEntity.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui_internal.h>

namespace Editor
{
	using namespace Engine;

	void EntityInspectorPanel::Draw(Engine::Entity entity)
	{
		if (!entity)
		{
			return;
		}

		DrawComponents(entity);
		ImGui::Separator();
		DrawAddComponentButton(entity);

	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Engine::Entity entity, UIFunction function)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		if (!entity.HasComponent<T>()) { return; }

		auto& component = entity.GetComponent<T>();
		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 4));
		float lineHeigth = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;
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

	void EntityInspectorPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<NameComponent>())
		{
			std::string& name = entity.GetComponent<NameComponent>().Name;

			char buffer[64];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), name.c_str());

			ImGui::PushItemWidth(-1);
			bool tmp = true;
			ImGui::Checkbox("##Enabled", &entity.GetComponent<IdentityComponent>().Enabled);
			ImGui::SameLine();
			if (ImGui::InputText("##Name", buffer, sizeof(buffer))) { name = std::string(buffer); }
		}

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
			{
				//TODO: Use command for transform changes
				float availWidth = glm::max(ImGui::GetContentRegionAvail().x - 100, 100.0f);
				ImGui::Columns(2, "transform", false);
				ImGui::SetColumnWidth(0, 100);
				ImGui::SetColumnWidth(1, availWidth);

				ImGui::Text("Position");
				ImGui::NextColumn();
				UI::Vec3Field("Position", component.Position);
				ImGui::NextColumn();

				ImGui::Text("Rotation");
				ImGui::NextColumn();
				UI::Vec3Field("Rotation", component.Rotation);
				ImGui::NextColumn();

				ImGui::Text("Scale");
				ImGui::NextColumn();
				UI::Vec3Field("Scale", component.Scale);
				ImGui::NextColumn();
				ImGui::Columns(1);
			});


		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
			{
				Ref<Camera> camera = component.Camera;

				//UI::BoolControl("Primary", component.Main);
				//UI::BoolControl("Fixed Aspect Ratio", component.FixedAspectRatio);


				std::vector<std::string> projectionTypes = { "Perspective", "Orthographic" };

				//const char* currentProjectionTypeString = projectionTypeString[(int)camera.GetProjectionType()];

		/*		Camera::ProjectionType projectionType = camera.GetProjectionType();
				if (UI::EnumField("Projection", (int&)projectionType, projectionTypes))
				{
					camera.SetProjectionType((Camera::ProjectionType)projectionType);
				}

				if (camera.GetProjectionType() == Camera::ProjectionType::Perspective)
				{
					Engine::PerspectiveSettings& perspective = camera.GetPerspectiveSettings();

					float FOV = glm::degrees(perspective.FOV);
					if (UI::FloatField("Vertical FOV", FOV))
					{
						perspective.FOV(glm::radians(FOV));
					}

					float perspectiveNearClip = camera.GetPerspectiveNearClip();
					if (UI::FloatField("Near Clip", perspectiveNearClip))
					{
						camera.SetPerspectiveNearClip(perspectiveNearClip);
					}

					float perspectiveFarClip = camera.GetPerspectiveFarClip();
					if (UI::FloatField("Far Clip", perspectiveFarClip))
					{
						camera.SetPerspectiveFarClip(perspectiveFarClip);
					}
				}

				if (camera.GetProjectionType() == Camera::ProjectionType::Orthographic)
				{
					float orthographicSize = camera.GetOrthographicSize();
					if (UI::FloatField("Size", orthographicSize))
					{
						camera.SetOrthographicSize(orthographicSize);
					}

					float orthographicNearClip = camera.GetOrthographicNearClip();
					if (UI::FloatField("Near Clip", orthographicNearClip))
					{
						camera.SetOrthographicNearClip(orthographicNearClip);
					}

					float orthographicFarClip = camera.GetOrthographicFarClip();
					if (UI::FloatField("Far Clip", orthographicFarClip))
					{
						camera.SetOrthographicFarClip(orthographicFarClip);
					}
				}*/
			});

		DrawComponent<LightComponent>("Light", entity, [](auto& component)
			{
				float availWidth = glm::max(ImGui::GetContentRegionAvail().x - 100, 100.0f);
				ImGui::Columns(2, "light_body", false);
				ImGui::SetColumnWidth(0, 100);
				ImGui::SetColumnWidth(1, availWidth);


				ImGui::Text("Type");
				ImGui::NextColumn();
				std::vector<std::string> lightTypes = { "Point", "Directional", "Spot" };
				if (UI::EnumField("Type", (int&)component.Type, lightTypes))
				{
					component.Type = (LightType)component.Type;
				}
				ImGui::NextColumn();


				ImGui::Text("Color");
				ImGui::NextColumn();
				UI::ColorField("##color", component.Color);
				ImGui::NextColumn();

				ImGui::Text("Intensity");
				ImGui::NextColumn();
				UI::FloatField("Intensity", component.Intensity);
				ImGui::NextColumn();


				switch (component.Type)
				{
				case LightType::Directional:
					break;
				case LightType::Point:
					ImGui::Text("Range");
					ImGui::NextColumn();
					UI::FloatField("Range", component.Range);
					ImGui::NextColumn();

					ImGui::Text("Attenuation");
					ImGui::NextColumn();
					UI::Vec3Field("Attenuation", component.Attenuation);
					ImGui::NextColumn();
					break;

				case LightType::Spot:
					ImGui::Text("Inner Angle");
					ImGui::NextColumn();
					UI::FloatField("Inner Angle", component.InnerAngle);
					ImGui::NextColumn();

					ImGui::Text("Outer Angle");
					ImGui::NextColumn();
					UI::FloatField("Outer Angle", component.OuterAngle);
					ImGui::NextColumn();

					ImGui::Text("Attenuation");
					ImGui::NextColumn();
					UI::Vec3Field("Attenuation", component.Attenuation);
					ImGui::NextColumn();
					break;
				}

				ImGui::Columns(1);
			});

		DrawComponent<MeshComponent>("MeshRenderer", entity, [](auto& component)
			{
				float availWidth = glm::max(ImGui::GetContentRegionAvail().x - 100, 100.0f);
				ImGui::Columns(2, "light_body", false);
				ImGui::SetColumnWidth(0, 100);
				ImGui::SetColumnWidth(1, availWidth);


				ImGui::Text("Mesh");
				ImGui::NextColumn();

				std::string meshButtonText = component.Mesh != nullptr ? std::to_string((uint64_t)component.Mesh->id) : "##Mesh";
				ImGui::Button(meshButtonText.c_str(), ImVec2(availWidth, 0.0f));

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("UUID"))
					{
						Engine::UUID droppedUUID = *static_cast<const Engine::UUID*>(payload->Data);
						if (AssetManager::GetAssetType(droppedUUID) == AssetType::Mesh)
						{
							component.Mesh = AssetManager::GetAsset<Mesh>(droppedUUID);
						}

					}
					ImGui::EndDragDropTarget();
				}

				ImGui::NextColumn();

				ImGui::Text("Material");
				ImGui::NextColumn();
				std::string materialButtonText = component.Material != nullptr ? std::to_string((uint64_t)component.Material->id) : "##Material";
				ImGui::Button(materialButtonText.c_str(), ImVec2(availWidth, 0.0f));

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("UUID"))
					{
						Engine::UUID droppedUUID = *static_cast<const Engine::UUID*>(payload->Data);
						if (AssetManager::GetAssetType(droppedUUID) == AssetType::Material)
						{
							component.Material = AssetManager::GetAsset<Material>(droppedUUID);
						}
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::NextColumn();
				ImGui::Columns(1);
			});
	}

	void EntityInspectorPanel::DrawAddComponentButton(Entity entity)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.FramePadding = ImVec2(20, 5);
		style.FrameRounding = 7.5f;



		float lineHeigth = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;

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

			if (ImGui::MenuItem("Light"))
			{
				entity.AddComponent<LightComponent>();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("MeshRenderer"))
			{
				auto& component = entity.AddComponent<MeshComponent>();

				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
}
