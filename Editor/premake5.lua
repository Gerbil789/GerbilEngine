project "Editor"
kind "ConsoleApp"
language "C++"
cppdialect "C++23"
staticruntime "on"
conformancemode "On"
externalwarnings "Off"
warnings "Extra"
fatalwarnings { "All" }

files
{
	"src/Editor/**.h",
	"src/Editor/**.cpp"
}

includedirs
{
	"src",
	"%{wks.location}/Engine/src",
	"%{wks.location}/vendor/dawn/include" 
}

externalincludedirs
{
	"%{wks.location}/vendor/glm",
	"%{wks.location}/vendor/entt/include",
	"%{wks.location}/vendor/imgui",
	"%{wks.location}/vendor/ImGuizmo",
	"%{wks.location}/vendor/yaml-cpp/include",
}

links
{
	"Engine",
	"yaml-cpp",
	"ImGui"
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
		"YAML_CPP_STATIC_DEFINE"
	}

filter "configurations:Debug"
	defines { "DEBUG" }
	symbols "on"
	runtime "Debug"

filter "configurations:Release"
	defines { "RELEASE" }
	optimize "on"
	runtime "Release"