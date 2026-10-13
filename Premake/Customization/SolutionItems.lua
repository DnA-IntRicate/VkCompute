require("vstudio")


----------------------------------------------------------------------
-- This property is used to specify extra files for Visual Studio
-- to include and track as part of the solution. Typically used for
-- repo root-level configuration files such as .editorconfig or
-- .clang-format.
----------------------------------------------------------------------
premake.api.register
{
    name = "SolutionItems",
    scope = "workspace",
    kind = "list:string"
}

-- Visual Studio 2026 Solution override
premake.override(premake.vstudio.sln2026, "projects", function(base, wks)
    if wks.SolutionItems and (#wks.SolutionItems > 0) then
        premake.push("<Folder Name=\"/Solution Items/\">")
        for _, itemPath in ipairs(wks.SolutionItems) do
            premake.w("<File Path=\"%s\" />", path.getrelative(wks.location, path.getabsolute(itemPath)))
        end

        premake.pop("</Folder>")
    end

    base(wks)
end)
