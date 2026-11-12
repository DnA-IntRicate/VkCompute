#pragma once
#include "VkcDescriptorAllocator.hpp"
#include "VkcDescriptorLayoutCache.hpp"
#include "VkcDevice.hpp"
#include <Ilm/Ilm.hpp>
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>
#include <string>
#include <string_view>
#include <vector>


namespace vkc
{
    /**
     * @brief Specifies the Vulkan descriptor and usage category of a buffer.
     *
     * Determines how a @ref VkcBuffer is created, including its Vulkan buffer usage flags,
     * descriptor type, and minimum offset alignment requirements.
     *
     * Each supported buffer kind has a corresponding specialization of @ref VkcBufferTraits.
     */
    enum class VkcBufferKind
    {
        /** @brief No buffer kind specified. Not a valid kind for VkcBuffer or VkcBufferFactory. */
        None = 0,

        /** @brief Storage buffer (SSBO). */
        StorageBuffer,

        /** @brief Uniform buffer (UBO). */
        UniformBuffer
    };

    template <VkcBufferKind Kind>
    class VkcBuffer;

    /**
     * @brief Provides compile-time Vulkan properties and behaviour for a buffer kind.
     *
     * This traits template is specialized for each supported @ref VkcBufferKind.
     *
     * @tparam Kind The buffer kind for which traits are provided.
     */
    template <VkcBufferKind Kind>
    struct VkcBufferTraits
    {
    };

    /**
     * @brief Traits specialization for Vulkan storage buffers (SSBOs).
     *
     * Provides the Vulkan properties required to create and describe a storage buffer.
     */
    template <>
    struct VkcBufferTraits<VkcBufferKind::StorageBuffer>
    {
        /** @brief Human-readable name identifying this buffer kind. */
        static constexpr std::string_view KindName = "StorageBuffer";

        /** @brief Vulkan buffer usage flags required for storage buffer operations. */
        static constexpr VkBufferUsageFlags UsageFlags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

        /** @brief Vulkan descriptor type associated with storage buffers. */
        static constexpr VkDescriptorType DescriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

        /**
         * @brief Retrieves the minimum offset alignment for storage buffer descriptors.
         *
         * @param device Logical Vulkan device whose physical device properties are queried.
         *
         * @return Minimum required offset alignment in bytes, as specified by
         *         VkPhysicalDeviceLimits::minStorageBufferOffsetAlignment.
         */
        static constexpr VkDeviceSize GetMinOffsetAlignment(const ilm::Ref<VkcDevice>& device) noexcept
        {
            return device->GetPhysicalDevice().Props.limits.minStorageBufferOffsetAlignment;
        }
    };

    /**
     * @brief Traits specialization for Vulkan uniform buffers (UBOs).
     *
     * Provides the Vulkan properties required to create and describe a uniform buffer.
     */
    template <>
    struct VkcBufferTraits<VkcBufferKind::UniformBuffer>
    {
        /** @brief Human-readable name identifying this buffer kind. */
        static constexpr std::string_view KindName = "UniformBuffer";

        /** @brief Vulkan buffer usage flags required for uniform buffer operations. */
        static constexpr VkBufferUsageFlags UsageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

        /** @brief Vulkan descriptor type associated with uniform buffers. */
        static constexpr VkDescriptorType DescriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

        /**
         * @brief Retrieves the minimum offset alignment for uniform buffer descriptors.
         *
         * @param device Logical Vulkan device whose physical device properties are queried.
         *
         * @return Minimum required offset alignment in bytes, as specified by
         *         VkPhysicalDeviceLimits::minUniformBufferOffsetAlignment.
         */
        static constexpr VkDeviceSize GetMinOffsetAlignment(const ilm::Ref<VkcDevice>& device) noexcept
        {
            return device->GetPhysicalDevice().Props.limits.minUniformBufferOffsetAlignment;
        }
    };

    /**
     * @brief Factory for configuring and creating Vulkan buffers.
     *
     * VkcBufferFactory provides a builder-style interface for configuring a
     * @ref VkcBuffer of a compile-time specified kind.
     *
     * Configuration is performed through chained setter calls, followed by
     * a call to @ref Create.
     *
     * @tparam Kind Buffer kind to create.
     *
     * @note VkcBufferKind::None is explicitly rejected at compile time.
     *
     * @warning The factory does not independently validate that all required
     *          configuration has been supplied.
     *
     * @see VkcBuffer
     * @see VkcBufferTraits
     */
    template <VkcBufferKind Kind>
    class VkcBufferFactory
    {
    public:
        static_assert(Kind != VkcBufferKind::None, "VkcBufferKind cannot be None!");

        /** @brief Traits describing the Vulkan properties of the selected buffer kind. */
        using Traits = VkcBufferTraits<Kind>;

    public:
        /**
         * @brief Constructs an empty buffer factory.
         *
         * All configuration fields are initialized to their respective defaults.
         */
        constexpr VkcBufferFactory() = default;

        /**
         * @brief Destroys the factory.
         */
        ~VkcBufferFactory() = default;

