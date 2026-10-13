----------------------------------------------------------------------
-- What operating system are we running on?
----------------------------------------------------------------------
Platform = {}

-- Are we on a Windows machine?
Platform.Windows = os.target() == "windows"

-- Are we on a Linux machine?
Platform.Linux = os.target() == "linux"
