require("vstudio")


----------------------------------------------------------------------
-- This property is used to copy the C++ runtime DLLs to the
-- project's output directory after compilation.
----------------------------------------------------------------------
premake.api.register
{
    name = "CopyRuntime",
    scope = "config",
    kind = "boolean"
}

-- Visual Studio C++ override
premake.override(premake.vstudio.vc2010.elements, "configurationProperties", function(base, cfg)
    local elements = base(cfg)
    table.insert(elements, function()
        if cfg.CopyRuntime then
            premake.vstudio.vc2010.element("CopyCppRuntimeToOutputDir", nil, "true")
        end
    end)

    return elements
end)
