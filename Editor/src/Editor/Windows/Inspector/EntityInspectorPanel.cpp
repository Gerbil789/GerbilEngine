#include "EntityInspectorPanel.h"

#include "Editor/Windows/Utility/ScopedStyle.h"
#include "Editor/Windows/Utility/Property.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Editor/Command/TransformEntity.h"

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
#include "Engine/Script/Script.h"

#include "Editor/Command/PropertyChangeCommand.h"
#include "Editor/Command/ComponentSnapshotCommand.h"
#include "Editor/Command/AddComponentCommand.h"
#include "Editor/Command/RemoveComponentCommand.h"

#include <memory>

namespace Editor
{
	struct EntityHeader
	{
		EntityHeader(Engine::Entity entity)
		{
			ImGui::PushID(static_cast<int>(entity.GetUUID()));

			BoolField("Enabled", entity.GetComponent<Engine::IdentityComponent>().enabled);
			ImGui::SameLine();

			std::string& name = entity.GetComponent<Engine::NameComponent>().name;
			if (TextField("Name", name).finished)
			{
				//TODO: somehow store the original name
				//EditorCommandManager::ModifyComponent<Engine::NameComponent>(entity, { name }, { name });
			}
		}

		~EntityHeader()
		{
			ImGui::PopID();
		}
	};

	struct ComponentMenuAction
	{
		const std::string label;
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
					if (ImGui::MenuItem(a.label.c_str(), nullptr, false, a.enabled) && a.enabled)
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
			{ "Reset", [this] {auto before = m_Entity.GetComponent<Engine::TransformComponent>(); auto after = before; after.Reset();
						EditorCommandManager::ModifyComponent<Engine::TransformComponent>(m_Entity, before, after); } },
		};

		ComponentHeader header("Transform", menuActions);
		if (!header.open) return;

		auto& tc = m_Entity.GetComponent<Engine::TransformComponent>();

		EditResult result;
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
				EditorCommandManager::Enqueue(std::make_unique<TransformEntityCommand>(m_Entity, s_TransformBefore, after));
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
			{ "Reset", [this] {auto before = m_Entity.GetComponent<Engine::MeshComponent>(); auto after = before; after.Reset();
						EditorCommandManager::ModifyComponent<Engine::MeshComponent>(m_Entity, before, after); } },

