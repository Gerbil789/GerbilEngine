project "ImGui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++23"
  staticruntime "on"

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
		"%{wks.location}/vendor/ImGuizmo"
	}

	filter "system:windows"
		systemversion "latest"

	filter "system:linux"
		pic "on"
		systemversion "latest"
		

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"


