#include "EntityInspectorPanel.h"

#include "Editor/Windows/Utility/ScopedStyle.h"
#include "Editor/Windows/Utility/Property.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Editor/Command/Entity/TransformEntity.h"
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
#include <functional>

#include "Engine/Script/ScriptRegistry.h"

#include "Editor/Command/Component/MeshCommand.h"
#include "Editor/Command/Component/TransformCommand.h"

namespace Editor
{
	struct EntityHeader
	{
		std::array<char, 256> buffer{};

		EntityHeader(Engine::Entity entity)
		{
			auto& name = entity.GetComponent<Engine::NameComponent>().name;

			if (buffer[0] == '\0')
			{
				std::snprintf(buffer.data(), buffer.size(), "%s", name.c_str());
			}

			ImGui::PushID((int)entity.GetUUID());

			ImGui::PushItemWidth(-1);
			ImGui::Checkbox("##Enabled", &entity.GetComponent<Engine::IdentityComponent>().enabled);
			ImGui::SameLine();
			if (ImGui::InputText("##Name", buffer.data(), buffer.size())) { name = buffer.data(); }
		}

		~EntityHeader()
		{
			ImGui::PopID();
		}
	};

	struct ComponentMenuAction
	{
		const char* label;
		std::function<void()> action;
		bool enabled = true;
	};

	struct ComponentHeader
	{
		bool open = false;

		ComponentHeader(const char* label, std::initializer_list<ComponentMenuAction> actions = {})
		{
			open = ImGui::CollapsingHeader(label, ImGuiTreeNodeFlags_DefaultOpen);

			if (actions.size() == 0) return;

			if (ImGui::BeginPopupContextItem())
			{
				for (const auto& a : actions)
				{
					if (!a.label) continue;

					bool enabled = a.action && a.enabled;

					if (ImGui::MenuItem(a.label, nullptr, false, enabled) && enabled)
					{
						a.action();
					}
				}
				ImGui::EndPopup();
			}
		}
	};

