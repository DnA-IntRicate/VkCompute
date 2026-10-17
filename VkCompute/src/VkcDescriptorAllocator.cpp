#include <VkCompute/VkcDescriptorAllocator.hpp>
#include <VkCompute/Log.hpp>
#include <array>
#include <utility>


namespace vkc
{
    // NOTE(Adam): For VkCompute, we only actually care about uniform & storage buffers.
    static constexpr std::array<std::pair<VkDescriptorType, uint32_t>, 4> POOL_SIZES =
    {{
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,            1 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,    1 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,            2 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,    2 }
    }};

    // The maximum number of descriptor sets that can be allocated per pool.
    static constexpr uint32_t MAX_SETS_PER_POOL = 64;

    VkcDescriptorAllocator::~VkcDescriptorAllocator() noexcept
    {
        Destroy();
    }

    void VkcDescriptorAllocator::Destroy() noexcept
    {
        for (auto& pool : m_FreePools)
        {
            if (pool)
                vkDestroyDescriptorPool(m_Device->GetDevice(), pool, nullptr);
        }

        for (auto& pool : m_ActivePools)
        {
            if (pool)
                vkDestroyDescriptorPool(m_Device->GetDevice(), pool, nullptr);
        }
    }

    bool VkcDescriptorAllocator::AllocateSet(const VkDescriptorSetLayout& layout, VkDescriptorSet& outDstSet) noexcept
    {
        if (!m_CurrentPool)
        {
            if (GrabPool(m_CurrentPool) != VK_SUCCESS)
            {
                vkc::Error("Failed to grab a new pool during descriptor set allocation!");
                return false;
            }

            m_ActivePools.push_back(m_CurrentPool);
        }

        VkDescriptorSetAllocateInfo dstSetInfo{};
        dstSetInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        dstSetInfo.pNext              = nullptr;
        dstSetInfo.descriptorPool     = m_CurrentPool;
        dstSetInfo.descriptorSetCount = 1;
        dstSetInfo.pSetLayouts        = &layout;
        VkResult err                  = vkAllocateDescriptorSets(m_Device->GetDevice(), &dstSetInfo, &outDstSet);

        // Success
        if (err == VK_SUCCESS)
            return true;

        // Irrecoverable errors
        if ((err != VK_ERROR_FRAGMENTED_POOL) && (err != VK_ERROR_OUT_OF_POOL_MEMORY))
        {
            vkc::Error("Failed to allocate descriptor set! : {:#x}.", static_cast<int>(err));
            return false;
        }

        // Attempt reallocation
        if (GrabPool(m_CurrentPool) != VK_SUCCESS)
        {
            vkc::Error("Failed to grab a replacement descriptor pool!");
            return false;
        }

        m_ActivePools.push_back(m_CurrentPool);
        return AllocateSet(layout, outDstSet);
    }

    void VkcDescriptorAllocator::ResetPools() noexcept
    {
        m_Device->WaitIdle();

        for (auto& pool : m_ActivePools)
        {
            if (pool)
            {
                vkResetDescriptorPool(m_Device->GetDevice(), pool, 0);
                m_FreePools.emplace_back(pool);
            }
        }

        m_ActivePools.clear();
        m_ActivePools.shrink_to_fit();
        m_CurrentPool = nullptr;
    }

    ilm::Ref<VkcDescriptorAllocator> VkcDescriptorAllocator::Create(const ilm::Ref<VkcDevice>& device) noexcept
    {
        return ilm::CreateRef<VkcDescriptorAllocator>(device);
    }

    VkResult VkcDescriptorAllocator::CreatePool(VkDescriptorPoolCreateFlags flags, VkDescriptorPool& outPool) const noexcept
    {
        std::vector<VkDescriptorPoolSize> sizes;
        for (const auto& [dstType, count] : POOL_SIZES)
            sizes.emplace_back(dstType, count);

        VkDescriptorPoolCreateInfo poolCreateInfo{};
        poolCreateInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolCreateInfo.pNext         = nullptr;
        poolCreateInfo.flags         = flags;
        poolCreateInfo.maxSets       = MAX_SETS_PER_POOL;
        poolCreateInfo.poolSizeCount = static_cast<uint32_t>(sizes.size());
        poolCreateInfo.pPoolSizes    = sizes.data();

        return vkCreateDescriptorPool(m_Device->GetDevice(), &poolCreateInfo, nullptr, &outPool);
    }

    VkResult VkcDescriptorAllocator::GrabPool(VkDescriptorPool& outPool) noexcept
    {
        if (m_FreePools.size() > 0)
        {
            outPool = m_FreePools.back();
            m_FreePools.pop_back();

            return VK_SUCCESS;
        }

        return CreatePool(0, outPool);
    }
}  // namespace vkc
