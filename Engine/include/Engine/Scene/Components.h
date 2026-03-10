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

		MeshComponent() = default;
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

	enum class LightType : std::uint8_t { Point = 0, Directional = 1, Spot = 2 };

	struct ENGINE_API LightComponent
	{
		glm::vec3 color = { 1.0f, 1.0f, 1.0f };
		float intensity = 1.0f;
		float range = 10.0f;
		float angle = 45.0f;
		LightType type = LightType::Point;

		void Reset()
		{
			type = LightType::Point;
			color = { 1.0f, 1.0f, 1.0f };
			intensity = 1.0f;
			range = 10.0f;
			angle = 45.0f;
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