			{ "Remove", [this] {EditorCommandManager::RemoveComponent<Engine::MeshComponent>(m_Entity); } }
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
				Engine::Uuid id = *static_cast<const Engine::Uuid*>(data);
				if (Engine::AssetManager::GetAssetType(id) == Engine::AssetType::Mesh) {
					component.mesh = Engine::AssetManager::GetAsset<Engine::Mesh>(id);
				}
				});
		}

		{
			PropertyRow row("Material");
			ImGui::Button(materialText.c_str(), ImVec2(-FLT_MIN, 0));
			DragDropTarget{}.Accept("UUID", [&](const void* data) {
				Engine::Uuid id = *static_cast<const Engine::Uuid*>(data);
				if (Engine::AssetManager::GetAssetType(id) == Engine::AssetType::Material) {
					component.material = Engine::AssetManager::GetAsset<Engine::Material>(id);
				}
				});
		}
	}

	void EntityInspectorPanel::DrawLight()
	{
		if (!m_Entity.HasComponent<Engine::LightComponent>()) return;
		auto& component = m_Entity.GetComponent<Engine::LightComponent>();

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [this] {auto before = m_Entity.GetComponent<Engine::LightComponent>(); auto after = before; after.Reset();
						EditorCommandManager::ModifyComponent<Engine::LightComponent>(m_Entity, before, after); } },

			{ "Remove", [this] {EditorCommandManager::RemoveComponent<Engine::LightComponent>(m_Entity); } }
		};

		ComponentHeader header("Light", menuActions);
		if (!header.open) return;

		PropertyTable table;
		if (!table) return;

		{
			PropertyRow row("Type");
			Engine::LightType type = component.type;
			int current = static_cast<int>(type);
			if (EnumField("Type", current, { "Point", "Directional", "Spot" }).changed)
			{
				component.type = static_cast<Engine::LightType>(current);
			}
		}

		{
			PropertyRow row("Color");
			ColorField("Color", component.color);
		}

		{
			PropertyRow row("Intensity");
			FloatField("Intensity", component.intensity, 0.0f);
		}

		if (component.type != Engine::LightType::Directional)
		{
			PropertyRow row("Range");
			FloatField("Range", component.range, 0.0f);
		}

		if (component.type == Engine::LightType::Spot)
		{
			{
				PropertyRow row("Angle");
				FloatField("Angle", component.angle, 0.0f, 180.0f);
			}
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
			BoolField("Is Active", component.isActive);
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
				Engine::Uuid id = *static_cast<const Engine::Uuid*>(data);
				if (Engine::AssetManager::GetAssetType(id) == Engine::AssetType::Audio) {
					component.clip = Engine::AssetManager::GetAsset<Engine::AudioClip>(id);
				}
				});
		}

		{
			PropertyRow row("Play on awake");
			BoolField("PlayOnAwake", component.playOnAwake);

			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Not implemented");
				ImGui::EndTooltip();
			}
		}

		{
			PropertyRow row("Looping");
			if (BoolField("Looping", component.loop).changed)
			{
				component.SetLooping(component.loop);
			}
		}

		{
			PropertyRow row("Volume");
			if (FloatField("Volume", component.volume, 0.0f, 1.0f, 0.01f).changed)
			{
				component.SetVolume(component.volume);
			}
		}
	}

	void EntityInspectorPanel::DrawScript()
	{
		if (!m_Entity.HasComponent<Engine::ScriptComponent>()) return;

		static int id = -1;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [this] {id = -1; auto before = m_Entity.GetComponent<Engine::ScriptComponent>(); auto after = before; after.Reset();
						EditorCommandManager::ModifyComponent<Engine::ScriptComponent>(m_Entity, before, after); } },

			{ "Remove", [this] {EditorCommandManager::RemoveComponent<Engine::ScriptComponent>(m_Entity); } }
		};

		ComponentHeader header("Script", menuActions);
		if (!header.open) return;

		PropertyTable table;
		if (!table) return;

		Engine::ScriptRegistry& registry = Engine::ScriptRegistry::Get();
		Engine::ScriptComponent& component = m_Entity.GetComponent<Engine::ScriptComponent>();

		const auto& scriptNames = registry.GetAllScriptNames();

		{
			PropertyRow row("Script");

			if (ImGui::BeginCombo("##Combo", id >= 0 ? scriptNames[id].c_str() : nullptr, ImGuiComboFlags_NoArrowButton))
			{
				static ImGuiTextFilter filter; 
				if (ImGui::IsWindowAppearing()) 
				{ 
					ImGui::SetKeyboardFocusHere(); 
					filter.Clear(); 
				} 
				
				filter.Draw("##Filter", -FLT_MIN); 
				for (int n = 0; n < scriptNames.size(); n++)
				{ 
					const bool is_selected = (id == n); 
					if (filter.PassFilter(scriptNames[n].c_str()))
					{ 
						if (ImGui::Selectable(scriptNames[n].c_str(), is_selected)) 
						{ 
							id = n;
							const Engine::ScriptDescriptor& desc = registry.GetDescriptor(scriptNames[n]);
							component.id = desc.name;
							component.instance = desc.factory();
							component.instance->Self = m_Entity;
							component.instance->OnCreate();
						} 
					} 
				}
				ImGui::EndCombo();
			}
		}

		if (!component.instance) return;

		ImGui::Separator();

		Engine::ScriptDescriptor& desc = registry.GetDescriptor(component.id);

		for (const Engine::ScriptField& field : desc.fields)
		{
			PropertyRow row(field.name.c_str());

			std::byte* base = reinterpret_cast<std::byte*>(component.instance);
			void* fieldPtr = base + field.offset;

			switch (field.type)
			{
			case Engine::ScriptFieldType::Float:
			{
				float& value = *reinterpret_cast<float*>(fieldPtr);
				FloatField(field.name.c_str(), value, FLT_MIN, FLT_MAX);
				break;
			}

			case Engine::ScriptFieldType::Bool:
			{
				bool& value = *reinterpret_cast<bool*>(fieldPtr);
				BoolField(field.name.c_str(), value);
				break;
			}

			case Engine::ScriptFieldType::Int:
			{
				int& value = *reinterpret_cast<int*>(fieldPtr);
				IntField(field.name.c_str(), value);
				break;
			}

			case Engine::ScriptFieldType::Texture:
			{
				Engine::Texture2D*& texture = *reinterpret_cast<Engine::Texture2D**>(fieldPtr);
				TextureField(field.name, texture);
				break;
			}

			case Engine::ScriptFieldType::AudioClip:
			{
				Engine::AudioClip*& audioClip = *reinterpret_cast<Engine::AudioClip**>(fieldPtr);
				AudioClipField(field.name, audioClip);
				break;
			}

			}
		}
	}

	void EntityInspectorPanel::DrawAddComponentButton()
	{
		struct AddComponentEntry
		{
			const char* name;
			void (*add)(Engine::Entity);
		};

		static constexpr std::array<AddComponentEntry, 6> entries
		{
			AddComponentEntry{ "Camera",        [](Engine::Entity e) { auto& component = e.AddComponent<Engine::CameraComponent>(); component.camera = std::make_unique<Engine::Camera>().release(); }},
			AddComponentEntry{ "Mesh",          [](Engine::Entity e) { e.AddComponent<Engine::MeshComponent>(); } },
			AddComponentEntry{ "Light",         [](Engine::Entity e) { e.AddComponent<Engine::LightComponent>(); } },
			AddComponentEntry{ "AudioSource",   [](Engine::Entity e) { e.AddComponent<Engine::AudioSourceComponent>(); } },
			AddComponentEntry{ "AudioListener", [](Engine::Entity e) { e.AddComponent<Engine::AudioListenerComponent>(); } },
			AddComponentEntry{ "Script",				[](Engine::Entity e) { e.AddComponent<Engine::ScriptComponent>(); } }
		};

		ImGui::Separator();

		const float buttonWidth = 200.0f;
		float cursorX = (ImGui::GetContentRegionAvail().x - buttonWidth) * 0.5f;
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cursorX);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

		if (ImGui::Button("Add Component", { buttonWidth, 30.0f }))
		{
			ImGui::OpenPopup("AddComponentPopup");
		}

		ImGui::SetNextWindowSize({ 300, 200 });
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetCursorScreenPos().x + (ImGui::GetContentRegionAvail().x - 300) * 0.5f, ImGui::GetCursorScreenPos().y), ImGuiCond_Always);

		if (ImGui::BeginPopup("AddComponentPopup"))
		{
			static ImGuiTextFilter filter;
			if (ImGui::IsWindowAppearing())
			{
				ImGui::SetKeyboardFocusHere();
				filter.Clear();
			}
			filter.Draw("##Filter", -FLT_MIN);

			for (int n = 0; n < entries.size(); n++)
			{
				if (filter.PassFilter(entries[n].name))
				{
					if (ImGui::Selectable(entries[n].name))
					{
						entries[n].add(m_Entity);
						ImGui::CloseCurrentPopup();
					}
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
		DrawLight();
		DrawAudioListener();
		DrawAudioSource();
		DrawScript();

		DrawAddComponentButton();
	}
}