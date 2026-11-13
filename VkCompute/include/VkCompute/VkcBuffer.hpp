#pragma once
#include "VkcBufferFactory.hpp"
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>
#include <string_view>


namespace vkc
{
    /**
     * @brief Represents a GPU buffer resource.
     *
     * VkcBuffer manages the lifetime, memory allocation, and host-memory access
     * of a Vulkan buffer. It also maintains the descriptor configuration required
     * to expose the buffer to shader stages.
     *
     * The buffer kind is specified at compile time.
     *
     * @tparam Kind Buffer kind.
     *
     * @see VkcBufferFactory
     * @see VkcBufferTraits
     */
    template <VkcBufferKind Kind>
    class VkcBuffer
    {
    public:
        static constexpr std::string_view Name = "VkcBuffer";

        static constexpr VkcBufferKind BufferKind = Kind;

    public:
        /**
         * @brief Constructs an empty buffer resource.
         *
         * Initializes the resource handles and associated state to their
         * default, invalid values.
         *
         * @note Buffer resources should generally be constructed through
         *       @ref VkcBufferFactory rather than directly. This constructor
         *       should never be called directly!
         */
        constexpr VkcBuffer() = default;

        /**
         * @brief Destroys the buffer resource.
         */
        ~VkcBuffer() noexcept;

        // Allow copy-assignment by ilm::Ref only
        constexpr VkcBuffer(const VkcBuffer&)            = delete;
        constexpr VkcBuffer& operator=(const VkcBuffer&) = delete;

        // Allow move-assignment by ilm::Ref only
        constexpr VkcBuffer(VkcBuffer&&)            = delete;
        constexpr VkcBuffer& operator=(VkcBuffer&&) = delete;

        /**
         * @brief Explicitly destroys the buffer resource.
         *
         * @warning Ensure that the buffer is no longer in use by the GPU before
         *          destroying it. Appropriate synchronization must be performed
         *          by the caller.
         */
        void Destroy() noexcept;

        /**
         * @brief Maps the buffer's underlying GPU allocation into host address space.
         *
         * Makes the allocation accessible to the host CPU and stores the
         * resulting mapped memory pointer internally.
         *
         * @return true if the allocation was successfully mapped, otherwise false.
         *
         * @note Mapping behaviour depends on the memory properties selected
         *       during allocation. Host-visible memory is required!
         *
         * @see UnmapMemory
         * @see MapData
         * @see Read
         */
        bool MapMemory() noexcept;

        /**
         * @brief Unmaps the buffer's underlying GPU allocation.
         *
         * Releases the host mapping previously established through
         * @ref MapMemory and clears the internally stored mapping pointer.
         *
         * @note Unmapping an allocation that is not currently mapped should
         *       be handled safely by the implementation.
         *
         * @see MapMemory
         */
        void UnmapMemory() noexcept;

        /**
         * @brief Writes data into the buffer's mapped memory.
         *
         * Copies the specified number of bytes from the supplied source
         * pointer into the buffer's mapped allocation.
         *
         * @param data Pointer to the source data.
         * @param size Number of bytes to write.
         *
         * @return true if the operation succeeds, otherwise false.
         *
         * @pre The buffer must be valid and its allocation must be mapped.
         * @pre data must reference at least size readable bytes.
         * @pre size must not exceed the buffer's allocated size.
         *
         * @note For non-coherent host-visible memory, call @ref Flush to ensure
         *       that host writes become visible to the device.
         *
         * @see MapMemory
         * @see Flush
         */
        bool MapData(const void* data, size_t size) noexcept;

        /**
         * @brief Reads data from the buffer's mapped memory.
         *
         * Copies the requested number of bytes from the buffer's mapped
         * allocation into the supplied destination pointer.
         *
         * @param[out] outPtr Destination pointer receiving the copied data.
         * @param size Number of bytes to read.
         *
         * @return true if the operation succeeds, otherwise false.
         *
         * @pre The buffer must be valid and its allocation must be mapped.
         * @pre outPtr must reference at least size writable bytes.
         * @pre size must not exceed the buffer's allocated size.
         *
         * @note If the device has written to non-coherent host-visible memory,
         *       call @ref Invalidate before reading to ensure that the host
         *       observes the latest device writes.
         *
         * @see MapMemory
         * @see Invalidate
         */
        bool Read(void* outPtr, size_t size) const noexcept;

        /**
         * @brief Reads the entire buffer into the supplied destination pointer.
         *
         * Equivalent to calling @ref Read(void*, size_t) with the buffer's
         * configured size.
         *
         * @param[out] outPtr Destination pointer receiving the buffer contents.
         *
         * @return true if the operation succeeds, otherwise false.
         *
         * @pre The buffer must be valid and its allocation must be mapped.
         * @pre outPtr must reference at least GetSize() writable bytes.
         *
         * @see Read(void*, size_t)
         */
        bool Read(void* outPtr) const noexcept;

        /**
         * @brief Flushes a range of mapped memory to make host writes visible to the device.
         *
         * Flushes the specified range of the buffer's GPU allocation.
         * This is required when the allocation uses non-coherent host-visible
         * memory and the host has modified its contents.
         *
         * @param size Number of bytes to flush.
         * @param offset Byte offset into the allocation from which flushing begins.
         *
         * @return true if the operation succeeds, otherwise false.
         *
         * @note Defaults to flushing from offset zero.
         *
         * @see MapData
         * @see Flush()
         */
        bool Flush(size_t size, size_t offset = 0) const noexcept;

