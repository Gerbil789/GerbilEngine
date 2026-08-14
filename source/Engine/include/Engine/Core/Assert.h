#pragma once

#include "Engine/Core/Log.h"

#define ENGINE_ENABLE_ASSERTS //TODO: configure in premake?

#if defined(ENGINE_PLATFORM_WINDOWS)
#define DEBUG_BREAK() __debugbreak()
#elif defined(__GNUC__) || defined(__clang__)
#define DEBUG_BREAK() __builtin_trap()
#else
#include <cstdlib>
#define DEBUG_BREAK() std::abort()
#endif

#ifdef ENGINE_ENABLE_ASSERTS
#define ENGINE_ASSERT(x, ...) \
        do { \
            if (!(x)) { \
                LOG_CRITICAL("Assertion Failed: " __VA_ARGS__); \
                DEBUG_BREAK(); \
            } \
        } while(0)
#else
#define ENGINE_ASSERT(x, ...)
#endif
