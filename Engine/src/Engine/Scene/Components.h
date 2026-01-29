#pragma once

#include "Engine/Core/UUID.h"
#include <entt.hpp>
#include <glm/glm.hpp>

namespace Engine
{
	// Forward declarations
	class Camera;
	class Mesh;
	class Material;
	class AudioClip;

	struct IdentityComponent
	{
		UUID id = UUID();
		bool enabled = true;
	};

	struct NameComponent
	{
		std::string name = "Entity";
	};

	struct TransformComponent
	{
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f }; 
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

		entt::entity parent = entt::null;
		entt::entity firstChild = entt::null;
		entt::entity nextSibling = entt::null;
		entt::entity prevSibling = entt::null;

		TransformComponent(const glm::vec3& position) : position(position) {}

		glm::mat4 GetLocalMatrix() const;
		glm::mat4 GetWorldMatrix(const entt::registry& registry) const; //TODO: passing registry is not ideal
	};

	struct MeshComponent
	{
		Material* material = nullptr;
		Mesh* mesh = nullptr;
	};

	struct CameraComponent
	{
		Camera* camera = nullptr;
	};

	enum class LightType { Point = 0, Directional = 1, Spot = 2 }; //TODO: move to the light system
	struct LightComponent
	{
		LightType type = LightType::Point;
		glm::vec3 color = { 1.0f, 1.0f, 1.0f };
		float intensity = 1.0f;
		float range = 10.0f;
		float innerAngle = 30.0f;
		float outerAngle = 50.0f;
		glm::vec3 attenuation = { 1.0f, 0.09f, 0.032f }; // Constant, Linear, Quadratic
	};

	struct AudioSourceComponent
	{
		AudioClip* clip = nullptr;
		float volume = 1.0f;
		bool loop = false;
		bool playOnAwake = false;
	};

	struct AudioListenerComponent
	{
		bool isActive = true;
	};


	struct ScriptComponent
	{
		UUID id;
		void* data;
	};
}