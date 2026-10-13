require("Utils")


----------------------------------------------------------------------
-- The table of all repo-wide compile-configuration settings.
----------------------------------------------------------------------
Config = {}

-- Set 'true' to use dynamic linking, 'false' to use static linking
Config.DynamicLinking = true

-- Configures the Dll Import/Export symbols if DynamicLinking is enabled
if Config.DynamicLinking then
    Config.DllImport = Platform.Windows and "__declspec(dllimport)" or "__attribute__ ((visibility(\"default\")))"
    Config.DllExport = Platform.Windows and "__declspec(dllexport)" or "__attribute__ ((visibility(\"default\")))"
else
    Config.DllImport = ""
    Config.DllExport = ""
end

-- Set 'true' to use precompiled headers, 'false' otherwise
Config.EnablePch = false

-- The version of the C++ language standard to use
--
-- NOTE: The 'PickCppVersion' function does a platform-specific mapping for the language standard
-- E.g: gnu++ for Linux
Config.CppVersion = PickCppVersion("23") -- TODO: Change to C++26/C++latest

-- The version of the C++ language toolset to use
Config.CppToolsetVersion = "msc-v145"
