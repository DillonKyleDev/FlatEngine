project "FlatEngine-Core"
   kind "StaticLib"
   language "C++"
   cppdialect "C++20"
   targetdir "Binaries/%{cfg.buildcfg}"
   staticruntime "off"

   files { 
        "Source/**.h",
        "Source/**.cpp", 
        "../Vendor/includes/ImGui/Backends/**",
        "../Vendor/includes/ImGui/ImGui_Docking/**.h",
        "../Vendor/includes/ImGui/ImGui_Docking/**.cpp",
        "../Vendor/includes/ImPlot/**",
        "../Vendor/includes/Lua/**",
        "../Vendor/includes/Sol2/**",
    }

   includedirs
    {
      "Source"
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
    }


   defines
   {
    "_WINDOWS"
   }

   targetdir ("C:/Users/<UserName>/Desktop/FlatEngine2D_Build/" .. OutputDir .. "/%{prj.name}")
   objdir ("C:/Users/<UserName>/Desktop/FlatEngine2D_Build/" .. OutputDir .. "/%{prj.name}")

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
