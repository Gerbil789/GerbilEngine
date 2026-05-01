#pragma once

#include "Engine/Scene/Entity.h"
#include "Engine/Core/Resources.h"
#include "Engine/Graphics/Mesh.h" //TODO: remove this dependency, only need AABB
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
		Uuid id{ 0 };
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

		Entity parent;
		Entity firstChild;
		Entity nextSibling;
		Entity prevSibling;

		glm::mat4 GetLocal() const;
		glm::mat4 GetWorld() const;
	};

	struct ENGINE_API MeshComponent
	{
		Uuid meshId{ 0 };
		std::vector<Uuid> materials{ Uuid{RESOURCES::MATERIAL::PINK} };
	};

	enum class BodyType { Static = 0, Dynamic, Kinematic };

	struct ENGINE_API ColliderComponent
	{
		Uuid meshId{ RESOURCES::MESH::SPHERE };
		AABB worldAABB;
		BodyType type = BodyType::Dynamic;
		bool isTrigger = false;
	};

	struct ENGINE_API CameraComponent
	{
		Camera* camera = nullptr;
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
		std::string id; //TODO: dont use string as id
		Script* instance = nullptr;
	};
}