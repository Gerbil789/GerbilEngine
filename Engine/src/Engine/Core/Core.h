#pragma once

#include <memory>
#include "Engine/Core/Log.h"

#ifdef ENGINE_PLATFORM_WINDOWS

#else
	#error Engine only supports Windows!
#endif

#ifdef ENGINE_DEBUG
	#define ENGINE_ENABLE_ASSERTS
#endif


#ifdef ENGINE_ENABLE_ASSERTS
	#define ASSERT(x, ...) { if(!(x)) { ENGINE_LOG_CRITICAL("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)
#define ENGINE_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }


namespace Engine 
{
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
}