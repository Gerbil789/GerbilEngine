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
	"%{wks.location}/vendor/glm",
	"%{wks.location}/vendor/entt/include",
	"%{wks.location}/vendor/imgui",
	"%{wks.location}/vendor/tinygltf",
	"%{wks.location}/vendor/glaze/include",
	"%{wks.location}/vendor/miniaudio",
	"%{wks.location}/vendor/renderdoc"
}

filter "not platforms:Web"
    externalincludedirs
    {
				"%{wks.location}/vendor/glfw/include",
        "%{wks.location}/vendor/dawn/include"
    }
filter {}

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
}

filter "not platforms:Web"
    defines
    {
        "IMGUI_IMPL_WEBGPU_BACKEND_DAWN",
    }
filter {}

filter "configurations:not Dist"
	-- defines
	-- {
	-- 	"WGPU_SHARED_LIBRARY",
	-- }

	libdirs
	{
		"%{wks.location}/vendor/dawn/shared"
	}

filter {"system:windows", "configurations:Dist"}
	-- disablewarnings { "4006" }

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

filter "platforms:Linux"
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


filter "platforms:Web"
  defines
  {
    "GLFW_INCLUDE_NONE",
  }

  buildoptions 
  { 
    "--use-port=emdawnwebgpu", 
    "-pthread"
  }

  linkoptions 
  { 
    "--use-port=emdawnwebgpu",
    "-s USE_GLFW=3",
    "-s WASM=1",
    "-s ALLOW_MEMORY_GROWTH=1", -- Crucial for game engines allocating assets dynamically
    "-s MIN_WEBGL_VERSION=2",
    "-s MAX_WEBGL_VERSION=2",
    "-pthread"
  }