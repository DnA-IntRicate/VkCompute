#pragma once
#include "VkcDevice.hpp"
#include <Ilm/Ilm.hpp>
#include <vma/vk_mem_alloc.h>
#include <Volk.h>
#include <string>
#include <vector>


namespace vkc
{
    /**
     * @brief Manages the Vulkan instance and associated resources.
     *
     * Owns the Vulkan instance, debug messenger, VMA allocator, and references
     * to other Vulkan objects.
     */
    class VkcInstance
    {
    public:
        constexpr VkcInstance() noexcept = default;
        ~VkcInstance()                   = default;

        /**
         * @brief Initializes the Vulkan instance and associated resources.
         *
         * @param appName Name of the application using the Vulkan instance.
         */
        void Initialize(const std::string& appName) noexcept;

        /**
         * @brief Releases the Vulkan instance and associated resources.
         */
        void Shutdown() noexcept;

        /**
         * @brief Gets the underlying Vulkan instance.
         *
         * @return The managed VkInstance handle, or nullptr if uninitialized.
         */
        constexpr VkInstance GetInstance() const noexcept { return m_Instance; }

        /**
         * @brief Retrieves the Vulkan Memory Allocator instance.
         *
         * @return The managed VmaAllocator handle, or nullptr if uninitialized.
         */
        constexpr VmaAllocator GetAllocator() const noexcept { return m_Allocator; }

        /**
         * @brief Retrieves a reference to the logical device.
         *
         * @return A reference to the managed VkcDevice.
         */
        constexpr ilm::Ref<VkcDevice> GetDevice() const noexcept { return m_Device; }

        /**
         * @brief Retrieves the enabled Vulkan instance extensions.
         *
         * @return A constant reference to the vector of enabled extension names.
         */
        const std::vector<const char*>& GetEnabledExtensions() const noexcept;

        /**
         * @brief Retrieves the enabled Vulkan instance layers.
         *
         * @return A constant reference to the vector of enabled layer names.
         */
        const std::vector<const char*>& GetEnabledLayers() const noexcept;

    private:
        VkInstance m_Instance                     = nullptr;
        VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;
        VmaAllocator m_Allocator                  = nullptr;

        ilm::Ref<VkcDevice> m_Device;
    };
}  // namespace vkc
