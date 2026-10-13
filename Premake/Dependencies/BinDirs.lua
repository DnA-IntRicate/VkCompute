require("Env")
require("IncludeDirs")
require("../Config")


----------------------------------------------------------------------
-- The table of directories to all VkCompute's shared dependencies.
----------------------------------------------------------------------
BinDir = {}

-- Vulkan Sdk binary directory
BinDir.VulkanSdk = Env.VulkanSdk .. "/Bin"
