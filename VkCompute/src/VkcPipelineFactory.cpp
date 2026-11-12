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
        // Descriptor set creation
        // NOTE(Adam): The dst sets must be stored somewhere so that they can be updated/freed
        // It should probably belong to the pipeline!

        const auto createDstSet = [](const std::vector<VkcPipelineInput>& setInputs)
            {
                return nullptr;
            };

        std::vector<VkDescriptorSetLayout> dstSetLayouts;
        dstSetLayouts.resize(m_Inputs.size());

        for (const auto& [binding, input] : std::views::enumerate(m_Inputs))
        {
            for (const auto& [resource, offset] : input)
            {
                // We need to fucking rework the fucking descriptor builder.

                // Bind into the dst build here
                // m_DstBuilder->BindBuffer(binding)
            }

            // Build here
        }


        //
        // Pipeline layout creation
        //

        VkPipelineLayoutCreateInfo layoutInfo{};
        layoutInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutInfo.pNext          = nullptr;
        layoutInfo.flags          = 0;
        layoutInfo.setLayoutCount = 0;  // THIS WE NEED TO SET!!!!!!!!!!!!
        const VkDescriptorSetLayout* pSetLayouts;
        layoutInfo.pushConstantRangeCount = 0;
        const VkPushConstantRange* pPushConstantRanges;

        // vkCreatePipelineLayout


        return ilm::Ref<VkcPipeline<Kind>>();
    }
}  // namespace vkc
