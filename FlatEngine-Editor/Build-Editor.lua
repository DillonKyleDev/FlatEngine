project "FlatEngine-Editor"
kind "ConsoleApp"
language "C++"
cppdialect "C++20"
staticruntime "off"

files {
    "Source/**.h",
    "Source/**.cpp",
}

-- All platforms
includedirs {
    "Source",
    "../FlatEngine-Core/Source",
    "../scripts",
    "../projects/*/scripts/cpp",
    "../Vendor/includes/ImGui/Backends",
    "../Vendor/includes/ImGui/ImGui_Docking",
    "../Vendor/includes/ImGui/ImGui_Docking/misc/debuggers",
    "../Vendor/includes/Json_Formatter",
    -- "../Vendor/includes/ImPlot",
    "../Vendor/includes/ImSequencer",
    "../Vendor/includes/Lua",
    "../Vendor/includes/Sol2/include",
    "../Vendor/includes/Sol2/include/sol",
    "../Vendor/includes/Box2D/include",
    "../Vendor/includes/spdlog/include"
}

-- Always link Core
links { "FlatEngine-Core" }

targetdir ("../Build/" .. OutputDir .. "/%{prj.name}")
objdir ("../Build/Intermediates/" .. OutputDir .. "/%{prj.name}")

-- Windows only
filter "system:windows"
systemversion "latest"
defines { "_WINDOWS", "WINDOWS" }
includedirs {
    "../Vendor/includes/SDL2/include",
    "../Vendor/includes/SDL2_Image/include",
    "../Vendor/includes/SDL2_Text/include",
    "../Vendor/includes/SDL2_Mixer/include",
}
externalincludedirs {
    "../Vendor/includes/Vulkan/include",
    "../Vendor/includes/GLM/glm",
}
libdirs {
    "../Vendor/includes/SDL2/lib/x64",
    "../Vendor/includes/SDL2_Image/lib/x64",
    "../Vendor/includes/SDL2_Text/lib/x64",
    "../Vendor/includes/SDL2_Mixer/lib/x64",
    -- "../Vendor/includes/ImPlot",
    "../Vendor/includes/ImSequencer",
    "../Vendor/includes/Lua",
    "../Vendor/includes/Box2D/lib",
}
links {
    "SDL2.lib",
    "SDL2main.lib",
    "SDL2_ttf.lib",
    "SDL2_image.lib",
    "SDL2_mixer.lib",
    -- Removed: d3d12.lib, d3dcompiler.lib, dxgi.lib (DirectX — not needed for Vulkan)
    "lua54.lib",
    "box2d.lib",
    "box2dd.lib",
}

-- Linux only
filter "system:linux"
defines { "_LINUX", "LINUX" }
includedirs {
    "/usr/include/SDL2",
    "/usr/include/vulkan",
    "/usr/include/glm",
}
libdirs { "/usr/local/lib64" }
links {
    "vulkan",
    "SDL2",
    "SDL2_ttf",
    "SDL2_image",
    "SDL2_mixer",
    "lua-5.4",
    ":libbox2d.a",
    "dl",
    "pthread",
}

-- Reset filter
filter {}

filter "configurations:Debug"
defines { "_DEBUG" }
runtime "Debug"
symbols "On"

filter "configurations:Release"
defines { "NDEBUG" }
runtime "Release"
optimize "On"
symbols "On"

filter "configurations:Dist"
defines { "NDEBUG" }
runtime "Release"
optimize "On"
symbols "Off"
