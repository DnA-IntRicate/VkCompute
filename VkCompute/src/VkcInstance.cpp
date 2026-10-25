#include <VkCompute/VkcInstance.hpp>
#include <VkCompute/Assert.hpp>
#include <VkCompute/Log.hpp>
#include <vma/vk_mem_alloc.h>
#include <algorithm>
#include <array>
#include <cstdint>


namespace vkc
{
    static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugMessageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        switch (messageSeverity)
        {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:   vkc::Error("{}", pCallbackData->pMessage); break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: vkc::Warn("{}", pCallbackData->pMessage); break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:    vkc::Trace("{}", pCallbackData->pMessage); break;
            default:                                              VKC_ASSERT(false, "Unknown VkDebugUtilsMessageSeverityFlagBitsEXT level!"); break;
        }

        return VK_FALSE;
    }

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
        "VK_LAYER_LUNARG_standard_validation",
        "VK_LAYER_KHRONOS_validation"
#endif
        ""  // No layers in release mode
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

    // Helper function to setup the Vulkan debug messenger
    static inline VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) noexcept
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func)
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);

        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    // Helper function to destroy the Vulkan debug messenger
    static inline void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator) noexcept
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func)
            func(instance, debugMessenger, pAllocator);
    }

    static inline VkResult CreateVmaAllocator(VkInstance instance, const ilm::Ref<VkcDevice>& device, VmaAllocator& allocator) noexcept
    {
        VmaVulkanFunctions functions{};
        functions.vkGetInstanceProcAddr                   = vkGetInstanceProcAddr;
        functions.vkGetDeviceProcAddr                     = vkGetDeviceProcAddr;
        functions.vkGetPhysicalDeviceProperties           = vkGetPhysicalDeviceProperties;
        functions.vkGetPhysicalDeviceMemoryProperties     = vkGetPhysicalDeviceMemoryProperties;
        functions.vkAllocateMemory                        = vkAllocateMemory;
        functions.vkFreeMemory                            = vkFreeMemory;
        functions.vkMapMemory                             = vkMapMemory;
        functions.vkUnmapMemory                           = vkUnmapMemory;
        functions.vkFlushMappedMemoryRanges               = vkFlushMappedMemoryRanges;
        functions.vkInvalidateMappedMemoryRanges          = vkInvalidateMappedMemoryRanges;
        functions.vkBindBufferMemory                      = vkBindBufferMemory;
        functions.vkBindImageMemory                       = vkBindImageMemory;
        functions.vkGetBufferMemoryRequirements           = vkGetBufferMemoryRequirements;
        functions.vkGetImageMemoryRequirements            = vkGetImageMemoryRequirements;
        functions.vkCreateBuffer                          = vkCreateBuffer;
        functions.vkDestroyBuffer                         = vkDestroyBuffer;
        functions.vkCreateImage                           = vkCreateImage;
        functions.vkDestroyImage                          = vkDestroyImage;
        functions.vkCmdCopyBuffer                         = vkCmdCopyBuffer;
        functions.vkGetBufferMemoryRequirements2KHR       = vkGetBufferMemoryRequirements2KHR;
        functions.vkGetImageMemoryRequirements2KHR        = vkGetImageMemoryRequirements2KHR;
        functions.vkBindBufferMemory2KHR                  = vkBindBufferMemory2KHR;
        functions.vkBindImageMemory2KHR                   = vkBindImageMemory2KHR;
        functions.vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2KHR;
        functions.vkGetDeviceBufferMemoryRequirements     = vkGetDeviceBufferMemoryRequirements;
        functions.vkGetDeviceImageMemoryRequirements      = vkGetDeviceImageMemoryRequirements;

        VmaAllocatorCreateInfo allocatorInfo{};
        allocatorInfo.flags                          = VmaAllocatorCreateFlags();
        allocatorInfo.physicalDevice                 = device->GetPhysicalDevice().Handle;
        allocatorInfo.device                         = device->GetDevice();
        allocatorInfo.preferredLargeHeapBlockSize    = 0;
        allocatorInfo.pAllocationCallbacks           = nullptr;
        allocatorInfo.pDeviceMemoryCallbacks         = nullptr;
        allocatorInfo.pHeapSizeLimit                 = nullptr;
        allocatorInfo.pVulkanFunctions               = &functions;
        allocatorInfo.instance                       = instance;
        allocatorInfo.vulkanApiVersion               = VK_API_VERSION_1_4;
        allocatorInfo.pTypeExternalMemoryHandleTypes = nullptr;

        return vmaCreateAllocator(&allocatorInfo, &allocator);
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

        // Initialize the Vulkan debug messenger for debug/dev builds
#if defined(VKC_DEBUG) || defined(VKC_DEV)
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        debugCreateInfo.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                          | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                                          | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = VulkanDebugMessageCallback;
        debugCreateInfo.pUserData       = nullptr;

        if (CreateDebugUtilsMessengerEXT(m_Instance, &debugCreateInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
            vkc::Error("Failed to setup Vulkan debug messenger!");

        vkc::Success("Initialized the Vulkan debug messenger.");
#endif

        std::vector<VkcPhysicalDevice> physicalDevices = VkcDevice::EnumeratePhysicalDevices(m_Instance);

        // Check if compute is supported. We actualy want this assert to run even on Release builds.
        const auto it = std::ranges::find_if(physicalDevices, [](const VkcPhysicalDevice& device) { return device.ComputeEnabled; });
        vkc::Assert(it != physicalDevices.end(), "Vulkan compute is not supported on this machine!");

        const VkcPhysicalDevice& physicalDevice = *it;
        vkc::Warn("Using Vulkan compute device ({}): {}.", physicalDevice.Index, physicalDevice.Name);

        m_Device = VkcDevice::Create(physicalDevice);
        if (m_Device->Valid())
            vkc::Success("Created logical device.");

        vkc::Assert(CreateVmaAllocator(m_Instance, m_Device, m_Allocator) == VK_SUCCESS, "Failed to create VMA allocator!");
        vkc::Success("Created VMA allocator.");
    }

    void VkcInstance::Shutdown() noexcept
    {
        vkc::Warn("Vulkan shutting-down...");

        m_Device->Destroy();
        m_Device = nullptr;

        if (m_DebugMessenger)
        {
            DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
            m_DebugMessenger = nullptr;
        }

        if (m_Allocator)
        {
            vmaDestroyAllocator(m_Allocator);
            m_Allocator = nullptr;
        }

        if (m_Instance)
        {
            vkDestroyInstance(m_Instance, nullptr);
            m_Instance = nullptr;
        }

        volkFinalize();
        vkc::Success("Vulkan shutdown.");
    }

    const std::vector<const char*>& VkcInstance::GetEnabledExtensions() const noexcept
    {
        return g_Extensions;
    }

    const std::vector<const char*>& VkcInstance::GetEnabledLayers() const noexcept
    {
        return g_Layers;
    }
}  // namespace vkc
