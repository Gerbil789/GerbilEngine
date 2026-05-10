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
  filter {} -- Reset the filter so it doesn't break subsequent setup
end

workspace "GerbilEngine"
architecture "x64"
startproject "Editor"

configurations { "Debug", "Release" }
platforms { "Windows", "Web" }

filter "action:vs*"
  removeplatforms { "Web" }
filter { "platforms:Windows" }
	toolset "clang"
filter { "platforms:Web" }
	toolset "clang"
filter {} -- Reset filter

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

targetdir ("bin/" .. outputdir .. "/%{prj.name}")
objdir    ("bin-int/" .. outputdir .. "/%{prj.name}")

group "Dependencies"
	include "vendor/glfw"
	include "vendor/yaml-cpp"
	include "vendor/miniaudio"
	include "vendor/imgui"

group ""
	include "Engine"
	include "Editor"
	include "Template"

group "Games"
	include "Projects/TestProject"
