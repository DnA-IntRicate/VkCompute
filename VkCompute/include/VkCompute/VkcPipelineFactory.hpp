#pragma once
#include "VkcBuffer.hpp"
#include "VkcDescriptorAllocator.hpp"
#include "VkcDescriptorLayoutCache.hpp"
#include "VkcShader.hpp"
#include <Ilm/Ilm.hpp>
#include <vulkan/vulkan_core.h>
#include <optional>
#include <string>
#include <string_view>
#include <vector>


namespace vkc
{
    /**
     * @brief Specifies the type of Vulkan pipeline.
     *
     * Determines how a @ref VkcPipeline is created.
     */
    enum class VkcPipelineKind
    {
        /** @brief No pipeline kind specified. Not a valid kind for VkcPipeline or VkcPipelineFactory. */
        None = 0,

        /** @brief Compute pipeline. */
        Compute,

        /** @brief Graphics pipeline. */
        Graphics
    };

    template <VkcPipelineKind Kind>
    class VkcPipeline;

    /**
     * @brief Provides compile-time Vulkan properties for a pipeline kind.
     *
     * This traits template is specialized for each supported @ref VkcPipelineKind.
     *
     * @tparam Kind The pipeline kind for which traits are defined.
     */
    template <VkcPipelineKind Kind>
    struct VkcPipelineTraits
    {
    };

    /**
     * @brief Traits specialization for Vulkan compute pipelines.
     *
     * Provides the Vulkan properties required to create and describe a compute pipeline.
     */
    template <>
    struct VkcPipelineTraits<VkcPipelineKind::Compute>
    {
        /** @brief Human-readable name identifying this pipeline kind. */
        static constexpr std::string_view KindName = "Compute";

        /** @brief The Vulkan pipeline bind-point. */
        static constexpr VkPipelineBindPoint BindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;

        /** @brief The Vulkan shader stage flags. */
        static constexpr VkShaderStageFlags StageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    };

    /**
     * @brief Traits specialization for Vulkan graphics pipelines.
     *
     * Provides the Vulkan properties required to create and describe a graphics pipeline.
     */
    template <>
    struct VkcPipelineTraits<VkcPipelineKind::Graphics>
    {
        /** @brief Human-readable name identifying this pipeline kind. */
        static constexpr std::string_view KindName = "Graphics";

        /** @brief The Vulkan pipeline bind-point. */
        static constexpr VkPipelineBindPoint BindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        /** @brief The Vulkan shader stage flags. */
        static constexpr VkShaderStageFlags StageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    };

    /**
     * @brief Describes a buffer resource bound as an input to a pipeline.
     *
     * Stores a reference to a descriptor buffer resource and the byte offset
     * from which the resource is accessed.
     */
    struct VkcPipelineInput
    {
        /**
         * @brief Constructs a pipeline input from a descriptor buffer resource.
         *
         * @param res Descriptor buffer resource to bind.
         * @param offset Byte offset into the descriptor set.
         */
        constexpr VkcPipelineInput(const VkcDescriptorBufferResource& res, size_t offset) noexcept
            : Resource(res), Offset(offset)
        {
        }

        /**
         * @brief Constructs an empty pipeline input.
         */
        constexpr VkcPipelineInput() = default;

        /** @brief Descriptor buffer resource associated with this input. */
        VkcDescriptorBufferResource Resource;

        /** @brief Byte offset into the descriptor set. */
        size_t Offset = 0;
    };

    /**
     * @brief Factory for configuring and creating Vulkan pipelines.
     *
     * VkcPipelineFactory provides a builder-style interface for configuring
     * graphics and compute pieplines.
     *
     * Configuration is performed through chained setter calls, followed by
     * a call to @ref Create.
     *
     * @tparam Kind Pipeline kind to create. Must not be VkcPipelineKind::None.
     *
     * @note VkcPipelineKind::None is explicitly rejected at compile time.
     *
     * @warning The factory does not independently validate that all required
     *          configuration has been supplied.
     *
     * @see VkcPipeline
     * @see VkcPipelineTraits
     */
    template <VkcPipelineKind Kind>
    class VkcPipelineFactory
    {
    public:
        static_assert(Kind != VkcPipelineKind::None, "VkcPipelineKind cannot be None!");

        /** @brief Traits describing the Vulkan properties of the selected pipeline kind. */
        using Traits = VkcPipelineTraits<Kind>;

