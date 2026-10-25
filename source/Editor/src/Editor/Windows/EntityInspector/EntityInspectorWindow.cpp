#include "Editor/Windows/EntityInspector/EntityInspectorWindow.h"

#include "Editor/Windows/Utility/Property.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Editor/Command/PropertyChangeCommand.h"
#include "Editor/Command/ComponentSnapshotCommand.h"
#include "Editor/Command/AddComponentCommand.h"
#include "Editor/Command/RemoveComponentCommand.h"

#include "Engine/Core/Components.h"
#include "Engine/Core/SceneManager.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Graphics/Mesh.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui_internal.h>
#include <entt/entity/entity.hpp>
#include <functional>
#include <memory>
#include <ranges>

namespace editor
{
	entt::entity m_Entity;

	struct EntityHeader
	{
		EntityHeader(entt::entity entity, entt::registry& registry)
		{
			ImGui::PushID(static_cast<int>(entity));

			bool isActive = !registry.any_of<engine::DisabledTag>(entity);
			if (PropertyField("Enabled", isActive, { .showLabel = false }).changed)
			{
				if (isActive)
				{
					registry.remove<engine::DisabledTag>(entity);
				}
				else
				{
					registry.emplace_or_replace<engine::DisabledTag>(entity);
				}
			}
			ImGui::SameLine();

			std::string& name = registry.get<engine::NameComponent>(entity).name;
			if (PropertyField("Name", name, { .showLabel = false }).finished)
			{
				//TODO: somehow store the original name
				//EditorCommandManager::ModifyComponent<Engine::NameComponent>(entity, { name }, { name });
			}
		}

		// ~EntityHeader()
		// {
		// 	ImGui::PopID();
		// }
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

	void DrawTransform(entt::entity entity, entt::registry& registry)
	{
		engine::TransformComponent* component = registry.try_get<engine::TransformComponent>(entity);
		if (!component) return;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {
				auto before = *component;
				auto after = engine::TransformComponent{};
				EditorCommandManager::ModifyComponent<engine::TransformComponent>(entity, before, after);
				registry.emplace_or_replace<engine::TransformDirty>(entity);
			} },
			{ "Remove", [&] {EditorCommandManager::RemoveComponent<engine::TransformComponent>(entity); } }
		};

		ComponentHeader header("Transform", menuActions);
		if (!header.open) return;

		EditResult result;
		static engine::TransformComponent s_TransformBefore;

		PropertyTable table;

		result |= PropertyField("Position", component->position);
		result |= PropertyField("Rotation", component->rotation, { .useDegrees = true });
		result |= PropertyField("Scale", component->scale);

		if (result.started)
		{
			s_TransformBefore = { component->position, component->rotation, component->scale };
		}
		else if (result.finished)
		{
			EditorCommandManager::ModifyComponent<engine::TransformComponent>(entity, s_TransformBefore, *component);
		}
		
