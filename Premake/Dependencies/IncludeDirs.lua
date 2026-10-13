require("Env")
require("Vendor")
require("../Config")


----------------------------------------------------------------------
-- The table of all C/C++ include directories.
----------------------------------------------------------------------
IncludeDir = {}

-- ThirdParty submodule includes
-- IncludeDir.Fmt              = Vendor.Fmt .. "/include"
-- IncludeDir.Glm              = Vendor.Glm
-- IncludeDir.Spdlog           = Vendor.Spdlog .. "/include"

-- Intrynzic submodule includes
-- IncludeDir.Ilm = Vendor.Ilm .. "/Ilm/src/include"

-- Vulkan Sdk includes
IncludeDir.VulkanSdk    = Env.VulkanSdk .. "/Include"
IncludeDir.VulkanVolk   = IncludeDir.VulkanSdk .. "/Volk"
IncludeDir.SpirvReflect = Env.VulkanSdk .. "/Source/SPIRV-Reflect/include"

-- VkCompute's includes
IncludeDir.VkCompute = "VkCompute/include"
