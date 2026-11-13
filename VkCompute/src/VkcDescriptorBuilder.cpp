#include <VkCompute/VkcDescriptorBuilder.hpp>
#include <VkCompute/Log.hpp>


namespace vkc
{
    VkcDescriptorBuilder::~VkcDescriptorBuilder() noexcept
    {
        Destroy();
    }

    void VkcDescriptorBuilder::Destroy() noexcept
    {
        m_Allocator   = nullptr;
        m_LayoutCache = nullptr;

        m_DstSetBindings.clear();
        m_DstSetWrites.clear();
    }

    bool VkcDescriptorBuilder::Free(VkcDescriptorSetAllocation& dstAlloc) noexcept
    {
        const bool res = m_Allocator->FreeSet(dstAlloc);
        if (!res)
            vkc::Error("Failed to free descriptor set!");

        return res;
    }

    void VkcDescriptorBuilder::BindBuffer(uint32_t binding, size_t offset, const BufferResource& buffer) noexcept
    {
        std::visit(
            [&](auto&& bufferRef)
            {
                using BufferType = std::decay_t<decltype(*bufferRef)>;
                using Traits     = VkcBufferTraits<BufferType::BufferKind>;

                m_DstBufferInfos.emplace_back(
                    VkDescriptorBufferInfo{ .buffer = bufferRef->GetBuffer(), .offset = offset, .range = bufferRef->GetSize() }
                );

                // NOTE(Adam): We explicitly configure this for compute only.
                m_DstSetBindings.emplace_back(
                    VkDescriptorSetLayoutBinding{ .binding            = binding,
                                                  .descriptorType     = Traits::DescriptorType,
                                                  .descriptorCount    = 1,
                                                  .stageFlags         = VK_SHADER_STAGE_COMPUTE_BIT,
                                                  .pImmutableSamplers = nullptr }
                );

                // The dstSet gets set just before the descriptor-write is performed.
                m_DstSetWrites.emplace_back(
                    VkWriteDescriptorSet{ .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                          .pNext            = nullptr,
                                          .dstSet           = nullptr,
                                          .dstBinding       = binding,
                                          .dstArrayElement  = 0,
                                          .descriptorCount  = 1,
                                          .descriptorType   = Traits::DescriptorType,
                                          .pImageInfo       = nullptr,
                                          .pBufferInfo      = &m_DstBufferInfos.back(),
                                          .pTexelBufferView = nullptr }
                );
            },
            buffer
        );
    }

    VkcDescriptorSetAllocation VkcDescriptorBuilder::Build() noexcept
    {
        VkDescriptorSetLayoutCreateInfo layoutCreateInfo{};
        layoutCreateInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutCreateInfo.pNext        = nullptr;
        layoutCreateInfo.flags        = 0;
        layoutCreateInfo.bindingCount = static_cast<uint32_t>(m_DstSetBindings.size());
        layoutCreateInfo.pBindings    = m_DstSetBindings.data();

        VkDescriptorSetLayout dstLayout;
        if (!m_LayoutCache->GetOrCreateDescriptorSetLayout(layoutCreateInfo, dstLayout))
        {
            vkc::Error("Failed to create descriptor set layout while building descriptors!");
            return VkcDescriptorSetAllocation(nullptr);
        }

        VkcDescriptorSetAllocation alloc;
        if (!m_Allocator->AllocateSet(dstLayout, alloc))
        {
            vkc::Error("Failed to allocate descriptor set while building descriptors!");
            return VkcDescriptorSetAllocation(nullptr);
        }

        Update(alloc.DescriptorSet);
        return alloc;
    }

    // TODO(Adam): Reimplement this when its needed!
    void VkcDescriptorBuilder::Update(
        VkDescriptorSet& dstSet,
        uint32_t binding,
        VkDescriptorBufferInfo& info,
        VkDescriptorType type,
        uint32_t count
    ) noexcept
    {
        m_DstSetWrites.clear();

        // The dstSet gets set just before the descriptor-write is performed.
        m_DstSetWrites.emplace_back(
            VkWriteDescriptorSet{ .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                  .pNext            = nullptr,
                                  .dstSet           = nullptr,
                                  .dstBinding       = binding,
                                  .dstArrayElement  = 0,
                                  .descriptorCount  = count,
                                  .descriptorType   = type,
                                  .pImageInfo       = nullptr,
                                  .pBufferInfo      = &info,
                                  .pTexelBufferView = nullptr }
        );

        Update(dstSet);
    }

    void VkcDescriptorBuilder::Update(VkDescriptorSet& dstSet) noexcept
    {
        for (auto& write : m_DstSetWrites)
            write.dstSet = dstSet;

        vkUpdateDescriptorSets(
            m_Allocator->GetDevice()->GetDevice(),
            static_cast<uint32_t>(m_DstSetWrites.size()),
            m_DstSetWrites.data(),
            0,
            nullptr
        );
    }

    ilm::Ref<VkcDescriptorBuilder> VkcDescriptorBuilder::Create(
        const ilm::Ref<VkcDescriptorAllocator>& allocator,
        const ilm::Ref<VkcDescriptorLayoutCache>& cache
    ) noexcept
    {
        return ilm::CreateRef<VkcDescriptorBuilder>(allocator, cache);
    }
}  // namespace vkc
