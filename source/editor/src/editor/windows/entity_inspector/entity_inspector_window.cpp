#include "editor/windows/entity_inspector/entity_inspector_window.h"
#include "editor/core/editor_context.h"
#include "editor/windows/utility/property.h"
#include "editor/core/editor_command_manager.h"
#include "engine/core/components.h"
#include "engine/core/context.h"
#include "engine/asset/asset_manager.h"
#include "engine/graphics/mesh.h"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui_internal.h>
#include <entt/entity/entity.hpp>
#include <functional>
#include <memory>
#include <ranges>

namespace editor
{
	struct EntityHeader
	{
		EntityHeader(entt::entity entity, entt::registry& registry)
		{
			ImGui::PushID(static_cast<int>(entity));

			bool isActive = !registry.any_of<engine::tag::Disabled>(entity);
			if (PropertyField("Enabled", isActive, { .showLabel = false }).changed)
			{
				if (isActive)
				{
					registry.remove<engine::tag::Disabled>(entity);
				}
				else
				{
					registry.emplace_or_replace<engine::tag::Disabled>(entity);
				}
			}
			ImGui::SameLine();

			std::string& name = registry.get<engine::component::Name>(entity).name;
			if (PropertyField("Name", name, { .showLabel = false }).finished)
			{
				//TODO: somehow store the original name
				//editorcommandManager::ModifyComponent<engine::NameComponent>(entity, { name }, { name });
			}
		}
	};

	template<typename T>
	struct ComponentHeader
	{
		bool open = false;

