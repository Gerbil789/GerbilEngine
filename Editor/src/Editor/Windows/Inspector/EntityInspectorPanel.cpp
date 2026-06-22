#include "EntityInspectorPanel.h"

#include "Editor/Windows/Utility/Property.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Editor/Command/TransformEntity.h"
#include "Editor/Command/PropertyChangeCommand.h"
#include "Editor/Command/ComponentSnapshotCommand.h"
#include "Editor/Command/AddComponentCommand.h"
#include "Editor/Command/RemoveComponentCommand.h"

#include "Engine/Scene/Components.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Graphics/Camera.h"
#include "Editor/Core/SelectionManager.h"
#include "Engine/Script/ScriptRegistry.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Script/Script.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui_internal.h>
#include <functional>
#include <memory>

namespace Editor
{
	struct EntityHeader
	{
		EntityHeader(entt::registry& registry, entt::entity entity)
		{
			ImGui::PushID(static_cast<int>(entity));

			PropertyField("Enabled", registry.get<Engine::IdentityComponent>(entity).enabled, { .showLabel = false });
			ImGui::SameLine();

			std::string& name = registry.get<Engine::NameComponent>(entity).name;
			if (PropertyField("Name", name, { .showLabel = false }).finished)
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

	void DrawTransform(entt::registry& registry, entt::entity entity)
	{
		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {auto before = registry.get<Engine::TransformComponent>(entity);
				auto after = before;
				after.position = { 0.0f, 0.0f, 0.0f };
				after.rotation = { 0.0f, 0.0f, 0.0f };
				after.scale = { 1.0f, 1.0f, 1.0f };
				registry.patch<Engine::TransformComponent>(entity);
				EditorCommandManager::ModifyComponent<Engine::TransformComponent>(entity, before, after); } },
		};

		ComponentHeader header("Transform", menuActions);
		if (!header.open) return;

		auto& tc = registry.get<Engine::TransformComponent>(entity);

		EditResult result;
		static TransformData s_TransformBefore;

		PropertyTable table;

		result |= PropertyField("Position", tc.position);
		result |= PropertyField("Rotation", tc.rotation);
		result |= PropertyField("Scale", tc.scale);

		if (result.started)
		{
			s_TransformBefore = { tc.position, tc.rotation, tc.scale };
		}
		else if (result.finished)
		{
			TransformData after{ tc.position, tc.rotation, tc.scale };

			if (memcmp(&s_TransformBefore, &after, sizeof(TransformData)) != 0) //TODO: float comparison is unsafe
			{
				EditorCommandManager::TransformEntity(entity, s_TransformBefore, after);
			}
		}
	}

	void DrawCamera(entt::registry& registry, entt::entity entity)
	{
		if (!registry.any_of<Engine::CameraComponent>(entity)) return;

		ComponentHeader header("Camera");
		if (!header.open) return;

		auto& component = registry.get<Engine::CameraComponent>(entity);
		Engine::Camera& camera = *component.camera;

		PropertyTable table;

		PropertyField("Primary", component.primary);

		Engine::Camera::Projection projType = camera.GetProjection();
		int currentProjection = static_cast<int>(projType);

		if (EnumField("Projection", currentProjection, { "Perspective", "Orthographic" }).changed)
		{
			camera.SetProjection(static_cast<Engine::Camera::Projection>(currentProjection));
		}

		Engine::Camera::Background bg = camera.GetBackground();
		int currentBg = static_cast<int>(bg);

		if (EnumField("Background", currentBg, { "Color", "Skybox" }).changed)
		{
			camera.SetBackground(static_cast<Engine::Camera::Background>(currentBg));
		}

		if (camera.GetBackground() == Engine::Camera::Background::Color)
		{
			PropertyField("Clear Color", camera.GetClearColor(), { .mode = DisplayMode::Color });
		}
	}

	void DrawMesh(entt::registry& registry, entt::entity entity)
	{
		if (!registry.any_of<Engine::MeshComponent>(entity)) return;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {auto before = registry.get<Engine::MeshComponent>(entity);
				auto after = before;
				after.meshId = {};
				after.materials.clear();
				EditorCommandManager::ModifyComponent<Engine::MeshComponent>(entity, before, after); }
			},

