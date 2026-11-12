#pragma once
#include "VkcBuffer.hpp"
#include "VkcDescriptorBuilder.hpp"
#include "VkcShader.hpp"
#include <Ilm/Ilm.hpp>
#include <vulkan/vulkan_core.h>
#include <string>
#include <string_view>
#include <variant>
#include <vector>


namespace vkc
{
    enum class VkcPipelineKind
    {
        None = 0,
        Compute,
        Graphics
    };

    template <VkcPipelineKind Kind>
    struct VkcPipelineTraits
    {
    };

    template <>
    struct VkcPipelineTraits<VkcPipelineKind::Compute>
    {
        /** @brief Human-readable name identifying this pipeline kind. */
        static constexpr std::string_view KindName = "Compute";

        /** @brief The Vulkan pipeline bind-point. */
        static constexpr VkPipelineBindPoint BindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
    };

    template <>
    struct VkcPipelineTraits<VkcPipelineKind::Graphics>
    {
        /** @brief Human-readable name identifying this pipeline kind. */
        static constexpr std::string_view KindName = "Graphics";

        /** @brief The Vulkan pipeline bind-point. */
        static constexpr VkPipelineBindPoint BindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    };

    struct VkcPipelineInput
    {
        /** @brief A variant over the possible shader resource inputs for a pipeline. */
        using ResourceType = std::variant<ilm::Ref<VkcStorageBuffer>, ilm::Ref<VkcUniformBuffer>>;

        constexpr VkcPipelineInput(const ResourceType& res, size_t offset) noexcept
            : Resource(res), Offset(offset)
        {
        }

        constexpr VkcPipelineInput() = default;

        ResourceType Resource;
        // size_t Binding = 0;
        size_t Offset  = 0;
    };

    template <VkcPipelineKind Kind>
    class VkcPipeline;

    template <VkcPipelineKind Kind>
    class VkcPipelineFactory
    {
    public:
        static_assert(Kind != VkcPipelineKind::None, "VkcPipelineKind cannot be None!");

        /** @brief Traits describing the Vulkan properties of the selected pipeline kind. */
        using Traits = VkcPipelineTraits<Kind>;

    public:
        constexpr VkcPipelineFactory() = default;
        ~VkcPipelineFactory()          = default;

        /**
         * @brief Sets the debug name assigned to the pipeline.
         *
         * @param name Human-readable name used for pipeline identification and debugging.
         *
         * @return Reference to this factory for method chaining.
         */
        constexpr VkcPipelineFactory& SetName(const std::string& name) noexcept
        {
            m_DebugName = name;
            return *this;
        }

        constexpr VkcPipelineFactory& SetShader(const ilm::Ref<VkcShader>& shader, const std::string& entryPoint = "main") noexcept
        {
            m_Shader     = shader;
            m_EntryPoint = entryPoint;
            return *this;
        }

        // TODO(Adam): I don't like the binding + offset being defined in the buffer
        // I think it should rather be defined HERE during pipeline creation!
        template <VkcBufferKind Kind>
        constexpr VkcPipelineFactory& SetInput(const ilm::Ref<VkcBuffer<Kind>>& input, size_t binding, size_t offset = 0) noexcept
        {
            SetInputImpl(input, binding, offset);
            return *this;
        }

        // constexpr VkcPipelineFactory& SetInput(const ilm::Ref<VkcUniformBuffer>& input, size_t binding, size_t offset = 0) noexcept
        // {
        //     SetInputImpl(input, binding, offset);
        //     return *this;
        // }

        // TODO: Add support for push constants!

        ilm::Ref<VkcPipeline<Kind>> Create() const noexcept;

    private:
        constexpr void SetInputImpl(const VkcPipelineInput::ResourceType& res, size_t binding, size_t offset) noexcept
        {
            if (m_Inputs.size() >= binding)
                m_Inputs.resize(binding + 1);

            auto& slot = m_Inputs[binding];
            slot.emplace_back(res, offset);
        }

    private:
        std::string m_DebugName;

        ilm::Ref<VkcShader> m_Shader;
        std::string m_EntryPoint = "main";

        ilm::Ref<VkcDescriptorBuilder> m_DstBuilder;

        // Maps bindings dst set bindings to their inputs
        std::vector<std::vector<VkcPipelineInput>> m_Inputs;
    };

    // Explicit instantiation declarations to avoid redundancies in including TUs
    // since this templated-class contains implementation in a source file.
    extern template class VkcPipelineFactory<VkcPipelineKind::Compute>;
    extern template class VkcPipelineFactory<VkcPipelineKind::Graphics>;

    /** @brief Convenience alias for a compute pipeline factory. */
    typedef VkcPipelineFactory<VkcPipelineKind::Compute> VkcComputePipelineFactory;

    /** @brief Convenience alias for a graphics pipeline factory. */
    typedef VkcPipelineFactory<VkcPipelineKind::Graphics> VkcGraphicsPipelineFactory;

}  // namespace vkc
