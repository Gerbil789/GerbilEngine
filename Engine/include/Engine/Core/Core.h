#pragma once

#include "Engine/Core/Log.h" //TODO: remove dependency

#ifdef DEBUG
#define ENGINE_ENABLE_ASSERTS
//#define ENGINE_PROFILE
#endif

#if defined(ENGINE_PLATFORM_WINDOWS)
#define DEBUG_BREAK() __debugbreak()
#elif defined(__GNUC__) || defined(__clang__)
#define DEBUG_BREAK() __builtin_trap()
#else
#include <cstdlib>
#define DEBUG_BREAK() std::abort()
#endif

//TODO: move assert into separate file
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