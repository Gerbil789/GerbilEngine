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

namespace Engine //TODO: remove all default constructors later, now its needed for something...
{
	struct IdentityComponent
	{
		UUID ID;
		bool Enabled = true;

		IdentityComponent() : ID(UUID()) {}
		IdentityComponent(UUID uuid, bool enabled = true) : ID(uuid), Enabled(enabled) {}
	};

	struct NameComponent
	{
		std::string Name;

		NameComponent() = default;
		NameComponent(const std::string& name) : Name(name.empty() ? "Entity" : name) {}

		operator std::string& () { return Name; }
		operator const std::string& () const { return Name; }
	};

	struct TransformComponent
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f }; 
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		entt::entity Parent = entt::null;
		entt::entity FirstChild = entt::null;
		entt::entity NextSibling = entt::null;
		entt::entity PrevSibling = entt::null;

		TransformComponent() = default;
		TransformComponent(const glm::vec3& position) : Position(position) {}

		glm::mat4 GetLocalMatrix() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(glm::radians(Rotation)));
			return glm::translate(glm::mat4(1.0f), Position) * rotation * glm::scale(glm::mat4(1.0f), Scale);
		}

		glm::mat4 GetWorldMatrix(const entt::registry& registry) const
		{
			glm::mat4 local = GetLocalMatrix();
			if (Parent != entt::null)
			{
				const auto& parentTransform = registry.get<TransformComponent>(Parent);
				return parentTransform.GetWorldMatrix(registry) * local;
			}
			return local;
		}
	};

	struct MeshComponent
	{
		Ref<Material> Material = nullptr;
		Ref<Mesh> Mesh = nullptr;

		MeshComponent() = default;
	};


	struct CameraComponent
	{
		Ref<Camera> Camera = nullptr;

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
		LightType Type = LightType::Point;
		glm::vec3 Color = { 1.0f, 1.0f, 1.0f };
		float Intensity = 1.0f;
		float Range = 10.0f;
		float InnerAngle = 30.0f;
		float OuterAngle = 50.0f;
		glm::vec3 Attenuation = { 1.0f, 0.09f, 0.032f }; // Constant, Linear, Quadratic

		LightComponent() = default;
	};

	struct AudioSourceComponent
	{
		Ref<AudioClip> Clip = nullptr;
		float Volume = 1.0f;
		bool Loop = false;
		bool PlayOnAwake = false;

		AudioSourceComponent() = default;
	};

	struct AudioListenerComponent
	{
		bool IsActive = true;
		AudioListenerComponent() = default;
	};
}