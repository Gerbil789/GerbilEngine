project "ImGui"
kind "StaticLib"
warnings "Off"

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
	"%{wks.location}/vendor/dawn/include",
	"%{wks.location}/vendor/ImGuizmo",
}

defines
{
	"IMGUI_IMPL_WEBGPU_BACKEND_DAWN",
	"GLFW_INCLUDE_NONE",
}