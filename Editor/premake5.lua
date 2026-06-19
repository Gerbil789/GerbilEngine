project "Editor"
kind "ConsoleApp"
removeplatforms { "Linux", "Web" }

files
{
	"src/Editor/**.h",
	"src/Editor/**.cpp",
}

includedirs
{
	"src",
	"%{wks.location}/Engine/include",
}

externalincludedirs
{
	"%{wks.location}/vendor/dawn/include",
	"%{wks.location}/vendor/glfw/include",
	"%{wks.location}/vendor/glm",
	"%{wks.location}/vendor/entt/include",
	"%{wks.location}/vendor/imgui",
	"%{wks.location}/vendor/ImGuizmo",
	"%{wks.location}/vendor/glaze/include",
	"%{wks.location}/vendor/renderdoc"
}

links
{
	"Engine",
	"glfw",
	"ImGui",
	"webgpu_dawn"
}

defines 
{ 
	"IMGUI_IMPL_WEBGPU_BACKEND_DAWN",
	"GLFW_INCLUDE_NONE",
	"GLM_ENABLE_EXPERIMENTAL",
}

postbuildcommands 
{
	"{ECHO} Copying renderdoc.dll",
	"{COPYFILE} %{wks.location}/vendor/renderdoc/renderdoc.dll %{cfg.targetdir}",
}

filter "configurations:not Dist"
	postbuildcommands 
  {
		"{ECHO} Copying webgpu_dawn.dll",
    "{COPYFILE} %{wks.location}/vendor/dawn/shared/webgpu_dawn.dll %{cfg.targetdir}",
  }

	defines
	{
		"ENGINE_SHARED_IMPORT",
	}

	libdirs 
	{
		"%{wks.location}/vendor/dawn/shared",
	}

filter "configurations:Dist"
	libdirs
	{
		"%{wks.location}/vendor/dawn/static",
	}