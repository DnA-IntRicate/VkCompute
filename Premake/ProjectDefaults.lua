require("Config")
require("Paths")


----------------------------------------------------------------------
-- C++ project defaults.
----------------------------------------------------------------------

-- Configures a standard C++ project
function ConfigureCppProject(options)
    -- List of options that can be passed to this function:
    -- Kind
    -- PchHeader
    -- PchSource
    -- DllApi
    options = options or {}

    kind(options.Kind)
    language("C++")
    cppdialect(Config.CppVersion)

    debugdir(Paths.Output)
    targetdir(Paths.Output)
    objdir(Paths.Intermediate)

    enablepch(Config.EnablePch and "On" or "Off")
    if Config.EnablePch then
        pchheader(options.PchHeader)
        pchsource(options.PchSource)

        files
        {
            options.PchHeader,
            options.PchSource
        }
    end

    files
    {
        "include/**.hpp",
        "src/**.cpp"
    }

    includedirs
    {
        ".",
        "include"
    }

    libdirs
    {
        Paths.Output
    }

    bindirs
    {
        Paths.Output
    }

    defines
    {
        "VKC_API=" .. options.DllApi
    }

    -- Disable PCH for all vendored source files compiled into this project
    filter "files:not src/**.cpp"
        enablepch "Off"
    filter {}
end