    public:
        /**
         * @brief Constructs an empty pipeline factory.
         */
        constexpr VkcPipelineFactory() = default;

        /**
         * @brief Destroys the factory.
         */
        ~VkcPipelineFactory() = default;

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

        /**
         * @brief Sets the shader and entry point used by the pipeline.
         *
         * @param shader Reference to the shader used during pipeline creation.
         * @param entryPoint Shader entry point to use. Defaults to "main".
         *
         * @return Reference to this factory for method chaining.
         */
        constexpr VkcPipelineFactory& SetShader(const ilm::Ref<VkcShader>& shader, const std::string& entryPoint = "main") noexcept
        {
            m_Shader     = shader;
            m_EntryPoint = entryPoint;
            return *this;
        }

        /**
         * @brief Sets the descriptor allocator used to allocate pipeline descriptor sets.
         *
         * @param alloc Reference to the descriptor allocator.
         *
         * @return Reference to this factory for method chaining.
         */
        constexpr VkcPipelineFactory& SetDescriptorAllocator(const ilm::Ref<VkcDescriptorAllocator>& alloc) noexcept
        {
            m_DstAllocator = alloc;
            return *this;
        }

        /**
         * @brief Sets the descriptor layout cache used to retrieve or create descriptor layouts.
         *
         * @param cache Reference to the descriptor layout cache.
         *
         * @return Reference to this factory for method chaining.
         */
        constexpr VkcPipelineFactory& SetDescriptorLayoutCache(const ilm::Ref<VkcDescriptorLayoutCache>& cache) noexcept
        {
            m_DstCache = cache;
            return *this;
        }

        /**
         * @brief Configures the push-constant range associated with the pipeline.
         *
         * Specifies the size and byte offset of the push-constant range exposed
         * to shader stages through this pipeline's layout.
         *
         * @param size Size of the push-constant range in bytes.
         * @param offset Byte offset into the push-constant block. Defaults to zero.
         *
         * @return Reference to this factory for method chaining.
         *
         * @note The specified range must satisfy Vulkan's push-constant alignment
         *       and device-limit requirements.
         */
        constexpr VkcPipelineFactory& SetPushConstantRange(uint32_t size, uint32_t offset = 0) noexcept
        {
            m_PcSize   = size;
            m_PcOffset = offset;
            return *this;
        }

        /**
         * @brief Binds a buffer resource to a pipeline descriptor binding.
         *
         * Associates the specified buffer with a descriptor binding and
         * optionally specifies a byte offset into the descriptor set.
         *
         * @param input Reference to the buffer resource to bind.
         * @param binding Descriptor binding index to associate with the buffer.
         * @param offset Byte offset into the descriptor set. Defaults to 0.
         *
         * @return Reference to this factory for method chaining.
         */
        constexpr VkcPipelineFactory& SetInput(const VkcDescriptorBufferResource& input, size_t binding, size_t offset = 0) noexcept
        {
            if (m_Inputs.size() <= binding)
                m_Inputs.resize(binding + 1);

            auto& slot = m_Inputs[binding];
            slot.emplace_back(input, offset);

            return *this;
        }

        /**
         * @brief Creates a @ref VkcPipeline using the configured parameters.
         *
         * Constructs a pipeline of the kind specified by the factory's template
         * parameter and returns an @ref ilm::Ref to the resulting object.
         *
         * @return Reference to the created pipeline, or nullptr if creation fails.
         *
         * @warning All required configuration must be valid before calling this method.
         *
         * @note The factory configuration is not modified by this operation.
         */
        ilm::Ref<VkcPipeline<Kind>> Create() const noexcept;

    private:
        // Debug name assigned to the buffer being created.
        std::string m_DebugName;

        // Shader used to create the pipeline.
        ilm::Ref<VkcShader> m_Shader;

        // Shader entry point.
        std::string m_EntryPoint = "main";

        // Descriptor allocator used for creating the pipeline's descriptor sets.
        ilm::Ref<VkcDescriptorAllocator> m_DstAllocator;

        // Descriptor layout cache used for creating the pipeline's descriptor sets.
        ilm::Ref<VkcDescriptorLayoutCache> m_DstCache;

        // Maps descriptor binding indices to their associated inputs.
        std::vector<std::vector<VkcPipelineInput>> m_Inputs;

        // Push constant buffer size (optional).
        std::optional<uint32_t> m_PcSize = std::nullopt;

        // Push constant buffer offset (optional).
        uint32_t m_PcOffset = 0;
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
