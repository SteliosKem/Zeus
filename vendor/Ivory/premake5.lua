outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "vendor/GLFW/include"
IncludeDir["Glad"] = "vendor/Glad/include"
IncludeDir["ImGui"] = "vendor/imgui"
IncludeDir["glm"] = "vendor/glm"
IncludeDir["stb"] = "vendor/stb"
IncludeDir["entt"] = "vendor/entt/include"
IncludeDir["yaml_cpp"] = "vendor/yaml-cpp/include"
IncludeDir["ImGuizmo"] = "vendor/ImGuizmo"

include "vendor/GLFW"
include "vendor/Glad"
include "vendor/imgui"
include "vendor/yaml-cpp"

project "Ivory"
	location ""
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "src/pch.cpp"

	files {
		"src/**.h",
		"src/**.cpp",
		"src/ImGui/**.ttf",
		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl",
		"vendor/ImGuizmo/ImGuizmo.h",
		"vendor/ImGuizmo/ImGuizmo.cpp"
	}

	includedirs {
		"vendor/spdlog/include",
		"src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.yaml_cpp}",
	}

	links {
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib",
		"yaml-cpp",
	}

	filter "vendor/ImGuizmo/**.cpp"
	flags {"NoPCH"}

	filter "system:windows"
		systemversion "latest"

		defines {
			"IV_PLATFORM_WINDOWS",
			"IV_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
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