#pragma once
#include "VkcDevice.hpp"
#include <Ilm/Ilm.hpp>
#include <Volk.h>
#include <string>
#include <vector>


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
        constexpr ilm::Ref<VkcDevice> GetDevice() const noexcept { return m_Device; }

        const std::vector<const char*>& GetEnabledExtensions() const noexcept;
        const std::vector<const char*>& GetEnabledLayers() const noexcept;

    private:
        VkInstance m_Instance                     = nullptr;
        VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;

        ilm::Ref<VkcDevice> m_Device;
    };
}  // namespace vkc
