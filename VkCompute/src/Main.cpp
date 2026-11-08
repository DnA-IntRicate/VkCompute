#include <VkCompute/Assert.hpp>
#include <VkCompute/Log.hpp>
#include <VkCompute/VkcBuffer.hpp>
#include <VkCompute/VkcDescriptorAllocator.hpp>
#include <VkCompute/VkcDescriptorBuilder.hpp>
#include <VkCompute/VkcDescriptorLayoutCache.hpp>
#include <VkCompute/VkcShader.hpp>
#include <VkCompute/VkcInstance.hpp>
using namespace vkc;


VkcInstance g_Instance;

int main(int argc, char** argv)
{
    vkc::Info("Hello world from VkCompute!");

    g_Instance.Initialize("VkCompute-Demo");
    const auto& device = g_Instance.GetDevice();

    // Test creating a new dst set builder
    ilm::Ref dstAllocator   = VkcDescriptorAllocator::Create(device);
    ilm::Ref dstLayoutCache = VkcDescriptorLayoutCache::Create(device);

    // Shader creation test
    ilm::Ref demoShader = VkcShader::Create(device, "ComputeDemo.glsl");
    vkc::Warn("Shader valid: {0}", demoShader->Valid());

    // The buffer creation pattern we want
    VkcBuffer buffer;
    if (!VkcBufferFactory()
             .SetName("SampleBuffer")
             .SetAllocator(g_Instance.GetAllocator())
             .SetBufferKind(VkcBufferKind::StorageBuffer)
             .SetDevice(device)
             .SetMemoryUsage(VMA_MEMORY_USAGE_AUTO)
             .SetDescriptorAllocator(dstAllocator)
             .SetDescriptorLayoutCache(dstLayoutCache)
             .SetBinding(0)
             .SetOffset(0)
             .SetSize(48)
             .Create(buffer))
    {
        vkc::Error("FAILED TO CREATE BUFFER!");
    }

    dstLayoutCache = nullptr;
    dstAllocator   = nullptr;
    demoShader     = nullptr;
    g_Instance.Shutdown();

    return 0;
}
