#pragma once

#include "Engine/Core/Log.h"
#include <filesystem>

#ifdef DEBUG
	#define ENGINE_ENABLE_ASSERTS
	#define ENGINE_PROFILE
#endif

#ifdef ENGINE_ENABLE_ASSERTS
	#define ASSERT(x, ...) { if(!(x)) { LOG_CRITICAL("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define ASSERT(x, ...)
#endif

#ifdef ENGINE_PROFILE
#include "Engine/Debug/Instrumentor.h"
#define ENGINE_PROFILE_BEGIN(name, filepath)::Engine::Instrumentor::Get().BeginSession(name, filepath)
#define ENGINE_PROFILE_END()::Engine::Instrumentor::Get().EndSession()
#define ENGINE_PROFILE_SCOPE(name)::Engine::InstrumentationTimer timer##__LINE__(name);
#define ENGINE_PROFILE_FUNCTION() ENGINE_PROFILE_SCOPE(__FUNCSIG__)
#else
#define ENGINE_PROFILE_BEGIN(name, filepath)
#define ENGINE_PROFILE_END()
#define ENGINE_PROFILE_SCOPE(name)
#define ENGINE_PROFILE_FUNCTION()
#endif

#define ENGINE_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

template<typename T>
using Scope = std::unique_ptr<T>;
template<typename T, typename ... Args>
constexpr Scope<T> CreateScope(Args&& ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Ref = std::shared_ptr<T>;
template<typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

inline std::filesystem::path ENGINE_RESOURCES; // Path to the Engine resources directory
inline std::filesystem::path ASSETS; // Path to the Assets directory of the current project

namespace Engine::Resources
{
	namespace Shaders
	{
		inline std::filesystem::path EntityId() { return ENGINE_RESOURCES / "shaders/entityId.wgsl"; }
		inline std::filesystem::path Flat() { return ENGINE_RESOURCES / "shaders/flatColor.wgsl"; }
		inline std::filesystem::path Phong() { return ENGINE_RESOURCES / "shaders/phong.wgsl"; }
	}

	namespace Models
	{
		inline std::filesystem::path Cube() { return ENGINE_RESOURCES / "models/cube.glb"; }
	}

	namespace Materials
	{
		inline std::filesystem::path Default() { return ENGINE_RESOURCES / "materials/default.material"; }
		inline std::filesystem::path PBR() { return ENGINE_RESOURCES / "materials/pbr.material"; }
		inline std::filesystem::path Unlit() { return ENGINE_RESOURCES / "materials/unlit.material"; }
	}

	namespace Fonts
	{
		namespace Roboto
		{
			inline std::filesystem::path Regular() { return ENGINE_RESOURCES / "fonts/roboto/Roboto-Regular.ttf"; }
			inline std::filesystem::path Bold() { return ENGINE_RESOURCES / "fonts/roboto/Roboto-Bold.ttf"; }
			inline std::filesystem::path Italic() { return ENGINE_RESOURCES / "fonts/roboto/Roboto-Italic.ttf"; }
		}
	}
}