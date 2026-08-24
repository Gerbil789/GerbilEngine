workspace "GerbilEngine"
architecture "x64"
startproject "TestProject"
configurations { "Debug", "Release", "EditorDebug", "EditorRelease" }
platforms { "windows", "linux", "web" }
language "C++"
cppdialect "C++23"
systemversion "latest"
staticruntime "off"
conformancemode "On"
externalwarnings "Off"
warnings "Extra"
multiprocessorcompile "On"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
targetdir ("bin/" .. outputdir)
objdir    ("bin-int/" .. outputdir .. "/%{prj.name}")

filter "action:vs*"
  removeplatforms { "web", "linux" }

filter "platforms:windows"
	toolset "msc"
	defines { "ENGINE_PLATFORM_WINDOWS" }

filter "platforms:linux"
  toolset "clang"
  buildoptions { "-stdlib=libc++" }
  linkoptions  { "-stdlib=libc++" }
	defines { "ENGINE_PLATFORM_LINUX" }

filter "platforms:web"
	toolset "clang"
	defines { "ENGINE_PLATFORM_WEB" }

filter "configurations:Debug"
	defines { "DEBUG" }
	symbols "on"
	runtime "Debug"

filter "configurations:Release"
	defines { "RELEASE" }
	optimize "on"
	symbols "off"
	runtime "Release"

filter "configurations:EditorDebug"
	defines { "DEBUG", "EDITOR" }
	symbols "on"
	runtime "Debug"

filter "configurations:EditorRelease"
	defines { "RELEASE", "EDITOR" }
	optimize "on"
	symbols "off"
	runtime "Release"

group ""
	include "source/Engine"
  include "source/Editor"

group "Projects"
	include "projects/TestProject"

group "Dependencies"
	include "vendor/glfw"
	include "vendor/miniaudio"
	include "vendor/imgui"