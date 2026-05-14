function LinkEngine()
  links { "Engine" }
  includedirs { "%{wks.location}/Engine/include" }

	filter { "platforms:Windows" }
    postbuildcommands 
    {
      "{COPYFILE} %{wks.location}/bin/" .. outputdir .. "/Engine/Engine.dll %{cfg.targetdir}",
      "{COPYFILE} %{wks.location}/bin/" .. outputdir .. "/glfw/glfw.dll %{cfg.targetdir}",
      "{COPYFILE} %{wks.location}/vendor/dawn/webgpu_dawn.dll %{cfg.targetdir}",
    }

	filter "system:linux"
    postbuildcommands 
    {
      "{COPYFILE} %{wks.location}/bin/" .. outputdir .. "/Engine/libEngine.so %{cfg.targetdir}",
      "{COPYFILE} %{wks.location}/vendor/dawn/libwebgpu_dawn.so %{cfg.targetdir}",
    }
    
    -- If your GLFW is also built as a SharedLib on Linux, uncomment the line below:
    -- "{COPYFILE} %{wks.location}/bin/" .. outputdir .. "/glfw/libglfw.so %{cfg.targetdir}",

    -- CRITICAL FOR LINUX: Tell the executable to look for .so files in its own directory
    linkoptions { "-Wl,-rpath,'$$ORIGIN'" }

  filter {} -- reset filter
end

workspace "GerbilEngine"
architecture "x64"
startproject "Editor"
toolset "clang"
configurations { "Debug", "Release" }
platforms { "Windows", "Linux", "Web" }
language "C++"
cppdialect "C++23"
staticruntime "off"
conformancemode "On"
externalwarnings "Off"
warnings "Extra"

filter "action:vs*"
  removeplatforms { "Web", "Linux" }
filter "system:linux"
  buildoptions { "-stdlib=libc++" }
  linkoptions  { "-stdlib=libc++" }
filter {} -- reset filter

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
