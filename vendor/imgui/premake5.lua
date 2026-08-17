project "ImGui"
kind "StaticLib"
warnings "Off"
targetdir ("bin/" .. outputdir .. "/%{prj.name}")
objdir    ("bin-int/" .. outputdir .. "/%{prj.name}")

files
{
	"**.h",
	"**.cpp",
	"%{wks.location}/vendor/ImGuizmo/ImGuizmo.cpp"
}

includedirs
{
	"%{wks.location}/vendor/glfw/include",
	"%{wks.location}/vendor/imgui",
	"%{wks.location}/vendor/ImGuizmo",
}

filter "not platforms:Web"
  includedirs
  {
    "%{wks.location}/vendor/dawn/include"
  }
filter {}	

defines
{
	"GLFW_INCLUDE_NONE",
}

filter "not platforms:Web"
  defines
  {
    "IMGUI_IMPL_WEBGPU_BACKEND_DAWN",
  }
filter {}


filter "platforms:Web"
  buildoptions
  {
    "--use-port=emdawnwebgpu",
    "-s USE_GLFW=3",
		"-pthread",
  }

	linkoptions 
  { 
    "--use-port=emdawnwebgpu",
    "-s USE_GLFW=3",
    "-s WASM=1",
    "-s ALLOW_MEMORY_GROWTH=1", 
    "-pthread",
  }
filter {}