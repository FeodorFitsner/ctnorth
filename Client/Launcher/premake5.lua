project "Launcher"
    language "C++"
    kind "WindowedApp"
    targetname "CTN"
    targetdir(buildpath("."))
    debugdir(buildpath("."))

    pchheader "Precompiled.hpp"
    pchsource "Precompiled.cpp"

    entrypoint "wmainCRTStartup"
    flags { "NoIncrementalLink", "NoEditAndContinue" }
    linkoptions "/IGNORE:4254 /OPT:NOLBR /SAFESEH:NO /DYNAMICBASE:NO /LARGEADDRESSAWARE /LAST:.zdata"

    links
    {
        "Utility",
        "dbghelp", "psapi", "comctl32", "wininet", "winhttp", "Shlwapi"
    }

    vpaths
    {
        ["Headers/*"] = "**.hpp",
        ["Sources/*"] = "**.cpp",
        ["Resources/*"] = {"**.rc", "res/*.ico"},
        ["*"] = "premake5.lua"
    }

    files
    {
        "premake5.lua",
        "*.hpp",
        "*.cpp",
        "**.rc"
    }
    
    -- TODO: Insiders version
    --files "res/ctn.ico"
    files "res/ctn-insiders.ico"

    filter "architecture:x64"
        flags { "ExcludeFromBuild" }

    filter "system:not windows"
        flags { "ExcludeFromBuild" }