		if (result.changed)
		{
			registry.emplace_or_replace<engine::TransformDirty>(entity);
		}
	}

	void DrawCamera(entt::entity entity, entt::registry& registry)
	{
		engine::CameraComponent* component = registry.try_get<engine::CameraComponent>(entity);
		if (!component) return;

		ComponentHeader header("Camera");
		if (!header.open) return;

		PropertyTable table;

		bool primary = registry.any_of<engine::PrimaryCameraTag>(entity);

		if (PropertyField("Primary", primary).changed)
		{
			if (primary)
			{
				auto view = registry.view<engine::PrimaryCameraTag>(entt::exclude<engine::EditorTag>);
				registry.remove<engine::PrimaryCameraTag>(view.begin(), view.end());
				registry.emplace<engine::PrimaryCameraTag>(entity);
			}
			else
			{
				registry.remove<engine::PrimaryCameraTag>(entity);
			}
		}

		int currentProjection = static_cast<int>(component->projectionType);
		if (EnumField("Projection", currentProjection, { "Perspective", "Orthographic" }).changed)
		{
			component->projectionType = static_cast<engine::CameraComponent::Projection>(currentProjection);
			registry.emplace_or_replace<engine::CameraProjectionDirty>(entity);
		}

		EnumField("Background", (int&)component->backgroundMode, { "Color", "Skybox" });
		if (component->backgroundMode == engine::CameraComponent::Background::Color)
		{
			PropertyField("Clear Color", component->clearColor, { .mode = DisplayMode::Color });
		}
	}

	void DrawMesh(entt::entity entity, entt::registry& registry)
	{
		engine::MeshComponent* component = registry.try_get<engine::MeshComponent>(entity);
		if (!component) return;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {
				auto before = *component;
				auto after = engine::MeshComponent{};
				EditorCommandManager::ModifyComponent<engine::MeshComponent>(entity, before, after); }
			},

			{ "Remove", [&] {EditorCommandManager::RemoveComponent<engine::MeshComponent>(entity); } }
		};

		ComponentHeader header("Mesh", menuActions);
		if (!header.open) return;

		PropertyTable table;

		if (AssetField("Mesh", component->mesh).changed)
		{
			component->materialInstances.resize(component->mesh.Get().materialCount);
		}

		for (size_t i = 0; i < component->materialInstances.size(); ++i)
		{
			AssetField(std::format("Material {}", i), component->materialInstances[i]);
		}
	}

	void DrawCollider(entt::entity entity, entt::registry& registry)
	{
		engine::ColliderComponent* component = registry.try_get<engine::ColliderComponent>(entity);
		if (!component) return;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {
				auto before = *component;
				auto after = engine::ColliderComponent{};
				EditorCommandManager::ModifyComponent<engine::ColliderComponent>(entity, before, after); }
			},

			{ "Remove", [&] {EditorCommandManager::RemoveComponent<engine::ColliderComponent>(entity); } }
		};

		ComponentHeader header("Collider", menuActions);
		if (!header.open) return;

		PropertyTable table;

		AssetField("Mesh", component->collisionMesh);
		PropertyField("Is trigger", component->isTrigger);
	}

	void DrawLight(entt::entity entity, entt::registry& registry)
	{
		engine::LightComponent* component = registry.try_get<engine::LightComponent>(entity);
		if (!component) return;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {
				auto before = *component;
				auto after = engine::LightComponent{};
				EditorCommandManager::ModifyComponent<engine::LightComponent>(entity, before, after); }
			},

			{ "Remove", [&] {EditorCommandManager::RemoveComponent<engine::LightComponent>(entity); } }
		};

		ComponentHeader header("Light", menuActions);
		if (!header.open) return;

		PropertyTable table;

		int current = static_cast<int>(component->type);
		if (EnumField("Type", current, { "Directional", "Spot", "Point" }).changed)
		{
			component->type = static_cast<engine::LightType>(current);
		}

		PropertyField("Color", component->color, { .mode = DisplayMode::Color });
		PropertyField("Intensity", component->intensity, { .min = 0.0f });

		if (component->type == engine::LightType::Spot)
		{
			PropertyField("Angle", component->angle, { .min = 0.0f, .max = 180.0f });
		}
	}

	void DrawUICanvas(entt::entity entity, entt::registry& registry)
	{
		engine::UI::Canvas* component = registry.try_get<engine::UI::Canvas>(entity);
		if (!component) return;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {
				auto before = *component;
				auto after = engine::UI::Canvas{};
				EditorCommandManager::ModifyComponent<engine::UI::Canvas>(entity, before, after);
			} },
		};

		ComponentHeader header("Canvas", menuActions);
		if (!header.open) return;

		EditResult result;
		static engine::UI::Canvas s_CanvasBefore;

		PropertyTable table;
		result |= PropertyField("Screen Space", component->isScreenSpace);
		result |= PropertyField("Reference Resolution", component->referenceResolution);
		result |= PropertyField("Match Width/Height", component->matchWidthOrHeight, { .min = 0.0f, .max = 1.0f });

		if (result.finished)
		{
			EditorCommandManager::ModifyComponent<engine::UI::Canvas>(entity, s_CanvasBefore, *component);
		}
		else if (result.changed)
		{
			registry.emplace_or_replace<engine::TransformComponent>(entity);
		}
	}

	void DrawUIRect(entt::entity entity, entt::registry& registry)
	{
		engine::UI::RectTransform* component = registry.try_get<engine::UI::RectTransform>(entity);
		if (!component) return;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {
				auto before = *component;
				auto after = engine::UI::RectTransform{};
				EditorCommandManager::ModifyComponent<engine::UI::RectTransform>(entity, before, after);
			} },
		};

		ComponentHeader header("Rect", menuActions);
		if (!header.open) return;

		EditResult result;


		PropertyTable table;

		result |= PropertyField("Position", component->anchoredPosition);
		result |= PropertyField("Size", component->size);
		//result |= PropertyField("Color", rc.color);
		//result |= PropertyField("Texture", rc.textureId);

		result |= PropertyField("Anchor Min", component->anchorMin);
		result |= PropertyField("Anchor Max", component->anchorMax);

		static engine::UI::RectTransform s_RectBefore;

		if (result.started)
		{
			s_RectBefore = *component;
		}
		else if (result.finished)
		{
			EditorCommandManager::ModifyComponent<engine::UI::RectTransform>(entity, s_RectBefore, *component);
		}
		else if (result.changed)
		{
			registry.emplace_or_replace<engine::UI::LayoutDirtyTag>(entity);
		}
	}

	void DrawUIImage(entt::entity entity, entt::registry& registry)
	{
		engine::UI::Image* component = registry.try_get<engine::UI::Image>(entity);
		if (!component) return;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {
				auto before = *component;
				auto after = engine::UI::Image{};
				EditorCommandManager::ModifyComponent<engine::UI::Image>(entity, before, after);
			} },
		};

		ComponentHeader header("Image", menuActions);
		if (!header.open) return;

		PropertyTable table;

		EditResult result;
		result |= PropertyField("Tint", component->tint, { .mode = DisplayMode::Color });
		result |= PropertyField("Texture", component->iconName);

		static engine::UI::Image s_ImageBefore;

		if (result.started)
		{
			s_ImageBefore = *component;
		}
		else if (result.finished)
		{
			EditorCommandManager::ModifyComponent<engine::UI::Image>(entity, s_ImageBefore, *component);
		}
		else if (result.changed)
		{
			registry.emplace_or_replace<engine::TransformDirty>(entity);
		}
	}

	void DrawUIText(entt::entity entity, entt::registry& registry)
	{
		engine::UI::Text* component = registry.try_get<engine::UI::Text>(entity);
		if (!component) return;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {
				auto before = *component;
				auto after = engine::UI::Text{};
				EditorCommandManager::ModifyComponent<engine::UI::Text>(entity, before, after);
			} },
		};

		ComponentHeader header("Text", menuActions);
		if (!header.open) return;

		PropertyTable table;

		EditResult result;
		result |= PropertyField("Text", component->text, { .mode = DisplayMode::Multiline });
		result |= PropertyField("Color", component->color, { .mode = DisplayMode::Color });
		result |= PropertyField("Font", component->fontName);
		result |= PropertyField("Font Size", component->fontSize);
		result |= PropertyField("Line Spacing", component->lineSpacing);
		result |= PropertyField("Wrap Text", component->wrapText);

		static engine::UI::Text s_TextBefore;

		if (result.started)
		{
			s_TextBefore = *component;
		}
		else if (result.finished)
		{
			EditorCommandManager::ModifyComponent<engine::UI::Text>(entity, s_TextBefore, *component);
		}
		else if (result.changed)
		{
			registry.emplace_or_replace<engine::TransformDirty>(entity);
		}
	}


	void DrawAddComponentButton(entt::entity entity, entt::registry& registry)
	{
		struct AddComponentEntry
		{
			const char* name;
			void (*add)(entt::registry&, entt::entity);
		};

		static constexpr std::array<AddComponentEntry, 9> entries
    {
      AddComponentEntry{ "Transform",     [](entt::registry& r, entt::entity e) { (void)r.get_or_emplace<engine::TransformComponent>(e); (void)r.emplace_or_replace<engine::WorldTransformComponent>(e); } },
      AddComponentEntry{ "Camera",        [](entt::registry& r, entt::entity e) { (void)r.get_or_emplace<engine::CameraComponent>(e); } },
      AddComponentEntry{ "Mesh",          [](entt::registry& r, entt::entity e) { (void)r.get_or_emplace<engine::MeshComponent>(e); } },
      AddComponentEntry{ "Collider",      [](entt::registry& r, entt::entity e) { (void)r.get_or_emplace<engine::ColliderComponent>(e); } },
      AddComponentEntry{ "Light",         [](entt::registry& r, entt::entity e) { (void)r.get_or_emplace<engine::LightComponent>(e); } },
      AddComponentEntry{ "UI Rect",       [](entt::registry& r, entt::entity e) { (void)r.get_or_emplace<engine::UI::RectTransform>(e); } },
      AddComponentEntry{ "UI Canvas",     [](entt::registry& r, entt::entity e) { (void)r.get_or_emplace<engine::UI::Canvas>(e); (void)r.emplace_or_replace<engine::UI::RectTransform>(e); (void)r.emplace_or_replace<engine::UI::LayoutDirtyTag>(e); } },
      AddComponentEntry{ "UI Image",      [](entt::registry& r, entt::entity e) { (void)r.get_or_emplace<engine::UI::Image>(e); (void)r.emplace_or_replace<engine::UI::RectTransform>(e); (void)r.emplace_or_replace<engine::UI::LayoutDirtyTag>(e); } },
      AddComponentEntry{ "UI Text",       [](entt::registry& r, entt::entity e) { (void)r.get_or_emplace<engine::UI::Text>(e); (void)r.emplace_or_replace<engine::UI::RectTransform>(e); (void)r.emplace_or_replace<engine::UI::LayoutDirtyTag>(e); } }
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
						entries[n].add(registry, entity); // Pass the registry here
						ImGui::CloseCurrentPopup();
					}
				}
			}

			ImGui::EndPopup();
		}
	}

	void EntityInspectorWindow::Initialize()
	{
		engine::EventBus::Subscribe<SelectEntityEvent>([](const SelectEntityEvent& e)
			{
				m_Entity = e.entity;
				return false;
			});
	}

	void EntityInspectorWindow::Draw()
	{
		ImGui::Begin("Entity Inspector");

		if (m_Entity == entt::null) return;

		entt::registry& registry = engine::SceneManager::GetActiveScene().GetRegistry();

		EntityHeader header(m_Entity, registry);

		DrawTransform(m_Entity, registry);
		DrawCamera(m_Entity, registry);
		DrawMesh(m_Entity, registry);
		DrawCollider(m_Entity, registry);
		DrawLight(m_Entity, registry);
		DrawUIRect(m_Entity, registry);
		DrawUICanvas(m_Entity, registry);
		DrawUIImage(m_Entity, registry);
		DrawUIText(m_Entity, registry);

		DrawAddComponentButton(m_Entity, registry);

		ImGui::PopID();

		ImGui::End();
	}
}