-- CtNorth build configuration script
-- Author(s):       iFarbod <ifarbod@outlook.com>
--
-- Copyright (c) 2015-2017 Project CtNorth
--
-- Distributed under the MIT license (See accompanying file LICENSE or copy at
-- https://opensource.org/licenses/MIT)

-- Add buildactions to path
premake.path = premake.path .. ";Utils/buildactions"
require "compose_files"
require "install_cef"
require "install_data"
dofile "buildoptions.lua"
dofile "findDirectX9.lua"

---
-- CtN main workspace
---
workspace "CtNorth"
    location "Build"
    platforms { "x86", "x64" }
    targetprefix ""
    configurations { "Debug", "Release" }
    buildoptions "/std:c++latest"
    flags { "C++14" }
    symbols "On"
    characterset "Unicode"
    pic "On"
    startproject "Launcher"

    -- Preprocessor definitions
    defines
    {
        -- Disable deprecation warnings and errors
        "_CRT_SECURE_NO_WARNINGS",
        "_CRT_SECURE_NO_DEPRECATE",
        "_CRT_NONSTDC_NO_WARNINGS",
        "_CRT_NONSTDC_NO_DEPRECATE",
        "_SCL_SECURE_NO_WARNINGS",
        "_SCL_SECURE_NO_DEPRECATE",
        "_WINSOCK_DEPRECATED_NO_WARNINGS",

        -- Enable wchar_t mode for pugixml
        "PUGIXML_WCHAR_MODE",

        -- No windows.h min()/max() macros
        "NOMINMAX",
        
        -- Enable SSE
        "CTN_SSE"
    }
    
    -- License header definition file for the LicenseHeaderManager VS extension
    files "ctn.licenseheader"

    -- Find DirectX SDK
    findDirectX9()

    includedirs
    {
        "Vendor",
        "Shared/Utility"
    }

    filter "platforms:x86"
        architecture "x86"
    filter "platforms:x64"
        architecture "x86_64"

    filter "configurations:Debug"
        defines { "CTN_DEBUG" }
        targetsuffix "_d"

    filter "configurations:Release"
        optimize "Speed"

    if CI_BUILD then
        filter {}
            defines { "CI_BUILD=1" }
    end

    -- Generate PDB files at \Build\Symbols
    filter {"system:windows", "configurations:Release", "kind:not StaticLib"}
        os.mkdir("Build/Symbols")
        linkoptions "/PDB:\"Symbols\\$(ProjectName).pdb\""

    filter { "system:windows", "action:vs2015" }
        toolset "v140"

    filter { "system:windows", "action:vs2017" }
        toolset "v141"

    filter "system:windows"
        flags { "StaticRuntime" }

    filter { "system:windows", "platforms:x86" }
        defines { "WIN32" }

    filter { "system:windows", "platforms:x64" }
        defines { "WIN64" }

    filter { "system:windows", "kind:StaticLib" }
        defines { "_LIB" }

    -- Enable visual styles
    filter { "system:windows", "platforms:x86", "kind:not StaticLib" }
        linkoptions "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\""

    -- Helper functions for output path
    buildpath = function(p) return "%{wks.location}/../Bin/"..p.."/" end
    copy = function(p) return "{COPY} %{cfg.buildtarget.abspath} %{wks.location}../Bin/" .. p .. "/" end
    buildhost = os.computer_name()

    -- Include the projects we are going to build
    group "Client"
    include "Client/Core"
    include "Client/Launcher"
    include "Client/Game"
    --include "Client/Updater"
    if buildhost == "IFARBOD-PC" then
        include "Client/Network"
    end

    group "Server"
    include "Server/DS"
    --if buildhost == "IFARBOD-PC" then
        --include "Server/Network"
    --end
    
    group "Vendor"
    include "Vendor/angelscript"
    --include "Vendor/bzip2"
    include "Vendor/cef3"
    --include "Vendor/cryptopp"
    --include "Vendor/freetype"
    --include "Vendor/jo"
    include "Vendor/libcpuid"
    include "Vendor/libcurl"
    --include "Vendor/lz4"
    --include "Vendor/lzma"
    --include "Vendor/minhook"
    --include "Vendor/miniupnpc"
    include "Vendor/pugixml"
    --include "Vendor/RakNet"
    include "Vendor/stb"
    --include "Vendor/sqlite"
    --include "Vendor/yaml-cpp"
    --include "Vendor/zlib"
    --include "Vendor/zip"

    group "Shared"
    include "Shared/Utility"

-- Cleanup
if _ACTION == "clean" then
    os.rmdir("Bin");
    os.rmdir("Build");
end
