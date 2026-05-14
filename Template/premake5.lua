newoption 
{
   trigger     = "project_name",
   value       = "Name",
   description = "The name of the output executable"
}

project (_OPTIONS["project_name"] or "Template")
kind "ConsoleApp"
systemversion "latest"

files
{
	"src/**.h",
	"src/**.cpp"
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
	"%{wks.location}/vendor/glaze/include",
}



-- links
-- {
-- 	"glfw",
-- 	"webgpu_dawn"
-- }

libdirs 
{
	"%{wks.location}/vendor/dawn"
}

defines 
{ 
	"GLFW_INCLUDE_NONE",
	"GLM_ENABLE_EXPERIMENTAL",
}


postbuildcommands 
{
	"{ECHO} Copying dependencies...",
	"{COPYFILE} %{wks.location}/vendor/dawn/webgpu_dawn.dll %{cfg.targetdir}",
	"{COPYDIR} %{wks.location}/Resources %{cfg.targetdir}/Resources",
}


LinkEngine()

filter "system:windows"
	buildoptions 
	{ 
		"/permissive-", 
		"/std:c++latest",
	}

	defines 
	{ 
		"ENGINE_PLATFORM_WINDOWS",
		"NOMINMAX",
	}

	links
	{
		"glfw",
		"webgpu_dawn"
	}

filter "system:linux"
  pic "On"

	buildoptions 
	{ 
		"-stdlib=libc++" 
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
  -- Trick Premake into generating GCC/Clang compatible Makefiles
  system "linux" 
  toolset "clang"
  
  -- Change the output file to an HTML page (Emscripten will generate the .wasm and .js alongside it)
  targetextension ".html"
  -- Emscripten Compiler Flags
  buildoptions {
      -- "-s USE_GLFW=3",        -- Use Emscripten's built-in GLFW3 port
      "--use-port=emdawnwebgpu",      -- Enable native browser WebGPU headers
      "-pthread"              -- Only if you plan to keep std::thread, otherwise remove
	}
  -- Emscripten Linker Flags
  linkoptions {
      "-s USE_GLFW=3",
      "--use-port=emdawnwebgpu",
      "-s WASM=1",
      "-s ALLOW_MEMORY_GROWTH=1",     -- Crucial for dynamic memory allocation in games
      "-s ASYNCIFY",                  -- Helpful if you struggle to refactor your while(true) loop immediately
      "--preload-file ../assets@/assets" -- Mount your local assets folder to the browser's virtual file system
	}
  -- Remove Desktop-Specific Files (Dawn)
  -- Since the browser handles WebGPU natively, you DO NOT want to compile Dawn
  removefiles { 
      "src/vendor/dawn/**.cpp", -- Adjust to your actual Dawn source paths
      "src/vendor/dawn/**.c" 
  }

  removelinks 
	{ 
    "dawn_native", 
    "dawn_proc" 
  }

filter "configurations:Debug"
	defines { "DEBUG", "BUILD_CONFIG=\"Debug\"" }
	symbols "on"
	runtime "Debug"

filter "configurations:Release"
	defines { "RELEASE", "BUILD_CONFIG=\"Release\"" }
	optimize "on"
	runtime "Release"