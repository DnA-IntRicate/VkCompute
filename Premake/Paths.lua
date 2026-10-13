----------------------------------------------------------------------
-- The table of all repo-defined special paths.
----------------------------------------------------------------------
Paths = {}

-- The absolute path of the repos' root directory.
Paths.Root = "%{wks.location}"

-- The output path for all compiled binaries; also functions as the debug directory
Paths.Output = Paths.Root .. "/bin/build/%{cfg.system}-%{cfg.architecture}-%{cfg.buildcfg}/VkCompute"

-- The output path for all intermediate-compile files - the 'obj' dir
Paths.Intermediate = Paths.Root .. "/bin/intermediate/%{cfg.system}-%{cfg.architecture}-%{cfg.buildcfg}"
