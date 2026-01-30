project "TestProject"
kind "SharedLib"
language "C++"
cppdialect "C++23"
staticruntime "on"
conformancemode "On"
externalwarnings "Off"
warnings "Extra"
-- fatalwarnings { "All" }

-- override output location
targetdir ("%{prj.location}/bin/%{cfg.system}/%{cfg.buildcfg}")
objdir ("%{prj.location}/bin-int/%{cfg.system}/%{cfg.buildcfg}")

files
{
	"src/TestProject/**.h",
	"src/TestProject/**.cpp"
}

includedirs
{
	"src",
	"%{wks.location}/Engine/src",
	"%{wks.location}/vendor/dawn/include"
}

externalincludedirs
{
	"%{wks.location}/vendor/spdlog/include",
	"%{wks.location}/vendor/glfw/include",
	"%{wks.location}/vendor/glm",
	"%{wks.location}/vendor/entt/include",
	"%{wks.location}/vendor/tinygltf",
	"%{wks.location}/vendor/portable-file-dialogs",
	-- "%{wks.location}/vendor/yaml-cpp/include",
	"%{wks.location}/vendor/miniaudio"
}

links
{
	"Engine",
	"glfw",
	"webgpu_dawn",
	-- "yaml-cpp",
	"miniaudio",
	"spdlog"
}

libdirs 
{
	"%{wks.location}/vendor/dawn"
}



filter "system:windows"
	systemversion "latest"
	buildoptions { "/MP", "/permissive-" } -- MP = Enable multithreading for Visual Studio
	defines
	{
		"ENGINE_PLATFORM_WINDOWS",
		"ENGINE_BUILD_DLL",
		"GLFW_INCLUDE_NONE",
		"YAML_CPP_STATIC_DEFINE",
		"GAME_BUILD_DLL"
	}

filter "configurations:Debug"
	defines { "DEBUG" }
	symbols "on"
	runtime "Debug"

filter "configurations:Release"
	defines { "RELEASE" }
	optimize "on"
	runtime "Release"