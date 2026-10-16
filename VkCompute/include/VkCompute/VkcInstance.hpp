#pragma once
#include <string>
#include <Volk.h>


namespace vkc
{
    class VkcInstance
    {
    public:
        constexpr VkcInstance() noexcept = default;
        ~VkcInstance()                   = default;

        void Initialize(const std::string& appName) noexcept;
        void Shutdown() noexcept;

        constexpr VkInstance GetInstance() const noexcept { return m_Instance; }
        constexpr VkPhysicalDevice GetPhysicalDevice() const noexcept { return m_PhysicalDevice; }
        constexpr VkDevice GetDevice() const noexcept { return m_Device; }

    private:
        VkInstance m_Instance                     = nullptr;
        VkPhysicalDevice m_PhysicalDevice         = nullptr;
        VkDevice m_Device                         = nullptr;
        VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;
    };
}  // namespace vkc