        /**
         * @brief Flushes the entire buffer allocation.
         *
         * Equivalent to calling @ref Flush(size_t, size_t) with the buffer's
         * configured size and an offset of zero.
         *
         * @return true if the operation succeeds, otherwise false.
         *
         * @see Flush(size_t, size_t)
         */
        bool Flush() const noexcept;

        /**
         * @brief Invalidates a range of mapped memory to make device writes visible to the host.
         *
         * Invalidates the specified range of the buffer's GPU allocation.
         * This is required when the allocation uses non-coherent host-visible
         * memory and the device has modified its contents.
         *
         * @param size Number of bytes to invalidate.
         * @param offset Byte offset into the allocation from which invalidation begins.
         *
         * @return true if the operation succeeds, otherwise false.
         *
         * @note Defaults to invalidating from offset zero.
         *
         * @see Read
         * @see Invalidate()
         */
        bool Invalidate(size_t size, size_t offset = 0) const noexcept;

        /**
         * @brief Invalidates the entire buffer allocation.
         *
         * Equivalent to calling @ref Invalidate(size_t, size_t) with the
         * buffer's configured size and an offset of zero.
         *
         * @return true if the operation succeeds, otherwise false.
         *
         * @see Invalidate(size_t, size_t)
         */
        bool Invalidate() const noexcept;

        /**
         * @brief Retrieves the underlying Vulkan buffer handle.
         *
         * @return VkBuffer handle associated with this resource.
         */
        constexpr VkBuffer GetBuffer() const noexcept { return m_Buffer; }

        /**
         * @brief Retrieves the VMA allocation associated with this buffer.
         *
         * @return VMA allocation handle.
         */
        constexpr VmaAllocation GetAllocation() const noexcept { return m_Allocation; }

        /**
         * @brief Retrieves the VMA allocator associated with this buffer.
         *
         * @return VMA allocator used to manage the buffer's memory.
         *
         * @note The returned allocator is non-owning. Its lifetime must
         *       exceed that of the buffer allocation.
         */
        constexpr VmaAllocator GetAllocator() const noexcept { return m_Allocator; }

        /**
         * @brief Retrieves the debug name assigned to the buffer.
         *
         * @return Constant reference to the buffer's debug name.
         */
        constexpr const std::string& GetName() const noexcept { return m_DebugName; }

        /**
         * @brief Retrieves the descriptor binding associated with the buffer.
         *
         * @return Descriptor binding index within the descriptor set layout.
         */
        constexpr size_t GetBinding() const noexcept { return m_Binding; }

        /**
         * @brief Retrieves the descriptor offset into the buffer.
         *
         * @return Byte offset from the beginning of the buffer.
         */
        constexpr size_t GetOffset() const noexcept { return m_Offset; }

        /**
         * @brief Retrieves the configured buffer size.
         *
         * @return Buffer size in bytes.
         */
        constexpr size_t GetSize() const noexcept { return m_Size; }

        /**
         * @brief Checks whether the buffer resource has been successfully created.
         *
         * A buffer is considered valid when both its Vulkan buffer handle
         * and VMA allocation handle are non-null.
         *
         * @return true if both handles are valid, otherwise false.
         *
         * @note This check does not guarantee that the buffer is currently
         *       mapped, that its descriptor set is valid, or that it is safe
         *       to access from the GPU.
         */
        constexpr bool Valid() const noexcept { return (bool)m_Buffer && (bool)m_Allocation; }

    private:
        // Vulkan buffer handle managed by this resource.
        VkBuffer m_Buffer = nullptr;

        // VMA allocator used to manage the buffer allocation
        VmaAllocator m_Allocator = nullptr;

        // VMA allocation backing the Vulkan buffer.
        VmaAllocation m_Allocation = nullptr;

        // Allocation metadata provided by VMA & populated by the buffer factory.
        VmaAllocationInfo m_AllocationInfo{};

        // Pointer to the currently mapped allocation memory.
        void* m_Mapping = nullptr;

        // Debug name assigned to the buffer.
        std::string m_DebugName;

        // Descriptor binding index associated with this buffer.
        size_t m_Binding = 0;

        // Descriptor offset in bytes.
        size_t m_Offset = 0;

        // Configured buffer size in bytes.
        size_t m_Size = 0;

    private:
        // VkcBufferFactory requires private access to this class.
        friend class VkcBufferFactory<Kind>;
    };

    // Explicit instantiation declarations to avoid redundancies in including TUs
    // since this templated-class splits contains implementation in a source file.
    extern template class VkcBuffer<VkcBufferKind::StorageBuffer>;
    extern template class VkcBuffer<VkcBufferKind::UniformBuffer>;

    /** @brief Convenience alias for a storage buffer. */
    typedef VkcBuffer<VkcBufferKind::StorageBuffer> VkcStorageBuffer;

    /** @brief Convenience alias for a uniform buffer. */
    typedef VkcBuffer<VkcBufferKind::UniformBuffer> VkcUniformBuffer;
}  // namespace vkc
