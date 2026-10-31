#include <VkCompute/VkcDescriptorLayoutCache.hpp>
#include <VkCompute/Log.hpp>
#include <algorithm>
#include <functional>
#include <ranges>


namespace vkc
{
    VkcDescriptorLayoutCache::~VkcDescriptorLayoutCache() noexcept
    {
        Destroy();
    }

    void VkcDescriptorLayoutCache::Destroy() noexcept
    {
        for (auto& [info, layout] : m_LayoutCache)
            if (layout)
                vkDestroyDescriptorSetLayout(m_Device->GetDevice(), layout, nullptr);

        m_LayoutCache.clear();
        m_Device = nullptr;
    }

    bool VkcDescriptorLayoutCache::CreateDescriptorSetLayout(
        VkDescriptorSetLayoutCreateInfo& info,
        VkDescriptorSetLayout& outDstSetLayout
    ) noexcept
    {
        VkcDescriptorLayoutInfo layoutInfo;
        layoutInfo.Bindings.resize(info.bindingCount);

        bool isSorted   = true;
        int lastBinding = -1;

        for (auto&& [i, dstBinding] : std::views::enumerate(layoutInfo.Bindings))
        {
            dstBinding = info.pBindings[i];
            if (dstBinding.binding > lastBinding)
                lastBinding = dstBinding.binding;
            else
                isSorted = false;
        }

        if (!isSorted)
            std::ranges::sort(
                layoutInfo.Bindings,
                [](VkDescriptorSetLayoutBinding& a, VkDescriptorSetLayoutBinding& b) { return a.binding < b.binding; }
            );

        // Fast cache lookup - only hash once
        if (const auto it = m_LayoutCache.find(layoutInfo); it != m_LayoutCache.end())
        {
            outDstSetLayout = it->second;
            return true;
        }

        VkResult err = vkCreateDescriptorSetLayout(m_Device->GetDevice(), &info, nullptr, &outDstSetLayout);
        if (err != VK_SUCCESS)
        {
            vkc::Error("Failed to create descriptor set layout! : {:#x}.", static_cast<int>(err));
            outDstSetLayout = nullptr;

            return false;
        }

        m_LayoutCache[layoutInfo] = outDstSetLayout;
        return true;
    }

    ilm::Ref<VkcDescriptorLayoutCache> VkcDescriptorLayoutCache::Create(const ilm::Ref<VkcDevice>& device) noexcept
    {
        return ilm::CreateRef<VkcDescriptorLayoutCache>(device);
    }
}  // namespace vkc
