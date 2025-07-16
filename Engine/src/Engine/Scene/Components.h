#pragma once

#include "Engine/Core/UUID.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Renderer/Material.h"

#include <entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Engine
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

	struct HierarchyComponent 
	{
		entt::entity Parent = entt::null;
		entt::entity FirstChild = entt::null;
		entt::entity NextSibling = entt::null;
		entt::entity PrevSibling = entt::null;
	};

	struct TransformComponent
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f }; 
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };


		TransformComponent() = default;
		TransformComponent(const glm::vec3& position) : Position(position) {}

		glm::mat4 GetModelMatrix() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(glm::radians(Rotation)));
			return glm::translate(glm::mat4(1.0f), Position) * rotation * glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct MeshComponent
	{
		Ref<Material> Material = nullptr;
		Ref<Mesh> Mesh = nullptr;

		wgpu::Buffer ModelBuffer;
		wgpu::BindGroup ModelBindGroup;

		MeshComponent() = default;
	};

	struct SpriteComponent //TODO: rework whole 2D renderer system
	{
		Ref<Material> Material = nullptr;

		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		Ref<Texture2D> Texture = nullptr;
		glm::vec2 TilingFactor = { 1.0f, 1.0f };

		SpriteComponent() = default;
		SpriteComponent(const glm::vec4& color) : Color(color) {}

		operator glm::vec4& () { return Color; }
		operator const glm::vec4& () const { return Color; }
	};


	struct CameraComponent
	{
		Camera Camera; //TODO: use Ref<Camera> instead?
		bool Main = true; // TODO: Move to Scene
		bool FixedAspectRatio = false;

		CameraComponent() = default;
	};


	enum class LightType //TODO: move to some light file
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
}