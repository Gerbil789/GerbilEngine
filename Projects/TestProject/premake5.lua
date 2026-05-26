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
}

links
{
	"Engine",
}

filter { "system:windows", "configurations:not Dist" }
	kind "SharedLib"
	postbuildcommands 
	{
		"{COPYFILE} %{wks.location}/bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/Engine.dll %{cfg.targetdir}"
	}

	defines
	{
		"GAME_SHARED_EXPORT",
	}