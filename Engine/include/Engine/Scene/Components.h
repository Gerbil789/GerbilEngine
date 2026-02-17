#pragma once

#include "Engine/Core/API.h"
#include "Engine/Core/UUID.h"
#include <entt.hpp>
#include <glm/glm.hpp>

namespace Engine
{
	class Camera;
	class Mesh;
	class Material;
	class AudioClip;
	class Script;

	struct ENGINE_API IdentityComponent
	{
		Uuid id = Uuid();
		bool enabled = true;
	};

	struct ENGINE_API NameComponent
	{
		std::string name = "Entity";
	};

	struct ENGINE_API TransformComponent
	{
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f }; 
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

		entt::entity parent = entt::null;
		entt::entity firstChild = entt::null;
		entt::entity nextSibling = entt::null;
		entt::entity prevSibling = entt::null;

		TransformComponent() = default;
		TransformComponent(const glm::vec3& position) : position(position) {}

		glm::mat4 GetLocalMatrix() const;
		glm::mat4 GetWorldMatrix(const entt::registry& registry) const; //TODO: passing registry is not ideal

		void Reset()
		{
			position = { 0.0f, 0.0f, 0.0f };
			rotation = { 0.0f, 0.0f, 0.0f };
			scale = { 1.0f, 1.0f, 1.0f };
		}
	};

	struct ENGINE_API MeshComponent
	{
		Material* material = nullptr;
		Mesh* mesh = nullptr;

		MeshComponent() = default;
		MeshComponent(Material* material, Mesh* mesh) : material(material), mesh(mesh) {}

		void Reset()
		{
			material = nullptr;
			mesh = nullptr;
		}
	};

	struct ENGINE_API CameraComponent
	{
		Camera* camera = nullptr;
	};

	enum class LightType { Point = 0, Directional = 1, Spot = 2 }; //TODO: move to the light system
	struct ENGINE_API LightComponent
	{
		LightType type = LightType::Point;
		glm::vec3 color = { 1.0f, 1.0f, 1.0f };
		float intensity = 1.0f;
		float range = 10.0f;
		float innerAngle = 30.0f;
		float outerAngle = 50.0f;
		glm::vec3 attenuation = { 1.0f, 0.09f, 0.032f }; // Constant, Linear, Quadratic

		void Reset()
		{
			type = LightType::Point;
			color = { 1.0f, 1.0f, 1.0f };
			intensity = 1.0f;
			range = 10.0f;
			innerAngle = 30.0f;
			outerAngle = 50.0f;
			attenuation = { 1.0f, 0.09f, 0.032f };
		}
	};

	struct ENGINE_API AudioSourceComponent
	{
		AudioClip* clip = nullptr;
		float volume = 1.0f;
		bool loop = false;
		bool playOnAwake = false;

		void Reset()
		{
			clip = nullptr;
			volume = 1.0f;
			loop = false;
			playOnAwake = false;
		}

		void Play();
		void Stop();
		void SetVolume(float volume);
		void SetLooping(bool loop);

	};

	struct ENGINE_API AudioListenerComponent
	{
		bool isActive = true;
	};

	struct ENGINE_API ScriptComponent
	{
		std::string id;
		Script* instance = nullptr;

		void Reset()
		{
			id.clear();
			instance = nullptr;
		}	
	};
}