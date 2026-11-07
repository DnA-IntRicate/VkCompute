#include <VkCompute/Assert.hpp>
#include <VkCompute/Log.hpp>
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
    ilm::Ref<VkcDescriptorAllocator> dstAllocator     = VkcDescriptorAllocator::Create(device);
    ilm::Ref<VkcDescriptorLayoutCache> dstLayoutCache = VkcDescriptorLayoutCache::Create(device);
    ilm::Ref dstBuilder                               = VkcDescriptorBuilder::Create(dstAllocator, dstLayoutCache);

    // Shader creation test
    ilm::Ref<VkcShader> demoShader = VkcShader::Create(device, "ComputeDemo.glsl");
    vkc::Warn("Shader valid: {0}", demoShader->Valid());

    dstBuilder     = nullptr;
    dstLayoutCache = nullptr;
    dstAllocator   = nullptr;
    demoShader     = nullptr;
    g_Instance.Shutdown();

    return 0;
}
