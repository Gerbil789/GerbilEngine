#pragma once

#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include <string>
#include <entt/entity/entity.hpp>

namespace engine::tag
{
	struct DisabledTag {};						// This entity is disabled
	struct TransformDirty {};					// World transform must be recalculated
	struct CameraViewDirty {};				// View camera matrix must be recalculated 
	struct CameraProjectionDirty {};	// Projection camera matrix must be recalculated
	struct LayoutDirtyTag {};					// UI layout must be recalculated
}

namespace engine::component
{
	struct Identity
	{
		Uuid id;
	};

	struct Name
	{
		std::string name;
	};

	struct Transform
	{
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f }; // radians
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
	};

	struct WorldTransform
	{
		glm::mat4 worldMatrix{ 1.0f };
	};

	struct Hierarchy
	{
		entt::entity parent{ entt::null };
		std::vector<entt::entity> children; //TODO: vector uses heap allocation, not good in hot path...
	};

	struct Mesh
	{
		MeshHandle mesh;
		std::vector<MaterialInstanceHandle> materialInstances{};
	};

	struct Collider
	{
		enum class BodyType { Static = 0, Dynamic, Kinematic } type = BodyType::Static;
		enum class ColliderShape { Box = 0, Sphere, Mesh } shape = ColliderShape::Box;

		MeshHandle collisionMesh;
		AABB worldAABB;
		bool isTrigger = false;
	};

	struct CameraComponent
	{
		enum class Projection { Perspective, Orthographic } projection = Projection::Perspective;

		struct Perspective
		{
			float fov = glm::radians(45.0f);
			float nearClip = 0.1f;
			float farClip = 512.0f;
		} perspective;

		struct Orthographic
		{
			float size = 10.0f;
			float nearClip = -1.0f;
			float farClip = 512.0f;
		} orthographic;

		glm::mat4 projectionMatrix{ 1.0f };
		glm::mat4 viewMatrix{ 1.0f };
		glm::mat4 viewProjectionMatrix{ 1.0f };

		enum class Background { Color, Skybox };

		Background backgroundMode = Background::Color;
		glm::vec4 ambientLight = { 0.1f, 0.1f, 0.1f, 1.0f };
		glm::vec4 clearColor = { 1.0f, 0.05f, 1.0f, 1.0f };
	};

	struct Light
	{
		enum class LightType { Directional = 0, Spot, Point } type = LightType::Directional;

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

	struct RectTransform
	{
		glm::vec2 anchorMin{ 0.5f, 0.5f };
		glm::vec2 anchorMax{ 0.5f, 0.5f };
		glm::vec2 pivot{ 0.5f, 0.5f };

		glm::vec2 anchoredPosition{ 0.0f, 0.0f };
		glm::vec2 size{ 100.0f, 100.0f };

		glm::vec2 absolutePosition{ 0.0f, 0.0f };
		glm::vec2 absoluteSize{ 0.0f, 0.0f };
	};

	struct Canvas
	{
		bool isScreenSpace = true;
		glm::vec2 referenceResolution{ 1920.0f, 1080.0f };
		float matchWidthOrHeight = 0.5f; // 0 = match width, 1 = match height, 0.5 = balance
	};

	struct Image
	{
		std::string iconName; //TODO: dont use strings as key!!!
		glm::vec4 tint{ 1.0f, 1.0f, 1.0f, 1.0f };
	};

	struct Text
	{
		std::string text = "Gerbil";
		std::string fontName = "Arvo-Regular";
		glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
		float fontSize = 32.0f;
		float lineSpacing = 1.2f;
		bool wrapText = false;
	};

}