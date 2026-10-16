#pragma once
#include <Ilm/Ilm.hpp>
#include <Volk/volk.h>
#include <cstdint>
#include <string>
#include <vector>


namespace vkc
{
    struct VkcPhysicalDevice
    {
        constexpr VkcPhysicalDevice(VkPhysicalDevice handle, uint32_t index, const char* name, VkPhysicalDeviceType type,
            bool computeEnabled, uint32_t computeQfIndex) noexcept
            : Handle(handle), Index(index), Name(name), Type(type), ComputeEnabled(computeEnabled), ComputeQueueFamilyIndex(computeQfIndex)
        {
        }

        constexpr VkcPhysicalDevice(const VkcPhysicalDevice&) = default;
        constexpr VkcPhysicalDevice(VkcPhysicalDevice&&)      = default;
        constexpr VkcPhysicalDevice()                         = default;

        VkPhysicalDevice Handle = nullptr;
        uint32_t Index          = 0;
        std::string Name;
        VkPhysicalDeviceType Type = VK_PHYSICAL_DEVICE_TYPE_OTHER;

        bool ComputeEnabled              = false;
        uint32_t ComputeQueueFamilyIndex = 0;
    };

    class VkcDevice
    {
    public:
        // Don't default this!
        // Add a dtor
        constexpr VkcDevice() = default;
        ~VkcDevice();

        // Maybe move this to the ctor?
        // void Create() noexcept;

        void Destroy() noexcept;

        constexpr VkcPhysicalDevice GetPhysicalDevice() const noexcept { return m_PhysicalDevice; }
        constexpr uint32_t GetDeviceIndex() const noexcept { return GetPhysicalDevice().Index; }
        constexpr VkDevice GetDevice() const noexcept { return m_Device; }

    public:
        static ilm::Ref<VkcDevice> Create(const VkcPhysicalDevice& physicalDevice) noexcept;

        static std::vector<VkcPhysicalDevice> EnumeratePhysicalDevices(VkInstance instance) noexcept;

    private:
        VkcPhysicalDevice m_PhysicalDevice;
        VkDevice m_Device = nullptr;
    };
}  // namespace vkc
