require("Env")
require("IncludeDirs")


----------------------------------------------------------------------
-- The table of directories to all VkCompute's static dependencies.
----------------------------------------------------------------------
LibDir = {}

-- Vulkan Sdk library directory
LibDir.VulkanSdk = Env.VulkanSdk .. "/Lib"
