#include <VkCompute/VkcPipelineFactory.hpp>
#include <VkCompute/Log.hpp>
#include <VkCompute/VkcDescriptorBuilder.hpp>
#include <VkCompute/VkcPipeline.hpp>
#include <Volk/volk.h>
#include <ranges>


namespace vkc
{
    template class VkcPipelineFactory<VkcPipelineKind::Compute>;
    template class VkcPipelineFactory<VkcPipelineKind::Graphics>;

    template <VkcPipelineKind Kind>
    ilm::Ref<VkcPipeline<Kind>> VkcPipelineFactory<Kind>::Create() const noexcept
    {
        // Graphics is not currently supported!
        if constexpr (Kind == VkcPipelineKind::Graphics)
        {
            vkc::Critical("Graphics pipelines are not currently supported!");
            return nullptr;
        }

        //
        // VkcPipeline object creation
        //

        ilm::Ref<VkcPipeline<Kind>> pipeline = ilm::CreateRef<VkcPipeline<Kind>>();
        pipeline->m_DebugName                = m_DebugName;
        pipeline->m_Shader                   = m_Shader;
        pipeline->m_Device                   = m_Shader->GetDevice();
        pipeline->m_DstAllocator             = m_DstAllocator;

        VkPipelineLayout& pipelineLayout = pipeline->m_PipelineLayout;
        VkPipeline& pipelineHandle       = pipeline->m_Pipeline;

        //
        // Descriptor set creation
        //

        std::vector<VkcDescriptorSetAllocation>& dstSetAllocations = pipeline->m_DstSetAllocations;
        dstSetAllocations.resize(m_Inputs.size());

        std::vector<VkDescriptorSetLayout> dstSetLayouts(m_Inputs.size());
        for (const auto& [binding, input] : std::views::enumerate(m_Inputs))
        {
            VkcDescriptorBuilder dstBuilder(m_DstAllocator, m_DstCache);
            for (const auto& [resource, offset] : input)
                dstBuilder.BindBuffer(binding, offset, resource);

            VkcDescriptorSetAllocation dstAlloc = dstBuilder.Build();
            dstSetAllocations[binding]          = dstAlloc;
            dstSetLayouts[binding]              = dstAlloc.Layout;
        }

        //
        // Pipeline layout creation
        //

        VkPipelineLayoutCreateInfo layoutInfo{};
        layoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutInfo.pNext                  = nullptr;
        layoutInfo.flags                  = 0;
        layoutInfo.setLayoutCount         = static_cast<uint32_t>(dstSetLayouts.size());
        layoutInfo.pSetLayouts            = dstSetLayouts.data();
        layoutInfo.pushConstantRangeCount = 0;
        layoutInfo.pPushConstantRanges    = nullptr;

        const auto& device = m_Shader->GetDevice()->GetDevice();
        if (vkCreatePipelineLayout(device, &layoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            vkc::Error("Failed to create pipeline layout for pipeline '{0}'!", m_DebugName);
            return nullptr;
        }

        //
        // Pipeline creation
        //

        VkPipelineShaderStageCreateInfo shaderStageInfo{};
        shaderStageInfo.sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageInfo.pNext               = nullptr;
        shaderStageInfo.flags               = 0;
        shaderStageInfo.stage               = VK_SHADER_STAGE_COMPUTE_BIT;
        shaderStageInfo.module              = m_Shader->GetShaderModule();
        shaderStageInfo.pName               = m_EntryPoint.data();
        shaderStageInfo.pSpecializationInfo = nullptr;

        VkComputePipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType              = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext              = nullptr;
        pipelineInfo.flags              = 0;
        pipelineInfo.stage              = shaderStageInfo;
        pipelineInfo.layout             = pipelineLayout;
        pipelineInfo.basePipelineHandle = nullptr;
        pipelineInfo.basePipelineIndex  = 0;

        // TODO(Adam): Use VkPipelineCache?
        if (vkCreateComputePipelines(device, nullptr, 1, &pipelineInfo, nullptr, &pipelineHandle) != VK_SUCCESS)
        {
            vkc::Error("Failed to create pipeline '{0}'!", m_DebugName);
            return nullptr;
        }

        vkc::Success(
            "Created compute pipeline (name = '{0}', shader = '{1}', descriptor sets = {2}).",
            m_DebugName,
            m_Shader->GetName(),
            dstSetAllocations.size()
        );

        return pipeline;
    }
}  // namespace vkc
