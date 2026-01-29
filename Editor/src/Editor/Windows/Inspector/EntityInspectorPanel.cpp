#include "EntityInspectorPanel.h"

#include "Editor/Windows/Utility/ScopedStyle.h"
#include "Editor/Windows/Utility/Property.h"
#include "Editor/Command/CommandManager.h"
#include "Editor/Command/TransformEntity.h"
#include "Editor/Core/EditorContext.h"

#include "Engine/Scene/Components.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Camera.h"
#include "Engine/Audio/AudioClip.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui_internal.h>

#include "Engine/Script/ScriptRegistry.h"

namespace Editor
{
	struct ComponentSection
	{
		bool open = false;

		ComponentSection(const char* label, bool hasComponent)
		{
			open = hasComponent && ImGui::CollapsingHeader(label, ImGuiTreeNodeFlags_DefaultOpen);
		}

		explicit operator bool() const { return open; }
	};

	bool DrawEntityHeader(Engine::Entity entity)
	{
		if (!entity.HasComponent<Engine::NameComponent>())
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Unknown Entity");
			return false;
		}

		std::string& name = entity.GetComponent<Engine::NameComponent>().name;

		std::array<char, 256> buffer{};
		std::snprintf(buffer.data(), buffer.size(), "%s", name.c_str());

		ImGui::PushID((int)entity.GetUUID());

		ImGui::PushItemWidth(-1);
		ImGui::Checkbox("##Enabled", &entity.GetComponent<Engine::IdentityComponent>().enabled);
		ImGui::SameLine();
		if (ImGui::InputText("##Name", buffer.data(), buffer.size())) { name = std::string(buffer.data()); }


