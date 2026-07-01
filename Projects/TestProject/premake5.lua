project "TestProject"
kind "StaticLib"
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
	"%{wks.location}/Engine/include",
}

externalincludedirs
{
	"%{wks.location}/vendor/dawn/include",
	"%{wks.location}/vendor/glm",
	"%{wks.location}/vendor/entt/include",
	"%{wks.location}/vendor/imgui",
}

links
{
	"Engine",
	"ImGui",
}

defines 
{ 
	"IMGUI_IMPL_WEBGPU_BACKEND_DAWN",
}

filter { "system:windows", "configurations:not Dist" }
	kind "SharedLib"
	postbuildcommands 
	{
		"{ECHO} Copying Engine.dll",
		"{COPYFILE} %{wks.location}/bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/Engine.dll %{cfg.targetdir}"
	}