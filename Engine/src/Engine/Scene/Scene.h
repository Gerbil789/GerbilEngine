#pragma once

#include "entt.hpp"
#include "Engine/Core/Timestep.h"
#include "Engine/Renderer/EditorCamera.h"
#include "Engine/Core/UUID.h"
#include "Engine/Core/Asset.h"
#include "Engine/Scene/Material.h"

namespace Engine 
{
	class Entity; // Forward declaration

	class SceneFactory : public IAssetFactory
	{
	public:
		virtual Ref<Asset> Load(const std::string& filePath) override;
		virtual Ref<Asset> Create(const std::string& filePath) override;
	};

	class Scene : public Asset
	{
	public:
		enum class SceneState { Editor = 0, Runtime = 1 };

	public:
		Scene(const std::string& filePath) : Asset(filePath) {}
		~Scene();

		static Ref<Scene> Copy(const Ref<Scene>& other);
		SceneState GetSceneState() const { return m_SceneState; }
		bool IsPlaying() const { return m_IsPlaying; }
		bool IsPaused() const { return m_IsPaused; }
		void OnUpdate(Timestep ts);
		void OnUpdate(Timestep ts, EditorCamera& camera);

		void OnPlay();
		void OnStop();
		void OnPause();
		void OnResume();
		void OnNextFrame();
		void OnViewportResize(uint32_t width, uint32_t height);
		void OnDestroy();

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntity(UUID uuid, const std::string& name = std::string());
		void DestroyEntity(Entity entity);
		Entity GetEntityByUUID(UUID uuid);
		std::vector<Entity> GetLightEntities();
		std::vector<Entity> GetEntities();
		std::vector<Entity> GetEntitiesOrdered();
		void DuplicateEntity(Entity entity);
		void CopyEntity(Entity entity);
		void PasteEntity();
		void SelectEntity(Entity entity);
		void DeselectEntity();
		bool IsEntitySelected(Entity entity) const;
		const Entity& GetSelectedEntity();
		const std::vector<UUID>& GetEntityOrder() const { return m_EntityOrder; }
		void ReorderEntity(Entity sourceEntity, Entity targetEntity);

		//temp
		void SelectMaterial(const Ref<Material>& material);
		const Ref<Material>& GetSelectedMaterial() const { return m_SelectedMaterial; }
	private:
		void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		void OnUpdateEditor(Timestep ts, Camera& camera, const glm::mat4& transform);



		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

	private:
		entt::registry m_Registry;
		std::unordered_map<UUID, entt::entity> m_EntityMap;
		std::vector<UUID> m_EntityOrder;

		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		bool m_IsPlaying = false;
		bool m_IsPaused = false;

		//TODO: move to project when projtects are implemented & implement observer pattern to notify selection change
		UUID m_CopiedEntityUUID = 0;
		entt::entity m_SelectedEntity = entt::null;
		Ref<Material> m_SelectedMaterial = nullptr; 

		SceneState m_SceneState = SceneState::Editor;

		friend class Entity;
		friend class SceneHierarchyPanel;
	};

}