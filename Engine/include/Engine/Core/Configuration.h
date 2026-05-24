#pragma once
#include <string_view>

namespace Engine
{
#if defined(DEBUG)
	constexpr std::string_view Configuration = "Debug";
#elif defined(RELEASE)
	constexpr std::string_view Configuration = "Release";
#elif defined(DIST)
	constexpr std::string_view Configuration = "Dist";
#else
	constexpr std::string_view Configuration = "Unknown";
#endif
}