	void EntityInspectorPanel::DrawTransform()
	{
		if (!m_Entity.HasComponent<Engine::TransformComponent>()) return;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [this] { EditorCommandManager::EnqueueCommand<ResetTransformComponentCommand>(m_Entity); } }
		};

		ComponentHeader header("Transform", menuActions);
		if (!header.open) return;

		auto& tc = m_Entity.GetComponent<Engine::TransformComponent>();

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
				EditorCommandManager::ExecuteCommand<TransformEntityCommand>(m_Entity, s_TransformBefore, after);
			}
		}
	}

	void EntityInspectorPanel::DrawCamera()
	{
		if (!m_Entity.HasComponent<Engine::CameraComponent>()) return;

		ComponentHeader header("Camera");
		if (!header.open) return;

		auto& component = m_Entity.GetComponent<Engine::CameraComponent>();
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

	void EntityInspectorPanel::DrawMesh()
	{
		if (!m_Entity.HasComponent<Engine::MeshComponent>()) return;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [this] { EditorCommandManager::EnqueueCommand<ResetMeshComponentCommand>(m_Entity); } },
			{ "Remove", [this] { EditorCommandManager::EnqueueCommand<RemoveMeshComponentCommand>(m_Entity); } }
		};

		ComponentHeader header("Mesh", menuActions);
		if (!header.open) return;

		auto& component = m_Entity.GetComponent<Engine::MeshComponent>();

		std::string meshText = component.mesh != nullptr ? Engine::AssetManager::GetAssetName(component.mesh->id) : "##Mesh";
		std::string materialText = component.material != nullptr ? Engine::AssetManager::GetAssetName(component.material->id) : "##Material";

		PropertyTable table;
		if (!table) return;

		{
			PropertyRow row("Mesh");
			ImGui::Button(meshText.c_str(), ImVec2(-FLT_MIN, 0));
			DragDropTarget{}.Accept("UUID", [&](const void* data) {
				Engine::UUID id = *static_cast<const Engine::UUID*>(data);
				if (Engine::AssetManager::GetAssetType(id) == Engine::AssetType::Mesh) {
					component.mesh = Engine::AssetManager::GetAsset<Engine::Mesh>(id);
				}
				});
		}

		{
			PropertyRow row("Material");
			ImGui::Button(materialText.c_str(), ImVec2(-FLT_MIN, 0));
			DragDropTarget{}.Accept("UUID", [&](const void* data) {
				Engine::UUID id = *static_cast<const Engine::UUID*>(data);
				if (Engine::AssetManager::GetAssetType(id) == Engine::AssetType::Material) {
					component.material = Engine::AssetManager::GetAsset<Engine::Material>(id);
				}
				});
		}
	}

	void EntityInspectorPanel::DrawAudioListener()
	{
		if (!m_Entity.HasComponent<Engine::AudioListenerComponent>()) return;

		ComponentHeader header("AudioListener");
		if (!header.open) return;

		auto& component = m_Entity.GetComponent<Engine::AudioListenerComponent>();

		PropertyTable table;
		if (!table) return;

		{
			PropertyRow row("Is Active");
			ImGui::Checkbox("Is Active", &component.isActive);
		}
	}

	void EntityInspectorPanel::DrawAudioSource()
	{
		if (!m_Entity.HasComponent<Engine::AudioSourceComponent>()) return;

		ComponentHeader header("AudioSource");
		if (!header.open) return;

		auto& component = m_Entity.GetComponent<Engine::AudioSourceComponent>();

		PropertyTable table;
		if (!table) return;

		{
			PropertyRow row("Audio Clip");

			std::string audioClipButtonText = component.clip != nullptr ? Engine::AssetManager::GetAssetName(component.clip->id) : "##AudioClip";
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

	void EntityInspectorPanel::DrawScript()
	{
		if (!m_Entity.HasComponent<Engine::ScriptComponent>()) return;

		ComponentHeader header("Script");
		if (!header.open) return;

		auto& component = m_Entity.GetComponent<Engine::ScriptComponent>();

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

	void EntityInspectorPanel::DrawAddComponentButton()
	{
		ImGui::Separator();

		ScopedStyle style
		{
			{ ImGuiStyleVar_FramePadding, ImVec2(20, 5) },
			{ ImGuiStyleVar_FrameRounding, 7.5f }
		};

		const float buttonWidth = 200.0f;
		float cursorX = (ImGui::GetContentRegionAvail().x - buttonWidth) * 0.5f;
		if (cursorX > 0.0f)
		{
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cursorX);
		}

		if (ImGui::Button("Add Component", { buttonWidth, 30.0f }))
		{
			ImGui::OpenPopup("AddComponent");
		}

		struct AddComponentEntry
		{
			const char* name;
			void (*add)(Engine::Entity);
		};

		static constexpr std::array<AddComponentEntry, 6> entries
		{
			AddComponentEntry{ "Camera",        [](Engine::Entity e) { auto& component = e.AddComponent<Engine::CameraComponent>(); component.camera = new Engine::Camera(); }},
			AddComponentEntry{ "Mesh",          [](Engine::Entity e) { e.AddComponent<Engine::MeshComponent>(); } },
			AddComponentEntry{ "Light",         [](Engine::Entity e) { e.AddComponent<Engine::LightComponent>(); } },
			AddComponentEntry{ "AudioSource",   [](Engine::Entity e) { e.AddComponent<Engine::AudioSourceComponent>(); } },
			AddComponentEntry{ "AudioListener", [](Engine::Entity e) { e.AddComponent<Engine::AudioListenerComponent>(); } },
			AddComponentEntry{ "Script",				[](Engine::Entity e)
			{
				auto& component = e.AddComponent<Engine::ScriptComponent>();
				component.id = Engine::ScriptRegistry::GetByName("PlayerController")->ID;
				component.data = malloc(sizeof(float)); // Temporary
				float value = 0.0f;
				memcpy(component.data, &value, sizeof(float));
			} }
		};

		if (ImGui::BeginPopup("AddComponent"))
		{
			for (auto& entry : entries)
			{
				if (ImGui::MenuItem(entry.name))
				{
					entry.add(m_Entity);
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
	}

	void EntityInspectorPanel::Draw(Engine::Entity entity)
	{
		if (!entity) return;
		m_Entity = entity;

		EntityHeader header(m_Entity);

		DrawTransform();
		DrawCamera();
		DrawMesh();
		DrawAudioListener();
		DrawAudioSource();
		DrawScript();

		DrawAddComponentButton();
	}
}