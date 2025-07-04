project "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "enginepch.h"
	pchsource "src/enginepch.cpp"

	files
	{
		"src/**.h",
		"src/**.cpp",
		"%{wks.location}/vendor/stb_image/**.h",
		"%{wks.location}/vendor/stb_image/**.cpp",
		-- "%{wks.location}/vendor/glm/glm/**.hpp",
		"%{wks.location}/vendor/glm/glm/**.inl",
		"%{wks.location}/vendor/yaml-cpp/include/**.h",
		"%{wks.location}/vendor/assimp/include/**.h",
		"%{wks.location}/vendor/assimp/include/**.hpp"
	}

	includedirs
	{
		"src",
		"%{wks.location}/vendor/spdlog/include",
		"%{wks.location}/vendor/glfw/include",
		"%{wks.location}/vendor/dawn/include",
		"%{wks.location}/vendor/glm",
		"%{wks.location}/vendor/stb_image",
		"%{wks.location}/vendor/entt/include",
		"%{wks.location}/vendor/yaml-cpp/include",
		"%{wks.location}/vendor/assimp/include"
	}

	links
	{
		"glfw",
		"webgpu_dawn",
		"yaml-cpp",
		"assimp",
	}

	libdirs 
	{
		"%{wks.location}/vendor/dawn"
	}

	filter "system:windows"
		systemversion "latest"
		buildoptions { "/MP" } -- Enable multithreading for Visual Studio
		defines
		{
			"ENGINE_PLATFORM_WINDOWS",
			"ENGINE_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "on"
		runtime "Debug"

	filter "configurations:Release"
		defines { "RELEASE" }
		optimize "on"
		runtime "Release"