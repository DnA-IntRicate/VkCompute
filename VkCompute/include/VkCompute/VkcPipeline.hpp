#pragma once
#include "VkcDescriptorAllocator.hpp"
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
        constexpr VkcPipeline() = default;
        ~VkcPipeline() noexcept;

        /**
         * @brief Destroys this pipeline. 
         */
        void Destroy() noexcept;

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
