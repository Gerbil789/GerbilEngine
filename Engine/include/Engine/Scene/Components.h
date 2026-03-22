#pragma once

#include "Engine/Scene/Entity.h"
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

		Entity parent;
		Entity firstChild;
		Entity nextSibling;
		Entity prevSibling;

		TransformComponent() = default;
		TransformComponent(const glm::vec3& position) : position(position) {}

		glm::mat4 GetLocalMatrix() const;
		glm::mat4 GetWorldMatrix() const;

		void Reset()
		{
			position = { 0.0f, 0.0f, 0.0f };
			rotation = { 0.0f, 0.0f, 0.0f };
			scale = { 1.0f, 1.0f, 1.0f };
		}
	};

	struct ENGINE_API MeshComponent
	{
		Mesh* mesh = nullptr;
		std::vector<Material*> materials;

		MeshComponent();
		MeshComponent(Mesh* mesh) : mesh(mesh) {}

		Material* GetMaterial(uint32_t index)
		{
			if (index >= materials.size())
			{
				return nullptr;
			}
			return materials[index];
		}

		void SetMaterial(uint32_t index, Material* material)
		{
			if (index >= materials.size())
			{
				materials.resize(index + 1);
			}
			materials[index] = material;
		}

		void Reset()
		{
			mesh = nullptr;
			materials.clear();
		}
	};

	struct ENGINE_API CameraComponent
	{
		Camera* camera = nullptr;
	};

	enum class LightType { Directional = 0, Spot, Point };

	struct ENGINE_API LightComponent
	{
		LightType type = LightType::Directional;

		// common
		glm::vec3 color = { 1.0f, 1.0f, 1.0f };
		float intensity = 1.0f;

		// shadowing
		bool castsShadows = true;
		float nearPlane = 0.1f;
		float farPlane = 100.0f;



		float range = 50.0f; // for point and spot lights, ignored for directional
		float angle;

		// type-specific
		union
		{
			struct // Directional
			{
			} directional;

			struct // Spot
			{
				float fov;      // radians
				float aspect;   // usually 1 for shadow map
			} spot;

			struct // Point
			{
				// no extra params needed beyond position + range
			} point;
		};

		void Reset()
		{
			type = LightType::Directional;
			color = { 1.0f, 1.0f, 1.0f };
			intensity = 1.0f;

		}
	};

	struct ENGINE_API ScriptComponent
	{
		std::string id; //TODO: dont use string as id
		Script* instance = nullptr;

		void Reset()
		{
			id.clear();
			instance = nullptr;
		}	
	};
}