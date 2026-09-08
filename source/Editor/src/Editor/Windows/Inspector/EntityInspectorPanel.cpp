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
#include "Engine/Graphics/Mesh.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui_internal.h>
#include <functional>
#include <memory>
#include <ranges>

namespace editor
{
	struct EntityHeader
	{
		EntityHeader(engine::Entity entity)
		{
			ImGui::PushID(static_cast<int>(entity.GetHandle()));

			bool isActive = entity.IsActive();
			if (PropertyField("Enabled", isActive, { .showLabel = false }).finished)
			{
				entity.SetActive(isActive);
			}
			ImGui::SameLine();

			std::string& name = entity.GetComponent<engine::NameComponent>().name;
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

	void DrawTransform(engine::Entity entity)
	{
		if (!entity.HasComponent<engine::TransformComponent>()) return;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {
				auto before = entity.GetComponent<engine::TransformComponent>();
				auto after = engine::TransformComponent{};
				EditorCommandManager::ModifyComponent<engine::TransformComponent>(entity, before, after);
				entity.AddTag<engine::TransformDirty>();
			}},
			{ "Remove", [&] {EditorCommandManager::RemoveComponent<engine::TransformComponent>(entity); } }
		};

		ComponentHeader header("Transform", menuActions);
		if (!header.open) return;

		auto& tc = entity.GetComponent<engine::TransformComponent>();

