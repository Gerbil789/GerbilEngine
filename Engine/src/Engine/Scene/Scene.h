#pragma once

#include "entt.hpp"
#include "Engine/Core/Timestep.h"
#include "Engine/Renderer/EditorCamera.h"
#include "Engine/Core/UUID.h"
#include "Engine/Core/Asset.h"
#include "Engine/Renderer/Material.h"

namespace Engine 
{
	class Entity; // Forward declaration

	class SceneFactory : public IAssetFactory
	{
	public:
		virtual Ref<Asset> Load(const std::filesystem::path& path, const std::any& data = std::any()) override;
		virtual Ref<Asset> Create(const std::filesystem::path& path, const std::any& data = std::any()) override;
	};

	class Scene : public Asset
	{
	public:
		enum class SceneState { Editor = 0, Runtime = 1 };

	public:
		Scene(const std::filesystem::path& path) : Asset(path) {}
		~Scene();
		void RefreshRootEntities();

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

		Entity CreateEntity(const std::string& name = "");
		Entity CreateEntity(UUID uuid, const std::string& name = "");
		void DestroyEntity(Entity entity);
		std::vector<Entity> GetLightEntities();
		std::vector<Entity> GetEntities();

		std::vector<entt::entity>& GetRootEntities() { return m_RootEntities; }
		const std::vector<entt::entity>& GetRootEntities() const { return m_RootEntities; }

		void AddRootEntity(entt::entity entity);
		void RemoveRootEntity(entt::entity entity);
		void ReorderRootEntity(entt::entity entity, size_t newIndex);

		//void ReorderChild(entt::entity parent, entt::entity child, size_t newIndex);


		void SetParent(entt::entity child, entt::entity newParent);
		bool IsDescendant(entt::entity parent, entt::entity child);

		entt::registry m_Registry;
	private:
		void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		void OnUpdateEditor(Timestep ts, Camera& camera, const glm::mat4& transform);


	private:
		std::vector<entt::entity> m_RootEntities;


		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		bool m_IsPlaying = false;
		bool m_IsPaused = false;

		SceneState m_SceneState = SceneState::Editor;

		friend class SceneController;
		friend class Entity;
		friend class SceneHierarchyPanel;
	};

}