workspace "GerbilEngine"
	architecture "x64"
	startproject "Editor"

	configurations
	{
		"Debug",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

Includedir = {}
Includedir["GLFW"] = "Engine/vendor/GLFW/include"
Includedir["Glad"] = "Engine/vendor/Glad/include"
Includedir["ImGui"] = "Engine/vendor/imgui"
Includedir["glm"] = "Engine/vendor/glm"
Includedir["stb_image"] = "Engine/vendor/stb_image"
Includedir["entt"] = "Engine/vendor/entt/include"
Includedir["yaml_cpp"] = "Engine/vendor/yaml-cpp/include"
Includedir["ImGuizmo"] = "Engine/vendor/ImGuizmo"
Includedir["FBXSDK"] = "Engine/vendor/FBXSDK/2020.3.7/include"

Libdir = {}
Libdir["FBXSDK"] = "Engine/vendor/FBXSDK/2020.3.7/lib/x64"

group "Dependencies"
	include "Engine/vendor/GLFW"
	include "Engine/vendor/Glad"
	include "Engine/vendor/imgui"
	include "Engine/vendor/yaml-cpp"
group ""



project "Engine"
	location "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "enginepch.h"
	pchsource "Engine/src/enginepch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/vendor/ImGuizmo/ImGuizmo.h",
		"%{prj.name}/vendor/ImGuizmo/ImGuizmo.cpp",
		"%{prj.name}/vendor/FBXSDK/2020.3.7/include/**.h"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{Includedir.GLFW}",
		"%{Includedir.Glad}",
		"%{Includedir.ImGui}",
		"%{Includedir.glm}",
		"%{Includedir.stb_image}",
		"%{Includedir.entt}",
		"%{Includedir.yaml_cpp}",
		"%{Includedir.ImGuizmo}",
		"%{Includedir.FBXSDK}"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib",
		"yaml-cpp"
	}

	filter "files:Engine/vendor/ImGuizmo/**.cpp"
		flags { "NoPCH" }

	filter "system:windows"
		systemversion "latest"
		buildoptions { "/MP" }  -- Enable multithreading for Visual Studio

		defines
		{
			"ENGINE_PLATFORM_WINDOWS",
			"ENGINE_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "ENGINE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "ENGINE_RELEASE"
		runtime "Release"
		optimize "on"

project "Editor"
	location "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs
	{
		"Engine/vendor/spdlog/include",
		"Engine/src",
		"Engine/vendor",
		"%{Includedir.glm}",
		"%{Includedir.entt}",
		"%{Includedir.ImGuizmo}",
		"%{Includedir.FBXSDK}"
	}

	links
	{
		"Engine",
		"libfbxsdk-mt.lib",
		"libxml2-mt.lib",
		"zlib-mt.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"
		buildoptions { "/MP" }
		defines
		{
			"ENGINE_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "ENGINE_DEBUG"
		runtime "Debug"
		symbols "on"
		libdirs
		{
			"%{Libdir.FBXSDK}/debug"
		}

	filter "configurations:Release"
		defines "ENGINE_RELEASE"
		runtime "Release"
		optimize "on"
		libdirs
		{
			"%{Libdir.FBXSDK}/release"
		}