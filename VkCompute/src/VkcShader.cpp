#include <VkCompute/VkcShader.hpp>
#include <VkCompute/Log.hpp>
#include <VkCompute/VkcTimer.hpp>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <Volk/volk.h>
#include <chrono>
#include <fstream>
#include <string_view>
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

    static inline void Reflect(std::string_view name, uint64_t compileTimeMs, const std::vector<uint32_t>& spv) noexcept
    {
        spirv_cross::Compiler compiler(spv);
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        vkc::Info("Compiled shader '{0}' of type VK_SHADER_STAGE_COMPUTE_BIT in {1}ms.", name, compileTimeMs);
        vkc::Trace("    {0} uniform buffers", resources.uniform_buffers.size());
        vkc::Trace("    {0} push constant buffers", resources.push_constant_buffers.size());
        vkc::Trace("    {0} storage buffers", resources.storage_buffers.size());
        vkc::Trace("    {0} resources", resources.sampled_images.size());

        const auto reflectBuffers = [&compiler](std::string_view header, const spirv_cross::SmallVector<spirv_cross::Resource>& resources)
        {
            vkc::Trace("    {0}:", header);
            for (const auto& resource : resources)
            {
                const auto& bufferType = compiler.get_type(resource.base_type_id);
                size_t bufferSize      = compiler.get_declared_struct_size(bufferType);
                uint32_t binding       = compiler.get_decoration(resource.id, ::spv::Decoration::DecorationBinding);
                size_t memberCount     = bufferType.member_types.size();

                vkc::Trace("      {0}", resource.name);
                vkc::Trace("        Size = {0}", bufferSize);
                vkc::Trace("        Binding = {0}", binding);
                vkc::Trace("        Members = {0}", memberCount);
            }
        };

        reflectBuffers("Uniform buffers", resources.uniform_buffers);
        reflectBuffers("Push constant buffers", resources.push_constant_buffers);
        reflectBuffers("Storage buffers", resources.storage_buffers);
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

        outSpv = std::vector<uint32_t>(compResult.cbegin(), compResult.cend());
        return true;
    }

    static inline bool CreateShader(const ilm::Ref<VkcDevice>& device, const std::vector<uint32_t>& spv, VkShaderModule& outShader) noexcept
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.pNext    = nullptr;
        createInfo.flags    = 0;
        createInfo.codeSize = spv.size() * sizeof(uint32_t);
        createInfo.pCode    = spv.data();

        return vkCreateShaderModule(device->GetDevice(), &createInfo, nullptr, &outShader) == VK_SUCCESS;
    }

    VkcShader::VkcShader(const ilm::Ref<VkcDevice>& device, const std::filesystem::path& path) noexcept
        : m_Device(device), m_Name(path.stem().string())
    {
        // Time shader load, compilation and creation
        VkcTimer<std::chrono::milliseconds> timer;

        std::string source;
        if (!ReadFile(path, source))
        {
            vkc::Error("Failed to create shader!");
            return;
        }

        std::vector<uint32_t> spv;
        if (!CompileToSpv(source, m_Name, spv))
        {
            vkc::Error("Failed to create shader!");
            return;
        }

        if (!CreateShader(m_Device, spv, m_Shader))
        {
            vkc::Error("Failed to create '{0}' shader module!", m_Name);
            return;
        }

        Reflect(m_Name, timer.Elapsed(), spv);
    }

    VkcShader::VkcShader(const ilm::Ref<VkcDevice>& device, const std::string& name, std::span<uint8_t>& spv) noexcept
        : m_Device(device), m_Name(name)
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
        return ilm::CreateRef<VkcShader>(device, path);
    }

    ilm::Ref<VkcShader> VkcShader::Create(const ilm::Ref<VkcDevice>& device, const std::string& name, std::span<uint8_t>& spv) noexcept
    {
        return ilm::CreateRef<VkcShader>(device, name, spv);
    }
}  // namespace vkc