		return true;
	}

	void DrawTransform(Engine::Entity entity)
	{
		ComponentSection header("Transform", entity.HasComponent<Engine::TransformComponent>());
		if (!header) return;

		auto& tc = entity.GetComponent<Engine::TransformComponent>();

		PropertyEditResult result;
		static TransformData s_TransformBefore;

		PropertyTable table;
		if (!table) return;

		{
			PropertyRow row("Position");
			result |= Vec3Field("Position", tc.position);
		}

		{
			PropertyRow row("Rotation");
			result |= Vec3Field("Rotation", tc.rotation);
		}

		{
			PropertyRow row("Scale");
			result |= Vec3Field("Scale", tc.scale);
		}

		if (result.started)
		{
			s_TransformBefore = { tc.position, tc.rotation, tc.scale };
		}
		else if (result.finished)
		{
			TransformData after{ tc.position, tc.rotation, tc.scale };

			if (memcmp(&s_TransformBefore, &after, sizeof(TransformData)) != 0) //TODO: float comparison is unsafe
			{
				CommandManager::ExecuteCommand<TransformEntityCommand>(
					entity, s_TransformBefore, after
				);
			}
		}
	}

	void DrawCamera(Engine::Entity entity)
	{
		ComponentSection header("Camera", entity.HasComponent<Engine::CameraComponent>());
		if (!header) return;

		auto& component = entity.GetComponent<Engine::CameraComponent>();
		Engine::Camera* camera = component.camera;

		PropertyTable table;
		if (!table) return;

		{
			PropertyRow row("Projection");
			Engine::Camera::Projection projType = camera->GetProjection();
			int current = static_cast<int>(projType);

			if (EnumField("Projection", current, { "Perspective", "Orthographic" }).changed) //TODO: use static reflection? (cpp26)
			{
				camera->SetProjection(static_cast<Engine::Camera::Projection>(current));
			}
		}

		{
			PropertyRow row("Background");
			Engine::Camera::Background bg = camera->GetBackground();
			int current = static_cast<int>(bg);

			if (EnumField("Background", current, { "Color", "Skybox" }).changed) //TODO: use static reflection? (cpp26)
			{
				camera->SetBackground(static_cast<Engine::Camera::Background>(current));
			}
		}

		if (camera->GetBackground() == Engine::Camera::Background::Color)
		{
			PropertyRow row("Clear Color");

			glm::vec4 color = camera->GetClearColor();
			if (ColorField("Clear Color", color).changed)
			{
				camera->SetClearColor(color);
			}
		}
	}

	void DrawMesh(Engine::Entity entity)
	{
		ComponentSection header("Mesh", entity.HasComponent<Engine::MeshComponent>());
		if (!header) return;

		auto& component = entity.GetComponent<Engine::MeshComponent>();

		std::string meshButtonText = component.mesh != nullptr ? std::to_string((uint64_t)component.mesh->id) : "##Mesh";
		std::string materialButtonText = component.material != nullptr ? std::to_string((uint64_t)component.material->id) : "##Material";

		PropertyTable table;
		if (!table) return;

		{
			PropertyRow row("Mesh");
			ImGui::Button(meshButtonText.c_str(), ImVec2(-FLT_MIN, 0));
			DragDropTarget{}.Accept("UUID", [&](const void* data) {
				Engine::UUID id = *static_cast<const Engine::UUID*>(data);
				if (Engine::AssetManager::GetAssetType(id) == Engine::AssetType::Mesh) {
					component.mesh = Engine::AssetManager::GetAsset<Engine::Mesh>(id);
				}
				});
		}

		{
			PropertyRow row("Material");
			ImGui::Button(materialButtonText.c_str(), ImVec2(-FLT_MIN, 0));
			DragDropTarget{}.Accept("UUID", [&](const void* data) {
				Engine::UUID id = *static_cast<const Engine::UUID*>(data);
				if (Engine::AssetManager::GetAssetType(id) == Engine::AssetType::Material) {
					component.material = Engine::AssetManager::GetAsset<Engine::Material>(id);
				}
				});
		}
	}

	void DrawAudioListener(Engine::Entity entity)
	{
		ComponentSection header("AudioListener", entity.HasComponent<Engine::AudioListenerComponent>());
		if (!header) return;

		auto& component = entity.GetComponent<Engine::AudioListenerComponent>();

		PropertyTable table;
		if (!table) return;

		{
			PropertyRow row("Is Active");
			ImGui::Checkbox("Is Active", &component.isActive);
		}
	}

	void DrawAudioSource(Engine::Entity entity)
	{
		ComponentSection header("AudioSource", entity.HasComponent<Engine::AudioSourceComponent>());
		if (!header) return;

		auto& component = entity.GetComponent<Engine::AudioSourceComponent>();

		PropertyTable table;
		if (!table) return;

		{
			PropertyRow row("Audio Clip");

			std::string audioClipButtonText = component.clip != nullptr ? std::to_string((uint64_t)component.clip->id) : "##AudioClip";
			ImGui::Button(audioClipButtonText.c_str(), ImVec2(-FLT_MIN, 0.0f));
			DragDropTarget{}.Accept("UUID", [&](const void* data) {
				Engine::UUID id = *static_cast<const Engine::UUID*>(data);
				if (Engine::AssetManager::GetAssetType(id) == Engine::AssetType::Audio) {
					component.clip = Engine::AssetManager::GetAsset<Engine::AudioClip>(id);
				}
				});
		}

		{
			PropertyRow row("Looping");
			ImGui::Checkbox("Looping", &component.loop);
		}

		{
			PropertyRow row("Volume");
			FloatField("Volume", component.volume);
		}
	}

	void DrawScript(Engine::Entity entity)
	{
		ComponentSection header("Script", entity.HasComponent<Engine::ScriptComponent>());
		if (!header) return;

		auto& component = entity.GetComponent<Engine::ScriptComponent>();

		PropertyTable table;
		if (!table) return;

		const Engine::RegisteredScript* script = Engine::ScriptRegistry::Get(component.id);

		if (!script)
		{
			PropertyRow row("Script not found!");
			return;
		}

		for (auto& field : script->Desc.Fields)
		{
			switch (field.Type)
			{
			case Engine::ScriptFieldType::Float:
			{
				PropertyRow row(field.Name);
				float& value = *reinterpret_cast<float*>(reinterpret_cast<uint8_t*>(component.data) + field.Offset);
				FloatField(field.Name, value);
				break;
			}
			}
		}
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

		struct AddComponentEntry
		{
			const char* name;
			void (*add)(Engine::Entity);
		};

		static AddComponentEntry entries[] = {
		{ "Camera", [](Engine::Entity e) { e.AddComponent<Engine::CameraComponent>(); } },
		{ "Mesh",   [](Engine::Entity e) { e.AddComponent<Engine::MeshComponent>(); } },
		{ "Light",  [](Engine::Entity e) { e.AddComponent<Engine::LightComponent>(); } },
		{ "AudioSource", [](Engine::Entity e) { e.AddComponent<Engine::AudioSourceComponent>(); } },
		{ "AudioListener", [](Engine::Entity e) { e.AddComponent<Engine::AudioListenerComponent>(); } },
		{ "Script", [](Engine::Entity e)
			{
				auto& component = e.AddComponent<Engine::ScriptComponent>();
				component.id = Engine::ScriptRegistry::GetByName("PlayerController")->ID;
				component.data = malloc(sizeof(float)); // Temporary
				float value = 0.0f;
				memcpy(component.data, &value, sizeof(float));
			} },
		};

		if (ImGui::BeginPopup("AddComponent"))
		{
			for (auto& entry : entries)
			{
				if (ImGui::MenuItem(entry.name))
				{
					entry.add(entity);
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
	}

	void EntityInspectorPanel::Draw(Engine::Entity entity)
	{
		if (!entity) return;
		if (!DrawEntityHeader(entity)) return;

		DrawTransform(entity);
		DrawCamera(entity);
		DrawMesh(entity);
		DrawAudioListener(entity);
		DrawAudioSource(entity);
		DrawScript(entity);

		ImGui::Separator();

		DrawAddComponentButton(entity);

		ImGui::PopID();
	}
}