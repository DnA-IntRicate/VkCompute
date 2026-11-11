#pragma once
#include "VkcBufferFactory.hpp"
#include "VkcDevice.hpp"
#include <Ilm/Ilm.hpp>
#include <vulkan/vulkan_core.h>
#include <string_view>
#include <vector>


namespace vkc
{
    template <VkcBufferKind Kind>
    class VkcBuffer
    {
    public:
        static constexpr std::string_view Name = "VkcBuffer";

    public:
        constexpr VkcBuffer() = default;

        // Dtors and everything now...


    private:
        VkBuffer m_Buffer          = nullptr;
        VkDescriptorSet m_DstSet   = nullptr;
        VmaAllocation m_Allocation = nullptr;
        VmaAllocationInfo m_AllocationInfo{};

        std::string m_DebugName;

    private:
        friend class VkcBufferFactory<Kind>;
    };

    typedef VkcBuffer<VkcBufferKind::StorageBuffer> VkcStorageBuffer;
    typedef VkcBuffer<VkcBufferKind::UniformBuffer> VkcUniformBuffer;
}  // namespace vkc
