#include <VkCompute/VkcInstance.hpp>

// TODO(Adam): Add spdlog + fmt (or just fmt) and write logging functionality!
#include <cstdint>
#include <print>


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

    // Find which of the Vulkan extensions we want enabled are actually supported by this device
    static constexpr std::vector<const char*> FindBestExtensions(const std::vector<VkExtensionProperties>& installed,
        const std::vector<const char*>& wanted) noexcept
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
    static constexpr std::vector<const char*> FindBestLayers(const std::vector<VkLayerProperties>& installed,
        const std::vector<const char*>& wanted) noexcept
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
        if (volkInitialize() != VK_SUCCESS)
        {
            std::println("Volk initialization failed!");
            return;
        }

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        m_InstalledExtensions.resize((size_t)extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, m_InstalledExtensions.data());
        m_Extensions = FindBestExtensions(m_InstalledExtensions, m_WantedExtensions);

        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        m_InstalledLayers.resize((size_t)layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, m_InstalledLayers.data());
        m_Layers = FindBestLayers(m_InstalledLayers, m_WantedLayers);

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
        instanceInfo.enabledExtensionCount   = (uint32_t)m_Extensions.size();
        instanceInfo.ppEnabledExtensionNames = m_Extensions.data();
        instanceInfo.enabledLayerCount       = (uint32_t)m_Layers.size();
        instanceInfo.ppEnabledLayerNames     = m_Layers.data();
        VkResult err                         = vkCreateInstance(&instanceInfo, nullptr, &m_Instance);

        if (err != VK_SUCCESS)
        {
            std::println("Vulkan initialization failed!");
            return;
        }

        volkLoadInstance(m_Instance);
        std::println("Vulkan initialization succeeded.");

        // TODO(Adam): Create the graphics device here

        // TODO(Adam): Create the command pool too!
    }

    void VkcInstance::Shutdown() noexcept
    {

    }
}  // namespace vkc
