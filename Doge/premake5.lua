project "Doge"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" ..outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "src/pch.cpp"

	files
	{
		"src/**.h",
		"src/**.cpp",
		
		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl",

		"vendor/stb_image/**.h",
		"vendor/stb_image/**.cpp",

		"vendor/assimp/**.h",
		"vendor/assimp/**.hpp",
		"vendor/assimp/**.inl",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	includedirs
	{
		"src",
		"vendor",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.assimp}"
	}

	libdirs
	{
		"%{LibraryDir.assimp}"
	}

	links
	{
		"GLFW",
		"Glad",
		"opengl32.lib",
		"assimp-vc142-mt.lib"
	}

	filter "configurations:Debug"
		defines "DEBUG_ENABLED"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"