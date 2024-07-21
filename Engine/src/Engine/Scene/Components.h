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
		glm::mat4 Transform = glm::mat4(1.0f);

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4& transform) : Transform(transform) {}

		operator glm::mat4& () { return Transform; }
		operator const glm::mat4& () const { return Transform; }
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