#pragma once

#if defined(ENGINE_STATIC)
  #define ENGINE_API
#elif defined(ENGINE_BUILD_DLL)
  #define ENGINE_API __declspec(dllexport)
#else
  #define ENGINE_API __declspec(dllimport)
#endif