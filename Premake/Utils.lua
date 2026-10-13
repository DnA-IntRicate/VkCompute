require("Platform")


--------------------------------------------------------------------------------
-- File system & environment utils
--------------------------------------------------------------------------------

-- Normalizes a path by replacing all backslashes with forward slashes
function NormPath(path)
    if not path then return "" end
    return path:gsub("\\", "/")
end

-- Returns 'true' if the given string is a file path, 'false' otherwise
function IsPath(str)
    return (type(str) == "string") and (path.isabsolute(str) or str:find("[/\\]") ~= nil)
end

-- Converts 'x' to a string and wraps it in double quotes.
function Stringify(x)
    x = tostring(x)
    return string.format("\"%s\"", x)
end

-- Executes a system command
function Run(cmd)
    local f = io.popen(cmd)
    if not f then return nil end

    local out = f:read("*a")
    f:close()

    return out
end

-- Finds the full path of an executable
function Where(prog)
    local command = (package.config:sub(1, 1) == "\\" and "where " or "which ") .. prog
    local result = Run(command)
    if not result or result == "" then return nil end

    local first = result:match("([^\r\n]+)")
    return first and first:match("(.+)[/\\][^/\\]+$") -- Parent path
end


--------------------------------------------------------------------------------
-- C++ language utils
--------------------------------------------------------------------------------

-- Returns a Premake-specific C++ language version string
function PickCppVersion(targetVersion)
    if Platform.Windows then return "C++" .. targetVersion end
    if Platform.Linux then return "gnu++" .. targetVersion end

    return "Default"
end