        /**
         * @brief Sets the debug name assigned to the buffer.
         *
         * @param name Human-readable name used for buffer identification and debugging.
         *
         * @return Reference to this factory for method chaining.
         */
        constexpr VkcBufferFactory& SetName(const std::string& name) noexcept
        {
            m_DebugName = name;
            return *this;
        }

        /**
         * @brief Sets the Vulkan Memory Allocator instance used for buffer allocation.
         *
         * @param allocator VMA allocator responsible for managing the buffer's memory.
         *
         * @return Reference to this factory for method chaining.
         *
         * @note The allocator must be valid and compatible with the selected device.
         */
        constexpr VkcBufferFactory& SetAllocator(VmaAllocator allocator) noexcept
        {
            m_Allocator = allocator;
            return *this;
        }

        /**
         * @brief Sets the logical Vulkan device used to create the buffer.
         *
         * @param device Reference to the logical device associated with the buffer.
         *
         * @return Reference to this factory for method chaining.
         *
         * @note The device is also used to retrieve physical device properties,
         *       including the minimum descriptor offset alignment.
         */
        constexpr VkcBufferFactory& SetDevice(const ilm::Ref<VkcDevice>& device) noexcept
        {
            m_Device = device;
            return *this;
        }

        /**
         * @brief Sets the VMA memory usage preference for the buffer allocation.
         *
         * @param usage VMA memory usage strategy used when selecting the allocation's
         *              memory type.
         *
         * @return Reference to this factory for method chaining.
         *
         * @note Defaults to VMA_MEMORY_USAGE_AUTO.
         */
        constexpr VkcBufferFactory& SetMemoryUsage(VmaMemoryUsage usage) noexcept
        {
            m_Usage = usage;
            return *this;
        }

        /**
         * @brief Sets the queue families that may access the buffer.
         *
         * @param indices Vector of Vulkan queue family indices associated with the buffer.
         *
         * @return Reference to this factory for method chaining.
         *
         * @note These indices are used to configure the buffer's queue sharing behaviour.
         */
        constexpr VkcBufferFactory& SetQueueFamilies(const std::vector<uint32_t>& indices) noexcept
        {
            m_QueueFamilies = indices;
            return *this;
        }

        /**
         * @brief Sets the descriptor allocator used to allocate the buffer's descriptor set.
         *
         * @param allocator Reference to the descriptor allocator.
         *
         * @return Reference to this factory for method chaining.
         */
        constexpr VkcBufferFactory& SetDescriptorAllocator(const ilm::Ref<VkcDescriptorAllocator>& allocator) noexcept
        {
            m_DstAllocator = allocator;
            return *this;
        }

        /**
         * @brief Sets the descriptor layout cache used to retrieve or create the buffer's descriptor layout.
         *
         * @param cache Reference to the descriptor layout cache.
         *
         * @return Reference to this factory for method chaining.
         */
        constexpr VkcBufferFactory& SetDescriptorLayoutCache(const ilm::Ref<VkcDescriptorLayoutCache>& cache) noexcept
        {
            m_DstLayoutCache = cache;
            return *this;
        }

        /**
         * @brief Sets the descriptor binding associated with the buffer.
         *
         * @param binding Binding index within the descriptor set layout.
         *
         * @return Reference to this factory for method chaining.
         */
        constexpr VkcBufferFactory& SetBinding(size_t binding) noexcept
        {
            m_Binding = binding;
            return *this;
        }

        /**
         * @brief Sets the descriptor offset into the buffer.
         *
         * @param offset Byte offset from the beginning of the buffer.
         *
         * @return Reference to this factory for method chaining.
         *
         * @note The offset must satisfy the minimum alignment requirement of the
         *       selected buffer kind and physical device.
         */
        constexpr VkcBufferFactory& SetOffset(size_t offset) noexcept
        {
            m_Offset = offset;
            return *this;
        }

        /**
         * @brief Sets the buffer size in bytes.
         *
         * @param size Size of the buffer allocation in bytes.
         *
         * @return Reference to this factory for method chaining.
         *
         * @note The size must satisfy the relevant Vulkan descriptor range limits.
         */
        constexpr VkcBufferFactory& SetSize(size_t size) noexcept
        {
            m_Size = size;
            return *this;
        }

        /**
         * @brief Creates a @ref VkcBuffer using the configured parameters.
         *
         * Constructs a buffer of the kind specified by the factory's template
         * parameter and returns an @ref ilm::Ref to the resulting object.
         *
         * @return Reference to the created buffer, or nullptr if creation fails.
         *
         * @warning All required configuration must be valid before calling this method.
         */
        ilm::Ref<VkcBuffer<Kind>> Create() const noexcept;

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

    // Explicit instantiation declarations to avoid redundancies in including TUs
    // since this templated-class splits contains implementation in a source file.
    extern template class VkcBufferFactory<VkcBufferKind::StorageBuffer>;
    extern template class VkcBufferFactory<VkcBufferKind::UniformBuffer>;

    /** @brief Convenience alias for a storage buffer factory. */
    typedef VkcBufferFactory<VkcBufferKind::StorageBuffer> VkcStorageBufferFactory;

    /** @brief Convenience alias for a uniform buffer factory. */
    typedef VkcBufferFactory<VkcBufferKind::UniformBuffer> VkcUniformBufferFactory;
}  // namespace vkc