		EditResult result;
		static engine::TransformComponent s_TransformBefore;

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
			EditorCommandManager::ModifyComponent<engine::TransformComponent>(entity, s_TransformBefore, tc);
		}
		else if (result.changed)
		{
			entity.AddTag<engine::TransformDirty>();

			if(entity.HasComponent<engine::CameraComponent>())
			{
				entity.AddTag<engine::CameraProjectionDirty>();
				entity.AddTag<engine::CameraViewDirty>();
			}
		}
	}

	void DrawCamera(engine::Entity entity)
	{
		if (!entity.HasComponent<engine::CameraComponent>()) return;

		ComponentHeader header("Camera");
		if (!header.open) return;

		auto& cc = entity.GetComponent<engine::CameraComponent>();

		PropertyTable table;

		bool primary = entity.HasTag<engine::PrimaryCameraTag>();

		if(PropertyField("Primary", primary).changed)
		{
			if(primary)
			{
				entt::registry& registry = entity.GetScene()->GetRegistry();
				auto view = registry.view<engine::PrimaryCameraTag>(entt::exclude<engine::EditorTag>);
				registry.remove<engine::PrimaryCameraTag>(view.begin(), view.end());

				entity.AddTag<engine::PrimaryCameraTag>();
			}
			else
			{
				entity.RemoveTag<engine::PrimaryCameraTag>();
			}
		}

		int currentProjection = static_cast<int>(cc.projectionType);
		if (EnumField("Projection", currentProjection, { "Perspective", "Orthographic" }).changed)
		{
			cc.projectionType = static_cast<engine::CameraComponent::Projection>(currentProjection);
			entity.AddTag<engine::CameraProjectionDirty>();
		}

		EnumField("Background", (int&)cc.backgroundMode, { "Color", "Skybox" });

		if (cc.backgroundMode == engine::CameraComponent::Background::Color)
		{
			PropertyField("Clear Color", cc.clearColor, { .mode = DisplayMode::Color });
		}
	}

	void DrawMesh(engine::Entity entity)
	{
		if (!entity.HasComponent<engine::MeshComponent>()) return;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {auto before = entity.GetComponent<engine::MeshComponent>();
				auto after = before;
				after.mesh = {};
				after.materials.clear();
				EditorCommandManager::ModifyComponent<engine::MeshComponent>(entity, before, after); }
			},

			{ "Remove", [&] {EditorCommandManager::RemoveComponent<engine::MeshComponent>(entity); } }
		};

		ComponentHeader header("Mesh", menuActions);
		if (!header.open) return;

		engine::MeshComponent& component = entity.GetComponent<engine::MeshComponent>();

		PropertyTable table;

		if (AssetField("Mesh", component.mesh).changed)
		{
			if (component.mesh)
			{
				engine::MeshAsset& mesh = engine::AssetManager::GetAsset<engine::MeshAsset>(component.mesh);

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

		size_t i = 0;
		for (auto& material : component.materials)
		{
			if (AssetField(std::format("Material {}", i), material).changed)
			{
				SelectionManager::Assets.Select(material.id);
			}
			i++;
		}
	}

	void DrawCollider(engine::Entity entity)
	{
		if (!entity.HasComponent<engine::ColliderComponent>()) return;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {auto before = entity.GetComponent<engine::ColliderComponent>();
				auto after = before;
				after.collisionMesh = {};
				EditorCommandManager::ModifyComponent<engine::ColliderComponent>(entity, before, after); }
			},

			{ "Remove", [&] {EditorCommandManager::RemoveComponent<engine::ColliderComponent>(entity); } }
		};

		ComponentHeader header("Collider", menuActions);
		if (!header.open) return;

		auto& component = entity.GetComponent<engine::ColliderComponent>();

		PropertyTable table;

		AssetField("Mesh", component.collisionMesh);
		PropertyField("Is trigger", component.isTrigger);
	}

	void DrawLight(engine::Entity entity)
	{
		if (!entity.HasComponent<engine::LightComponent>()) return;
		auto& component = entity.GetComponent<engine::LightComponent>();

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {auto before = component;
				auto after = before;
				after.type = engine::LightType::Directional;
				after.color = { 1.0f, 1.0f, 1.0f };
				after.intensity = 1.0f;
				EditorCommandManager::ModifyComponent<engine::LightComponent>(entity, before, after); }
			},

			{ "Remove", [&] {EditorCommandManager::RemoveComponent<engine::LightComponent>(entity); } }
		};

		ComponentHeader header("Light", menuActions);
		if (!header.open) return;

		PropertyTable table;

		int current = static_cast<int>(component.type);
		if (EnumField("Type", current, { "Directional", "Spot", "Point" }).changed)
		{
			component.type = static_cast<engine::LightType>(current);
		}

		PropertyField("Color", component.color, { .mode = DisplayMode::Color });
		PropertyField("Intensity", component.intensity, { .min = 0.0f });

		if (component.type == engine::LightType::Spot)
		{
			PropertyField("Angle", component.angle, { .min = 0.0f, .max = 180.0f });
		}
	}
	
	void DrawUICanvas(engine::Entity entity)
	{
		if (!entity.HasComponent<engine::UI::Canvas>()) return;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {
				auto before = entity.GetComponent<engine::UI::Canvas>();
				auto after = engine::UI::Canvas{};
				EditorCommandManager::ModifyComponent<engine::UI::Canvas>(entity, before, after);
			} },
		};

		ComponentHeader header("Canvas", menuActions);
		if (!header.open) return;
		auto& canvas = entity.GetComponent<engine::UI::Canvas>();
		EditResult result;
		static engine::UI::Canvas s_CanvasBefore;

		PropertyTable table;
		result |= PropertyField("Screen Space", canvas.isScreenSpace);
		result |= PropertyField("Reference Resolution", canvas.referenceResolution);
		result |= PropertyField("Match Width/Height", canvas.matchWidthOrHeight, { .min = 0.0f, .max = 1.0f });

		if (result.finished)
		{
			EditorCommandManager::ModifyComponent<engine::UI::Canvas>(entity, s_CanvasBefore, canvas);
		}
		else if (result.changed)
		{
			entity.AddTag<engine::TransformDirty>();
		}
	}

	void DrawUIRect(engine::Entity entity)
	{
		if (!entity.HasComponent<engine::UI::RectTransform>()) return;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {
				auto before = entity.GetComponent<engine::UI::RectTransform>();
				auto after = engine::UI::RectTransform{};
				EditorCommandManager::ModifyComponent<engine::UI::RectTransform>(entity, before, after);
			} },
		};

		ComponentHeader header("Rect", menuActions);
		if (!header.open) return;

		EditResult result;
		static engine::UI::RectTransform s_RectBefore;

		PropertyTable table;

		auto& rc = entity.GetComponent<engine::UI::RectTransform>();

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
			EditorCommandManager::ModifyComponent<engine::UI::RectTransform>(entity, s_RectBefore, rc);
		}
		else if (result.changed)
		{
			entity.AddTag<engine::UI::LayoutDirtyTag>();
		}
	}

	void DrawUIImage(engine::Entity entity)
	{
		if (!entity.HasComponent<engine::UI::Image>()) return;

		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {
				auto before = entity.GetComponent<engine::UI::Image>();
				auto after = engine::UI::Image{};
				EditorCommandManager::ModifyComponent<engine::UI::Image>(entity, before, after);
			} },
		};

		ComponentHeader header("Image", menuActions);
		if (!header.open) return;
		auto& ic = entity.GetComponent<engine::UI::Image>();

		EditResult result;
		static engine::UI::Image s_ImageBefore;
		PropertyTable table;

		result |= PropertyField("Tint", ic.tint, { .mode = DisplayMode::Color });
		result |= PropertyField("Texture", ic.iconName);

		if (result.started)
		{
			s_ImageBefore = { ic.iconName, ic.tint };
		}
		else if (result.finished)
		{
			EditorCommandManager::ModifyComponent<engine::UI::Image>(entity, s_ImageBefore, ic);
		}
		else if (result.changed)
		{
			entity.AddTag<engine::TransformDirty>();
		}
	}

	void DrawUIText(engine::Entity entity)
	{
		if (!entity.HasComponent<engine::UI::Text>()) return;
		const std::initializer_list<ComponentMenuAction> menuActions
		{
			{ "Reset", [&] {
				auto before = entity.GetComponent<engine::UI::Text>();
				auto after = engine::UI::Text{};
				EditorCommandManager::ModifyComponent<engine::UI::Text>(entity, before, after);
			} },
		};
		ComponentHeader header("Text", menuActions);
		if (!header.open) return;
		auto& tc = entity.GetComponent<engine::UI::Text>();
		EditResult result;
		static engine::UI::Text s_TextBefore;
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
			EditorCommandManager::ModifyComponent<engine::UI::Text>(entity, s_TextBefore, tc);
		}
		else if (result.changed)
		{
			entity.AddTag<engine::TransformDirty>();
		}
	}


	void DrawAddComponentButton(engine::Entity entity)
	{
		struct AddComponentEntry
		{
			const char* name;
			void (*add)(engine::Entity);
		};

		static constexpr std::array<AddComponentEntry, 9> entries
		{
			AddComponentEntry{ "Transform",     [](engine::Entity e) { e.GetOrAddComponent<engine::TransformComponent>(); e.GetOrAddComponent<engine::WorldTransformComponent>(); } },
			AddComponentEntry{ "Camera",        [](engine::Entity e) { e.GetOrAddComponent<engine::CameraComponent>(); } },
			AddComponentEntry{ "Mesh",          [](engine::Entity e) { e.GetOrAddComponent<engine::MeshComponent>(); } },
			AddComponentEntry{ "Collider",      [](engine::Entity e) { e.GetOrAddComponent<engine::ColliderComponent>(); } },
			AddComponentEntry{ "Light",         [](engine::Entity e) { e.GetOrAddComponent<engine::LightComponent>(); } },
			AddComponentEntry{ "UI Rect",				[](engine::Entity e) { e.GetOrAddComponent<engine::UI::RectTransform>(); } },
			AddComponentEntry{ "UI Canvas",			[](engine::Entity e) { e.GetOrAddComponent<engine::UI::Canvas>(); e.GetOrAddComponent<engine::UI::RectTransform>(); e.AddTag<engine::UI::LayoutDirtyTag>(); } },
			AddComponentEntry{ "UI Image",			[](engine::Entity e) { e.GetOrAddComponent<engine::UI::Image>(); e.GetOrAddComponent<engine::UI::RectTransform>(); e.AddTag<engine::UI::LayoutDirtyTag>(); } },
			AddComponentEntry{ "UI Text",				[](engine::Entity e) { e.GetOrAddComponent<engine::UI::Text>(); e.GetOrAddComponent<engine::UI::RectTransform>(); e.AddTag<engine::UI::LayoutDirtyTag>(); } }
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

	void EntityInspectorPanel::Draw(engine::Uuid entityId)
	{
		engine::SceneAsset& scene = engine::AssetManager::GetAsset<engine::SceneAsset>(engine::SceneManager::GetActiveScene());
		engine::Entity entity = scene.GetEntity(entityId);
		if (!entity) return;

		EntityHeader header(entity);

		DrawTransform(entity);
		DrawCamera(entity);
		DrawMesh(entity);
		DrawCollider(entity);
		DrawLight(entity);
		DrawUIRect(entity);
		DrawUICanvas(entity);
		DrawUIImage(entity);
		DrawUIText(entity);

		DrawAddComponentButton(entity);
	}
}