			{ "Remove", [&] {EditorCommandManager::RemoveComponent<Engine::MeshComponent>(entity); } }
		};

		ComponentHeader header("Mesh", menuActions);
		if (!header.open) return;

		Engine::MeshComponent& component = registry.get<Engine::MeshComponent>(entity);

		PropertyTable table;

		if (AssetField("Mesh", component.meshId, Engine::AssetType::Mesh).changed)
		{
			if (component.meshId)
			{
				Engine::Mesh& mesh = Engine::AssetManager::GetAsset<Engine::Mesh>(component.meshId);

				//TODO: store material count in mesh?
				uint32_t materialCount = 0;
				for (const auto& sub : mesh.GetSubMeshes())
				{
					if (sub.materialIndex > materialCount)
					{
						materialCount = sub.materialIndex;
					}
				}
				component.materials.resize(materialCount + 1);
			}
			else
			{
				component.materials.clear();
			}
		}

		ImGui::Separator();

		for (auto&& [i, id] : std::views::enumerate(component.materials))
		{
			if (AssetField(std::format("Material {}", i), id, Engine::AssetType::Material).changed)
			{
				SelectionManager::Assets.Select(id);
			}
		}
	}

	void DrawCollider(entt::registry& registry, entt::entity entity)
	{
		if (!registry.any_of<Engine::ColliderComponent>(entity)) return;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {auto before = registry.get<Engine::ColliderComponent>(entity);
				auto after = before;
				after.collisionMeshId = {};
				EditorCommandManager::ModifyComponent<Engine::ColliderComponent>(entity, before, after); }
			},

			{ "Remove", [&] {EditorCommandManager::RemoveComponent<Engine::ColliderComponent>(entity); } }
		};

		ComponentHeader header("Collider", menuActions);
		if (!header.open) return;

		auto& component = registry.get<Engine::ColliderComponent>(entity);

		PropertyTable table;

		AssetField("Mesh", component.collisionMeshId, Engine::AssetType::Mesh);
		PropertyField("Is trigger", component.isTrigger);
	}

	void DrawLight(entt::registry& registry, entt::entity entity)
	{
		if (!registry.any_of<Engine::LightComponent>(entity)) return;
		auto& component = registry.get<Engine::LightComponent>(entity);

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {auto before = component;
				auto after = before;
				after.type = Engine::LightType::Directional;
				after.color = { 1.0f, 1.0f, 1.0f };
				after.intensity = 1.0f;
				EditorCommandManager::ModifyComponent<Engine::LightComponent>(entity, before, after); }
			},

			{ "Remove", [&] {EditorCommandManager::RemoveComponent<Engine::LightComponent>(entity); } }
		};

		ComponentHeader header("Light", menuActions);
		if (!header.open) return;

		PropertyTable table;

		int current = static_cast<int>(component.type);
		if (EnumField("Type", current, { "Directional", "Spot", "Point" }).changed)
		{
			component.type = static_cast<Engine::LightType>(current);
		}

		PropertyField("Color", component.color, { .mode = DisplayMode::Color });
		PropertyField("Intensity", component.intensity, { .min = 0.0f });

		if (component.type == Engine::LightType::Spot)
		{
			PropertyField("Angle", component.angle, { .min = 0.0f, .max = 180.0f });
		}
	}

	void DrawScript(entt::registry& registry, entt::entity entity)
	{
		if (!registry.any_of<Engine::ScriptComponent>(entity)) return;

		static uint32_t id = 0;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {id = 0; auto before = registry.get<Engine::ScriptComponent>(entity);
				auto after = before;
				after.id = 0;
				after.instance = nullptr;
				EditorCommandManager::ModifyComponent<Engine::ScriptComponent>(entity, before, after); }
			},

			{ "Remove", [&] {EditorCommandManager::RemoveComponent<Engine::ScriptComponent>(entity); } }
		};

		ComponentHeader header("Script", menuActions);
		if (!header.open) return;

		PropertyTable table;

		Engine::ScriptComponent& component = registry.get<Engine::ScriptComponent>(entity);

		const auto& scripts = Engine::ScriptRegistry::GetScripts();

		{
			PropertyRow row("Script");

			if (ImGui::BeginCombo("##Combo", id > 0 ? scripts.at(id).name.c_str() : nullptr, ImGuiComboFlags_NoArrowButton))
			{
				static ImGuiTextFilter filter;
				if (ImGui::IsWindowAppearing())
				{
					ImGui::SetKeyboardFocusHere();
					filter.Clear();
				}

				filter.Draw("##Filter", -FLT_MIN);
				for (const auto& [scriptId, scriptDesc] : scripts)
				{
					const bool is_selected = (id == scriptId);
					if (filter.PassFilter(scriptDesc.name.c_str()))
					{
						if (ImGui::Selectable(scriptDesc.name.c_str(), is_selected))
						{
							id = scriptId;
							const Engine::ScriptDescriptor& desc = Engine::ScriptRegistry::GetDescriptor(scriptId);
							component.id = desc.id;
							component.instance = desc.factory();
							component.instance->m_Entity = entity;
							component.instance->OnCreate();
						}
					}
				}
				ImGui::EndCombo();
			}
		}

		if (!component.instance) return;

		ImGui::Separator();

		const Engine::ScriptDescriptor& desc = Engine::ScriptRegistry::GetDescriptor(component.id);

		for (const Engine::ScriptField& field : desc.fields)
		{
			switch (field.type)
			{
			case Engine::ScriptFieldType::Float:
			{
				PropertyField(field.name.c_str(), field.GetValue<float>(component.instance));
				break;
			}

			case Engine::ScriptFieldType::Bool:
			{
				PropertyField(field.name.c_str(), field.GetValue<bool>(component.instance));
				break;
			}

			case Engine::ScriptFieldType::Int:
			{
				PropertyField(field.name.c_str(), field.GetValue<int>(component.instance));
				break;
			}

			case Engine::ScriptFieldType::Texture:
			{
				AssetField(field.name.c_str(), field.GetValue<Engine::Uuid>(component.instance), Engine::AssetType::Texture2D);
				break;
			}

			case Engine::ScriptFieldType::AudioClip:
			{
				AssetField(field.name.c_str(), field.GetValue<Engine::Uuid>(component.instance), Engine::AssetType::Audio);
				break;
			}

			case Engine::ScriptFieldType::Mesh:
			{
				AssetField(field.name.c_str(), field.GetValue<Engine::Uuid>(component.instance), Engine::AssetType::Mesh);
				break;
			}

			case Engine::ScriptFieldType::Shader:
			{
				AssetField(field.name.c_str(), field.GetValue<Engine::Uuid>(component.instance), Engine::AssetType::Shader);
				break;
			}

			case Engine::ScriptFieldType::Material:
			{
				AssetField(field.name.c_str(), field.GetValue<Engine::Uuid>(component.instance), Engine::AssetType::Material);
				break;
			}
			}
		}
	}

	void DrawAddComponentButton(entt::registry& registry, entt::entity entity)
	{
		struct AddComponentEntry
		{
			const char* name;
			void (*add)(entt::registry&, entt::entity);
		};

		static constexpr std::array<AddComponentEntry, 5> entries
		{
			AddComponentEntry{ "Camera",        [](entt::registry& registry, entt::entity e) { auto& component = registry.emplace<Engine::CameraComponent>(e); component.camera = std::make_unique<Engine::Camera>().release(); }},
			AddComponentEntry{ "Mesh",          [](entt::registry& registry, entt::entity e) { registry.emplace<Engine::MeshComponent>(e); } },
			AddComponentEntry{ "Collider",      [](entt::registry& registry, entt::entity e) { registry.emplace<Engine::ColliderComponent>(e); } },
			AddComponentEntry{ "Light",         [](entt::registry& registry, entt::entity e) { registry.emplace<Engine::LightComponent>(e); } },
			AddComponentEntry{ "Script",				[](entt::registry& registry, entt::entity e) { registry.emplace<Engine::ScriptComponent>(e); } }
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

		ImGui::SetNextWindowSizeConstraints(
			ImVec2(300.0f, 0.0f),        // min size
			ImVec2(300.0f, 200.0f)       // max size
		);

		if (ImGui::BeginPopup("AddComponentPopup"))
		{
			static ImGuiTextFilter filter;
			if (ImGui::IsWindowAppearing())
			{
				ImGui::SetKeyboardFocusHere();
				filter.Clear();
			}
			filter.Draw("##Filter", -FLT_MIN);

			for (size_t n = 0; n < entries.size(); n++)
			{
				if (filter.PassFilter(entries[n].name))
				{
					if (ImGui::Selectable(entries[n].name))
					{
						entries[n].add(registry, entity);
						ImGui::CloseCurrentPopup();
					}
				}
			}

			ImGui::EndPopup();
		}
	}



	void EntityInspectorPanel::Draw(Engine::Uuid entityId)
	{
		Engine::Scene& scene = Engine::SceneManager::GetActiveScene();
		entt::entity entity = scene.GetEntity(entityId);
		if (entity == entt::null) return;

		entt::registry& registry = scene.GetRegistry();

		EntityHeader header(registry, entity);

		DrawTransform(registry, entity);
		DrawCamera(registry, entity);
		DrawMesh(registry, entity);
		DrawCollider(registry, entity);
		DrawLight(registry, entity);
		DrawScript(registry, entity);

		DrawAddComponentButton(registry, entity);
	}
}