require("../Utils")


----------------------------------------------------------------------
-- The table of all directories on the system mapped by environment
-- variables.
----------------------------------------------------------------------
Env = {}

-- The location of the Vulkan Sdk installation
Env.VulkanSdk = NormPath(os.getenv("VK_SDK_PATH"))
