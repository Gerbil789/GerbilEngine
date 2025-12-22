#pragma once

#include "Engine/Core/UUID.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Renderer/Material.h"
#include "Engine/Audio/AudioClip.h"
#include <entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Engine //TODO: remove all default constructors later, now its needed for some template stuff that i dont like...
{
	struct IdentityComponent
	{
		UUID id;
		bool enabled = true;

		IdentityComponent() : id(UUID()) {}
		IdentityComponent(UUID uuid, bool enabled = true) : id(uuid), enabled(enabled) {}
	};

	struct NameComponent
	{
		std::string name;

		NameComponent() = default;
		NameComponent(const std::string& name) : name(name.empty() ? "Entity" : name) {}

		operator std::string& () { return name; }
		operator const std::string& () const { return name; }
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

		TransformComponent() = default;
		TransformComponent(const glm::vec3& position) : position(position) {}

		glm::mat4 GetLocalMatrix() const
		{
			return glm::translate(glm::mat4(1.0f), position) * glm::toMat4(glm::quat(glm::radians(rotation))) * glm::scale(glm::mat4(1.0f), scale);
		}

		glm::mat4 GetWorldMatrix(const entt::registry& registry) const
		{
			glm::mat4 local = GetLocalMatrix();
			if (parent != entt::null)
			{
				const auto& parentTransform = registry.get<TransformComponent>(parent);
				return parentTransform.GetWorldMatrix(registry) * local;
			}
			return local;
		}
	};

	struct MeshComponent
	{
		Material* material = nullptr;
		Mesh* mesh = nullptr;

		MeshComponent() = default;
	};


	struct CameraComponent
	{
		Camera* camera = nullptr;

		CameraComponent() = default;
	};


	enum class LightType
	{
		Point = 0,
		Directional = 1,
		Spot = 2
	};

	struct LightComponent
	{
		LightType type = LightType::Point;
		glm::vec3 color = { 1.0f, 1.0f, 1.0f };
		float intensity = 1.0f;
		float range = 10.0f;
		float innerAngle = 30.0f;
		float outerAngle = 50.0f;
		glm::vec3 attenuation = { 1.0f, 0.09f, 0.032f }; // Constant, Linear, Quadratic

		LightComponent() = default;
	};

	struct AudioSourceComponent
	{
		AudioClip* clip = nullptr;
		float volume = 1.0f;
		bool loop = false;
		bool playOnAwake = false;

		AudioSourceComponent() = default;
	};

	struct AudioListenerComponent
	{
		bool isActive = true;
		AudioListenerComponent() = default;
	};
}