function LinkEngine()
  links { "Engine" }
  
  -- Tell the project where the Engine headers are
  includedirs { "%{wks.location}/Engine/include" }
  
  -- Every project that links the engine likely needs these DLLs to run
  postbuildcommands 
	{
    "{COPY} %{wks.location}/bin/" .. outputdir .. "/Engine/Engine.dll %{cfg.targetdir}",
		"{COPY} %{wks.location}/bin/" .. outputdir .. "/glfw/glfw.dll %{cfg.targetdir}",
    "{COPY} %{wks.location}/vendor/dawn/webgpu_dawn.dll %{cfg.targetdir}",
  }
end





workspace "GerbilEngine"
architecture "x64"
startproject "Editor"
configurations { "Debug", "Release" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

targetdir ("bin/" .. outputdir .. "/%{prj.name}")
objdir    ("bin-int/" .. outputdir .. "/%{prj.name}")

group "Dependencies"
	include "vendor/glfw"
	include "vendor/imgui"
	include "vendor/yaml-cpp"
	include "vendor/miniaudio"

group ""
	include "Engine"
	include "Editor"
	include "Template"

group "Games"
	include "Projects/TestProject"
