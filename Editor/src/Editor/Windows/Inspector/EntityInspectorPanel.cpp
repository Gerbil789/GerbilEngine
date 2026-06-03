#include "EntityInspectorPanel.h"

#include "Editor/Windows/Utility/ScopedStyle.h"
#include "Editor/Windows/Utility/Property.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Editor/Command/TransformEntity.h"

#include "Engine/Scene/Components.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Camera.h"
#include "Engine/Audio/AudioClip.h"

#include "Editor/Core/SelectionManager.h"

#include "Engine/Script/ScriptRegistry.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Script/Script.h"

#include "Editor/Command/PropertyChangeCommand.h"
#include "Editor/Command/ComponentSnapshotCommand.h"
#include "Editor/Command/AddComponentCommand.h"
#include "Editor/Command/RemoveComponentCommand.h"


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

			BoolField("Enabled", registry.get<Engine::IdentityComponent>(entity).enabled);
			ImGui::SameLine();

			std::string& name = registry.get<Engine::NameComponent>(entity).name;
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

	void DrawMesh(entt::registry& registry, entt::entity entity)
	{
		if (!registry.any_of<Engine::MeshComponent>(entity)) return;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {auto before = registry.get<Engine::MeshComponent>(entity);
				auto after = before;
				after.meshId = 0;
				after.materials.clear();
				EditorCommandManager::ModifyComponent<Engine::MeshComponent>(entity, before, after); }
			},

			{ "Remove", [&] {EditorCommandManager::RemoveComponent<Engine::MeshComponent>(entity); } }
		};

		ComponentHeader header("Mesh", menuActions);
		if (!header.open) return;

		auto& component = registry.get<Engine::MeshComponent>(entity);

		PropertyTable table;
		if (!table) return;

		{
			PropertyRow row("Mesh");
			if(component.meshId)
			{
				Engine::Mesh& mesh = Engine::AssetManager::GetAsset<Engine::Mesh>(component.meshId);
				ImGui::Button(mesh.EditorOnly.name.c_str(), ImVec2(-FLT_MIN, 0));
			}
			else
			{
				ImGui::Button("##Mesh", ImVec2(-FLT_MIN, 0));
			}

			DragDropTarget{}.AcceptAsset<Engine::AssetType::Mesh>([&](Engine::Uuid id) {component.meshId = id; });
		}

		if (!component.meshId) return;

		auto materials = component.materials; //TODO: dont use index for loop?

		for (size_t i = 0; i < materials.size(); i++)
		{
			auto materialId = materials[i];
			std::string text = "##Material";
			if (materialId)
			{
				text = Engine::AssetManager::GetAsset<Engine::Material>(materialId).EditorOnly.name;
			}

			PropertyRow row("Material");
			ImGui::PushID(static_cast<int>(i));
			if (ImGui::Button(text.c_str(), ImVec2(-FLT_MIN, 0)))
			{
				SelectionManager::Select(SelectionType::Asset, materialId);
			}
			DragDropTarget{}.AcceptAsset<Engine::AssetType::Material>([&](Engine::Uuid id) { component.materials[i] = id; });
			ImGui::PopID();
		}
	}

	void DrawCollider(entt::registry& registry, entt::entity entity)
	{
		if (!registry.any_of<Engine::ColliderComponent>(entity)) return;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {auto before = registry.get<Engine::ColliderComponent>(entity);
				auto after = before;
				after.meshId = 0;
				EditorCommandManager::ModifyComponent<Engine::ColliderComponent>(entity, before, after); }
			},

			{ "Remove", [&] {EditorCommandManager::RemoveComponent<Engine::ColliderComponent>(entity); } }
		};

		ComponentHeader header("Collider", menuActions);
		if (!header.open) return;

		auto& component = registry.get<Engine::ColliderComponent>(entity);

		std::string meshText = component.meshId ? Engine::AssetManager::GetAsset<Engine::Mesh>(component.meshId).EditorOnly.name : "##Mesh";


		PropertyTable table;
		if (!table) return;

		{
			PropertyRow row("Mesh");
			ImGui::Button(meshText.c_str(), ImVec2(-FLT_MIN, 0));
			DragDropTarget{}.AcceptAsset<Engine::AssetType::Mesh>([&](Engine::Uuid id) { component.meshId = id; });
		}

		{
			PropertyRow row("Is trigger");
			BoolField("Is trigger", component.isTrigger);
		}
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
		if (!table) return;

		{
			PropertyRow row("Type");
			Engine::LightType type = component.type;
			int current = static_cast<int>(type);
			if (EnumField("Type", current, { "Directional", "Spot", "Point" }).changed)
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

		if (component.type == Engine::LightType::Spot)
		{
			{
				PropertyRow row("Angle");
				FloatField("Angle", component.angle, 0.0f, 180.0f);
			}
		}
	}

	void DrawScript(entt::registry& registry, entt::entity entity)
	{
		if (!registry.any_of<Engine::ScriptComponent>(entity)) return;

		static int id = -1;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {id = -1; auto before = registry.get<Engine::ScriptComponent>(entity);
				auto after = before;
				after.id.clear();
				after.instance = nullptr;
				EditorCommandManager::ModifyComponent<Engine::ScriptComponent>(entity, before, after); }
			},

			{ "Remove", [&] {EditorCommandManager::RemoveComponent<Engine::ScriptComponent>(entity); } }
		};

		ComponentHeader header("Script", menuActions);
		if (!header.open) return;

		PropertyTable table;
		if (!table) return;

		Engine::ScriptComponent& component = registry.get<Engine::ScriptComponent>(entity);

		const std::vector<std::string>& scriptNames = Engine::g_ScriptRegistry.GetAllScriptNames();

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
				for (size_t n = 0; n < scriptNames.size(); n++)
				{
					const bool is_selected = (id == static_cast<int>(n));
					if (filter.PassFilter(scriptNames[n].c_str()))
					{
						if (ImGui::Selectable(scriptNames[n].c_str(), is_selected))
						{
							id = static_cast<int>(n);
							const Engine::ScriptDescriptor& desc = Engine::g_ScriptRegistry.GetDescriptor(scriptNames[n]);
							component.id = desc.name;
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

		Engine::ScriptDescriptor& desc = Engine::g_ScriptRegistry.GetDescriptor(component.id);

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
				FloatField(field.name.c_str(), value);
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
				TextureField(field.name, texture->id);
				break;
			}

			case Engine::ScriptFieldType::AudioClip:
			{
				Engine::AudioClip*& audioClip = *reinterpret_cast<Engine::AudioClip**>(fieldPtr);
				AudioClipField(field.name, audioClip);
				break;
			}

			case Engine::ScriptFieldType::Mesh:
			{
				Engine::Mesh*& mesh = *reinterpret_cast<Engine::Mesh**>(fieldPtr);
				MeshField(field.name, mesh);
				break;
			}

			case Engine::ScriptFieldType::Shader:
			{
				Engine::Shader*& shader = *reinterpret_cast<Engine::Shader**>(fieldPtr);
				ShaderField(field.name, shader);
				break;
			}

			case Engine::ScriptFieldType::Material:
			{
				Engine::Material*& material = *reinterpret_cast<Engine::Material**>(fieldPtr);
				MaterialField(field.name, material);
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
		auto entity = scene.GetEntity(entityId);
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