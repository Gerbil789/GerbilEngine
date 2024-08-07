#pragma once

#include "entt.hpp"
#include "Engine/Core/Timestep.h"
#include "Engine/Renderer/EditorCamera.h"
#include "Engine/Core/UUID.h"

namespace Engine 
{
	class Entity; // Forward declaration

	class Scene
	{
	public:
		enum class SceneState
		{
			Edit = 0, Play = 1
		};
	public:
		Scene();
		~Scene();

		static Ref<Scene> Copy(const Ref<Scene>& other);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);

		void OnViewportResize(uint32_t width, uint32_t height);

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntity(UUID uuid, const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		Entity GetEntityByName(const std::string& name);

		Entity GetMainCameraEntity();

		void DuplicateEntity(Entity entity);
		void CopyEntity(Entity entity);
		void PasteEntity();

		void SelectEntity(Entity entity);
		void DeselectEntity();
		bool IsEntitySelected(Entity entity);
		const Entity& GetSelectedEntity();
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		bool m_IsRunning = false;

		UUID m_CopiedEntityUUID = 0;

		entt::entity selectedEntity = entt::null;

		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
	};

}