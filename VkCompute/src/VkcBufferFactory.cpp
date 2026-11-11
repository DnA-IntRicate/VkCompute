#include <VkCompute/VkcBufferFactory.hpp>
#include <VkCompute/VkcBuffer.hpp>
#include <VkCompute/Assert.hpp>
#include <VkCompute/Log.hpp>
#include <VkCompute/VkcDescriptorBuilder.hpp>


namespace vkc
{
    template class VkcBufferFactory<VkcBufferKind::StorageBuffer>;
    template class VkcBufferFactory<VkcBufferKind::UniformBuffer>;

    template <VkcBufferKind Kind>
    ilm::Ref<VkcBuffer<Kind>> VkcBufferFactory<Kind>::Create() const noexcept
    {
        //
        // Buffer creation
        //

        const uint32_t qfIndexCount = static_cast<uint32_t>(m_QueueFamilies.size());

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.pNext                 = nullptr;
        bufferInfo.flags                 = 0;
        bufferInfo.size                  = m_Size;
        bufferInfo.usage                 = Traits::UsageFlags;
        bufferInfo.sharingMode           = (qfIndexCount <= 1) ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
        bufferInfo.queueFamilyIndexCount = qfIndexCount;
        bufferInfo.pQueueFamilyIndices   = m_QueueFamilies.data();

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

        ilm::Ref<VkcBuffer<Kind>> buffer = ilm::CreateRef<VkcBuffer<Kind>>();
        buffer->m_Allocator              = m_Allocator;
        buffer->m_DebugName              = m_DebugName;
        buffer->m_Binding                = m_Binding;
        buffer->m_Offset                 = m_Offset;
        buffer->m_Size                   = m_Size;

        if (vmaCreateBufferWithAlignment(
                m_Allocator,
                &bufferInfo,
                &allocCreateInfo,
                Traits::GetMinOffsetAlignment(m_Device),
                &buffer->m_Buffer,
                &buffer->m_Allocation,
                &buffer->m_AllocationInfo
            )
            != VK_SUCCESS)
        {
            vkc::Error("Failed to create buffer '{0}'!", m_DebugName);
            return nullptr;
        }

        //
        // Descriptor set creation
        //

        VkDescriptorBufferInfo dstInfo{};
        dstInfo.buffer = buffer->m_Buffer;
        dstInfo.offset = m_Offset;
        dstInfo.range  = m_Size;

        // TODO(Adam): Maybe get this to follow the factory pattern too?
        ilm::Ref<VkcDescriptorBuilder> dstBuilder = VkcDescriptorBuilder::Create(m_DstAllocator, m_DstLayoutCache);
        dstBuilder->BindBuffer(m_Binding, dstInfo, Traits::DescriptorType);
        if (!dstBuilder->Build(buffer->m_DstSet))
        {
            vkc::Error("Failed to allocate descriptor sets for buffer '{0}'!", m_DebugName);
            return nullptr;
        }

        vkc::Success(
            "Created buffer '{0}' of type '{1}' (binding = {2}, offset = {3}, size = {4}).",
            m_DebugName,
            Traits::KindName,
            m_Binding,
            m_Offset,
            m_Size
        );
        return buffer;
    }
}  // namespace vkc
