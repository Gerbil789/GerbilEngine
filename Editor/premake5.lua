project "Editor"
kind "ConsoleApp"
language "C++"
cppdialect "C++23"
staticruntime "off"
conformancemode "On"
externalwarnings "Off"
warnings "Extra"
-- fatalwarnings { "All" }

files
{
	"src/Editor/**.h",
	"src/Editor/**.cpp"
}

includedirs
{
	"src",
	"%{wks.location}/Engine/include"
}

externalincludedirs
{
	"%{wks.location}/vendor/dawn/include",
	"%{wks.location}/vendor/glfw/include",
	"%{wks.location}/vendor/glm",
	"%{wks.location}/vendor/entt/include",
	"%{wks.location}/vendor/imgui",
	"%{wks.location}/vendor/ImGuizmo"
}

links
{
	"Engine",
	"glfw",
	"ImGui",
	"webgpu_dawn"
}

libdirs 
{
	"%{wks.location}/vendor/dawn"
}

postbuildcommands 
{
	"{COPY} %{wks.location}/vendor/dawn/webgpu_dawn.dll %{cfg.targetdir}",
	"{COPY} %{wks.location}/Resources %{cfg.targetdir}/Resources"
}

postbuildmessage "Copying dependencies..."
filter "system:windows"
	systemversion "latest"
	buildoptions { "/MP", "/permissive-" }
	defines 
	{ 
		"ENGINE_PLATFORM_WINDOWS",
		"GLFW_INCLUDE_NONE",
		"YAML_CPP_STATIC_DEFINE"
	}

filter "system:windows"
  disablewarnings { "4251" } -- 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'

filter "configurations:Debug"
	defines { "DEBUG" }
	symbols "on"
	runtime "Debug"

filter "configurations:Release"
	defines { "RELEASE" }
	optimize "on"
	runtime "Release"