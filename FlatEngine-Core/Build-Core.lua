project "FlatEngine-Core"
   kind "StaticLib"
   language "C++"
   cppdialect "C++20"
   targetdir "Binaries/%{cfg.buildcfg}"
   staticruntime "off"

   files { 
        "Source/**.h",
        "Source/**.cpp", 
        "../projects/*/scripts/cpp/**.cpp",
        "../projects/*/scripts/cpp/**.h",
        "../Vendor/includes/ImGui/Backends/**",
        "../Vendor/includes/ImGui/ImGui_Docking/**.h",
        "../Vendor/includes/ImGui/ImGui_Docking/**.cpp",
        "../Vendor/includes/ImPlot/**",
        "../Vendor/includes/Lua/**",
        "../Vendor/includes/Sol2/**",
        "../Vendor/includes/Box2D/**",
    }

   includedirs
    {
      "Source",
      "../projects/*/scripts/cpp",
      "../Vendor/includes/Box2D/include",
    }

    externalincludedirs 
    { 
        "../Vendor/includes/ImGui/Backends",
        "../Vendor/includes/ImGui/ImGui_Docking",
        "../Vendor/includes/ImGui/ImGui_Docking/misc/debuggers",
        "../Vendor/includes/SDL2/include",
        "../Vendor/includes/SDL2/lib/x64",
        "../Vendor/includes/SDL2_Image/include",
        "../Vendor/includes/SDL2_Image/lib/x64",
        "../Vendor/includes/SDL2_Text/include",
        "../Vendor/includes/SDL2_Text/lib/x64",
        "../Vendor/includes/SDL2_Mixer/include",
        "../Vendor/includes/SDL2_Mixer/lib/x64",
        "../Vendor/includes/Json_Formatter",
        "../Vendor/includes/ImPlot",
        "../Vendor/includes/ImSequencer",
        "../Vendor/includes/Lua",
        "../Vendor/includes/Sol2",
        "../Vendor/includes/Sol2/include",
        "../Vendor/includes/Sol2/include/sol",
        "../Vendor/includes/Box2D/include",
    }


   defines
   {
    "_WINDOWS"
   }

   
   targetdir ("../Build/" .. OutputDir .. "/%{prj.name}")
   objdir ("../Build/Intermediates/" .. OutputDir .. "/%{prj.name}")

   filter "system:windows"
       systemversion "latest"
       defines { }

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
