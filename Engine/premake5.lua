project "Engine"
kind "SharedLib"
language "C++"
cppdialect "C++23"
staticruntime "off"
conformancemode "On"
externalwarnings "Off"
warnings "Extra"
-- fatalwarnings { "All" }

pchheader "enginepch.h"
pchsource "src/enginepch.cpp"

files
{
	"include/Engine/**.h",
	"src/**.h",
	"src/**.cpp",
	"src/enginepch.cpp"
}

includedirs
{
	"include",	-- public headers
	"src"				-- private headers
}

externalincludedirs
{
	"%{wks.location}/vendor/dawn/include",
	"%{wks.location}/vendor/glfw/include",
	"%{wks.location}/vendor/glm",
	"%{wks.location}/vendor/entt/include",
	"%{wks.location}/vendor/tinygltf",
	"%{wks.location}/vendor/portable-file-dialogs",
	"%{wks.location}/vendor/yaml-cpp/include",
	"%{wks.location}/vendor/miniaudio"
}

links
{
	"glfw",
	"webgpu_dawn",
	"yaml-cpp",
	"miniaudio"
}

libdirs 
{
	"%{wks.location}/vendor/dawn"
}

postbuildcommands
{
    '{COPY} "%{cfg.buildtarget.abspath}" "%{wks.location}/bin/' .. outputdir .. '/Editor/"'
}

postbuildmessage "Copying Engine.dll to Editor directory"

filter "system:windows"
  disablewarnings { "4251" }
	systemversion "latest"
	buildoptions { "/MP", "/permissive-" } -- MP = Enable multithreading for Visual Studio
	defines
	{
		"ENGINE_PLATFORM_WINDOWS",
		"GLFW_INCLUDE_NONE",
		"YAML_CPP_STATIC_DEFINE",
		"ENGINE_BUILD_DLL"
	}

filter "configurations:Debug"
	defines { "DEBUG" }
	symbols "on"
	runtime "Debug"

filter "configurations:Release"
	defines { "RELEASE" }
	optimize "on"
	runtime "Release"