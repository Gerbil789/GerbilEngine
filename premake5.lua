workspace "GerbilEngine"
architecture "x64"
startproject "Editor"
toolset "clang"
configurations { "Debug", "Release", "Dist" }
platforms { "Windows", "Linux", "Web" }
language "C++"
cppdialect "C++23"
systemversion "latest"
staticruntime "off"
conformancemode "On"
externalwarnings "Off"
warnings "Extra"

filter "action:vs*"
  removeplatforms { "Web", "Linux" }
	toolset "msc"
	disablewarnings { "4251" }
	
filter "system:windows"
	defines 
	{ 
		"ENGINE_PLATFORM_WINDOWS",
		"NOMINMAX", --TODO: remove after removing all windows dependency
	}

filter "system:linux"
  buildoptions { "-stdlib=libc++" }
  linkoptions  { "-stdlib=libc++" }
	pic "on"

	defines
  {
    "ENGINE_PLATFORM_LINUX",
  }

filter "configurations:Debug"
	defines { "DEBUG" }
	symbols "on"
	runtime "Debug"

filter "configurations:Release"
	defines { "RELEASE" }
	optimize "on"
	runtime "Release"

filter "configurations:Dist"
    defines { "DIST" }
    optimize "on"
    symbols "off"
    runtime "Release"

filter {}


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

targetdir ("bin/" .. outputdir .. "/%{prj.name}")
objdir    ("bin-int/" .. outputdir .. "/%{prj.name}")

group "Dependencies"
	include "vendor/glfw"
	include "vendor/miniaudio"
	include "vendor/imgui"

group ""
	include "Engine"
	include "Editor"
	include "Template"

group "Games"
	include "Projects/TestProject"