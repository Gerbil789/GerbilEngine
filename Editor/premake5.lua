project "Editor"
kind "ConsoleApp"
removeplatforms { "Linux", "Web" }

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
	"%{wks.location}/vendor/ImGuizmo",
	"%{wks.location}/vendor/yaml-cpp/include",
	"%{wks.location}/vendor/renderdoc"
}

links
{
	"Engine",
	"glfw",
	"ImGui",
	"yaml-cpp",
	"webgpu_dawn"
}

libdirs 
{
	"%{wks.location}/vendor/dawn"
}

postbuildcommands 
{
	"{ECHO} Copying dependencies...",
	"{COPYFILE} %{wks.location}/vendor/renderdoc/renderdoc.dll %{cfg.targetdir}",
	"{COPYDIR} %{wks.location}/Resources %{cfg.targetdir}/Resources",
}

LinkEngine() -- This pulls in the links, includes, and DLL copy commands

filter "system:windows"
	systemversion "latest"
	buildoptions { "/permissive-", "/std:c++latest" }
	defines 
	{ 
		"_HAS_CXX23=1",
		"ENGINE_PLATFORM_WINDOWS",
		"IMGUI_IMPL_WEBGPU_BACKEND_DAWN",
		"GLFW_INCLUDE_NONE",
		"YAML_CPP_STATIC_DEFINE",
		"GLM_ENABLE_EXPERIMENTAL",
		"NOMINMAX", -- prevent windows.h from defining min and max macros
	}

--filter "system:windows"


filter "configurations:Debug"
	defines { "DEBUG", "BUILD_CONFIG=\"Debug\"" }
	symbols "on"
	runtime "Debug"

filter "configurations:Release"
	defines { "RELEASE", "BUILD_CONFIG=\"Release\"" }
	optimize "on"
	runtime "Release"


filter { "platforms:Web" }
  removefiles { "**.*" }