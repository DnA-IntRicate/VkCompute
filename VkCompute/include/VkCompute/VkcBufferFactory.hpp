#pragma once
#include "VkcDescriptorAllocator.hpp"
#include "VkcDescriptorLayoutCache.hpp"
#include "VkcDevice.hpp"
#include <Ilm/Ilm.hpp>
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>
#include <string>
#include <vector>


namespace vkc
{
    class VkcBuffer;

    enum class VkcBufferKind
    {
        None = 0,
        StorageBuffer,
        UniformBuffer
    };

    template <VkcBufferKind Kind>
    struct VkcBufferTraits
    {
    };

    // SSBO
    template <>
    struct VkcBufferTraits<VkcBufferKind::StorageBuffer>
    {
        static constexpr VkBufferUsageFlags UsageFlags   = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        static constexpr VkDescriptorType DescriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

        static constexpr VkDeviceSize GetMinOffsetAlignment(const ilm::Ref<VkcDevice>& device) noexcept
        {
            return device->GetPhysicalDevice().Props.limits.minStorageBufferOffsetAlignment;
        }
    };

    // UBO
    template <>
    struct VkcBufferTraits<VkcBufferKind::UniformBuffer>
    {
        static constexpr VkBufferUsageFlags UsageFlags   = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        static constexpr VkDescriptorType DescriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

        static constexpr VkDeviceSize GetMinOffsetAlignment(const ilm::Ref<VkcDevice>& device) noexcept
        {
            return device->GetPhysicalDevice().Props.limits.minUniformBufferOffsetAlignment;
        }
    };

    template <VkcBufferKind Kind>
    class VkcBufferFactory
    {
    public:
        static_assert(Kind != VkcBufferKind::None, "VkcBufferKind cannot be None!");
        using Traits = VkcBufferTraits<Kind>;

    public:
        constexpr VkcBufferFactory() = default;
        ~VkcBufferFactory()          = default;

        constexpr VkcBufferFactory& SetName(const std::string& name) noexcept
        {
            m_DebugName = name;
            return *this;
        }

        constexpr VkcBufferFactory& SetAllocator(VmaAllocator allocator) noexcept
        {
            m_Allocator = allocator;
            return *this;
        }

        constexpr VkcBufferFactory& SetDevice(const ilm::Ref<VkcDevice>& device) noexcept
        {
            m_Device = device;
            return *this;
        }

        constexpr VkcBufferFactory& SetMemoryUsage(VmaMemoryUsage usage) noexcept
        {
            m_Usage = usage;
            return *this;
        }

        constexpr VkcBufferFactory& SetQueueFamilies(const std::vector<uint32_t>& indices) noexcept
        {
            m_QueueFamilies = indices;
            return *this;
        }

        constexpr VkcBufferFactory& SetDescriptorAllocator(const ilm::Ref<VkcDescriptorAllocator>& allocator) noexcept
        {
            m_DstAllocator = allocator;
            return *this;
        }

        constexpr VkcBufferFactory& SetDescriptorLayoutCache(const ilm::Ref<VkcDescriptorLayoutCache>& cache) noexcept
        {
            m_DstLayoutCache = cache;
            return *this;
        }

        constexpr VkcBufferFactory& SetBinding(size_t binding) noexcept
        {
            m_Binding = binding;
            return *this;
        }

        constexpr VkcBufferFactory& SetOffset(size_t offset) noexcept
        {
            m_Offset = offset;
            return *this;
        }

        constexpr VkcBufferFactory& SetSize(size_t size) noexcept
        {
            m_Size = size;
            return *this;
        }

        ilm::Ref<VkcBuffer> Create() const noexcept;

    private:
        std::string m_DebugName;

        ilm::Ref<VkcDevice> m_Device;
        ilm::Ref<VkcDescriptorAllocator> m_DstAllocator;
        ilm::Ref<VkcDescriptorLayoutCache> m_DstLayoutCache;

        VmaAllocator m_Allocator = nullptr;
        VmaMemoryUsage m_Usage   = VMA_MEMORY_USAGE_AUTO;

        std::vector<uint32_t> m_QueueFamilies;

        size_t m_Binding = 0;
        size_t m_Offset  = 0;
        size_t m_Size    = 0;
    };

    extern template class VkcBufferFactory<VkcBufferKind::StorageBuffer>;
    extern template class VkcBufferFactory<VkcBufferKind::UniformBuffer>;

    typedef VkcBufferFactory<VkcBufferKind::StorageBuffer> VkcStorageBufferFactory;
    typedef VkcBufferFactory<VkcBufferKind::UniformBuffer> VkcUniformBufferFactory;
}  // namespace vkc
