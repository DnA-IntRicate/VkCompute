require("Premake.Customization.Index")
require("Premake.Dependencies.Index")
require("Premake.Config")
require("Premake.Paths")
require("Premake.ProjectDefaults")
require("Premake.Version")


----------------------------------------------------------------------
-- Diagnostic printing.
----------------------------------------------------------------------

function _PrintDiagnostics()
    local rows =
    {
        { "VkCompute Version",  Version.String                                              },
        { "Platform",           Platform.Windows and "Windows" or "Linux"                   },
        { "Root Dir",           Paths.Root                                                  },
        { "Output Dir",         Paths.Output                                                },
        { "Intermediate Dir",   Paths.Intermediate                                          },
        { "Dynamic Linking",    Config.DynamicLinking and "Enabled" or "Disabled"           },
        { "Dll Import/Export",  string.format("%s/%s", Config.DllImport, Config.DllExport)  },
        { "Precompiled HDR",    Config.EnablePch and "Enabled" or "Disabled"                },
        { "C++ Standard",       Config.CppVersion                                           },
    }

    print("")
    print("VkCompute Configuration Diagnostics")
    print(string.rep("-", 70))

    for _, row in ipairs(rows) do
        print(string.format("  %-18s : %s", row[1], row[2]))
    end

    print(string.rep("-", 70))
    print("")
end

_PrintDiagnostics()


----------------------------------------------------------------------
-- Workspace definition.
----------------------------------------------------------------------

workspace "VkCompute"
    startproject "VkCompute"
    multiprocessorcompile "On"

    configurations
    {
        "Debug",
        "Dev",
        "Release"
    }

    platforms
    {
        "x64"
    }

    SolutionItems
    {
        ".clang-format",
        ".clang-tidy",
        ".editorconfig",
        ".gitattributes",
        ".gitignore",
        ".gitmodules",
        "premake5.lua",
        "pyproject.toml",
        "raptor.toml",
        "README.md",
        "uv.lock"
    }

    filter "language:C++"
        defines
        {
            "VKC_VERSION_MAJOR=" .. Version.Major,
            "VKC_VERSION_MINOR=" .. Version.Minor,
            "VKC_VERSION_PATCH=" .. Version.Patch,
            string.format("VKC_VERSION_STRING=\"%s\"", Version.String),
            "VKC_DLL_IMPORT=" .. Config.DllImport,
            "VKC_DLL_EXPORT=" .. Config.DllExport,
            "VKC_ENABLE_PCH=" .. (Config.EnablePch and 1 or 0)
        }
    filter {}

    filter "system:windows"
        systemversion "latest"
        staticruntime "Off"
        defines "VKC_PLATFORM_WINDOWS"
    filter {}

    filter "system:linux"
        systemversion "latest"
        pic "On"
        staticruntime "Off"
        defines "VKC_PLATFORM_LINUX"
    filter {}

    filter "system:macosx"
        systemversion "latest"
        pic "On"
        staticruntime "Off"
        defines "_IE_PLATFORM_OSX"
    filter {}

    filter "configurations:Debug"
        runtime "Debug"
        symbols "Full"
        optimize "Debug"

        defines
        {
            "VKC_DEBUG",
            "_DEBUG"
        }
    filter {}

    filter "configurations:Dev"
        runtime "Release"
        symbols "On"
        optimize "On"

        defines
        {
            "VKC_DEV",
            "NDEBUG"
        }
    filter {}

    filter "configurations:Release"
        runtime "Release"
        symbols "Off"
        optimize "Full"
        linktimeoptimization "On"
        incrementallink "Off"
        buffersecuritycheck "Off"
        runtimechecks "Off"

        defines
        {
            "VKC_RELEASE",
            "VKC_ENABLE_LOGGING",
            "NDEBUG"
        }
    filter {}

    include "VkCompute"
