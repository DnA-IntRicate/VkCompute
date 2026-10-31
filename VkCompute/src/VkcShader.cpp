#include <VkCompute/VkcShader.hpp>
#include <VkCompute/Log.hpp>
#include <VkCompute/VkcTimer.hpp>
#include <shaderc/shaderc.hpp>
#include <Volk/volk.h>
#include <fstream>
#include <vector>


namespace vkc
{
    static inline bool ReadFile(const std::filesystem::path& path, std::string& outContent) noexcept
    {
        try
        {
            std::ifstream in(path, std::ios::in | std::ios::binary | std::ios::ate);
            if (!in)
            {
                vkc::Error("Failed to open shader source code file \"{}\"!", path.string());
                return false;
            }

            const std::streampos end = in.tellg();
            if (end < 0)
            {
                vkc::Error("Failed to determine shader source file size \"{}\"!", path.string());
                return false;
            }

            const size_t size = static_cast<size_t>(end);
            outContent.resize(size);

            in.seekg(0, std::ios::beg);
            if (!in.read(outContent.data(), static_cast<std::streamsize>(size)))
            {
                vkc::Error("Failed to read shader source file \"{}\"!", path.string());
                outContent.clear();

                return false;
            }
        }
        catch (const std::exception& e)
        {
            vkc::Error("Exception while reading shader source file \"{}\": {}", path.string(), e.what());
            outContent.clear();

            return false;
        }

        return true;
    }

    static inline bool CompileToSpv(const std::string& source, const std::string& sourceFileName, std::vector<uint32_t>& outSpv) noexcept
    {
        shaderc::Compiler compiler;
        shaderc::CompileOptions compileOpts;
        compileOpts.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_4);
        compileOpts.SetOptimizationLevel(shaderc_optimization_level_performance);

        // TODO(Adam): Implement shader caching.
        shaderc::SpvCompilationResult compResult = compiler.CompileGlslToSpv(
            source,
            shaderc_compute_shader,
            sourceFileName.c_str(),
            compileOpts
        );

        if (compResult.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            vkc::Error("Failed to compile shader '{}'! {}", sourceFileName, compResult.GetErrorMessage());
            return false;
        }

        // TODO(Adam): Reflection
        outSpv = std::vector<uint32_t>(compResult.cbegin(), compResult.cend());
        return true;
    }

    VkcShader::VkcShader(const ilm::Ref<VkcDevice>& device, const std::filesystem::path& path) noexcept
        : m_Name(path.stem().string())
    {
        std::string source;
        if (!ReadFile(path, source))
        {
            vkc::Error("Failed to create shader!");
            return;
        }

        {
            VkcTimer timer;

            // Time the shader compilation
            std::vector<uint32_t> spv;
            if (!CompileToSpv(source, m_Name, spv))
            {
            }

            // Timer.elapsed here
        }
    }

    VkcShader::VkcShader(const ilm::Ref<VkcDevice>& device, const std::string& name, std::span<uint8_t>& spv) noexcept
        : m_Name(name)
    {

    }

    VkcShader::~VkcShader() noexcept
    {
        Destroy();
    }

    void VkcShader::Destroy() noexcept
    {
        if (m_Shader)
        {
            vkDestroyShaderModule(m_Device->GetDevice(), m_Shader, nullptr);
            m_Shader = nullptr;
        }

        m_Device = nullptr;
    }

    ilm::Ref<VkcShader> VkcShader::Create(const ilm::Ref<VkcDevice>& device, const std::filesystem::path& path) noexcept
    {
        return ilm::Ref<VkcShader>();
    }

    ilm::Ref<VkcShader> VkcShader::Create(const ilm::Ref<VkcDevice>& device, const std::string& name, std::span<uint8_t>& spv) noexcept
    {
        return ilm::Ref<VkcShader>();
    }
}  // namespace vkc
