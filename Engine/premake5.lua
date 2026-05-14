project "Engine"
kind "SharedLib"
systemversion "latest"
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
	"webgpu_dawn",
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
}

filter "system:windows"
	buildoptions 
	{ 
		"/permissive-",
		"/std:c++latest",
	}

	defines
	{
		"ENGINE_PLATFORM_WINDOWS",
		"ENGINE_BUILD_SHARED",
		"IMGUI_IMPL_WEBGPU_BACKEND_DAWN",
		"NOMINMAX",
	}

filter "system:linux"
	kind "StaticLib"
  pic "On"

	buildoptions 
	{ 
		"-Wno-invalid-offsetof",
		"-stdlib=libc++",
	}

  linkoptions 
	{ 
		"-fuse-ld=lld",
		"-stdlib=libc++",
	}

  defines
  {
    "ENGINE_PLATFORM_LINUX",
  }
  
	links 
	{ 
		"pthread", 
		"dl", 
		"X11", 
		"Xrandr", 
		"Xi", 
		"Xcursor" 
	}

filter { "platforms:Web" }
  system "linux" 
	kind "StaticLib"
  
  targetextension ".html"

  buildoptions 
	{
      -- "-s USE_GLFW=3",        -- Use Emscripten's built-in GLFW3 port
      "--use-port=emdawnwebgpu",      -- Enable native browser WebGPU headers
      "-pthread"              -- Only if you plan to keep std::thread, otherwise remove
	}

  linkoptions 
	{
      "-s USE_GLFW=3",
      "--use-port=emdawnwebgpu",
      "-s WASM=1",
      "-s ALLOW_MEMORY_GROWTH=1",     -- Crucial for dynamic memory allocation in games
      "-s ASYNCIFY",                  -- Helpful if you struggle to refactor your while(true) loop immediately
      "--preload-file ../assets@/assets" -- Mount your local assets folder to the browser's virtual file system
	}

  removefiles 
	{ 
      "src/vendor/dawn/**.cpp", -- Adjust to your actual Dawn source paths
      "src/vendor/dawn/**.c" 
  }

	removeexternalincludedirs 
	{ 
		-- "%{wks.location}/vendor/dawn/include",
		-- "vendor/dawn/include",
		"vendor/imgui",
		"vendor/ImGuizmo",
	}

  removelinks 
	{ 
		"ImGui",
		"webgpu_dawn",
    "dawn_native", 
    "dawn_proc" 
  }

	defines
	{
		"ENGINE_BUILD_STATIC",
	}


	filter "configurations:Debug"
	defines { "DEBUG" }
	symbols "on"
	runtime "Debug"

filter "configurations:Release"
	defines { "RELEASE" }
	optimize "on"
	runtime "Release"