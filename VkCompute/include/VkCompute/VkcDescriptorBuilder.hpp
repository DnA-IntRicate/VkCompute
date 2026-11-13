#pragma once
#include "VkcBuffer.hpp"
#include "VkcBufferFactory.hpp"
#include "VkcDescriptorAllocator.hpp"
#include "VkcDescriptorLayoutCache.hpp"
#include <Ilm/Ilm.hpp>
#include <vulkan/vulkan_core.h>
#include <string_view>
#include <variant>
#include <vector>


namespace vkc
{
    /**
     * @brief A convenient Vulkan descriptor set builder.
     *
     * Uses a VkcDescriptorAllocator to manage descriptor set allocations and a
     * VkcDescriptorLayoutCache to retrieve or create descriptor set layouts.
     */
    class VkcDescriptorBuilder
    {
    public:
        /**
         * @brief Tagged-union of all possible buffer resources that can be bound by this descriptor Builder.
         */
        using BufferResource = std::variant<ilm::Ref<VkcStorageBuffer>, ilm::Ref<VkcUniformBuffer>>;

        /**
         * @brief Human-readable name identifying this type.
         */
        static constexpr std::string_view Name = "VkcDescriptorBuilder";

    public:
        constexpr VkcDescriptorBuilder(
            const ilm::Ref<VkcDescriptorAllocator>& allocator,
            const ilm::Ref<VkcDescriptorLayoutCache>& cache
        ) noexcept
            : m_Allocator(allocator), m_LayoutCache(cache)
        {
        }

        ~VkcDescriptorBuilder() noexcept;

        // Allow copy-assignment by ilm::Ref only
        constexpr VkcDescriptorBuilder(const VkcDescriptorBuilder&)            = delete;
        constexpr VkcDescriptorBuilder& operator=(const VkcDescriptorBuilder&) = delete;

        // Allow copy-assignment by ilm::Ref only
        constexpr VkcDescriptorBuilder(VkcDescriptorBuilder&&)            = delete;
        constexpr VkcDescriptorBuilder& operator=(VkcDescriptorBuilder&&) = delete;

        // Forbid default construction
        constexpr VkcDescriptorBuilder() = delete;

        /**
         * @brief Destroys this descriptor builder.
         * This doesn't free descriptor sets!
         */
        void Destroy() noexcept;

        /**
         * @brief Frees a descriptor set allocation.
         *
         * @param[in] dstAlloc Descriptor set allocation to free.
         *
         * @return true if the allocation was freed successfully; otherwise, false.
         */
        bool Free(VkcDescriptorSetAllocation& dstAlloc) noexcept;

        /**
         * @brief Adds a buffer descriptor binding to the builder.
         *
         * Configures a descriptor binding using the supplied buffer information
         * and descriptor type.
         *
         * @param binding Binding index within the descriptor set layout.
         * @param offset Offset range within the descriptor set layout.
         * @param buffer The buffer resource to bind.
         */
        void BindBuffer(uint32_t binding, size_t offset, const BufferResource& buffer) noexcept;

        /**
         * @brief Builds and allocates a descriptor set.
         *
         * @return the descriptor set allocation handle.
         */
        VkcDescriptorSetAllocation Build() noexcept;

        /**
         * @brief Updates a descriptor binding in an existing descriptor set.
         *
         * Configures and applies a buffer descriptor write to the specified
         * descriptor set binding.
         *
         * @param dstSet Descriptor set to update.
         * @param binding Binding index to update.
         * @param info Buffer descriptor information.
         * @param type Type of descriptor being written.
         * @param count Number of descriptors to update. Defaults to 1.
         */
        // TODO(Adam): Reimplement this when its needed!
        void Update(
            VkDescriptorSet& dstSet,
            uint32_t binding,
            VkDescriptorBufferInfo& info,
            VkDescriptorType type,
            uint32_t count = 1
        ) noexcept;

        /**
         * @brief Applies the builder's configured descriptor writes to a descriptor set.
         *
         * Updates the specified descriptor set using the descriptor write
         * operations currently stored by the builder.
         *
         * @param[in] dstSet Descriptor set to update.
         */
        void Update(VkDescriptorSet& dstSet) noexcept;

        /**
         * @brief Checks if this descriptor builder is valid.
         */
        constexpr bool Valid() const noexcept { return m_Allocator->Valid() && m_LayoutCache->Valid(); }

    public:
        /**
         * @brief Creates a new VkcDescriptorBuilder.
         */
        static ilm::Ref<VkcDescriptorBuilder> Create(
            const ilm::Ref<VkcDescriptorAllocator>& allocator,
            const ilm::Ref<VkcDescriptorLayoutCache>& cache
        ) noexcept;

    private:
        ilm::Ref<VkcDescriptorAllocator> m_Allocator;
        ilm::Ref<VkcDescriptorLayoutCache> m_LayoutCache;

        std::vector<VkDescriptorBufferInfo> m_DstBufferInfos;
        std::vector<VkDescriptorSetLayoutBinding> m_DstSetBindings;
        std::vector<VkWriteDescriptorSet> m_DstSetWrites;
    };
}  // namespace vkc
