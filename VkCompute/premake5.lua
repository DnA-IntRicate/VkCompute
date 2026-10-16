project "VkCompute"
    ConfigureCppProject
    {
        Kind = "ConsoleApp",
        PchHeader = "vkcpch.hpp",
        PchSource = "vkcpch.cpp",
        DllApi = Config.DllImport
    }

    externalincludedirs
    {
        IncludeDir.Fmt,
        IncludeDir.Ilm,
        IncludeDir.VulkanSdk,
        IncludeDir.VulkanVolk,
        IncludeDir.SpirvReflect
    }
    externalwarnings "Off"

    links
    {
        "fmt",
        path.join(LibDir.VulkanSdk, Lib.Volk)
    }

    defines
    {
        "VK_NO_PROTOTYPES"
    }

    filter "system:windows"
        defines
        {
            "WIN32_LEAN_AND_MEAN",
            "NOMINMAX",
            "VK_USE_PLATFORM_WIN32_KHR"
        }
    filter {}

    filter "system:linux"
        defines
        {
            "VK_USE_PLATFORM_XCB_KHR"
        }
    filter {}

    filter "system:macosx"
        defines
        {
            "VK_USE_PLATFORM_MACOS_MVK"
        }

    filter "configurations:Debug"
        CopyRuntime "Off"

        prebuildcommands
        {
            -- Copy ShadercSharedDebug to Paths.Output
            string.format(
                '{COPYFILE} "%s/%s" "%s"',
                BinDir.VulkanSdk,
                Bin.ShadercSharedDebug,
                Paths.Output
            )
        }

        links
        {
            path.join(LibDir.VulkanSdk, Lib.ShadercSharedDebug),
            path.join(LibDir.VulkanSdk, Lib.SpirvCrossDebug),
            path.join(LibDir.VulkanSdk, Lib.SpirvCrossGlslDebug)
        }
    filter {}

    filter "configurations:not Debug"
        CopyRuntime "On"

        prebuildcommands
        {
            -- Copy ShadercSharedRelease to Paths.Output
            string.format(
                '{COPYFILE} "%s/%s" "%s"',
                BinDir.VulkanSdk,
                Bin.ShadercSharedRelease,
                Paths.Output
            )
        }

        links
        {
            path.join(LibDir.VulkanSdk, Lib.ShadercSharedRelease),
            path.join(LibDir.VulkanSdk, Lib.SpirvCrossRelease),
            path.join(LibDir.VulkanSdk, Lib.SpirvCrossGlslRelease)
        }
    filter {}
