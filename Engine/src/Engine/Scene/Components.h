#pragma once
#include "Engine/Scene/SceneCamera.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Core/UUID.h"
#include "Engine/Renderer/Material.h"
#include "Engine/Core/AssetManager.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <entt.hpp>

namespace Engine
{
	struct IDComponent
	{
		UUID ID;

		IDComponent() : ID(UUID()) {}
		IDComponent(UUID uuid) : ID(uuid) {}
	};

	struct EnablingComponent
	{
		bool Enabled = true;

		EnablingComponent() = default;
		EnablingComponent(bool enabled) : Enabled(enabled) {}
	};


	struct NameComponent
	{
		std::string Name;

		NameComponent() = default;
		NameComponent(const std::string& name) : Name(name.empty() ? "Entity" : name) {}

		operator std::string& () { return Name; }
		operator const std::string& () const { return Name; }
	};

	//struct ParentComponent 
	//{
	//	entt::entity parent = entt::null;

	//	ParentComponent() = default;
	//};

	//struct ChildrenComponent 
	//{
	//	std::vector<entt::entity> children;

	//	ChildrenComponent() = default;
	//};

	struct HierarchyComponent {
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

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(glm::radians(Rotation)));
			return glm::translate(glm::mat4(1.0f), Position) * rotation * glm::scale(glm::mat4(1.0f), Scale);
		}
	};


	struct SpriteRendererComponent
	{
		Ref<Material> Material = nullptr;

		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		Ref<Texture2D> Texture = nullptr;
		glm::vec2 TilingFactor = { 1.0f, 1.0f };

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const glm::vec4& color) : Color(color) {}

		operator glm::vec4& () { return Color; }
		operator const glm::vec4& () const { return Color; }
	};


	struct CameraComponent
	{
		SceneCamera Camera;
		bool Main = true; // TODO: Move to Scene
		bool FixedAspectRatio = false;

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

	struct MeshRendererComponent
	{
		Ref<Material> Material = nullptr;
		Ref<Mesh> Mesh = nullptr;

		MeshRendererComponent() = default;
	};

	//class ScriptableEntity; //forward declaration

	//struct NativeScriptComponent
	//{
	//	ScriptableEntity* Instance = nullptr;

	//	ScriptableEntity*(*InstantiateScript)() = nullptr;
	//	void(*DestroyScript)(NativeScriptComponent*) = nullptr;

	//	void(*OnCreateFunction)(ScriptableEntity*) = nullptr;
	//	void(*OnDestroyFunction)(ScriptableEntity*) = nullptr;
	//	void(*OnUpdateFunction)(ScriptableEntity*, Timestep) = nullptr;

	//	template<typename T>
	//	void Bind()
	//	{
	//		InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
	//		DestroyScript = [](NativeScriptComponent* nsc) {delete nsc->Instance; nsc->Instance = nullptr; };

	//		OnCreateFunction = [](ScriptableEntity* entity) {static_cast<T*>(entity)->OnCreate(); };
	//		OnDestroyFunction = [](ScriptableEntity* entity) {static_cast<T*>(entity)->OnDestroy(); };
	//		OnUpdateFunction = [](ScriptableEntity* entity, Timestep ts) {static_cast<T*>(entity)->OnUpdate(ts); };
	//	}
	//};
}