#pragma once
#include "VkcBufferFactory.hpp"
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>
#include <string_view>


namespace vkc
{
    template <VkcBufferKind Kind>
    class VkcBuffer
    {
    public:
        static constexpr std::string_view Name = "VkcBuffer";

    public:
        constexpr VkcBuffer() = default;
        ~VkcBuffer();

        // Allow copy-assignment by ilm::Ref only
        constexpr VkcBuffer(const VkcBuffer&)            = delete;
        constexpr VkcBuffer& operator=(const VkcBuffer&) = delete;

        // Allow move-assignment by ilm::Ref only
        constexpr VkcBuffer(VkcBuffer&&)            = delete;
        constexpr VkcBuffer& operator=(VkcBuffer&&) = delete;

        void Destroy() noexcept;

        bool MapMemory() noexcept;
        void UnmapMemory() noexcept;

        bool MapData(const void* data, size_t size) noexcept;

        bool Read(void* outPtr, size_t size) const noexcept;
        bool Read(void* outPtr) const noexcept;

        bool Flush(size_t size, size_t offset = 0) const noexcept;
        bool Flush() const noexcept;

        bool Invalidate(size_t size, size_t offset = 0) const noexcept;
        bool Invalidate() const noexcept;

        constexpr VkBuffer GetBuffer() const noexcept { return m_Buffer; }

        constexpr VmaAllocation GetAllocation() const noexcept { return m_Allocation; }

        constexpr VmaAllocator GetAllocator() const noexcept { return m_Allocator; }

        constexpr const std::string& GetName() const noexcept { return m_DebugName; }

        constexpr size_t GetBinding() const noexcept { return m_Binding; }

        constexpr size_t GetOffset() const noexcept { return m_Offset; }

        constexpr size_t GetSize() const noexcept { return m_Size; }

        constexpr bool Valid() const noexcept { return (bool)m_Buffer && (bool)m_Allocation; }

    private:
        VkBuffer m_Buffer          = nullptr;
        VkDescriptorSet m_DstSet   = nullptr;  // TODO(Adam): Dsts should be owned by the pipeline, not the resources themselves!
        VmaAllocator m_Allocator   = nullptr;
        VmaAllocation m_Allocation = nullptr;
        VmaAllocationInfo m_AllocationInfo{};

        std::string m_DebugName;
        void* m_Mapping = nullptr;
        size_t m_Binding = 0;
        size_t m_Offset  = 0;
        size_t m_Size    = 0;

    private:
        friend class VkcBufferFactory<Kind>;
    };

    extern template class VkcBuffer<VkcBufferKind::StorageBuffer>;
    extern template class VkcBuffer<VkcBufferKind::UniformBuffer>;

    typedef VkcBuffer<VkcBufferKind::StorageBuffer> VkcStorageBuffer;
    typedef VkcBuffer<VkcBufferKind::UniformBuffer> VkcUniformBuffer;
}  // namespace vkc
