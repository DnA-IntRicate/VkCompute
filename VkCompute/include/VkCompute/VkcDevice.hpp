#pragma once
#include "VkcObject.hpp"
#include <Ilm/Ilm.hpp>
#include <Volk/volk.h>
#include <cstdint>
#include <string>
#include <vector>


namespace vkc
{
    /**
     * @brief Stores relevant information about a physical graphics device including its Vulkan handle.
     */
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

        // The core Vulkan handle
        VkPhysicalDevice Handle = nullptr;

        // The physical index of the device
        uint32_t Index = 0;

        // THe name of the device
        std::string Name;

        // The device type (discrete, integrated or virtual GPU/CPU)
        VkPhysicalDeviceType Type = VK_PHYSICAL_DEVICE_TYPE_OTHER;

        // Does this device support GPU compute
        bool ComputeEnabled = false;

        // The index of the device's compute queue family
        uint32_t ComputeQueueFamilyIndex = 0;
    };

    /**
     * @brief A wrapper around a Vulkan logical graphics/compute device.
     */
    class VkcDevice
    {
    public:
        VkcDevice(const VkcPhysicalDevice& physicalDevice) noexcept;
        ~VkcDevice() noexcept;

        // Allow copy-assignment by ilm::Ref only
        constexpr VkcDevice(const VkcDevice&)            = delete;
        constexpr VkcDevice& operator=(const VkcDevice&) = delete;

        // Allow move-assignment by ilm::Ref only
        constexpr VkcDevice(VkcDevice&&)            = delete;
        constexpr VkcDevice& operator=(VkcDevice&&) = delete;

        /**
         * @brief Destroys this device.
         */
        void Destroy() noexcept;

        /**
         * @brief Get the VkcPhysicalDevice that this instance was created off of.
         */
        constexpr const VkcPhysicalDevice& GetPhysicalDevice() const noexcept { return m_PhysicalDevice; }

        /**
         * @brief Get the physical index of this device.
         */
        constexpr uint32_t GetDeviceIndex() const noexcept { return GetPhysicalDevice().Index; }

        /**
         * @brief Get the index of this device's compute queue family.
         */
        constexpr uint32_t GetComputeQueueFamilyIndex() const noexcept { return GetPhysicalDevice().ComputeQueueFamilyIndex; }

        /**
         * @brief Get the Vulkan handle of this logical device instance.
         */
        constexpr VkDevice GetDevice() const noexcept { return m_Device; }

        /**
         * @brief Get the Vulkan handle of this instance's compute queue.
         */
        constexpr VkQueue GetComputeQueue() const noexcept { return m_ComputeQueue; }

        /**
         * @brief Get the Vulkan handle of this instance's command pool.
         */
        constexpr VkCommandPool GetCommandPool() const noexcept { return m_CommandPool; }

        /**
         * @brief Checks if this device is valid.
         */
        constexpr bool Valid() const noexcept { return (bool)m_Device && (bool)m_ComputeQueue; }

    public:
        /**
         * @brief Create a new VkcDevice.
         */
        static ilm::Ref<VkcDevice> Create(const VkcPhysicalDevice& physicalDevice) noexcept;

        /**
         * @brief Enumerates all physical devices on the system.
         *
         * @returns a vector containing all the physical devices found on the system.
         */
        static std::vector<VkcPhysicalDevice> EnumeratePhysicalDevices(VkInstance instance) noexcept;

    private:
        VkcPhysicalDevice m_PhysicalDevice;
        VkDevice m_Device           = nullptr;
        VkQueue m_ComputeQueue      = nullptr;
        VkCommandPool m_CommandPool = nullptr;
    };

    static_assert(VkcObject<VkcDevice>);
    static_assert(VkcCreatable<VkcDevice, const VkcPhysicalDevice&>);
}  // namespace vkc
