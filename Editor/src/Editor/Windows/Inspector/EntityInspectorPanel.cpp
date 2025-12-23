#include "EntityInspectorPanel.h"
#include "Engine/Scene/Components.h"
#include "Editor/Components/Widgets.h"
#include "Engine/Asset/AssetManager.h"

#include "Editor/Command/CommandManager.h"
#include "Editor/Command/TransformEntity.h"
#include "Editor/Core/EditorContext.h"
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
			std::string& name = entity.GetComponent<NameComponent>().name;

			char buffer[64];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), name.c_str());

			ImGui::PushID((int)entity.GetUUID());

			ImGui::PushItemWidth(-1);
			bool tmp = true;
			ImGui::Checkbox("##Enabled", &entity.GetComponent<IdentityComponent>().enabled);
			ImGui::SameLine();
			if (ImGui::InputText("##Name", buffer, sizeof(buffer))) { name = std::string(buffer); }
			ImGui::PopID();
		}

		DrawComponent<TransformComponent>("Transform", entity, [entity](auto& component)
			{


				WidgetResult result;
				static TransformData s_TransformBefore;

				float availWidth = glm::max(ImGui::GetContentRegionAvail().x - 100, 100.0f);
				ImGui::Columns(2, "transform", false);
				ImGui::SetColumnWidth(0, 100);
				ImGui::SetColumnWidth(1, availWidth);

				auto drawField = [&result](const char* label, glm::vec3& value)
					{
						ImGui::Text(label);
						ImGui::NextColumn();
						result |= Widget::Vec3Field(label, value);
						ImGui::NextColumn();
					};

				drawField("Position", component.position);
				drawField("Rotation", component.rotation);
				drawField("Scale", component.scale);
				ImGui::Columns(1);

				if (result.started)
				{
					s_TransformBefore = { component.position, component.rotation, component.scale };
				}
				else if (result.finished)
				{
					//std::vector<Engine::Entity>& selectedEntities = EditorContext::Entities().GetAll();


					TransformData after{ component.position, component.rotation, component.scale };

					if (memcmp(&s_TransformBefore, &after, sizeof(TransformData)) != 0)
					{
						CommandManager::ExecuteCommand<TransformEntityCommand>(entity, s_TransformBefore, after);
					}
				}
			});


		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
			{
				Camera* camera = component.camera;

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
				if (Widget::EnumField("Type", (int&)component.type, lightTypes).changed)
				{
					component.type = (LightType)component.type;
				}
				ImGui::NextColumn();


				ImGui::Text("Color");
				ImGui::NextColumn();
				Widget::ColorField("##color", component.color);
				ImGui::NextColumn();

				ImGui::Text("Intensity");
				ImGui::NextColumn();
				Widget::FloatField("Intensity", component.intensity);
				ImGui::NextColumn();


				switch (component.type)
				{
				case LightType::Directional:
					break;
				case LightType::Point:
					ImGui::Text("Range");
					ImGui::NextColumn();
					Widget::FloatField("Range", component.range);
					ImGui::NextColumn();

					ImGui::Text("Attenuation");
					ImGui::NextColumn();
					Widget::Vec3Field("Attenuation", component.attenuation);
					ImGui::NextColumn();
					break;

				case LightType::Spot:
					ImGui::Text("Inner Angle");
					ImGui::NextColumn();
					Widget::FloatField("Inner Angle", component.innerAngle);
					ImGui::NextColumn();

					ImGui::Text("Outer Angle");
					ImGui::NextColumn();
					Widget::FloatField("Outer Angle", component.outerAngle);
					ImGui::NextColumn();

					ImGui::Text("Attenuation");
					ImGui::NextColumn();
					Widget::Vec3Field("Attenuation", component.attenuation);
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

				std::string meshButtonText = component.mesh != nullptr ? std::to_string((uint64_t)component.mesh->id) : "##Mesh";
				ImGui::Button(meshButtonText.c_str(), ImVec2(availWidth, 0.0f));

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("UUID"))
					{
						Engine::UUID droppedUUID = *static_cast<const Engine::UUID*>(payload->Data);
						if (AssetManager::GetAssetType(droppedUUID) == AssetType::Mesh)
						{
							component.mesh = AssetManager::GetAsset<Mesh>(droppedUUID).get();
						}

					}
					ImGui::EndDragDropTarget();
				}

				ImGui::NextColumn();

				ImGui::Text("Material");
				ImGui::NextColumn();
				std::string materialButtonText = component.material != nullptr ? std::to_string((uint64_t)component.material->id) : "##Material";
				ImGui::Button(materialButtonText.c_str(), ImVec2(availWidth, 0.0f));

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("UUID"))
					{
						Engine::UUID droppedUUID = *static_cast<const Engine::UUID*>(payload->Data);
						if (AssetManager::GetAssetType(droppedUUID) == AssetType::Material)
						{
							component.material = AssetManager::GetAsset<Material>(droppedUUID).get();
						}
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::NextColumn();
				ImGui::Columns(1);
			});

		DrawComponent<AudioSourceComponent>("AudioSource", entity, [](auto& component)
			{
				float availWidth = glm::max(ImGui::GetContentRegionAvail().x - 100, 100.0f);
				ImGui::Columns(2, "audio_source_body", false);
				ImGui::SetColumnWidth(0, 100);
				ImGui::SetColumnWidth(1, availWidth);

				ImGui::Text("Audio Clip");

				ImGui::NextColumn();

				std::string audioClipButtonText = component.clip != nullptr ? std::to_string((uint64_t)component.clip->id) : "##AudioClip";
				ImGui::Button(audioClipButtonText.c_str(), ImVec2(availWidth, 0.0f));
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("UUID"))
					{
						Engine::UUID droppedUUID = *static_cast<const Engine::UUID*>(payload->Data);
						if (AssetManager::GetAssetType(droppedUUID) == AssetType::Audio)
						{
							component.clip = AssetManager::GetAsset<AudioClip>(droppedUUID).get();
						}
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::NextColumn();
				ImGui::Text("Looping");
				ImGui::NextColumn();
				ImGui::Checkbox("Looping", &component.loop);
				ImGui::NextColumn();
				ImGui::Text("Volume");
				ImGui::NextColumn();
				Widget::FloatField("Volume", component.volume);
				ImGui::NextColumn();
				ImGui::Columns(1);

			});


		DrawComponent<AudioListenerComponent>("AudioListener", entity, [](auto& component)
			{
				float availWidth = glm::max(ImGui::GetContentRegionAvail().x - 100, 100.0f);
				ImGui::Columns(2, "audio_listener_body", false);
				ImGui::SetColumnWidth(0, 100);
				ImGui::SetColumnWidth(1, availWidth);

				ImGui::Text("Is Active");
				ImGui::NextColumn();
				ImGui::Checkbox("Is Active", &component.isActive);

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
				entity.AddComponent<MeshComponent>();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("AudioSource"))
			{
				entity.AddComponent<AudioSourceComponent>();
				ImGui::CloseCurrentPopup();
			}


			if (ImGui::MenuItem("AudioListener"))
			{
				entity.AddComponent<AudioListenerComponent>();
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
}
