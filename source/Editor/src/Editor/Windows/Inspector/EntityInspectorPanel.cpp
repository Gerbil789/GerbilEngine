#include "EntityInspectorPanel.h"

#include "Editor/Windows/Utility/Property.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Editor/Command/PropertyChangeCommand.h"
#include "Editor/Command/ComponentSnapshotCommand.h"
#include "Editor/Command/AddComponentCommand.h"
#include "Editor/Command/RemoveComponentCommand.h"

#include "Engine/Core/Components.h"
#include "Engine/Core/SceneManager.h"
#include "Engine/Asset/AssetManager.h"
#include "Editor/Core/SelectionManager.h"
#include "Engine/Script/ScriptRegistry.h"
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
		EntityHeader(Engine::Entity entity)
		{
			ImGui::PushID(static_cast<int>(entity.GetHandle()));

			bool isActive = entity.IsActive();
			if (PropertyField("Enabled", isActive, { .showLabel = false }).finished)
			{
				entity.SetActive(isActive);
			}
			ImGui::SameLine();

			std::string& name = entity.GetComponent<Engine::NameComponent>().name;
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

	void DrawTransform(Engine::Entity entity)
	{
		if (!entity.HasComponent<Engine::TransformComponent>()) return;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {
				auto before = entity.GetComponent<Engine::TransformComponent>();
				auto after = Engine::TransformComponent{};
				EditorCommandManager::ModifyComponent<Engine::TransformComponent>(entity, before, after);
				entity.AddTag<Engine::TransformDirty>();
			}},
			{ "Remove", [&] {EditorCommandManager::RemoveComponent<Engine::TransformComponent>(entity); } }
		};

		ComponentHeader header("Transform", menuActions);
		if (!header.open) return;

		auto& tc = entity.GetComponent<Engine::TransformComponent>();

		EditResult result;
		static Engine::TransformComponent s_TransformBefore;

		PropertyTable table;

		result |= PropertyField("Position", tc.position);
		result |= PropertyField("Rotation", tc.rotation, { .useDegrees = true });
		result |= PropertyField("Scale", tc.scale);

		if (result.started)
		{
			s_TransformBefore = { tc.position, tc.rotation, tc.scale };
		}
		else if (result.finished)
		{
			EditorCommandManager::ModifyComponent<Engine::TransformComponent>(entity, s_TransformBefore, tc);
		}
		else if (result.changed)
		{
			entity.AddTag<Engine::TransformDirty>();

			if(entity.HasComponent<Engine::CameraComponent>())
			{
				entity.AddTag<Engine::CameraProjectionDirty>();
				entity.AddTag<Engine::CameraViewDirty>();
			}
		}
	}

	void DrawCamera(Engine::Entity entity)
	{
		if (!entity.HasComponent<Engine::CameraComponent>()) return;

		ComponentHeader header("Camera");
		if (!header.open) return;

		auto& cc = entity.GetComponent<Engine::CameraComponent>();

		PropertyTable table;

		bool primary = entity.HasTag<Engine::PrimaryCameraTag>();

		if(PropertyField("Primary", primary).changed)
		{
			if(primary)
			{
				entt::registry& registry = entity.GetScene()->GetRegistry();
				auto view = registry.view<Engine::PrimaryCameraTag>(entt::exclude<Engine::EditorTag>);
				registry.remove<Engine::PrimaryCameraTag>(view.begin(), view.end());

				entity.AddTag<Engine::PrimaryCameraTag>();
			}
			else
			{
				entity.RemoveTag<Engine::PrimaryCameraTag>();
			}
		}

		int currentProjection = static_cast<int>(cc.projectionType);
		if (EnumField("Projection", currentProjection, { "Perspective", "Orthographic" }).changed)
		{
			cc.projectionType = static_cast<Engine::CameraComponent::Projection>(currentProjection);
			entity.AddTag<Engine::CameraProjectionDirty>();
		}

		EnumField("Background", (int&)cc.background, { "Color", "Skybox" });

		if (cc.background == Engine::CameraComponent::Background::Color)
		{
			PropertyField("Clear Color", cc.clearColor, { .mode = DisplayMode::Color });
		}
	}

	void DrawMesh(Engine::Entity entity)
	{
		if (!entity.HasComponent<Engine::MeshComponent>()) return;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {auto before = entity.GetComponent<Engine::MeshComponent>();
				auto after = before;
				after.mesh = {};
				after.materials.clear();
				EditorCommandManager::ModifyComponent<Engine::MeshComponent>(entity, before, after); }
			},

			{ "Remove", [&] {EditorCommandManager::RemoveComponent<Engine::MeshComponent>(entity); } }
		};

		ComponentHeader header("Mesh", menuActions);
		if (!header.open) return;

		Engine::MeshComponent& component = entity.GetComponent<Engine::MeshComponent>();

		PropertyTable table;

		if (AssetField("Mesh", component.mesh).changed)
		{
			if (component.mesh)
			{
				Engine::MeshAsset& mesh = Engine::AssetManager::GetAsset<Engine::MeshAsset>(component.mesh);

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

		for (auto&& [i, material] : std::views::enumerate(component.materials))
		{
			if (AssetField(std::format("Material {}", i), material).changed)
			{
				SelectionManager::Assets.Select(material.id);
			}
		}
	}

	void DrawCollider(Engine::Entity entity)
	{
		if (!entity.HasComponent<Engine::ColliderComponent>()) return;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {auto before = entity.GetComponent<Engine::ColliderComponent>();
				auto after = before;
				after.collisionMesh = {};
				EditorCommandManager::ModifyComponent<Engine::ColliderComponent>(entity, before, after); }
			},

			{ "Remove", [&] {EditorCommandManager::RemoveComponent<Engine::ColliderComponent>(entity); } }
		};

		ComponentHeader header("Collider", menuActions);
		if (!header.open) return;

		auto& component = entity.GetComponent<Engine::ColliderComponent>();

		PropertyTable table;

		AssetField("Mesh", component.collisionMesh);
		PropertyField("Is trigger", component.isTrigger);
	}

	void DrawLight(Engine::Entity entity)
	{
		if (!entity.HasComponent<Engine::LightComponent>()) return;
		auto& component = entity.GetComponent<Engine::LightComponent>();

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

	void DrawScript(Engine::Entity entity)
	{
		if (!entity.HasComponent<Engine::ScriptComponent>()) return;

		static uint32_t id = 0;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {id = 0; auto before = entity.GetComponent<Engine::ScriptComponent>();
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

		Engine::ScriptComponent& component = entity.GetComponent<Engine::ScriptComponent>();

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
				AssetField(field.name.c_str(), field.GetValue<Engine::Texture2D>(component.instance));
				break;
			}

			case Engine::ScriptFieldType::AudioClip:
			{
				AssetField(field.name.c_str(), field.GetValue<Engine::AudioClip>(component.instance));
				break;
			}

			case Engine::ScriptFieldType::Mesh:
			{
				AssetField(field.name.c_str(), field.GetValue<Engine::Mesh>(component.instance));
				break;
			}

			case Engine::ScriptFieldType::Shader:
			{
				AssetField(field.name.c_str(), field.GetValue<Engine::Shader>(component.instance));
				break;
			}

			case Engine::ScriptFieldType::Material:
			{
				AssetField(field.name.c_str(), field.GetValue<Engine::Material>(component.instance));
				break;
			}
			}
		}
	}

	void DrawUICanvas(Engine::Entity entity)
	{
		if (!entity.HasComponent<Engine::UI::Canvas>()) return;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {
				auto before = entity.GetComponent<Engine::UI::Canvas>();
				auto after = Engine::UI::Canvas{};
				EditorCommandManager::ModifyComponent<Engine::UI::Canvas>(entity, before, after);
			} },
		};

		ComponentHeader header("Canvas", menuActions);
		if (!header.open) return;
		auto& canvas = entity.GetComponent<Engine::UI::Canvas>();
		EditResult result;
		static Engine::UI::Canvas s_CanvasBefore;

		PropertyTable table;
		result |= PropertyField("Screen Space", canvas.isScreenSpace);
		result |= PropertyField("Reference Resolution", canvas.referenceResolution);
		result |= PropertyField("Match Width/Height", canvas.matchWidthOrHeight, { .min = 0.0f, .max = 1.0f });

		if (result.finished)
		{
			EditorCommandManager::ModifyComponent<Engine::UI::Canvas>(entity, s_CanvasBefore, canvas);
		}
		else if (result.changed)
		{
			entity.AddTag<Engine::TransformDirty>();
		}
	}

	void DrawUIRect(Engine::Entity entity)
	{
		if (!entity.HasComponent<Engine::UI::RectTransform>()) return;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {
				auto before = entity.GetComponent<Engine::UI::RectTransform>();
				auto after = Engine::UI::RectTransform{};
				EditorCommandManager::ModifyComponent<Engine::UI::RectTransform>(entity, before, after);
			} },
		};

		ComponentHeader header("Rect", menuActions);
		if (!header.open) return;

		EditResult result;
		static Engine::UI::RectTransform s_RectBefore;

		PropertyTable table;

		auto& rc = entity.GetComponent<Engine::UI::RectTransform>();

		result |= PropertyField("Position", rc.anchoredPosition);
		result |= PropertyField("Size", rc.size);
		//result |= PropertyField("Color", rc.color);
		//result |= PropertyField("Texture", rc.textureId);

		result |= PropertyField("Anchor Min", rc.anchorMin);
		result |= PropertyField("Anchor Max", rc.anchorMax);

		if (result.started)
		{
			s_RectBefore = { rc.anchoredPosition, rc.size, rc.anchorMin, rc.anchorMax };
		}
		else if (result.finished)
		{
			EditorCommandManager::ModifyComponent<Engine::UI::RectTransform>(entity, s_RectBefore, rc);
		}
		else if (result.changed)
		{
			entity.AddTag<Engine::UI::LayoutDirtyTag>();
		}
	}

	void DrawUIImage(Engine::Entity entity)
	{
		if (!entity.HasComponent<Engine::UI::Image>()) return;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {
				auto before = entity.GetComponent<Engine::UI::Image>();
				auto after = Engine::UI::Image{};
				EditorCommandManager::ModifyComponent<Engine::UI::Image>(entity, before, after);
			} },
		};

		ComponentHeader header("Image", menuActions);
		if (!header.open) return;
		auto& ic = entity.GetComponent<Engine::UI::Image>();

		EditResult result;
		static Engine::UI::Image s_ImageBefore;
		PropertyTable table;

		result |= PropertyField("Tint", ic.tint, { .mode = DisplayMode::Color });
		result |= PropertyField("Texture", ic.iconName);

		if (result.started)
		{
			s_ImageBefore = { ic.iconName, ic.tint };
		}
		else if (result.finished)
		{
			EditorCommandManager::ModifyComponent<Engine::UI::Image>(entity, s_ImageBefore, ic);
		}
		else if (result.changed)
		{
			entity.AddTag<Engine::TransformDirty>();
		}
	}

	void DrawUIText(Engine::Entity entity)
	{
		if (!entity.HasComponent<Engine::UI::Text>()) return;
		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {
				auto before = entity.GetComponent<Engine::UI::Text>();
				auto after = Engine::UI::Text{};
				EditorCommandManager::ModifyComponent<Engine::UI::Text>(entity, before, after);
			} },
		};
		ComponentHeader header("Text", menuActions);
		if (!header.open) return;
		auto& tc = entity.GetComponent<Engine::UI::Text>();
		EditResult result;
		static Engine::UI::Text s_TextBefore;
		PropertyTable table;
		result |= PropertyField("Text", tc.text, { .mode = DisplayMode::Multiline });
		result |= PropertyField("Color", tc.color, { .mode = DisplayMode::Color });
		result |= PropertyField("Font", tc.fontName);
		result |= PropertyField("Font Size", tc.fontSize);
		result |= PropertyField("Line Spacing", tc.lineSpacing);
		result |= PropertyField("Wrap Text", tc.wrapText);

		if (result.started)
		{
			s_TextBefore = { tc.text, tc.fontName, tc.color };
		}
		else if (result.finished)
		{
			EditorCommandManager::ModifyComponent<Engine::UI::Text>(entity, s_TextBefore, tc);
		}
		else if (result.changed)
		{
			entity.AddTag<Engine::TransformDirty>();
		}
	}


	void DrawAddComponentButton(Engine::Entity entity)
	{
		struct AddComponentEntry
		{
			const char* name;
			void (*add)(Engine::Entity);
		};

		static constexpr std::array<AddComponentEntry, 10> entries
		{
			AddComponentEntry{ "Transform",     [](Engine::Entity e) { e.GetOrAddComponent<Engine::TransformComponent>(); e.GetOrAddComponent<Engine::WorldTransformComponent>(); } },
			AddComponentEntry{ "Camera",        [](Engine::Entity e) { e.GetOrAddComponent<Engine::CameraComponent>(); } },
			AddComponentEntry{ "Mesh",          [](Engine::Entity e) { e.GetOrAddComponent<Engine::MeshComponent>(); } },
			AddComponentEntry{ "Collider",      [](Engine::Entity e) { e.GetOrAddComponent<Engine::ColliderComponent>(); } },
			AddComponentEntry{ "Light",         [](Engine::Entity e) { e.GetOrAddComponent<Engine::LightComponent>(); } },
			AddComponentEntry{ "Script",				[](Engine::Entity e) { e.GetOrAddComponent<Engine::ScriptComponent>(); } },
			AddComponentEntry{ "UI Rect",				[](Engine::Entity e) { e.GetOrAddComponent<Engine::UI::RectTransform>(); } },
			AddComponentEntry{ "UI Canvas",			[](Engine::Entity e) { e.GetOrAddComponent<Engine::UI::Canvas>(); e.GetOrAddComponent<Engine::UI::RectTransform>(); e.AddTag<Engine::UI::LayoutDirtyTag>(); } },
			AddComponentEntry{ "UI Image",			[](Engine::Entity e) { e.GetOrAddComponent<Engine::UI::Image>(); e.GetOrAddComponent<Engine::UI::RectTransform>(); e.AddTag<Engine::UI::LayoutDirtyTag>(); } },
			AddComponentEntry{ "UI Text",				[](Engine::Entity e) { e.GetOrAddComponent<Engine::UI::Text>(); e.GetOrAddComponent<Engine::UI::RectTransform>(); e.AddTag<Engine::UI::LayoutDirtyTag>(); } }
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
						entries[n].add(entity);
						ImGui::CloseCurrentPopup();
					}
				}
			}

			ImGui::EndPopup();
		}
	}

	void EntityInspectorPanel::Draw(Engine::Uuid entityId)
	{
		Engine::SceneAsset& scene = Engine::AssetManager::GetAsset<Engine::SceneAsset>(Engine::SceneManager::GetActiveScene());
		Engine::Entity entity = scene.GetEntity(entityId);
		if (!entity) return;

		EntityHeader header(entity);

		DrawTransform(entity);
		DrawCamera(entity);
		DrawMesh(entity);
		DrawCollider(entity);
		DrawLight(entity);
		DrawScript(entity);
		DrawUIRect(entity);
		DrawUICanvas(entity);
		DrawUIImage(entity);
		DrawUIText(entity);

		DrawAddComponentButton(entity);
	}
}