#include <VkCompute/VkcPipeline.hpp>
#include <VkCompute/Log.hpp>
#include <Volk/volk.h>
#include <ranges>


namespace vkc
{
    template class VkcPipeline<VkcPipelineKind::Compute>;
    template class VkcPipeline<VkcPipelineKind::Graphics>;

    template <VkcPipelineKind Kind>
    VkcPipeline<Kind>::~VkcPipeline() noexcept
    {
        Destroy();
    }

    template <VkcPipelineKind Kind>
    void VkcPipeline<Kind>::Destroy() noexcept
    {
        if (m_Pipeline)
        {
            vkDestroyPipeline(m_Device->GetDevice(), m_Pipeline, nullptr);
            m_Pipeline = nullptr;
        }

        if (m_PipelineLayout)
        {
            vkDestroyPipelineLayout(m_Device->GetDevice(), m_PipelineLayout, nullptr);
            m_PipelineLayout = nullptr;
        }

        for (const auto& [binding, dstAlloc] : std::views::enumerate(m_DstSetAllocations))
        {
            if (!m_DstAllocator->FreeSet(dstAlloc))
                vkc::Error(
                    "Failed to free descriptor set binding {0} for pipeline (name = '{1}', shader = '{2}', descriptor sets = {3}).",
                    binding,
                    m_DebugName,
                    m_Shader->GetName(),
                    m_DstSetAllocations.size()
                );
        }

        m_DstSetAllocations.clear();
        m_DstAllocator = nullptr;
        m_Device       = nullptr;
        m_Shader       = nullptr;

        vkc::Warn("Destroyed pipeline (name = '{0}').", m_DebugName);
    }
}  // namespace vkc
