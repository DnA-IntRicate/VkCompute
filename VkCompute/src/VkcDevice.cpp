#include <VkCompute/VkcDevice.hpp>
#include <VkCompute/Assert.hpp>
#include <VkCompute/Log.hpp>
#include <ranges>


namespace vkc
{
    constexpr float COMPUTE_QUEUE_PRIORITY = 1.0f;

    VkcDevice::VkcDevice(const VkcPhysicalDevice& physicalDevice) noexcept
        : m_PhysicalDevice(physicalDevice)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.pNext            = nullptr;
        queueCreateInfo.flags            = 0;
        queueCreateInfo.queueFamilyIndex = m_PhysicalDevice.ComputeQueueFamilyIndex;
        queueCreateInfo.queueCount       = 1;
        queueCreateInfo.pQueuePriorities = &COMPUTE_QUEUE_PRIORITY;

        VkDeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pNext                = nullptr;
        deviceCreateInfo.flags                = 0;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pQueueCreateInfos    = &queueCreateInfo;
        deviceCreateInfo.pEnabledFeatures     = nullptr;
        VkResult err                          = vkCreateDevice(m_PhysicalDevice.Handle, &deviceCreateInfo, nullptr, &m_Device);

        // We actualy want this assert to run even on Release builds.
        vkc::Assert(err == VK_SUCCESS, "Failed to create logical device!");

        // Get compute queue from device
        vkGetDeviceQueue(m_Device, GetComputeQueueFamilyIndex(), 0, &m_ComputeQueue);

        // Create device command pool
        VkCommandPoolCreateInfo poolCreateInfo{};
        poolCreateInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolCreateInfo.pNext            = nullptr;
        poolCreateInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolCreateInfo.queueFamilyIndex = GetComputeQueueFamilyIndex();
        err                             = vkCreateCommandPool(m_Device, &poolCreateInfo, nullptr, &m_CommandPool);

        // We actualy want this assert to run even on Release builds.
        vkc::Assert(err == VK_SUCCESS, "Failed to create logical device command pool!");
    }

    VkcDevice::~VkcDevice() noexcept
    {
        Destroy();
    }

    void VkcDevice::Destroy() noexcept
    {
        if (m_CommandPool)
        {
            vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
            m_CommandPool = nullptr;
        }

        if (m_Device)
        {
            vkDestroyDevice(m_Device, nullptr);
            m_Device       = nullptr;
            m_ComputeQueue = nullptr;

            vkc::Warn("Destroyed logical device instance of device '{}'.", m_PhysicalDevice.Name);
        }
    }

    void VkcDevice::WaitIdle() const noexcept
    {
        vkDeviceWaitIdle(m_Device);
    }

    ilm::Ref<VkcDevice> VkcDevice::Create(const VkcPhysicalDevice& physicalDevice) noexcept
    {
        return ilm::CreateRef<VkcDevice>(physicalDevice);
    }

    std::vector<VkcPhysicalDevice> VkcDevice::EnumeratePhysicalDevices(VkInstance instance) noexcept
    {
        uint32_t deviceCount;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        std::vector<VkPhysicalDevice> physicalDevices(static_cast<size_t>(deviceCount));
        vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());

        std::vector<VkcPhysicalDevice> devices(static_cast<size_t>(deviceCount));
        for (const auto& [deviceIndex, pDevice] : std::views::enumerate(physicalDevices))
        {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(pDevice, &props);

            // Check if the device is compute-enabled
            uint32_t qfCount;
            vkGetPhysicalDeviceQueueFamilyProperties(pDevice, &qfCount, nullptr);

            std::vector<VkQueueFamilyProperties> queueFamilies(static_cast<size_t>(qfCount));
            vkGetPhysicalDeviceQueueFamilyProperties(pDevice, &qfCount, queueFamilies.data());

            bool computeEnabled              = false;
            uint32_t computeQueueFamilyIndex = (uint32_t)-1;
            for (const auto& [queueFamilyIndex, qf] : std::views::enumerate(queueFamilies))
            {
                if ((qf.queueCount > 0) && (qf.queueFlags & VK_QUEUE_COMPUTE_BIT))
                {
                    computeEnabled          = true;
                    computeQueueFamilyIndex = static_cast<uint32_t>(queueFamilyIndex);
                    break;
                }
            }

            devices.emplace_back(pDevice, deviceIndex, props.deviceName, props.deviceType, computeEnabled, computeQueueFamilyIndex);
        }

        return devices;
    }
}  // namespace vkc
