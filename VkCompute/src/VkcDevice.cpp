#include <VkCompute/VkcDevice.hpp>
#include <VkCompute/Assert.hpp>
#include <VkCompute/Log.hpp>
#include <ranges>


namespace vkc
{
    constexpr float QUEUE_PRIORITY = 1.0f;

    VkcDevice::VkcDevice(const VkcPhysicalDevice& physicalDevice) noexcept
        : m_PhysicalDevice(physicalDevice)
    {
        const VkcQueueFamilyIndices& qfIndices = m_PhysicalDevice.QueueFamilyIndices;

        std::vector<VkDeviceQueueCreateInfo> qcInfos;
        std::vector<float> queuePriorities;

        qcInfos.reserve(3);
        queuePriorities.reserve(3);

        // Specify the required queues to create during device creation
        const auto addQueueFamily = [&](std::optional<uint32_t> qfIndex)
        {
            if (!qfIndex)
                return;

            // Avoid creating multiple queue-info structs for the same family
            uint32_t idx = *qfIndex;
            if (std::ranges::any_of(qcInfos, [idx](const auto& info) { return info.queueFamilyIndex == idx; }))
                return;

            queuePriorities.push_back(QUEUE_PRIORITY);
            qcInfos.emplace_back(
                VkDeviceQueueCreateInfo{ .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                                         .pNext            = nullptr,
                                         .flags            = 0,
                                         .queueFamilyIndex = idx,
                                         .queueCount       = 1,
                                         .pQueuePriorities = &queuePriorities.back() }
            );
        };

        addQueueFamily(qfIndices.Compute);
        addQueueFamily(qfIndices.Graphics);
        addQueueFamily(qfIndices.Transfer);

        VkDeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pNext                = nullptr;
        deviceCreateInfo.flags                = 0;
        deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(qcInfos.size());
        deviceCreateInfo.pQueueCreateInfos    = qcInfos.data();
        deviceCreateInfo.pEnabledFeatures     = nullptr;
        VkResult err                          = vkCreateDevice(m_PhysicalDevice.Handle, &deviceCreateInfo, nullptr, &m_Device);

        // We actualy want this assert to run even on Release builds.
        vkc::Assert(err == VK_SUCCESS, "Failed to create logical device!");

        // Get device queues
        if (qfIndices.Compute)
        {
            vkGetDeviceQueue(m_Device, *qfIndices.Compute, 0, &m_ComputeQueue);
            vkc::Success("Created compute queue family at index {0}.", *qfIndices.Compute);
        }

        if (qfIndices.Graphics)
        {
            vkGetDeviceQueue(m_Device, *qfIndices.Graphics, 0, &m_GraphicsQueue);
            vkc::Success("Created graphics queue family at index {0}.", *qfIndices.Graphics);
        }

        if (qfIndices.Transfer)
        {
            vkGetDeviceQueue(m_Device, *qfIndices.Transfer, 0, &m_TransferQueue);
            vkc::Success("Created transfer queue family at index {0}.", *qfIndices.Transfer);
        }

        // Create device command pools, 1 per queue
        const auto createCommandPool = [&](std::optional<uint32_t> qfIndex, VkCommandPool& pool)
        {
            if (!qfIndex)
                return;

            VkCommandPoolCreateInfo poolCreateInfo{};
            poolCreateInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            poolCreateInfo.pNext            = nullptr;
            poolCreateInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            poolCreateInfo.queueFamilyIndex = *qfIndex;
            err                             = vkCreateCommandPool(m_Device, &poolCreateInfo, nullptr, &pool);

            // We actualy want this assert to run even on Release builds.
            vkc::Assert(err == VK_SUCCESS, "Failed to create logical device command pool!");
        };

        createCommandPool(qfIndices.Compute, m_ComputeCommandPool);
        createCommandPool(qfIndices.Graphics, m_GraphicsCommandPool);
        createCommandPool(qfIndices.Transfer, m_TransferCommandPool);
    }

    VkcDevice::~VkcDevice() noexcept
    {
        Destroy();
    }

    void VkcDevice::Destroy() noexcept
    {
        if (m_ComputeCommandPool)
        {
            vkDestroyCommandPool(m_Device, m_ComputeCommandPool, nullptr);
            m_ComputeCommandPool = nullptr;
        }

        if (m_GraphicsCommandPool)
        {
            vkDestroyCommandPool(m_Device, m_GraphicsCommandPool, nullptr);
            m_GraphicsCommandPool = nullptr;
        }

        if (m_TransferCommandPool)
        {
            vkDestroyCommandPool(m_Device, m_TransferCommandPool, nullptr);
            m_TransferCommandPool = nullptr;
        }

        if (m_Device)
        {
            vkDestroyDevice(m_Device, nullptr);
            m_Device       = nullptr;
            m_ComputeQueue = nullptr;
            m_GraphicsQueue = nullptr;
            m_TransferQueue = nullptr;

            vkc::Warn("Destroyed logical device instance of device '{}'.", m_PhysicalDevice.Props.deviceName);
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

            VkcQueueFamilyIndices qfIndices{};
            for (const auto& [queueFamilyIndex, qf] : std::views::enumerate(queueFamilies))
            {
                if (qf.queueCount == 0)
                    continue;

                const uint32_t index     = static_cast<uint32_t>(queueFamilyIndex);
                const VkQueueFlags flags = qf.queueFlags;

                const bool supportsGraphics = (flags & VK_QUEUE_GRAPHICS_BIT) != 0;
                const bool supportsCompute  = (flags & VK_QUEUE_COMPUTE_BIT) != 0;
                const bool supportsTransfer = (flags & VK_QUEUE_TRANSFER_BIT) != 0;

                if (supportsCompute)
                {
                    // Prefer dedicated compute queues.
                    if (!qfIndices.Compute || !supportsGraphics)
                        qfIndices.Compute = index;
                }

                if (supportsGraphics && !qfIndices.Graphics)
                    qfIndices.Graphics = index;

                if (supportsTransfer)
                {
                    // Prefer dedicated transfer queues.
                    if (!qfIndices.Transfer || (!supportsGraphics && !supportsCompute))
                        qfIndices.Transfer = index;
                }
            }

            devices.emplace_back(pDevice, deviceIndex, props, qfIndices);
        }

        return devices;
    }
}  // namespace vkc
