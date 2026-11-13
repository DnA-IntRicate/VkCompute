#pragma once
#include "VkcDevice.hpp"
#include <Ilm/Ilm.hpp>
#include <vulkan/vulkan_core.h>
#include <cstdint>
#include <filesystem>
#include <span>
#include <string>
#include <string_view>


namespace vkc
{
    /**
     * @brief Represents a Vulkan compute shader module.
     *
     * A VkcShader encapsulates a Vulkan shader module created from compiled SPIR-V bytecode.
     * Shader modules can be constructed either from a SPIR-V file on disk or directly
     * from an in-memory SPIR-V bytecode buffer.
     */
    class VkcShader
    {
    public:
        static constexpr std::string_view Name = "VkcShader";

    public:
        /**
         * @brief Constructs a shader module from a GLSL source code file.
         *
         * This constructor parses and compiles the source code to SPIR-V.
         *
         * @param device The logical device used to create the shader module.
         * @param path The filesystem path to the GLSL source code.
         */
        VkcShader(const ilm::Ref<VkcDevice>& device, const std::filesystem::path& path) noexcept;

        /**
         * @brief Constructs a shader module from in-memory SPIR-V bytecode.
         *
         * @param device The logical device used to create the shader module.
         * @param name The name associated with this shader module.
         * @param spv A span containing the compiled SPIR-V bytecode.
         */
        VkcShader(const ilm::Ref<VkcDevice>& device, const std::string& name, std::span<const uint32_t> spv) noexcept;

        /**
         * @brief Destroys this shader module.
         */
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
         * @brief Gets the logical device associated with this shader module.
         *
         * @return A reference to the logical device used to create this shader.
         */
        constexpr ilm::Ref<VkcDevice> GetDevice() const noexcept { return m_Device; }

        /**
         * @brief Gets the name associated with this shader module.
         *
         * @return The shader's name.
         */
        constexpr const std::string& GetName() const noexcept { return m_Name; }

        /**
         * @brief Gets the Vulkan shader module handle.
         *
         * @return The Vulkan shader module handle.
         */
        constexpr VkShaderModule GetShaderModule() const noexcept { return m_Shader; }

        /**
         * @brief Checks whether this shader module is valid.
         *
         * A shader is considered valid if its logical device exists and is valid,
         * and its underlying Vulkan shader module handle is non-null.
         *
         * @return True if the shader module is valid; otherwise, false.
         */
        constexpr bool Valid() const noexcept { return (bool)m_Device && m_Device->Valid() && (bool)m_Shader; }

    public:
        /**
         * @brief Creates a shader module from a GLSL source code file.
         *
         * Constructs and returns a reference-managed shader module using the
         * loaded GLSL source code after compiling it to SPIR-V bytecode.
         *
         * @param device The logical device used to create the shader module.
         * @param path The filesystem path to the compiled SPIR-V shader.
         *
         * @return A reference to the created shader module.
         */
        static ilm::Ref<VkcShader> Create(const ilm::Ref<VkcDevice>& device, const std::filesystem::path& path) noexcept;

        /**
         * @brief Creates a shader module from in-memory SPIR-V bytecode.
         *
         * Constructs and returns a reference-managed shader module directly from
         * the supplied in-memory SPIR-V bytecode.
         *
         * @param device The logical device used to create the shader module.
         * @param name The name associated with the shader module.
         * @param spv A span containing the compiled SPIR-V bytecode.
         *
         * @return A reference to the created shader module.
         */
        static ilm::Ref<VkcShader> Create(
            const ilm::Ref<VkcDevice>& device,
            const std::string& name,
            std::span<const uint32_t> spv
        ) noexcept;

    private:
        // The logical device reference.
        ilm::Ref<VkcDevice> m_Device;

        // The name of this shader module.
        // Either provided or derived from the source code's file name.
        std::string m_Name;

        // The underlying Vulkan shader module handle.
        VkShaderModule m_Shader = nullptr;
    };
}  // namespace vkc
