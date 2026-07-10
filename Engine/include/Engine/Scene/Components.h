#pragma once

#include "Engine/Core/UUID.h"
#include "Engine/Math/AABB.h"
#include <string>
#include <entt/fwd.hpp>

namespace Engine
{
	class Camera;
	class Script;

	struct ENGINE_API DisabledTag {};
	struct ENGINE_API DirtyTag {};

	struct ENGINE_API IdentityComponent
	{
		Uuid id;
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
	};

	struct ENGINE_API WorldTransformComponent
	{
		glm::mat4 worldMatrix{ 1.0f };
	};

	struct ENGINE_API HierarchyComponent
	{
		entt::entity parent{ entt::null };
		std::vector<entt::entity> children;
	};

	struct ENGINE_API MeshComponent
	{
		Uuid meshId;
		std::vector<Uuid> materials;
	};

	enum class BodyType { Static = 0, Dynamic, Kinematic };
	enum class ColliderShape { Box, Sphere, Mesh };

	struct ENGINE_API ColliderComponent
	{
		ColliderShape shape = ColliderShape::Box;
		BodyType type = BodyType::Dynamic;
		Uuid collisionMeshId;
		AABB worldAABB;
		bool isTrigger = false;
	};

	struct ENGINE_API CameraComponent
	{
		Camera* camera = nullptr;
		bool primary = false;
	};

	enum class LightType { Directional = 0, Spot, Point };

	struct ENGINE_API LightComponent
	{
		LightType type = LightType::Directional;

		glm::vec3 color = { 1.0f, 1.0f, 1.0f };
		float intensity = 1.0f;

		// shadows
		bool castsShadows = true;
		float nearPlane = 0.1f;
		float farPlane = 100.0f;

		float range = 50.0f;
		float angle;

		union
		{
			struct
			{

			} directional;

			struct
			{
				float fov;
				float aspect;
			} spot;

			struct
			{

			} point;
		};
	};

	struct ENGINE_API ScriptComponent
	{
		uint32_t id{ 0 };
		Script* instance = nullptr;
	};
}