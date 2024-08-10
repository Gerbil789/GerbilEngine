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
		enum class SceneState { Editor = 0, Runtime = 1 };
	public:
		Scene();
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

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntity(UUID uuid, const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		Entity GetEntityByName(const std::string& name);

		void DuplicateEntity(Entity entity);
		void CopyEntity(Entity entity);
		void PasteEntity();
		void SelectEntity(Entity entity);
		void DeselectEntity();
		bool IsEntitySelected(Entity entity);
		const Entity& GetSelectedEntity();
	private:
		void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		void OnUpdateEditor(Timestep ts, Camera& camera, const glm::mat4& transform);



		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		bool m_IsPlaying = false;
		bool m_IsPaused = false;
		
		UUID m_CopiedEntityUUID = 0;
		entt::entity m_SelectedEntity = entt::null;

		SceneState m_SceneState = SceneState::Editor;

		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
	};

}