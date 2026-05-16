project "Engine"
kind "StaticLib"
pchheader "enginepch.h"
pchsource "src/enginepch.cpp"

files
{
	"include/Engine/**.h",
	"src/**.cpp",
}

includedirs
{
	"include",
	"src",
}

externalincludedirs
{
	"%{wks.location}/vendor/dawn/include",
	"%{wks.location}/vendor/glfw/include",
	"%{wks.location}/vendor/glm",
	"%{wks.location}/vendor/entt/include",
	"%{wks.location}/vendor/imgui",
	"%{wks.location}/vendor/tinygltf",
	"%{wks.location}/vendor/glaze/include",
	"%{wks.location}/vendor/miniaudio",
	"%{wks.location}/vendor/renderdoc"
}

links
{
	"glfw",
	"ImGui",
	"miniaudio",
}

libdirs
{
	"%{wks.location}/vendor/dawn"
}

defines
{
	"GLFW_INCLUDE_NONE",
	"GLM_ENABLE_EXPERIMENTAL",
	"IMGUI_IMPL_WEBGPU_BACKEND_DAWN",
}

filter "configurations:not Dist"
	kind "SharedLib"
	links
	{
		"webgpu_dawn",
	}
	defines
	{
		"ENGINE_BUILD_SHARED",
	}

filter "system:linux"
	buildoptions 
	{ 
		"-Wno-invalid-offsetof",
	}

  linkoptions 
	{ 
		"-fuse-ld=lld",
	}
  
	links 
	{ 
		"pthread", 
		"dl", 
		"X11", 
		"Xrandr", 
		"Xi", 
		"Xcursor",
		":libwebgpu_dawn.a",
	}