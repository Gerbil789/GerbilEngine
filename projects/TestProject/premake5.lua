project "TestProject"
kind "ConsoleApp"
targetdir ("%{prj.location}/bin/%{cfg.system}/%{cfg.buildcfg}")
objdir ("%{prj.location}/bin-int/%{cfg.system}/%{cfg.buildcfg}")

files
{
	"src/TestProject/**.h",
	"src/TestProject/**.cpp"
}

includedirs
{
	"src",
	"%{wks.location}/source/Editor/include",
	"%{wks.location}/source/Engine/include",
}

externalincludedirs
{
	"%{wks.location}/vendor/glm",
	"%{wks.location}/vendor/entt/include",
	"%{wks.location}/vendor/imgui",
}

libdirs { "%{wks.location}/vendor/dawn/shared" }

filter "not platforms:Web"
    externalincludedirs
    {
        "%{wks.location}/vendor/dawn/include",
    }
filter {}

links
{
	"Engine",
	"Editor",
	"ImGui",
}

defines 
{ 
	"IMGUI_IMPL_WEBGPU_BACKEND_DAWN",
}

filter "system:windows"
  links { "webgpu_dawn" }


filter "platforms:Web"
  buildoptions 
  { 
    "--use-port=emdawnwebgpu", 
    "-pthread",
		"-Wno-invalid-offsetof",
  }

  linkoptions 
  { 
    "--use-port=emdawnwebgpu",
    "-s USE_GLFW=3",
    "-s WASM=1",
    "-s ALLOW_MEMORY_GROWTH=1",
    "-s MIN_WEBGL_VERSION=2",
    "-s MAX_WEBGL_VERSION=2",
    "-pthread"
  }

filter "configurations:not Dist"
	postbuildcommands 
  {
		"{ECHO} Copying webgpu_dawn.dll",
    "{COPYFILE} %{wks.location}/vendor/dawn/shared/webgpu_dawn.dll %{cfg.targetdir}",
  }

