#pragma once

#ifdef ENGINE_PLATFORM_WINDOWS
  #ifdef ENGINE_BUILD_SHARED
    #define ENGINE_API __declspec(dllexport)
  #else
     #define ENGINE_API __declspec(dllimport)
  #endif
#else
  #define ENGINE_API
#endif