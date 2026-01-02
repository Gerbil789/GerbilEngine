#include "EntityInspectorPanel.h"
#include "Editor/Components/ScopedStyle.h"
#include "Engine/Scene/Components.h"
#include "Editor/Components/Widgets.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Editor/Command/CommandManager.h"
#include "Editor/Command/TransformEntity.h"
#include "Engine/Audio/AudioClip.h"
#include "Editor/Core/EditorContext.h"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui_internal.h>

namespace Editor
{
	void DrawName(Engine::Entity entity)
	{
		if (entity.HasComponent<Engine::NameComponent>())
		{
			std::string& name = entity.GetComponent<Engine::NameComponent>().name;

			std::array<char, 256> buffer{};
			std::snprintf(buffer.data(), buffer.size(), "%s", name.c_str());

			ImGui::PushID((int)entity.GetUUID());

			ImGui::PushItemWidth(-1);
			ImGui::Checkbox("##Enabled", &entity.GetComponent<Engine::IdentityComponent>().enabled);
			ImGui::SameLine();
			if (ImGui::InputText("##Name", buffer.data(), buffer.size())) { name = std::string(buffer.data()); }
			ImGui::PopID();
		}
	}

	void DrawTransform(Engine::Entity entity)
	{
		if (!entity.HasComponent<Engine::TransformComponent>()) return;

		auto& tc = entity.GetComponent<Engine::TransformComponent>();

		WidgetResult result;
		static TransformData s_TransformBefore;

		float availWidth = glm::max(ImGui::GetContentRegionAvail().x - 100, 100.0f);
		ImGui::Columns(2, "transform", false);
		ImGui::SetColumnWidth(0, 100);
		ImGui::SetColumnWidth(1, availWidth);

		auto drawField = [&result](const char* label, glm::vec3& value)
			{
				ImGui::Text("%s", label);
				ImGui::NextColumn();
				result |= Widget::Vec3Field(label, value);
				ImGui::NextColumn();
			};

		drawField("Position", tc.position);
		drawField("Rotation", tc.rotation);
		drawField("Scale", tc.scale);
		ImGui::Columns(1);

		if (result.started)
		{
			s_TransformBefore = { tc.position, tc.rotation, tc.scale };
		}
		else if (result.finished)
		{
			//std::vector<Engine::Entity>& selectedEntities = EditorContext::Entities().GetAll();
			TransformData after{ tc.position, tc.rotation, tc.scale };

			if (memcmp(&s_TransformBefore, &after, sizeof(TransformData)) != 0)
			{
				CommandManager::ExecuteCommand<TransformEntityCommand>(entity, s_TransformBefore, after);
			}
		}
	}

	void DrawMesh(Engine::Entity entity)
	{
		if (!entity.HasComponent<Engine::MeshComponent>()) return;

		auto& component = entity.GetComponent<Engine::MeshComponent>();

		if (!ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			return;
		}


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
				if (Engine::AssetManager::GetAssetType(droppedUUID) == Engine::AssetType::Mesh)
				{
					component.mesh = Engine::AssetManager::GetAsset<Engine::Mesh>(droppedUUID).get();
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
				if (Engine::AssetManager::GetAssetType(droppedUUID) == Engine::AssetType::Material)
				{
					component.material = Engine::AssetManager::GetAsset<Engine::Material>(droppedUUID).get();
				}
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::NextColumn();
		ImGui::Columns(1);

	}

	void DrawAudioListener(Engine::Entity entity)
	{
		if (!entity.HasComponent<Engine::AudioListenerComponent>()) return;

		auto& component = entity.GetComponent<Engine::AudioListenerComponent>();

		if (!ImGui::CollapsingHeader("AudioListener", ImGuiTreeNodeFlags_DefaultOpen))
		{
			return;
		}

		float availWidth = glm::max(ImGui::GetContentRegionAvail().x - 100, 100.0f);
		ImGui::Columns(2, "audio_listener_body", false);
		ImGui::SetColumnWidth(0, 100);
		ImGui::SetColumnWidth(1, availWidth);

		ImGui::Text("Is Active");
		ImGui::NextColumn();
		ImGui::Checkbox("Is Active", &component.isActive);

		ImGui::NextColumn();
		ImGui::Columns(1);
	}

	void DrawAudioSource(Engine::Entity entity)
	{
		if (!entity.HasComponent<Engine::AudioSourceComponent>()) return;

		auto& component = entity.GetComponent<Engine::AudioSourceComponent>();

		if (!ImGui::CollapsingHeader("AudioSource", ImGuiTreeNodeFlags_DefaultOpen))
		{
			return;
		}

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
				if (Engine::AssetManager::GetAssetType(droppedUUID) == Engine::AssetType::Audio)
				{
					component.clip = Engine::AssetManager::GetAsset<Engine::AudioClip>(droppedUUID).get();
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
	}

	void DrawAddComponentButton(Engine::Entity entity)
	{
		ScopedStyle style({
			{ ImGuiStyleVar_FramePadding, ImVec2(20, 5) },
			{ ImGuiStyleVar_FrameRounding, 7.5f }
			});

		ImVec2 window_size = ImGui::GetWindowSize();
		ImVec2 button_size = ImVec2{ 200, 30 };

		float window_center_x = window_size.x / 2.0f;
		float button_center_x = button_size.x / 2.0f;

		ImGui::SetCursorPosX(window_center_x - button_center_x);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10); 		//set Y offset

		if (ImGui::Button("Add Component", button_size))
		{
			ImGui::OpenPopup("AddComponent");
		}

		auto menuItem = [&](const char* label, auto addFn)
			{
				if (ImGui::MenuItem(label))
				{
					addFn();
					ImGui::CloseCurrentPopup();
				}
			};

		if (ImGui::BeginPopup("AddComponent"))
		{
			menuItem("Camera", [&] { entity.AddComponent<Engine::CameraComponent>(); });
			menuItem("Light", [&] { entity.AddComponent<Engine::LightComponent>(); });
			menuItem("Mesh", [&] { entity.AddComponent<Engine::MeshComponent>(); });
			menuItem("AudioSource", [&] { entity.AddComponent<Engine::AudioSourceComponent>(); });
			menuItem("AudioListener", [&] { entity.AddComponent<Engine::AudioListenerComponent>(); });

			ImGui::EndPopup();
		}
	}

	void EntityInspectorPanel::Draw(Engine::Entity entity)
	{
		if (!entity) return;

		DrawName(entity);
		DrawTransform(entity);
		DrawMesh(entity);
		DrawAudioListener(entity);
		DrawAudioSource(entity);
		ImGui::Separator();
		DrawAddComponentButton(entity);
	}
}
