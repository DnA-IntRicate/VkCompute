#pragma once
#include "VkcDescriptorAllocator.hpp"
#include "VkcDevice.hpp"
#include "VkcPipelineFactory.hpp"
#include "VkcShader.hpp"
#include <Ilm/Ilm.hpp>
#include <vulkan/vulkan_core.h>
#include <string>
#include <string_view>
#include <variant>
#include <vector>


namespace vkc
{
    template <VkcPipelineKind Kind>
    class VkcPipeline
    {
    public:
        static constexpr std::string_view Name = "VkcPipeline";

        static constexpr VkcPipelineKind PipelineKind = Kind;

    public:
        constexpr VkcPipeline() = default;
        ~VkcPipeline() noexcept;

        // Allow copy-assignment by ilm::Ref only
        constexpr VkcPipeline(const VkcPipeline&)          = delete;
        constexpr VkcPipeline& operator=(const VkcPipeline&) = delete;

        // Allow move-assignment by ilm::Ref only
        constexpr VkcPipeline(VkcPipeline&&)        = delete;
        constexpr VkcPipeline& operator=(VkcPipeline&&) = delete;

        /**
         * @brief Destroys this pipeline. 
         */
        void Destroy() noexcept;

        /**
         * @brief Gets the logical device associated with this pipeline.
         *
         * @return A reference to the logical device used to create this pipeline.
         */
        constexpr ilm::Ref<VkcDevice> GetDevice() const noexcept { return m_Device; }

        /**
         * @brief Gets the debug name assigned to this pipeline.
         *
         * @return Constant reference to the pipeline's debug name.
         */
        constexpr const std::string& GetName() const noexcept { return m_DebugName; }

        /** @brief */
        constexpr bool Valid() const noexcept { return (bool)m_Pipeline && (bool)m_PipelineLayout; }

    private:
        VkPipeline m_Pipeline             = nullptr;
        VkPipelineLayout m_PipelineLayout = nullptr;

        std::string m_DebugName;

        ilm::Ref<VkcShader> m_Shader;
        ilm::Ref<VkcDevice> m_Device;

        ilm::Ref<VkcDescriptorAllocator> m_DstAllocator;
        std::vector<VkcDescriptorSetAllocation> m_DstSetAllocations;

    private:
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
