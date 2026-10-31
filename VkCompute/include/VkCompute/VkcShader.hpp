#pragma once
#include "VkcDevice.hpp"
#include "VkcObject.hpp"
#include <Ilm/Ilm.hpp>
#include <vulkan/vulkan_core.h>
#include <cstdint>
#include <filesystem>
#include <span>
#include <string>


namespace vkc
{
    /**
     * @brief A Vulkan compute shader module.
     */
    class VkcShader
    {
    public:
        /**
         * @brief
         */
        VkcShader(const ilm::Ref<VkcDevice>& device, const std::filesystem::path& path) noexcept;

        /**
         * @brief
         */
        VkcShader(const ilm::Ref<VkcDevice>& device, const std::string& name, std::span<uint8_t>& spv) noexcept;

        ~VkcShader() noexcept;

        // Allow copy-assignment by ilm::Ref only
        constexpr VkcShader(const VkcShader&)            = delete;
        constexpr VkcShader& operator=(const VkcShader&) = delete;

        // Allow move-assignment by ilm::Ref only
        constexpr VkcShader(VkcShader&&)            = delete;
        constexpr VkcShader& operator=(VkcShader&&) = delete;

        // Forbid default construction
        constexpr VkcShader() = delete;

        /**
         * @brief Destroys this shader module.
         */
        void Destroy() noexcept;

        /**
         * @brief Gets the device that this shader module is created for.
         */
        constexpr ilm::Ref<VkcDevice> GetDevice() const noexcept { return m_Device; }

        /**
         * @brief Gets this shader's name.
         */
        constexpr std::string GetName() const noexcept { return m_Name; }

        /**
         * @brief Checks if this descriptor allocator is valid.
         */
        constexpr bool Valid() const noexcept { return (bool)m_Device && m_Device->Valid() && (bool)m_Shader; }

    public:
        /**
         * @brief
         */
        static ilm::Ref<VkcShader> Create(const ilm::Ref<VkcDevice>& device, const std::filesystem::path& path) noexcept;

        /**
         * @brief
         */
        static ilm::Ref<VkcShader> Create(const ilm::Ref<VkcDevice>& device, const std::string& name, std::span<uint8_t>& spv) noexcept;

    private:
        ilm::Ref<VkcDevice> m_Device;

        VkShaderModule m_Shader = nullptr;
        std::string m_Name;
    };

    static_assert(VkcObject<VkcShader>);
    static_assert(VkcCreatable<VkcShader, const ilm::Ref<VkcDevice>&, const std::filesystem::path&>);
    static_assert(VkcCreatable<VkcShader, const ilm::Ref<VkcDevice>&, const std::string&, std::span<uint8_t>&>);
}  // namespace vkc
