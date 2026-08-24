#pragma once

#include "Engine/Asset/AssetHandle.h"
#include "Engine/Math/AABB.h"
#include <string>
#include <entt/entity/entity.hpp>

namespace Engine
{
	class Script;

	struct EditorTag {};
	struct DisabledTag {};
	struct TransformDirty {};

	struct IdentityComponent
	{
		Uuid id;
	};

	struct NameComponent
	{
		std::string name;
	};

	struct TransformComponent
	{
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f }; // radians
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
	};

	struct WorldTransformComponent
	{
		glm::mat4 worldMatrix{ 1.0f };
	};

	struct HierarchyComponent
	{
		entt::entity parent{ entt::null };
		std::vector<entt::entity> children; //TODO: vector uses heap allocation, not good in hot path...
	};

	struct MeshComponent
	{
		Mesh mesh;
		std::vector<Material> materials;
	};

	enum class BodyType { Static = 0, Dynamic, Kinematic };
	enum class ColliderShape { Box, Sphere, Mesh };

	struct ColliderComponent
	{
		ColliderShape shape = ColliderShape::Box;
		BodyType type = BodyType::Dynamic;
		Mesh collisionMesh;
		AABB worldAABB;
		bool isTrigger = false;
	};
	
	struct PrimaryCameraTag {};
	struct CameraViewDirty {};
	struct CameraProjectionDirty {};

	struct CameraComponent
	{
		enum class Projection { Perspective, Orthographic };
		enum class Background { Color, Skybox };

		Projection projectionType = Projection::Perspective;
		Background background = Background::Color;
		glm::vec4 clearColor = { 1.0f, 0.05f, 1.0f, 1.0f };

		glm::mat4 projectionMatrix{ 1.0f };
		glm::mat4 viewMatrix{ 1.0f };
		glm::mat4 viewProjectionMatrix{ 1.0f };

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
	};

	enum class LightType { Directional = 0, Spot, Point };

	struct LightComponent
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

	namespace UI
	{
		struct LayoutDirtyTag {};

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




		//struct Interactable
		//{
		//	bool isHovered{ false };
		//	bool isPressed{ false };
		//	bool isDisabled{ false };
		//};

		//struct Button
		//{
		//	glm::vec4 normalColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		//	glm::vec4 hoverColor{ 0.8f, 0.8f, 0.8f, 1.0f };
		//	glm::vec4 pressedColor{ 0.5f, 0.5f, 0.5f, 1.0f };
		//	glm::vec4 disabledColor{ 0.3f, 0.3f, 0.3f, 1.0f };

		//	uint32_t hoverIconHash{ 0 };
		//	uint32_t pressedIconHash{ 0 };
		//};
	}
}