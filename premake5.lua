workspace "Tunti"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Tunti/vendor/GLFW/include"
IncludeDir["Glad"] = "Tunti/vendor/Glad/include"
IncludeDir["ImGui"] = "Tunti/vendor/imgui"

group "Dependencies"
	include "Tunti/vendor/GLFW"
	include "Tunti/vendor/Glad"
	include "Tunti/vendor/imgui"

group ""

project "Tunti"
	location "Tunti"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "tpch.h"
	pchsource "Tunti/src/tpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"T_PLATFORM_WINDOWS",
			"T_BUILD_DLL"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		}

	filter "configurations:Debug"
		defines "T_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "T_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "T_DIST"
		runtime "Release"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Tunti/vendor/spdlog/include",
		"Tunti/src"
	}

	links
	{
		"Tunti"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"T_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "T_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "T_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "T_DIST"
		runtime "Release"
		optimize "On"