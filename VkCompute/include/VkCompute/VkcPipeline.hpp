#pragma once
#include "VkcBuffer.hpp"
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

    private:
        VkPipeline m_Pipeline             = nullptr;
        VkPipelineLayout m_PipelineLayout = nullptr;

        ilm::Ref<VkcShader> m_Shader;

    private:
        friend class VkcPipelineFactory<Kind>;
    };

}  // namespace vkc
