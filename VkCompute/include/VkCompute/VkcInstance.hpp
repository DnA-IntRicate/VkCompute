#pragma once
#include <string>
#include <vector>
#include <Volk.h>


namespace vkc
{
    class VkcInstance
    {
    public:
        constexpr VkcInstance() noexcept = default;
        ~VkcInstance() = default;

        void Initialize(const std::string& appName) noexcept;
        void Shutdown() noexcept;

        constexpr VkInstance GetInstance() const noexcept { return m_Instance; }
        constexpr VkPhysicalDevice GetPhysicalDevice() const noexcept { return m_PhysicalDevice; }
        constexpr VkDevice GetDevice() const noexcept { return m_Device; }

    private:
        VkInstance m_Instance             = nullptr;
        VkPhysicalDevice m_PhysicalDevice = nullptr;
        VkDevice m_Device                 = nullptr;

        // VkDebugUtilsMessengerEXT m_DebugMessenger;

        std::vector<const char*> m_Extensions;
        std::vector<VkExtensionProperties> m_InstalledExtensions;
        std::vector<const char*> m_WantedExtensions = {
#if defined(VKC_DEBUG) || defined(VKC_DEV)
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
            VK_KHR_SURFACE_EXTENSION_NAME,
            VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
#ifdef VK_USE_PLATFORM_WIN32_KHR
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#elif VK_USE_PLATFORM_MACOS_MVK
            VK_MVK_MACOS_SURFACE_EXTENSION_NAME
#elif VK_USE_PLATFORM_XCB_KHR
            VK_KHR_XCB_SURFACE_EXTENSION_NAME
#elif VK_USE_PLATFORM_ANDROID_KHR
            VK_KHR_ANDROID_SURFACE_EXTENSION_NAME
#elif VK_USE_PLATFORM_XLIB_KHR
            VK_KHR_XLIB_SURFACE_EXTENSION_NAME
#elif VK_USE_PLATFORM_WAYLAND_KHR
            VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME
#elif defined VK_USE_PLATFORM_MIR_KHR || VK_USE_PLATFORM_DISPLAY_KHR
            VK_KHR_DISPLAY_EXTENSION_NAME
#elif VK_USE_PLATFORM_ANDROID_KHR
            VK_KHR_ANDROID_SURFACE_EXTENSION_NAME
#elif VK_USE_PLATFORM_IOS_MVK
            VK_MVK_IOS_SURFACE_EXTENSION_NAME
#endif
        };

        std::vector<const char*> m_Layers;
        std::vector<VkLayerProperties> m_InstalledLayers;
        std::vector<const char*> m_WantedLayers = {
#if defined(VKC_DEBUG) || defined(VKC_DEV)
            "VK_LAYER_LUNARG_standard_validation", "VK_LAYER_KHRONOS_validation"
#endif
        };
    };
}  // namespace vkc
