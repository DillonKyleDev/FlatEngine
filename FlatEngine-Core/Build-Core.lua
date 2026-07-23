project "FlatEngine-Core"
kind "StaticLib"
language "C++"
cppdialect "C++20"
staticruntime "off"

files {
    "Source/**.h",
    "Source/**.cpp",
    "../projects/*/scripts/cpp/**.cpp",
    "../projects/*/scripts/cpp/**.h",
    "../Vendor/includes/ImGui/Backends/**",
    "../Vendor/includes/ImGui/ImGui_Docking/**.h",
    "../Vendor/includes/ImGui/ImGui_Docking/**.cpp",
    -- "../Vendor/includes/ImPlot/**",
    "../Vendor/includes/Lua/**",
    "../Vendor/includes/Sol2/**",
    "../Vendor/includes/Box2D/**"
}

-- All platforms
includedirs {
    "Source",
    "../projects/*/scripts/cpp",
    "../Vendor/includes/Box2D/include",
    "../Vendor/includes/ImGui/ImGui_Docking",
    "../Vendor/includes/spdlog/include"
}

-- All platforms
externalincludedirs {
    "../Vendor/includes/Stb_Image",
    "../Vendor/includes/tinyobjloader",
    "../Vendor/includes/ImGui/Backends",
    "../Vendor/includes/ImGui/ImGui_Docking",
    "../Vendor/includes/ImGui/ImGui_Docking/misc/debuggers",
    "../Vendor/includes/Json_Formatter",
    -- "../Vendor/includes/ImPlot",
    "../Vendor/includes/ImSequencer",
    "../Vendor/includes/Lua",
    "../Vendor/includes/Sol2",
    "../Vendor/includes/Sol2/include",
    "../Vendor/includes/Sol2/include/sol",
    "../Vendor/includes/Box2D/include",
}

targetdir ("../Build/" .. OutputDir .. "/%{prj.name}")
objdir ("../Build/Intermediates/" .. OutputDir .. "/%{prj.name}")

-- Shader compilation
-- Adjust shaderSrcDir to match where your .vert/.frag files live
local shaderSrcDir = "../FlatEngine-Core/Source/Shaders"
local shaderOutDir = "../FlatEngine-Core/Source/Shaders/compiled"

filter "system:linux"
prebuildcommands {
    "mkdir -p " .. shaderOutDir,
    "for f in " .. shaderSrcDir .. "/*.vert; do " ..
    "[ -f \"$f\" ] && glslc \"$f\" -o " .. shaderOutDir .. "/$(basename $f).spv && " ..
    "echo \"Compiled: $(basename $f)\" || true; " ..
    "done",
    "for f in " .. shaderSrcDir .. "/*.frag; do " ..
    "[ -f \"$f\" ] && glslc \"$f\" -o " .. shaderOutDir .. "/$(basename $f).spv && " ..
    "echo \"Compiled: $(basename $f)\" || true; " ..
    "done",
}

filter "system:windows"
prebuildcommands {
    "if not exist \"" .. shaderOutDir .. "\" mkdir \"" .. shaderOutDir .. "\"",
    "for %f in (" .. shaderSrcDir .. "\\*.vert) do " ..
    "glslc \"%f\" -o " .. shaderOutDir .. "\\%~nf.vert.spv",
    "for %f in (" .. shaderSrcDir .. "\\*.frag) do " ..
    "glslc \"%f\" -o " .. shaderOutDir .. "\\%~nf.frag.spv",
}

-- Windows only
filter "system:windows"
systemversion "latest"
defines { "_WINDOWS", "WINDOWS" }
externalincludedirs {
    "../Vendor/includes/Vulkan/include",
    "../Vendor/includes/GLM/glm",
    "../Vendor/includes/SDL2/include",
    "../Vendor/includes/SDL2/lib/x64",
    "../Vendor/includes/SDL2_Image/include",
    "../Vendor/includes/SDL2_Image/lib/x64",
    "../Vendor/includes/SDL2_Text/include",
    "../Vendor/includes/SDL2_Text/lib/x64",
    "../Vendor/includes/SDL2_Mixer/include",
    "../Vendor/includes/SDL2_Mixer/lib/x64",
}
libdirs {
    "../Vendor/includes/Vulkan/Lib",
    "../Vendor/includes/GLM/glm",
}
links {
    "vulkan-1.lib",
}

-- Linux only
filter "system:linux"
defines { "_LINUX", "LINUX" }
externalincludedirs {
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
    ":libbox2d.a",   -- Fixed: was linkoptions { "/usr/local/lib64/libbox2d.a" }
    "dl",
    "pthread",
}

removefiles {
    "../Vendor/includes/ImGui/Backends/imgui_impl_dx9*",
    "../Vendor/includes/ImGui/Backends/imgui_impl_dx10*",
    "../Vendor/includes/ImGui/Backends/imgui_impl_dx11*",
    "../Vendor/includes/ImGui/Backends/imgui_impl_dx12*",
    "../Vendor/includes/ImGui/Backends/imgui_impl_win32*",
    "../Vendor/includes/ImGui/Backends/imgui_impl_metal*",
    "../Vendor/includes/ImGui/Backends/imgui_impl_osx*",
    "../Vendor/includes/ImGui/Backends/imgui_impl_wgpu*",
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
-- symbols "Off"
