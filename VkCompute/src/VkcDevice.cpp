#include <VkCompute/VkcDevice.hpp>
#include <ranges>


namespace vkc
{
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
                    computeQueueFamilyIndex = queueFamilyIndex;
                    break;
                }
            }

            devices.emplace_back(pDevice, deviceIndex, props.deviceName, props.deviceType, computeEnabled, computeQueueFamilyIndex);
        }

        return devices;
    }
}  // namespace vkc
