#pragma once
#include <Ilm/Ilm.hpp>
#include <vulkan/vulkan_core.h>
#include <vector>


namespace vkc
{
    class VkcDescriptorAllocator;
    class VkcDescriptorLayoutCache;

    /**
     * @brief A scoped Vulkan descriptor set builder. 
     */
    class VkcDescriptorBuilder
    {
    public:
        constexpr VkcDescriptorBuilder(const ilm::Ref<VkcDescriptorAllocator>& allocator,
            const ilm::Ref<VkcDescriptorLayoutCache>& cache) noexcept
            : m_Allocator(allocator), m_LayoutCache(cache)
        {
        }

        ~VkcDescriptorBuilder() noexcept;

        // Forbid copy-assignment
        constexpr VkcDescriptorBuilder(const VkcDescriptorBuilder&)            = delete;
        constexpr VkcDescriptorBuilder& operator=(const VkcDescriptorBuilder&) = delete;

        // Forbid move-assignment
        constexpr VkcDescriptorBuilder(VkcDescriptorBuilder&&)            = delete;
        constexpr VkcDescriptorBuilder& operator=(VkcDescriptorBuilder&&) = delete;

        // Forbid default construction
        constexpr VkcDescriptorBuilder() = delete;

        /**
         * @brief
         */
        // TODO(Adam): Make this void?
        VkcDescriptorBuilder& BindBuffer(uint32_t binding, const VkDescriptorBufferInfo& info, VkDescriptorType type,
            uint32_t count = 1) noexcept;

        /**
         * @brief
         */
        bool Build(VkDescriptorSet& outDstSet, VkDescriptorSetLayout& outDstLayout) noexcept;

        /**
         * @brief
         */
        bool Build(VkDescriptorSet& outDstSet) noexcept;

        /**
         * @brief
         */
        void Update(VkDescriptorSet& dstSet) noexcept;

        /**
         * @brief
         */
        void Update(VkDescriptorSet& dstSet, uint32_t binding, const VkDescriptorBufferInfo& info, VkDescriptorType type,
            uint32_t count = 1) noexcept;

        /**
         * @brief
         */
        void Free(VkDescriptorSet& dstSet) noexcept;

    public:
        static ilm::Scope<VkcDescriptorBuilder> CreateUnique(const ilm::Ref<VkcDescriptorAllocator>& allocator,
            const ilm::Ref<VkcDescriptorLayoutCache>& cache) noexcept;

    private:
        ilm::Ref<VkcDescriptorAllocator> m_Allocator;
        ilm::Ref<VkcDescriptorLayoutCache> m_LayoutCache;

        std::vector<VkDescriptorSetLayoutBinding> m_DstSetBindings;
        std::vector<VkWriteDescriptorSet> m_DstSetWrites;
    };
}