		ComponentHeader(const char* label, entt::entity entity)
		{
			open = ImGui::CollapsingHeader(label, ImGuiTreeNodeFlags_DefaultOpen);

			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Reset", nullptr, false))
				{
					EditorCommandManager::Enqueue([entity]
						{
							entt::registry& registry = engine::Context::activeSceneHandle.Get().registry;
							T* component = registry.try_get<T>(entity);
							if (component)
							{
								*component = T{};
							}
						});
				}

				if (ImGui::MenuItem("Remove", nullptr, false))
				{
					EditorCommandManager::Enqueue([entity]
						{
							entt::registry& registry = engine::Context::activeSceneHandle.Get().registry;
							registry.remove<T>(entity);
						});
				}
				ImGui::EndPopup();
			}
		}
	};

	void DrawTransform(entt::entity entity, entt::registry& registry)
	{
		engine::component::Transform* component = registry.try_get<engine::component::Transform>(entity);
		if (!component) return;

		ComponentHeader<engine::component::Transform> header("Transform", entity);
		if (!header.open) return;

		EditResult result;
		PropertyTable table;

		result |= PropertyField("Position", component->position);
		result |= PropertyField("Rotation", component->rotation, { .useDegrees = true });
		result |= PropertyField("Scale", component->scale);

		if (result.changed)
		{
			registry.emplace_or_replace<engine::tag::TransformDirty>(entity);
		}
	}

	void DrawCamera(entt::entity entity, engine::Scene& scene)
	{
		engine::component::Camera* component = scene.registry.try_get<engine::component::Camera>(entity);
		if (!component) return;

		ComponentHeader<engine::component::Camera> header("Camera", entity);
		if (!header.open) return;

		PropertyTable table;

		bool primary = entity == scene.primaryCameraEntity;

		if (PropertyField("Primary", primary).changed)
		{
			scene.primaryCameraEntity = primary ? entity : entt::null;
		}

		int currentProjection = static_cast<int>(component->projection);
		if (EnumField("Projection", currentProjection, { "Perspective", "Orthographic" }).changed)
		{
			component->projection = static_cast<engine::component::Camera::Projection>(currentProjection);
			scene.registry.emplace_or_replace<engine::tag::CameraProjectionDirty>(entity);
		}

		EnumField("Background", (int&)component->backgroundMode, { "Color", "Skybox" });
		if (component->backgroundMode == engine::component::Camera::Background::Color)
		{
			PropertyField("Clear Color", component->clearColor, { .mode = DisplayMode::Color });
		}
	}

	void DrawMesh(entt::entity entity, entt::registry& registry)
	{
		engine::component::Mesh* component = registry.try_get<engine::component::Mesh>(entity);
		if (!component) return;

		ComponentHeader<engine::component::Mesh> header("Mesh", entity);
		if (!header.open) return;

		PropertyTable table;

		if (assetField("Mesh", component->mesh).changed)
		{
			component->materialInstances.resize(component->mesh.Get().materialCount);
		}

		if (ImGui::IsItemClicked(0))
		{
			editor::context::selectedasset = { component->mesh.id, engine::assetType::Mesh };
		}

		for (size_t i = 0; i < component->materialInstances.size(); ++i)
		{
			assetField(std::format("Material {}", i), component->materialInstances[i]);
			if (ImGui::IsItemClicked(0))
			{
				editor::context::selectedasset = { component->materialInstances[i].id, engine::assetType::MaterialInstance };
			}
		}
	}

	// void DrawCollider(entt::entity entity, entt::registry& registry)
	// {
	// 	engine::component::Collider* component = registry.try_get<engine::component::Collider>(entity);
	// 	if (!component) return;

	// 	const std::initializer_list<ComponentMenuAction> menuActions
	// 	{
	// 		{ "Reset", [&] {
	// 			auto before = *component;
	// 			auto after = engine::component::Collider{};
	// 			EditorCommandManager::ModifyComponent<engine::component::Collider>(entity, before, after); }
	// 		},

	// 		{ "Remove", [&] {EditorCommandManager::RemoveComponent<engine::component::Collider>(entity); } }
	// 	};

	// 	ComponentHeader header("Collider", menuActions);
	// 	if (!header.open) return;

	// 	PropertyTable table;

	// 	assetField("Mesh", component->collisionMesh);
	// 	PropertyField("Is trigger", component->isTrigger);
	// }

	// void DrawLight(entt::entity entity, entt::registry& registry)
	// {
	// 	engine::component::Light* component = registry.try_get<engine::component::Light>(entity);
	// 	if (!component) return;

	// 	const std::initializer_list<ComponentMenuAction> menuActions
	// 	{
	// 		{ "Reset", [&] {
	// 			auto before = *component;
	// 			auto after = engine::component::Light{};
	// 			EditorCommandManager::ModifyComponent<engine::component::Light>(entity, before, after); }
	// 		},

	// 		{ "Remove", [&] {EditorCommandManager::RemoveComponent<engine::component::Light>(entity); } }
	// 	};

	// 	ComponentHeader header("Light", menuActions);
	// 	if (!header.open) return;

	// 	PropertyTable table;

	// 	int current = static_cast<int>(component->type);
	// 	if (EnumField("Type", current, { "Directional", "Spot", "Point" }).changed)
	// 	{
	// 		component->type = static_cast<engine::component::Light::LightType>(current);
	// 	}

	// 	PropertyField("Color", component->color, { .mode = DisplayMode::Color });
	// 	PropertyField("Intensity", component->intensity, { .min = 0.0f });

	// 	if (component->type == engine::component::Light::LightType::Spot)
	// 	{
	// 		PropertyField("Angle", component->angle, { .min = 0.0f, .max = 180.0f });
	// 	}
	// }

	void DrawuiCanvas(entt::entity entity, entt::registry& registry)
	{
		engine::component::Canvas* component = registry.try_get<engine::component::Canvas>(entity);
		if (!component) return;
	
		ComponentHeader<engine::component::Canvas> header("Canvas", entity);
		if (!header.open) return;

		EditResult result;
		PropertyTable table;
		result |= PropertyField("Screen Space", component->isScreenSpace);
		result |= PropertyField("Reference Resolution", component->referenceResolution);
		result |= PropertyField("Match Width/Height", component->matchWidthOrHeight, { .min = 0.0f, .max = 1.0f });

		if (result.changed)
		{
			registry.emplace_or_replace<engine::tag::TransformDirty>(entity);
		}
	}

	void DrawuiRect(entt::entity entity, entt::registry& registry)
	{
		engine::component::RectTransform* component = registry.try_get<engine::component::RectTransform>(entity);
		if (!component) return;

		ComponentHeader<engine::component::RectTransform> header("Rect", entity);
		if (!header.open) return;

		EditResult result;

		PropertyTable table;
		result |= PropertyField("Position", component->anchoredPosition);
		result |= PropertyField("Size", component->size);
		result |= PropertyField("Anchor Min", component->anchorMin);
		result |= PropertyField("Anchor Max", component->anchorMax);

		if (result.changed)
		{
			registry.emplace_or_replace<engine::tag::LayoutDirty>(entity);
		}
	}

	void DrawuiImage(entt::entity entity, entt::registry& registry)
	{
		engine::component::Image* component = registry.try_get<engine::component::Image>(entity);
		if (!component) return;

		ComponentHeader<engine::component::Image> header("Image", entity);
		if (!header.open) return;

		PropertyTable table;

		EditResult result;
		result |= PropertyField("Tint", component->tint, { .mode = DisplayMode::Color });
		result |= PropertyField("texture", component->iconName);

		if (result.changed)
		{
			registry.emplace_or_replace<engine::tag::TransformDirty>(entity);
		}
	}

	void DrawuiText(entt::entity entity, entt::registry& registry)
	{
		engine::component::Text* component = registry.try_get<engine::component::Text>(entity);
		if (!component) return;

		ComponentHeader<engine::component::Text> header("Text", entity);
		if (!header.open) return;

		PropertyTable table;

		EditResult result;
		result |= PropertyField("Text", component->text, { .mode = DisplayMode::Multiline });
		result |= PropertyField("Color", component->color, { .mode = DisplayMode::Color });
		result |= PropertyField("Font", component->fontName);
		result |= PropertyField("Font Size", component->fontSize);
		result |= PropertyField("Line Spacing", component->lineSpacing);
		result |= PropertyField("Wrap Text", component->wrapText);

		if (result.changed)
		{
			registry.emplace_or_replace<engine::tag::TransformDirty>(entity);
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
			AddComponentEntry{ "Transform",     [](entt::registry& r, entt::entity e) { (void)r.get_or_emplace<engine::component::Transform>(e); (void)r.emplace_or_replace<engine::component::WorldTransform>(e); } },
			AddComponentEntry{ "Camera",        [](entt::registry& r, entt::entity e) { (void)r.get_or_emplace<engine::component::Camera>(e); } },
			AddComponentEntry{ "Mesh",          [](entt::registry& r, entt::entity e) { (void)r.get_or_emplace<engine::component::Mesh>(e); } },
			AddComponentEntry{ "Collider",      [](entt::registry& r, entt::entity e) { (void)r.get_or_emplace<engine::component::Collider>(e); } },
			AddComponentEntry{ "Light",         [](entt::registry& r, entt::entity e) { (void)r.get_or_emplace<engine::component::Light>(e); } },
			AddComponentEntry{ "ui Rect",       [](entt::registry& r, entt::entity e) { (void)r.get_or_emplace<engine::component::RectTransform>(e); } },
			AddComponentEntry{ "ui Canvas",     [](entt::registry& r, entt::entity e) { (void)r.get_or_emplace<engine::component::Canvas>(e); (void)r.emplace_or_replace<engine::component::RectTransform>(e); (void)r.emplace_or_replace<engine::tag::LayoutDirty>(e); } },
			AddComponentEntry{ "ui Image",      [](entt::registry& r, entt::entity e) { (void)r.get_or_emplace<engine::component::Image>(e); (void)r.emplace_or_replace<engine::component::RectTransform>(e); (void)r.emplace_or_replace<engine::tag::LayoutDirty>(e); } },
			AddComponentEntry{ "ui Text",       [](entt::registry& r, entt::entity e) { (void)r.get_or_emplace<engine::component::Text>(e); (void)r.emplace_or_replace<engine::component::RectTransform>(e); (void)r.emplace_or_replace<engine::tag::LayoutDirty>(e); } }
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
						entries[n].add(registry, entity); // pass the registry here
						ImGui::CloseCurrentPopup();
					}
				}
			}

			ImGui::EndPopup();
		}
	}

	void entity_inspectorWindow::Draw()
	{
		ImGui::Begin("Entity Inspector");

		if (editor::context::currentSelection.primary == entt::null)
		{
			ImGui::End();
			return;
		}

		engine::Scene& scene = engine::Context::activeSceneHandle.Get();
		entt::entity entity = editor::context::currentSelection.primary;

		EntityHeader header(entity, scene.registry);

		DrawTransform(entity, scene.registry);
		DrawCamera(entity, scene);
		DrawMesh(entity, scene.registry);
		// DrawCollider(entity, scene.registry);
		// DrawLight(entity, scene.registry);
		DrawuiRect(entity, scene.registry);
		DrawuiCanvas(entity, scene.registry);
		DrawuiImage(entity, scene.registry);
		DrawuiText(entity, scene.registry);

		DrawAddComponentButton(entity, scene.registry);

		ImGui::PopID();

		ImGui::End();
	}
}