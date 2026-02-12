project "TestProject"
kind "SharedLib"
language "C++"
cppdialect "C++23"
staticruntime "off"
conformancemode "On"
externalwarnings "Off"
warnings "Extra"
-- fatalwarnings { "All" }

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
	"%{wks.location}/Engine/include",

}

externalincludedirs
{
	-- "%{wks.location}/vendor/glfw/include",
	"%{wks.location}/vendor/dawn/include",
	"%{wks.location}/vendor/glm",
	"%{wks.location}/vendor/entt/include",
	-- "%{wks.location}/vendor/tinygltf",
	-- "%{wks.location}/vendor/portable-file-dialogs",
	-- "%{wks.location}/vendor/yaml-cpp/include",
	-- "%{wks.location}/vendor/miniaudio"
}

links
{
	"Engine",
	-- "glfw",
	-- "webgpu_dawn",
	-- "yaml-cpp",
	-- "miniaudio"
}

libdirs 
{
	--"%{wks.location}/vendor/dawn"
}

postbuildcommands 
{
	"{COPY} %{wks.location}/bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/Engine/Engine.dll %{cfg.targetdir}"
}

filter "system:windows"
  disablewarnings { "4251" }
	systemversion "latest"
	buildoptions { "/MP", "/permissive-" } -- MP = Enable multithreading for Visual Studio
	defines
	{
		"ENGINE_PLATFORM_WINDOWS",
		"GLFW_INCLUDE_NONE",
		-- "YAML_CPP_STATIC_DEFINE",
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