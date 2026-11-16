#include <VkCompute/Assert.hpp>
#include <VkCompute/Log.hpp>
#include <VkCompute/VkcBuffer.hpp>
#include <VkCompute/VkcBufferFactory.hpp>
#include <VkCompute/VkcDescriptorAllocator.hpp>
#include <VkCompute/VkcDescriptorBuilder.hpp>
#include <VkCompute/VkcDescriptorLayoutCache.hpp>
#include <VkCompute/VkcPipeline.hpp>
#include <VkCompute/VkcPipelineFactory.hpp>
#include <VkCompute/VkcShader.hpp>
#include <VkCompute/VkcInstance.hpp>
using namespace vkc;


VkcInstance g_Instance;

int main(int argc, char** argv)
{
    vkc::Info("Hello world from VkCompute!");

    g_Instance.Initialize("VkCompute-Demo");
    const auto& device = g_Instance.GetDevice();

    // Shader creation test
    ilm::Ref<VkcShader> demoShader = VkcShader::Create(device, "HelloWorld.glsl");
    vkc::Warn("Shader valid: {0}", demoShader->Valid());

    // Buffer creation
    ilm::Ref<VkcUniformBuffer> buffer = VkcUniformBufferFactory()
                                            .SetName("SampleBuffer")
                                            .SetDevice(device)
                                            .SetAllocator(g_Instance.GetAllocator())
                                            .SetMemoryUsage(VMA_MEMORY_USAGE_CPU_TO_GPU)
                                            .SetQueueFamilies({ *device->GetQueueFamilyIndices().Compute })
                                            .SetSize(48)
                                            .Create();

    if (!buffer)
        vkc::Error("FAILED TO CREATE BUFFER!");

    // Populate the buffer with some random data
    std::vector<unsigned char> v(48, 1);
    buffer->MapData((void*)v.data(), v.size());

    // Descriptor management objects
    ilm::Ref<VkcDescriptorAllocator> descriptorAllocator = VkcDescriptorAllocator::Create(device);
    ilm::Ref<VkcDescriptorLayoutCache> descriptorCache   = VkcDescriptorLayoutCache::Create(device);

    // Pipeline creation
    ilm::Ref<VkcComputePipeline> pipeline = VkcComputePipelineFactory()
                                                .SetName("Compute1")
                                                .SetShader(demoShader, "main")
                                                .SetDescriptorAllocator(descriptorAllocator)
                                                .SetDescriptorLayoutCache(descriptorCache)
                                                .SetInput(buffer, 0, 0)
                                                .Create();

    if (!pipeline)
        vkc::Error("FAILED TO CREATE COMPUTE PIPELINE!");

    pipeline            = nullptr;
    descriptorAllocator = nullptr;
    descriptorCache     = nullptr;
    buffer              = nullptr;
    demoShader          = nullptr;
    g_Instance.Shutdown();

    return 0;
}
