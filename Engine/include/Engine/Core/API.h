#pragma once

#ifdef ENGINE_PLATFORM_WINDOWS
  #ifdef ENGINE_SHARED_EXPORT
    #define ENGINE_API __declspec(dllexport)
  #else
    #define ENGINE_API __declspec(dllimport)
  #endif
#else
  #define ENGINE_API
#endif