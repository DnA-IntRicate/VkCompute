#pragma once
#include "VkcDescriptorAllocator.hpp"
#include "VkcDevice.hpp"
#include "VkcPipelineFactory.hpp"
#include "VkcShader.hpp"
#include <Ilm/Ilm.hpp>
#include <vulkan/vulkan_core.h>
#include <string>
#include <string_view>
#include <vector>


namespace vkc
{
    /**
     * @brief Represents a Vulkan pipeline resource.
     *
     * VkcPipeline manages the lifetime of a Vulkan pipeline and its associated
     * pipeline layout. It also maintains references to the shader, logical
     * device, descriptor allocator, and descriptor set allocations associated
     * with the pipeline.
     *
     * The pipeline kind is specified at compile time.
     *
     * @tparam Kind Pipeline kind.
     *
     * @see VkcPipelineFactory
     * @see VkcPipelineTraits
     */
    template <VkcPipelineKind Kind>
    class VkcPipeline
    {
    public:
        /** @brief Human-readable name identifying this type. */
        static constexpr std::string_view Name = "VkcPipeline";

        /** @brief Publicly-exposes the pipeline kind. */
        static constexpr VkcPipelineKind PipelineKind = Kind;

    public:
        /**
         * @brief Constructs an empty pipeline resource.
         *
         * Initializes the Vulkan handles and associated state to their
         * default, invalid values.
         *
         * @note Pipeline resources should generally be constructed through
         *       @ref VkcPipelineFactory rather than directly. This constructor
         *       should never be called directly!
         */
        constexpr VkcPipeline() = default;

        /**
         * @brief Destroys the pipeline resource.
         */
        ~VkcPipeline() noexcept;

        // Allow copy-assignment by ilm::Ref only
        constexpr VkcPipeline(const VkcPipeline&)            = delete;
        constexpr VkcPipeline& operator=(const VkcPipeline&) = delete;

        // Allow move-assignment by ilm::Ref only
        constexpr VkcPipeline(VkcPipeline&&)            = delete;
        constexpr VkcPipeline& operator=(VkcPipeline&&) = delete;

        /**
         * @brief Destroys the pipeline and frees all descriptor sets.
         */
        void Destroy() noexcept;

        /**
         * @brief Gets the debug name assigned to this pipeline.
         *
         * @return Constant reference to the pipeline's debug name.
         */
        constexpr const std::string& GetName() const noexcept { return m_DebugName; }

        /**
         * @brief Gets the logical device associated with this pipeline.
         *
         * @return A reference to the logical device used to create this pipeline.
         */
        constexpr ilm::Ref<VkcDevice> GetDevice() const noexcept { return m_Device; }

        /**
         * @brief Retrieves the shader associated with this pipeline.
         *
         * @return Reference to the shader used during pipeline creation.
         */
        constexpr ilm::Ref<VkcShader> GetShader() const noexcept { return m_Shader; }

        /**
         * @brief Retrieves all descriptor set allocations associated with the pipeline.
         *
         * Returns the descriptor set allocations indexed by descriptor set binding index.
         *
         * @return Constant reference to the descriptor set allocation collection.
         *
         * @note The returned reference remains valid only while this pipeline
         *       exists and its descriptor set allocations remain unchanged.
         */
        constexpr const std::vector<VkcDescriptorSetAllocation>& GetDescriptorSetAllocations() const noexcept
        {
            return m_DstSetAllocations;
        }

        /**
         * @brief Retrieves the underlying Vulkan pipeline handle.
         *
         * @return VkPipeline handle associated with this resource.
         */
        constexpr VkPipeline GetPipeline() const noexcept { return m_Pipeline; }

        /**
         * @brief Retrieves the Vulkan pipeline layout associated with this pipeline.
         *
         * @return VkPipelineLayout handle associated with this resource.
         */
        constexpr VkPipelineLayout GetPipelineLayout() const noexcept { return m_PipelineLayout; }

        /**
         * @brief Checks whether the pipeline resource has been successfully created.
         *
         * A pipeline is considered valid when both its Vulkan pipeline handle
         * and pipeline layout handle are non-null.
         *
         * @return true if both handles are valid, otherwise false.
         *
         * @note This check does not guarantee that the pipeline's shader,
         *       descriptor sets, or other associated resources are still valid,
         *       nor does it guarantee that the pipeline is safe to use on the GPU.
         */
        constexpr bool Valid() const noexcept { return (bool)m_Pipeline && (bool)m_PipelineLayout; }

    private:
        // Vulkan pipeline handle managed by this resource.
        VkPipeline m_Pipeline = nullptr;

        // Vulkan pipeline layout associated with this pipeline.
        VkPipelineLayout m_PipelineLayout = nullptr;

        // Debug name assigned to the pipeline.
        std::string m_DebugName;

        // Shader used to create the pipeline.
        ilm::Ref<VkcShader> m_Shader;

        // Logical device used to create the pipeline.
        ilm::Ref<VkcDevice> m_Device;

        // Descriptor allocator used to manage this pipeline's descriptor sets.
        ilm::Ref<VkcDescriptorAllocator> m_DstAllocator;

        // Descriptor set allocations associated with this pipeline.
        std::vector<VkcDescriptorSetAllocation> m_DstSetAllocations;

    private:
        // VkcPipelineFactory requires private access to this class.
        friend class VkcPipelineFactory<Kind>;
    };

    // Explicit instantiation declarations to avoid redundancies in including TUs
    // since this templated-class splits contains implementation in a source file.
    extern template class VkcPipeline<VkcPipelineKind::Compute>;
    extern template class VkcPipeline<VkcPipelineKind::Graphics>;

    /** @brief Convenience alias for a compute pipeline. */
    typedef VkcPipeline<VkcPipelineKind::Compute> VkcComputePipeline;

    /** @brief Convenience alias for a graphics pipeline. */
    typedef VkcPipeline<VkcPipelineKind::Graphics> VkcGraphicsPipeline;
}  // namespace vkc
