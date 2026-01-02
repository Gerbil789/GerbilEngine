#pragma once

#include "Engine/Core/Log.h" //TODO: remove dependency

#ifdef DEBUG
#define ENGINE_ENABLE_ASSERTS
//#define ENGINE_PROFILE
#endif

#if defined(_MSC_VER)
#define DEBUG_BREAK() __debugbreak()
#elif defined(__GNUC__) || defined(__clang__)
#define DEBUG_BREAK() __builtin_trap()
#else
#include <cstdlib>
#define DEBUG_BREAK() std::abort()
#endif

#ifdef ENGINE_ENABLE_ASSERTS
#define ASSERT(x, ...) { if(!(x)) { LOG_CRITICAL("Assertion Failed: {0}", __VA_ARGS__); DEBUG_BREAK(); } }
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