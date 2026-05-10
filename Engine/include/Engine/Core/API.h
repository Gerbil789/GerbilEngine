#pragma once

//#if defined(ENGINE_BUILD_STATIC)
//  #define ENGINE_API
//#elif defined(ENGINE_BUILD_SHARED)
//  #define ENGINE_API __declspec(dllexport)
//#else
//  #define ENGINE_API __declspec(dllimport)
//#endif


#ifdef ENGINE_PLATFORM_WINDOWS
  #ifdef ENGINE_BUILD_SHARED
    #define ENGINE_API __declspec(dllexport)
  #else
     #define ENGINE_API __declspec(dllimport)
  #endif
#elif defined(ENGINE_PLATFORM_LINUX) || defined(ENGINE_PLATFORM_WEB)
  #define ENGINE_API
#else
  #error Unknown platform!
#endif