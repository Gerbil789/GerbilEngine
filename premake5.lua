workspace "GerbilEngine"
	architecture "x64"
	startproject "Editor"
	configurations { "Debug", "Release" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

Includedir = {}
Includedir["glm"] = "Engine/vendor/glm"
Includedir["stb_image"] = "Engine/vendor/stb_image"
Includedir["entt"] = "Engine/vendor/entt/include"
Includedir["yaml_cpp"] = "Engine/vendor/yaml-cpp/include"
Includedir["assimp"] = "Engine/vendor/assimp/include"

Includedir["ImGui"] = "Editor/vendor/imgui"
Includedir["ImGuizmo"] = "Editor/vendor/ImGuizmo"


group "Dependencies"
	include "Editor/vendor/imgui"
	include "Engine/vendor/yaml-cpp"
	include "Engine/vendor/assimp"
	include "Engine/vendor/assimp/contrib/zlib"
group ""


project "Engine"
	location "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
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
		"%{prj.name}/vendor/yaml-cpp/include/**.h",
		"%{prj.name}/vendor/assimp/include/**.h",
		"%{prj.name}/vendor/assimp/include/**.hpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{Includedir.glm}",
		"%{Includedir.stb_image}",
		"%{Includedir.entt}",
		"%{Includedir.yaml_cpp}",
		"%{Includedir.assimp}"
	}


	links
	{
		"yaml-cpp",
		"assimp",
	}

	-- filter "files:Engine/vendor/ImGuizmo/**.cpp"
	-- 	flags { "NoPCH" }

	filter "system:windows"
		systemversion "latest"
		buildoptions { "/MP" }  -- Enable multithreading for Visual Studio

		defines
		{
			"ENGINE_PLATFORM_WINDOWS",
			"ENGINE_BUILD_DLL"
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
	cppdialect "C++20"
	staticruntime "on"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/ImGuizmo/ImGuizmo.h",
		"%{prj.name}/vendor/ImGuizmo/ImGuizmo.cpp",
	}

	includedirs
	{
		"Editor/src",
		"Engine/vendor/spdlog/include",
		"Engine/src",
		"Engine/vendor",
		"Editor/vendor",
		"%{Includedir.glm}",
		"%{Includedir.entt}",
		"%{Includedir.ImGui}",
		"%{Includedir.ImGuizmo}",
		"%{Includedir.assimp}"
	}

	links
	{
		"Engine",
		"ImGui",
	}



	filter "system:windows"
		cppdialect "C++20"
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

	filter "configurations:Release"
		defines "ENGINE_RELEASE"
		runtime "Release"
		optimize "on"