#include <VkCompute/VkcInstance.hpp>
#include <VkCompute/Log.hpp>
#include <array>
#include <cstdint>
#include <vector>


namespace vkc
{
    /*
    static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugMessageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        switch (messageSeverity)
        {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:   VKC_VULKAN_CRITICAL(pCallbackData->pMessage); break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: VKC_VULKAN_ERROR(pCallbackData->pMessage); break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:    VKC_VULKAN_INFO(pCallbackData->pMessage); break;
            default:                                              VKC_CORE_ASSERT(false, "Unknown VkDebugUtilsMessageSeverityFlagBitsEXT level!"); break;
        }

        return VK_FALSE;
    }
    */

    static std::vector<const char*> g_Extensions;
    static std::vector<VkExtensionProperties> g_InstalledExtensions;

    static constexpr std::array WANTED_EXTENSIONS = {
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

    static std::vector<const char*> g_Layers;
    static std::vector<VkLayerProperties> g_InstalledLayers;

    static constexpr std::array WANTED_LAYERS = {
#if defined(VKC_DEBUG) || defined(VKC_DEV)
        "VK_LAYER_LUNARG_standard_validation", "VK_LAYER_KHRONOS_validation"
#endif
    };

    // Find which of the Vulkan extensions we want enabled are actually supported by this device
    template<size_t N>
    static constexpr std::vector<const char*> FindBestExtensions(const std::vector<VkExtensionProperties>& installed,
        const std::array<const char*, N>& wanted) noexcept
    {
        std::vector<const char*> res;
        for (const char* const& s : wanted)
        {
            for (const VkExtensionProperties& ext : installed)
            {
                if (std::string(ext.extensionName).compare(s) == 0)
                {
                    res.emplace_back(s);
                    break;
                }
            }
        }

        return res;
    }

    // Find which of the Vulkan layers we want enabled are actually supported by the device
    template<size_t N>
    static constexpr std::vector<const char*> FindBestLayers(const std::vector<VkLayerProperties>& installed,
        const std::array<const char*, N>& wanted) noexcept
    {
        std::vector<const char*> res;
        for (const char* const& s : wanted)
        {
            for (const VkLayerProperties& props : installed)
            {
                if (std::string(props.layerName).compare(s) == 0)
                {
                    res.emplace_back(s);
                    break;
                }
            }
        }

        return res;
    }

    void VkcInstance::Initialize(const std::string& appName) noexcept
    {
        vkc::Info("Vulkan initializing...");

        if (volkInitialize() != VK_SUCCESS)
        {
            vkc::Critical("Volk initialization failed!");
            volkFinalize();

            return;
        }

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        g_InstalledExtensions.resize((size_t)extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, g_InstalledExtensions.data());
        g_Extensions = FindBestExtensions(g_InstalledExtensions, WANTED_EXTENSIONS);

        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        g_InstalledLayers.resize((size_t)layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, g_InstalledLayers.data());
        g_Layers = FindBestLayers(g_InstalledLayers, WANTED_LAYERS);

        VkApplicationInfo appInfo{};
        appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName   = appName.c_str();
        appInfo.pEngineName        = "VkCompute";
        appInfo.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
        appInfo.engineVersion      = VK_MAKE_API_VERSION(1, 0, 0, 0);
        appInfo.apiVersion         = VK_API_VERSION_1_4;

        VkInstanceCreateInfo instanceInfo{};
        instanceInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.flags                   = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        instanceInfo.pApplicationInfo        = &appInfo;
        instanceInfo.enabledExtensionCount   = (uint32_t)g_Extensions.size();
        instanceInfo.ppEnabledExtensionNames = g_Extensions.data();
        instanceInfo.enabledLayerCount       = (uint32_t)g_Layers.size();
        instanceInfo.ppEnabledLayerNames     = g_Layers.data();
        VkResult err                         = vkCreateInstance(&instanceInfo, nullptr, &m_Instance);

        if (err != VK_SUCCESS)
        {
            vkc::Critical("Vulkan initialization failed!");
            volkFinalize();

            return;
        }

        volkLoadInstance(m_Instance);
        vkc::Success("Vulkan initialized.");

        // TODO(Adam): Create the graphics device here

        // TODO(Adam): Create the command pool too!
    }

    void VkcInstance::Shutdown() noexcept
    {
        vkc::Warn("Vulkan shutting-down...");

        if (m_Device)
        {
            vkDestroyDevice(m_Device, nullptr);
            m_Device         = nullptr;
            m_PhysicalDevice = nullptr;
        }

        if (m_Instance)
        {
            vkDestroyInstance(m_Instance, nullptr);
            m_Instance = nullptr;
        }

        volkFinalize();
        vkc::Success("Vulkan shutdown.");
    }
}  // namespace vkc
