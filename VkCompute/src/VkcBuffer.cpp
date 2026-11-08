#include <VkCompute/VkcBuffer.hpp>
#include <VkCompute/Assert.hpp>
#include <VkCompute/Log.hpp>
#include <VkCompute/VkcDescriptorBuilder.hpp>


namespace vkc
{
    static constexpr VkBufferUsageFlags BufferKindToUsageFlags(VkcBufferKind kind) noexcept
    {
        switch (kind)
        {
            case VkcBufferKind::StorageBuffer: return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            case VkcBufferKind::UniformBuffer: return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        }

        VKC_ASSERT(false, "VkcBufferKind was None!");
        return 0;
    }

    static constexpr VkDescriptorType BufferKindToDescriptorType(VkcBufferKind kind) noexcept
    {
        switch (kind)
        {
            case VkcBufferKind::StorageBuffer: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            case VkcBufferKind::UniformBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }

        VKC_ASSERT(false, "VkcBufferKind was None!");
        return (VkDescriptorType)0;
    }

    static constexpr VkDeviceSize GetMinOffsetAlignment(const ilm::Ref<VkcDevice>& device, VkcBufferKind kind) noexcept
    {
        const auto& limits = device->GetPhysicalDevice().Props.limits;
        switch (kind)
        {
            case VkcBufferKind::StorageBuffer: return limits.minStorageBufferOffsetAlignment;
            case VkcBufferKind::UniformBuffer: return limits.minUniformBufferOffsetAlignment;
        }

        VKC_ASSERT(false, "VkcBufferKind was None!");
        return 0;
    }

    // TODO(Adam): Return an ilm::Ref to a buffer instead!
    bool VkcBufferFactory::Create(VkcBuffer& outBuffer) const noexcept
    {
        //
        // Buffer creation
        //

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.pNext                 = nullptr;
        bufferInfo.flags                 = 0;
        bufferInfo.size                  = m_Size;
        bufferInfo.usage                 = BufferKindToUsageFlags(m_Kind);
        bufferInfo.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
        bufferInfo.queueFamilyIndexCount = m_QueueFamilyIndexCount;
        bufferInfo.pQueueFamilyIndices   = m_QueueFamilyIndices;

        //
        // Buffer + allocation creation
        //

        VmaAllocationCreateInfo allocCreateInfo{};
        allocCreateInfo.flags          = 0;
        allocCreateInfo.usage          = m_Usage;
        allocCreateInfo.requiredFlags  = 0;
        allocCreateInfo.preferredFlags = 0;
        allocCreateInfo.memoryTypeBits = 0;
        allocCreateInfo.pool           = nullptr;
        allocCreateInfo.pUserData      = nullptr;
        allocCreateInfo.priority       = 0.0f;

        if (vmaCreateBufferWithAlignment(
                m_Allocator,
                &bufferInfo,
                &allocCreateInfo,
                GetMinOffsetAlignment(m_Device, m_Kind),
                &outBuffer.m_Buffer,
                &outBuffer.m_Allocation,
                &outBuffer.m_AllocationInfo
            )
            != VK_SUCCESS)
        {
            vkc::Error("Failed to create buffer '{0}'!", m_DebugName);
            return false;
        }

        outBuffer.m_DebugName = m_DebugName;
        outBuffer.m_Kind      = m_Kind;

        //
        // Descriptor set creation
        //

        VkDescriptorBufferInfo dstInfo{};
        dstInfo.buffer = outBuffer.m_Buffer;
        dstInfo.offset = m_Offset;
        dstInfo.range  = m_Size;

        // TODO(Adam): Maybe get this to follow the factory pattern too
        ilm::Ref<VkcDescriptorBuilder> dstBuilder = VkcDescriptorBuilder::Create(m_DstAllocator, m_DstLayoutCache);
        dstBuilder->BindBuffer(m_Binding, dstInfo, BufferKindToDescriptorType(m_Kind));
        if (!dstBuilder->Build(outBuffer.m_DstSet))
        {
            vkc::Error("Failed to allocate descriptor sets for buffer '{0}'!", m_DebugName);
            return false;
        }

        return true;
    }
}  // namespace vkc
