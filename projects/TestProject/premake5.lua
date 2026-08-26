project "TestProject"
kind "ConsoleApp"
-- targetdir ("%{prj.location}/bin/%{cfg.system}/%{cfg.buildcfg}")
-- objdir ("%{prj.location}/bin-int/%{cfg.system}/%{cfg.buildcfg}")

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

filter "not platforms:web"
  externalincludedirs
  {
	  "%{wks.location}/vendor/glfw/include",
    "%{wks.location}/vendor/dawn/include"
  }
filter {}

filter "platforms:windows"
	libdirs { "%{wks.location}/vendor/dawn/shared" }
filter "platforms:linux"
  libdirs { "%{wks.location}/vendor/dawn/static" }
  links { ":libwebgpu_dawn.a" }
  defines
	{
		"GLFW_EXPOSE_NATIVE_X11",
		"GLFW_EXPOSE_NATIVE_WAYLAND",
	}
filter {}

links { "Editor", "Engine", "ImGui", "glfw" }

defines { "IMGUI_IMPL_WEBGPU_BACKEND_DAWN" }

filter "not platforms:web"
  externalincludedirs { "%{wks.location}/vendor/dawn/include" }
	defines { "WEBGPU_CPP_IMPLEMENTATION" }

filter "platforms:windows"
  links { "webgpu_dawn" }
	postbuildcommands 
  {
		"{ECHO} Copying webgpu_dawn.dll",
    "{COPYFILE} %{wks.location}/vendor/dawn/shared/webgpu_dawn.dll %{cfg.targetdir}"
  }

filter "platforms:web"
  buildoptions 
  { 
    "--use-port=emdawnwebgpu", 
    "-pthread",
		-- "-Wno-invalid-offsetof"
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