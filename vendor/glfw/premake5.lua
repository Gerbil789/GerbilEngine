project "glfw"
language "C"
kind "StaticLib"
warnings "Off"

files
{
  "src/**.cpp"
}

files
{
	"include/GLFW/**.h",

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

filter "configurations:not Dist"
	kind "SharedLib"
  defines 
  { 
    "_GLFW_BUILD_DLL",
  }

filter "system:windows"
  defines 
  { 
    "_GLFW_WIN32",
    "_CRT_SECURE_NO_WARNINGS",
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
  defines 
  { 
    "_GLFW_X11"
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
    "src/glx_context.c",
		"src/posix_time.c",
    "src/posix_thread.c",
    "src/posix_module.c",
    "src/posix_poll.c",
  }