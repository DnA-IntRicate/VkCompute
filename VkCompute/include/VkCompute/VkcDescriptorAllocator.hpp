#pragma once
#include "VkcDevice.hpp"
#include "VkcObject.hpp"
#include <Ilm/Ilm.hpp>
#include <Volk/volk.h>
#include <vector>


namespace vkc
{
    /**
     * @brief Manages the allocation of descriptor sets.
     */
    class VkcDescriptorAllocator
    {
    public:
        constexpr VkcDescriptorAllocator(const ilm::Ref<VkcDevice>& device) noexcept
            : m_Device(device)
        {
        }

        ~VkcDescriptorAllocator() noexcept;

        // Allow copy-assignment by ilm::Ref only
        constexpr VkcDescriptorAllocator(const VkcDescriptorAllocator&)            = delete;
        constexpr VkcDescriptorAllocator& operator=(const VkcDescriptorAllocator&) = delete;

        // Allow move-assignment by ilm::Ref only
        constexpr VkcDescriptorAllocator(VkcDescriptorAllocator&&)            = delete;
        constexpr VkcDescriptorAllocator& operator=(VkcDescriptorAllocator&&) = delete;

        /**
         * @brief Destroys this descriptor allocator. All descriptor sets allocated
         * by this allocator must be destroyed before destroying this allocator.
         */
        void Destroy() noexcept;

        /**
         * @brief Allocates a new descriptor set from an available pool using the
         * specified VkDescriptorSetLayout.
         *
         * @param outDstSet The VkDescriptorSet handle to allocate.
         *
         * @returns True if the set was allocated, false otherwise.
         */
        bool AllocateSet(const VkDescriptorSetLayout& layout, VkDescriptorSet& outDstSet) noexcept;

        /**
         * @brief Resets all active & marks them as free pools.
         */
        void ResetPools() noexcept;

        /**
         * @brief Checks if this descriptor allocator is valid.
         */
        constexpr bool Valid() const noexcept { return (bool)m_Device && m_Device->Valid() && (bool)m_CurrentPool; }

    public:
        /**
         * @brief Creates a new VkcDescriptorAllocator.
         */
        static ilm::Ref<VkcDescriptorAllocator> Create(const ilm::Ref<VkcDevice>& device) noexcept;

    private:
        VkResult CreatePool(VkDescriptorPoolCreateFlags flags, VkDescriptorPool& outPool) const noexcept;
        VkResult GrabPool(VkDescriptorPool& outPool) noexcept;

    private:
        ilm::Ref<VkcDevice> m_Device;
        VkDescriptorPool m_CurrentPool = nullptr;

        std::vector<VkDescriptorPool> m_ActivePools;
        std::vector<VkDescriptorPool> m_FreePools;
    };

    static_assert(VkcObject<VkcDescriptorAllocator>);
    static_assert(VkcCreatable<VkcDescriptorAllocator, const ilm::Ref<VkcDevice>&>);
}  // namespace vkc
