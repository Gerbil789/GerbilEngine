newoption 
{
   trigger     = "project_name",
   value       = "Name",
   description = "The name of the output executable"
}

project (_OPTIONS["project_name"] or "Template")
kind "ConsoleApp"

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

links
{
	"Engine",
	"glfw",
	"TestProject", --TODO: make this configurable (also in not dist filter)
}

defines 
{ 
	"GLFW_INCLUDE_NONE",
	"GLM_ENABLE_EXPERIMENTAL",
}

postbuildcommands 
{
	"{ECHO} Copying dependencies...",
	"{COPYDIR} %{wks.location}/Resources %{cfg.targetdir}/Resources",
}

filter "configurations:not Dist"
	postbuildcommands 
  {
    "{COPYFILE} %{wks.location}/bin/" .. outputdir .. "/Engine/Engine.dll %{cfg.targetdir}",
    "{COPYFILE} %{wks.location}/bin/" .. outputdir .. "/glfw/glfw.dll %{cfg.targetdir}",
    "{COPYFILE} %{wks.location}/vendor/dawn/shared/webgpu_dawn.dll %{cfg.targetdir}",
  }

	removelinks
	{
		"TestProject",
	}

	defines
	{
		"ENGINE_SHARED_IMPORT",
		"WGPU_SHARED_LIBRARY",
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

filter "system:windows"
	links
	{
		"webgpu_dawn",
	}

filter "system:linux"
  linkoptions 
	{ 
		"-fuse-ld=lld",
		"-Wl",
		"-rpath",
		"'$$ORIGIN'",
	}

	links 
	{ 
		"pthread", 
		"dl", 
		"X11", 
		"Xrandr", 
		"Xi", 
		"Xcursor",
		"glfw",
    ":libwebgpu_dawn.a", -- force static linking
	}