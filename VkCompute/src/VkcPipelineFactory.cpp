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

    // TODO: This must be specialized for either pipeline!
    // Because of the separate creation structs for Graphics & Compute pipelines

    template <VkcPipelineKind Kind>
    ilm::Ref<VkcPipeline<Kind>> VkcPipelineFactory<Kind>::Create() const noexcept
    {
        //
        // Descriptor set creation
        //

        std::vector<VkcDescriptorSetAllocation> dstSetAllocations(m_Inputs.size());
        std::vector<VkDescriptorSetLayout> dstSetLayouts(m_Inputs.size());

        for (const auto& [binding, input] : std::views::enumerate(m_Inputs))
        {
            VkcDescriptorBuilder dstBuilder(m_DstAllocator, m_DstCache);
            for (const auto& [resource, offset] : input)
                dstBuilder.BindBuffer(binding, offset, resource);

            VkcDescriptorSetAllocation dstAlloc = dstBuilder.Build();
            dstSetAllocations[binding] = dstAlloc;
            dstSetLayouts[binding] = dstAlloc.Layout;
        }

        // NOTE(Adam): The dst sets must be stored somewhere so that they can be updated/freed
        // It should probably belong to the pipeline!
        ilm::Ref<VkcPipeline<Kind>> pipeline = ilm::CreateRef<VkcPipeline<Kind>>();
        pipeline->m_DebugName                = m_DebugName;
        // TODO: Assign dsts to pipeline here

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
        if (vkCreatePipelineLayout(device, &layoutInfo, nullptr, &pipeline->m_PipelineLayout) != VK_SUCCESS)
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
        pipelineInfo.layout             = pipeline->m_PipelineLayout;
        pipelineInfo.basePipelineHandle = nullptr;
        pipelineInfo.basePipelineIndex  = 0;

        // TODO(Adam): Use VkPipelineCache?
        if (vkCreateComputePipelines(device, nullptr, 1, &pipelineInfo, nullptr, &pipeline->m_Pipeline) != VK_SUCCESS)
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
