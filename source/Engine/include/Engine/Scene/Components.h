#pragma once

#include "Engine/Core/UUID.h"
#include "Engine/Math/AABB.h"
#include "Engine/Graphics/Camera.h"
#include <string>
#include <entt/fwd.hpp>

namespace Engine
{
	class Script;

	struct ENGINE_API DisabledTag {};
	struct ENGINE_API DirtyTag {};

	struct ENGINE_API IdentityComponent
	{
		Uuid id;
	};

	struct ENGINE_API NameComponent
	{
		std::string name;
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
		std::vector<entt::entity> children; //TODO: vector uses heap allocation, not good in hot path...
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

	struct ENGINE_API PrimaryCameraTag {};

	struct ENGINE_API CameraComponent
	{
		Camera camera;
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


	namespace UI
	{
		struct LayoutDirtyTag {};

		struct ENGINE_API RectTransform
		{
			glm::vec2 anchorMin{ 0.5f, 0.5f };
			glm::vec2 anchorMax{ 0.5f, 0.5f };
			glm::vec2 pivot{ 0.5f, 0.5f };

			glm::vec2 anchoredPosition{ 0.0f, 0.0f };
			glm::vec2 size{ 100.0f, 100.0f };

			glm::vec2 absolutePosition{ 0.0f, 0.0f };
			glm::vec2 absoluteSize{ 0.0f, 0.0f };
		};

		struct ENGINE_API Canvas
		{
			bool isScreenSpace = true;
			glm::vec2 referenceResolution{ 1920.0f, 1080.0f };
			float matchWidthOrHeight = 0.5f; // 0 = match width, 1 = match height, 0.5 = balance
		};

		struct ENGINE_API Image
		{
			std::string iconName; //TODO: dont use strings as key!!!
			glm::vec4 tint{ 1.0f, 1.0f, 1.0f, 1.0f };
		};

		struct ENGINE_API Text
		{
			std::string text = "Gerbil";
			std::string fontName = "Arvo-Regular";
			glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
			float fontSize = 32.0f;
			float lineSpacing = 1.2f;
			bool wrapText = false;
		};




		//struct ENGINE_API Interactable
		//{
		//	bool isHovered{ false };
		//	bool isPressed{ false };
		//	bool isDisabled{ false };
		//};

		//struct ENGINE_API Button
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