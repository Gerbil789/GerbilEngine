#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Engine/Renderer/OrthographicCamera.h"
#include "Engine/Scene/SceneCamera.h"
#include "Engine/Scene/ScriptableEntity.h"

namespace Engine
{
	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag) : Tag(tag.empty() ? "Entity" : tag) {}

		operator std::string& () { return Tag; }
		operator const std::string& () const { return Tag; }
	};


	struct TransformComponent
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f }; 
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& position) : Position(position) {}

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.x), { 1, 0, 0 })
				* glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.y), { 0, 1, 0 })
				* glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.z), { 0, 0, 1 });

			return glm::translate(glm::mat4(1.0f), Position)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};


	struct SpriteRendererComponent
	{
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color) : Color(color) {}

		operator glm::vec4& () { return Color; }
		operator const glm::vec4& () const { return Color; }
	};


	struct CameraComponent
	{
		SceneCamera Camera;
		bool Main = true; // TODO: Move to Scene
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};


	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity*(*InstantiateScript)() = nullptr;
		void(*DestroyScript)(NativeScriptComponent*) = nullptr;

		void(*OnCreateFunction)(ScriptableEntity*) = nullptr;
		void(*OnDestroyFunction)(ScriptableEntity*) = nullptr;
		void(*OnUpdateFunction)(ScriptableEntity*, Timestep) = nullptr;

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) {delete nsc->Instance; nsc->Instance = nullptr; };

			OnCreateFunction = [](ScriptableEntity* entity) {static_cast<T*>(entity)->OnCreate(); };
			OnDestroyFunction = [](ScriptableEntity* entity) {static_cast<T*>(entity)->OnDestroy(); };
			OnUpdateFunction = [](ScriptableEntity* entity, Timestep ts) {static_cast<T*>(entity)->OnUpdate(ts); };
		}
	};
}