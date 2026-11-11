#pragma once
#include <Ilm/Ilm.hpp>
#include <Volk/volk.h>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>


namespace vkc
{
    /**
     * @brief Stores the device-specific indices to different queue types.
     */
    struct VkcQueueFamilyIndices
    {
        constexpr VkcQueueFamilyIndices() = default;

        std::optional<uint32_t> Compute;
        std::optional<uint32_t> Graphics;
        std::optional<uint32_t> Transfer;
    };

    /**
     * @brief Stores relevant information about a physical graphics device including its Vulkan handle.
     */
    struct VkcPhysicalDevice
    {
        constexpr VkcPhysicalDevice(
            VkPhysicalDevice handle,
            uint32_t index,
            VkPhysicalDeviceProperties props,
            const VkcQueueFamilyIndices& qfIndices
        ) noexcept
            : Handle(handle), Index(index), Props(props), QueueFamilyIndices(qfIndices)
        {
        }

        constexpr VkcPhysicalDevice(const VkcPhysicalDevice&) = default;
        constexpr VkcPhysicalDevice(VkcPhysicalDevice&&)      = default;
        constexpr VkcPhysicalDevice()                         = default;

        // The core Vulkan handle
        VkPhysicalDevice Handle = nullptr;

        // The physical index of the device
        uint32_t Index = 0;

        // Stores the properties of the physical device
        VkPhysicalDeviceProperties Props{};

        // This device's queue family indices
        VkcQueueFamilyIndices QueueFamilyIndices;

        // Does this device support GPU compute
        const bool ComputeEnabled = QueueFamilyIndices.Compute.has_value();
    };

    /**
     * @brief A wrapper around a Vulkan logical graphics/compute device.
     */
    class VkcDevice
    {
    public:
        static constexpr std::string_view Name = "VkcDevice";

    public:
        VkcDevice(const VkcPhysicalDevice& physicalDevice) noexcept;
        ~VkcDevice() noexcept;

        // Allow copy-assignment by ilm::Ref only
        constexpr VkcDevice(const VkcDevice&)            = delete;
        constexpr VkcDevice& operator=(const VkcDevice&) = delete;

        // Allow move-assignment by ilm::Ref only
        constexpr VkcDevice(VkcDevice&&)            = delete;
        constexpr VkcDevice& operator=(VkcDevice&&) = delete;

        // Forbid default construction
        constexpr VkcDevice() = delete;

        /**
         * @brief Destroys this device.
         */
        void Destroy() noexcept;

        /**
         * @brief Blocks the CPU until all previously submitted work on the logical device has completed.
         */
        void WaitIdle() const noexcept;

        /**
         * @brief Get the VkcPhysicalDevice that this instance was created off of.
         */
        constexpr const VkcPhysicalDevice& GetPhysicalDevice() const noexcept { return m_PhysicalDevice; }

        /**
         * @brief Get the physical index of this device.
         */
        constexpr uint32_t GetDeviceIndex() const noexcept { return GetPhysicalDevice().Index; }

        /**
         * @brief Get the queue family indices of this device.
         */
        constexpr VkcQueueFamilyIndices GetQueueFamilyIndices() const noexcept { return GetPhysicalDevice().QueueFamilyIndices; }

        /**
         * @brief Get the Vulkan handle of this logical device instance.
         */
        constexpr VkDevice GetDevice() const noexcept { return m_Device; }

        /**
         * @brief Get the Vulkan handle of this instance's compute queue.
         */
        constexpr VkQueue GetComputeQueue() const noexcept { return m_ComputeQueue; }

        /**
         * @brief Get the Vulkan handle of this instance's compute command pool.
         */
        constexpr VkCommandPool GetComputeCommandPool() const noexcept { return m_ComputeCommandPool; }

        /**
         * @brief Get the Vulkan handle of this instance's graphics queue.
         */
        constexpr VkQueue GetGraphicsQueue() const noexcept { return m_GraphicsQueue; }

        /**
         * @brief Get the Vulkan handle of this instance's graphics command pool.
         */
        constexpr VkCommandPool GetGraphicsCommandPool() const noexcept { return m_GraphicsCommandPool; }

        /**
         * @brief Get the Vulkan handle of this instance's transfer queue.
         */
        constexpr VkQueue GetTransferQueue() const noexcept { return m_TransferQueue; }

        /**
         * @brief Get the Vulkan handle of this instance's transfer command pool.
         */
        constexpr VkCommandPool GetTransferCommandPool() const noexcept { return m_TransferCommandPool; }

        /**
         * @brief Checks if this device is valid.
         */
        constexpr bool Valid() const noexcept { return (bool)m_Device && (bool)m_ComputeQueue; }

    public:
        /**
         * @brief Creates a new VkcDevice.
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
        VkDevice m_Device = nullptr;

        VkQueue m_ComputeQueue             = nullptr;
        VkCommandPool m_ComputeCommandPool = nullptr;

        VkQueue m_GraphicsQueue             = nullptr;
        VkCommandPool m_GraphicsCommandPool = nullptr;

        VkQueue m_TransferQueue             = nullptr;
        VkCommandPool m_TransferCommandPool = nullptr;
    };
}  // namespace vkc
