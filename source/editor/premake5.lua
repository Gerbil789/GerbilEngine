project "editor"
kind "StaticLib"
removeplatforms { "web" }
removeconfigurations { "Debug", "Release" } --keep only editor* configurations

files
{
	"include/**.h",
	"src/editor/**.h",
	"src/**.cpp",
}

includedirs
{
	"include",
	"src",
	"%{wks.location}/source/engine/include",
}

externalincludedirs
{
	"%{wks.location}/vendor/glm",
	"%{wks.location}/vendor/entt/include",
	"%{wks.location}/vendor/imgui",
	"%{wks.location}/vendor/ImGuizmo",
	"%{wks.location}/vendor/glaze/include",
	"%{wks.location}/vendor/renderdoc" --TODO: remove
}

filter "not platforms:web"
  externalincludedirs
  {
		"%{wks.location}/vendor/glfw/include",
		"%{wks.location}/vendor/dawn/include"
  }
filter {}

links
{
	"engine",
	"glfw",
	"ImGui"
}

defines 
{ 
	"IMGUI_IMPL_WEBGPU_BACKEND_DAWN",
	"GLFW_INCLUDE_NONE",
	"GLM_ENABLE_EXPERIMENTAL",
}

filter "platforms:windows"
	links{ "webgpu_dawn" }

	postbuildcommands 
	{
		"{ECHO} Copying webgpu_dawn.dll",
  	"{COPYFILE} %{wks.location}/vendor/dawn/shared/webgpu_dawn.dll %{cfg.targetdir}",
	}

	libdirs 
	{
		"%{wks.location}/vendor/dawn/shared",
	}

filter "platforms:linux"
  libdirs { "%{wks.location}/vendor/dawn/static" }
  links { ":libwebgpu_dawn.a" }
  defines
	{
		"GLFW_EXPOSE_NATIVE_X11",
		"GLFW_EXPOSE_NATIVE_WAYLAND",
	}