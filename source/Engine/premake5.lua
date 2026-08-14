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

defines
{
	"GLFW_INCLUDE_NONE",
	"GLM_ENABLE_EXPERIMENTAL",
	"IMGUI_IMPL_WEBGPU_BACKEND_DAWN",
}

filter "configurations:not Dist"
	kind "SharedLib"
	defines
	{
		"ENGINE_SHARED_EXPORT",
		"WGPU_SHARED_LIBRARY",
	}

	libdirs
	{
		"%{wks.location}/vendor/dawn/shared"
	}


filter "configurations:Dist"
	libdirs
	{
		"%{wks.location}/vendor/dawn/static"
	}


filter {"system:windows", "configurations:Dist"}
	disablewarnings { "4006" }

	links 
	{
    "dxguid.lib",
    "dxgi.lib",
    "d3d11.lib",
    "d3d12.lib",
    "d3dcompiler.lib",
		"mincore.lib",
	}

filter "system:windows"
	links
	{
		"webgpu_dawn",
	}

	defines
	{
		"GLFW_EXPOSE_NATIVE_WIN32",
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

	defines
	{
		"GLFW_EXPOSE_NATIVE_X11",
		"GLFW_EXPOSE_NATIVE_WAYLAND",
	}