project "Engine"
kind "SharedLib"
language "C++"
cppdialect "C++23"
staticruntime "off"
conformancemode "On"
externalwarnings "Off"
warnings "Extra"
-- fatalwarnings { "All" }

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
	"include",	-- public headers
	"src"				-- private headers
}

externalincludedirs
{
	"%{wks.location}/vendor/dawn/include",
	"%{wks.location}/vendor/glfw/include",
	"%{wks.location}/vendor/glm",
	"%{wks.location}/vendor/entt/include",
	"%{wks.location}/vendor/imgui",
	"%{wks.location}/vendor/tinygltf",
	"%{wks.location}/vendor/yaml-cpp/include",
	"%{wks.location}/vendor/miniaudio",
	"%{wks.location}/vendor/renderdoc"
}

links
{
	"glfw",
	"ImGui",
	"webgpu_dawn",
	"yaml-cpp",
	"miniaudio"
}

libdirs 
{
	"%{wks.location}/vendor/dawn"
}

postbuildmessage "Copying Engine.dll to Editor directory"

filter "system:windows"
	systemversion "latest"
	buildoptions { "/permissive-", "/std:c++latest"}
	defines
	{
		"_HAS_CXX23=1",
		"ENGINE_PLATFORM_WINDOWS",
		"IMGUI_IMPL_WEBGPU_BACKEND_DAWN",
		"GLFW_INCLUDE_NONE",
		"YAML_CPP_STATIC_DEFINE",
		"GLM_ENABLE_EXPERIMENTAL",
		"NOMINMAX", -- prevent windows.h from defining min and max macros
		"ENGINE_BUILD_DLL"
	}


filter { "platforms:Web" }
  -- Trick Premake into generating GCC/Clang compatible Makefiles
  system "linux" 
	kind "StaticLib"
  
  -- Change the output file to an HTML page (Emscripten will generate the .wasm and .js alongside it)
  targetextension ".html"
  -- Emscripten Compiler Flags
  buildoptions 
	{
      -- "-s USE_GLFW=3",        -- Use Emscripten's built-in GLFW3 port
      "--use-port=emdawnwebgpu",      -- Enable native browser WebGPU headers
      "-pthread"              -- Only if you plan to keep std::thread, otherwise remove
	}
  -- Emscripten Linker Flags
  linkoptions 
	{
      "-s USE_GLFW=3",
      "--use-port=emdawnwebgpu",
      "-s WASM=1",
      "-s ALLOW_MEMORY_GROWTH=1",     -- Crucial for dynamic memory allocation in games
      "-s ASYNCIFY",                  -- Helpful if you struggle to refactor your while(true) loop immediately
      "--preload-file ../assets@/assets" -- Mount your local assets folder to the browser's virtual file system
	}

  removefiles { 
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