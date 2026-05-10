project "glfw"
language "C"
warnings "Off"

files
{
  "include/GLFW/glfw3.h",
  "include/GLFW/glfw3native.h",
  "src/glfw_config.h",
  "src/context.c",
  "src/init.c",
  "src/input.c",
  "src/monitor.c",
  "src/null_init.c",
  "src/null_joystick.c",
  "src/null_monitor.c",
  "src/null_window.c",
  "src/platform.c",
  "src/vulkan.c",
  "src/window.c",
  "src/egl_context.c",
  "src/osmesa_context.c"
}

filter "system:windows"
  kind "SharedLib"
  buildoptions { "/permissive-"}
  defines 
  { 
    "_GLFW_WIN32",
    "_CRT_SECURE_NO_WARNINGS",
    "_GLFW_BUILD_DLL",
  }
  files
  {
    "src/win32_init.c",
    "src/win32_joystick.c",
    "src/win32_module.c",
    "src/win32_monitor.c",
    "src/win32_time.c",
    "src/win32_thread.c",
    "src/win32_window.c",
    "src/wgl_context.c"
  }

filter "system:linux"
  kind "StaticLib"
  pic "On" -- Position Independent Code is required for static libs on Linux
  defines 
  { 
    "_GLFW_X11" -- Tell GLFW to use the X11 Windowing system
  }
  files
  {
    "src/x11_init.c",
    "src/x11_monitor.c",
    "src/x11_window.c",
    "src/xkb_unicode.c",
    "src/posix_time.c",
    "src/posix_thread.c",
    "src/posix_module.c",
    "src/linux_joystick.c",
    "src/glx_context.c"
    -- Note: If you are using GLFW 3.4+, you may also need to add "src/posix_poll.c" here.
  }

filter { "platforms:Web" }
  kind "StaticLib"       
  removefiles { "**.*" } 
  files { "dummy.c" }    

filter "configurations:Debug"
  runtime "Debug"
  symbols "on"

filter "configurations:Release"
  runtime "Release"
  optimize "on"