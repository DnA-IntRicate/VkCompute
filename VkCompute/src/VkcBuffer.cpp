#include <VkCompute/VkcBuffer.hpp>
#include <VkCompute/Assert.hpp>
#include <VkCompute/Log.hpp>
#include <VkCompute/VkcDescriptorBuilder.hpp>
#include <cstring>


namespace vkc
{
    template class VkcBuffer<VkcBufferKind::StorageBuffer>;
    template class VkcBuffer<VkcBufferKind::UniformBuffer>;

    template <VkcBufferKind Kind>
    VkcBuffer<Kind>::~VkcBuffer() noexcept
    {
        Destroy();
    }

    template <VkcBufferKind Kind>
    void VkcBuffer<Kind>::Destroy() noexcept
    {
        UnmapMemory();
        if (m_Allocator && m_Buffer && m_Allocation)
        {
            vmaDestroyBuffer(m_Allocator, m_Buffer, m_Allocation);
            m_Allocator  = nullptr;
            m_Buffer     = nullptr;
            m_Allocation = nullptr;
        }
    }

    template <VkcBufferKind Kind>
    bool VkcBuffer<Kind>::MapMemory() noexcept
    {
        if (!m_Allocator || !m_Allocation)
            return false;  // VK_ERROR_DEVICE_LOST

        return vmaMapMemory(m_Allocator, m_Allocation, &m_Mapping) == VK_SUCCESS;
    }

    template <VkcBufferKind Kind>
    void VkcBuffer<Kind>::UnmapMemory() noexcept
    {
        if (m_Mapping)
        {
            vmaUnmapMemory(m_Allocator, m_Allocation);
            m_Mapping = nullptr;
        }
    }

    template <VkcBufferKind Kind>
    bool VkcBuffer<Kind>::MapData(const void* data, size_t size) noexcept
    {
        // INVARIANT: Data pointer cannot be null!
        if ((size > 0) && !data)
        {
            vkc::Error("Cannot map null data to the '{0}' buffer!", m_DebugName);
            return false;
        }

        // INVARIANT: We cannot map data of a size greater than the allocated buffer size!
        if (m_Size < size)
        {
            vkc::Error(
                "Failed to map data for the '{0}' buffer - the data size of {1} bytes was larger than the buffer's allocated size of {2} "
                "bytes!",
                m_DebugName,
                size,
                m_Size
            );
            return false;
        }

        // Map GPU memory if not already mapped.
        if (!m_Mapping && !MapMemory())
        {
            vkc::Error("Failed to map memory for the '{0}' buffer!", m_DebugName);
            return false;
        }

        // NOTE(Adam): The mapping would never be null here, this 'if' is only done to satisfy
        // code analyzers.
        if (m_Mapping)
            std::memcpy(m_Mapping, data, size);

        return true;
    }

    template <VkcBufferKind Kind>
    bool VkcBuffer<Kind>::Read(void* outPtr, size_t size) const noexcept
    {
        // INVARIANT: Out pointer cannot be null!
        if ((size > 0) && !outPtr)
        {
            vkc::Error("Cannot read data from '{0}' buffer into a null pointer!", m_DebugName);
            return false;
        }

        // INVARIANT: Size cannot be bigger than buffer size
        if (m_Size < size)
        {
            vkc::Error("Failed to read {0} bytes from the '{1}' buffer of length {2} bytes!", size, m_DebugName, m_Size);
            return false;
        }

        if (!m_Mapping)
        {
            vkc::Error("Cannot read data from the unmapped '{0}' buffer!", m_DebugName);
            return false;
        }

        std::memcpy(outPtr, m_Mapping, size);
        return true;
    }

    template <VkcBufferKind Kind>
    bool VkcBuffer<Kind>::Read(void* outPtr) const noexcept
    {
        return Read(outPtr, m_Size);
    }

    template <VkcBufferKind Kind>
    bool VkcBuffer<Kind>::Flush(size_t size, size_t offset) const noexcept
    {
        // INVARIANT: Size cannot be bigger than buffer size
        if (m_Size < size)
        {
            vkc::Error("Failed to flush range (({0} + {1}) bytes) for the '{2}' buffer!", size, offset, m_DebugName);
            return false;
        }

        VkResult err = vmaFlushAllocation(m_Allocator, m_Allocation, offset, size);
        if (err != VK_SUCCESS)
        {
            vkc::Error("Failed to flush range (({0} + {1}) bytes) for the '{2}' buffer!", size, offset, m_DebugName);
            return false;
        }

        return true;
    }

    template <VkcBufferKind Kind>
    bool VkcBuffer<Kind>::Flush() const noexcept
    {
        return Flush(m_AllocationInfo.size, 0);
    }

    template <VkcBufferKind Kind>
    bool VkcBuffer<Kind>::Invalidate(size_t size, size_t offset) const noexcept
    {
        // INVARIANT: Size cannot be bigger than buffer size
        if (m_Size < size)
        {
            vkc::Error("Failed to invalidate range (({0} + {1}) bytes) for the '{2}' buffer!", size, offset, m_DebugName);
            return false;
        }

        VkResult err = vmaInvalidateAllocation(m_Allocator, m_Allocation, offset, size);
        if (err != VK_SUCCESS)
        {
            vkc::Error("Failed to invalidate range (({0} + {1}) bytes) for the '{2}' buffer!", size, offset, m_DebugName);
            return false;
        }

        return true;
    }

    template <VkcBufferKind Kind>
    bool VkcBuffer<Kind>::Invalidate() const noexcept
    {
        return Invalidate(m_AllocationInfo.size, 0);
    }
}  // namespace vkc
