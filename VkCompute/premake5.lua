project "VkCompute"
    ConfigureCppProject
    {
        Kind = "ConsoleApp",
        PchHeader = "vkcpch.hpp",
        PchSource = "vkcpch.cpp",
        DllApi = Config.DllImport
    }

    includedirs
    {
        IncludeDir.Ilm,
        IncludeDir.VulkanSdk,
        IncludeDir.VulkanVolk,
        IncludeDir.SpirvReflect
    }

    filter "configurations:Dev"
        CopyRuntime "On"
    filter {}

    filter "configurations:Profiling"
        CopyRuntime "On"
    filter {}

    filter "configurations:Release"
        CopyRuntime "On"
    filter {}

