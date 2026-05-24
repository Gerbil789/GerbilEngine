#pragma once

#define ENGINE_API

#ifdef ENGINE_PLATFORM_WINDOWS
  #ifdef ENGINE_SHARED_EXPORT
    #undef ENGINE_API
    #define ENGINE_API __declspec(dllexport)
  #elif ENGINE_SHARED_IMPORT
    #undef ENGINE_API
    #define ENGINE_API __declspec(dllimport)
  #endif
#endif