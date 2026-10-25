#include <VkCompute/Assert.hpp>
#include <VkCompute/Log.hpp>
#include <VkCompute/VkcDescriptorAllocator.hpp>
#include <VkCompute/VkcDescriptorBuilder.hpp>
#include <VkCompute/VkcDescriptorLayoutCache.hpp>
#include <VkCompute/VkcInstance.hpp>
using namespace vkc;


VkcInstance g_Instance;
ilm::Ref<VkcDescriptorAllocator> g_DstAllocator;
ilm::Ref<VkcDescriptorLayoutCache> g_DstLayoutCache;

int main(int argc, char** argv)
{
    vkc::Info("Hello world from VkCompute!");

    g_Instance.Initialize("VkCompute-Demo");
    g_DstAllocator   = VkcDescriptorAllocator::Create(g_Instance.GetDevice());
    g_DstLayoutCache = VkcDescriptorLayoutCache::Create(g_Instance.GetDevice());

    // Test creating a new dst set builder
    ilm::Ref dstBuilder = VkcDescriptorBuilder::Create(g_DstAllocator, g_DstLayoutCache);
    (void)dstBuilder;

    g_Instance.Shutdown();
    return 0;
}
