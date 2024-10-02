workspace "Zeus"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

ZeusIncludeDir = {}
ZeusIncludeDir["Ivory"] = "vendor/Ivory/src"
ZeusIncludeDir["Alchemist"] = "vendor/Ivory/vendor/Alchemist/src"
ZeusIncludeDir["glm"] = "vendor/Ivory/vendor/glm"
ZeusIncludeDir["entt"] = "vendor/Ivory/vendor/entt/include"
ZeusIncludeDir["ImGuizmo"] = "vendor/Ivory/vendor/ImGuizmo"

include "vendor/Ivory"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs {
		"vendor/Ivory/vendor/spdlog/include",
		"vendor/Ivory/src",
		"vendor/Ivory/vendor/imgui",
		"%{ZeusIncludeDir.glm}",
		"%{ZeusIncludeDir.ImGuizmo}",
		"%{ZeusIncludeDir.entt}", 
		"%{ZeusIncludeDir.Alchemist}"
	}

	links {
		"Ivory",
		"Alchemist"
	}

	filter "system:windows"
		systemversion "latest"

		defines {
			"IV_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "IV_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "IV_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "IV_DIST"
		runtime "Release"
		optimize "on"