#pragma once
#include "VkcObject.hpp"
#include "VkcDevice.hpp"
#include <Ilm/Ilm.hpp>
#include <Volk/volk.h>
#include <unordered_map>
#include <vector>


namespace vkc
{
    /**
     * @brief Stores a vector of VkDescriptorSetLayoutBinding.
     */
    struct VkcDescriptorLayoutInfo
    {
        constexpr VkcDescriptorLayoutInfo(const std::vector<VkDescriptorSetLayoutBinding>& bindings) noexcept
            : Bindings(bindings)
        {
        }

        constexpr VkcDescriptorLayoutInfo(const VkcDescriptorLayoutInfo&)            = default;
        constexpr VkcDescriptorLayoutInfo& operator=(const VkcDescriptorLayoutInfo&) = default;

        constexpr VkcDescriptorLayoutInfo(VkcDescriptorLayoutInfo&&)            = default;
        constexpr VkcDescriptorLayoutInfo& operator=(VkcDescriptorLayoutInfo&&) = default;

        constexpr VkcDescriptorLayoutInfo() = default;

        /**
         * @brief The hash function for this type.
         */
        size_t Hash() const noexcept
        {
            size_t seed = Bindings.size();
            const auto combine = [&seed](size_t value) { seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2); };

            for (const auto& binding : Bindings)
            {
                combine(std::hash<uint32_t>{}(binding.binding));
                combine(std::hash<int>{}(static_cast<int>(binding.descriptorType)));
                combine(std::hash<uint32_t>{}(binding.descriptorCount));
                combine(std::hash<uint32_t>{}(binding.stageFlags));
            }

            return seed;
        }

        /**
         * @brief Checks for equality between this instance and other.
         */
        constexpr bool operator==(const VkcDescriptorLayoutInfo& other) const noexcept
        {
            if (Bindings.size() != other.Bindings.size())
                return false;

            for (size_t i = 0; i < Bindings.size(); ++i)
            {
                const VkDescriptorSetLayoutBinding& a = Bindings[i];
                const VkDescriptorSetLayoutBinding& b = other.Bindings[i];

                if ((a.binding != b.binding) || (a.descriptorType != b.descriptorType) || (a.descriptorCount != b.descriptorCount)
                    || (a.stageFlags != b.stageFlags))
                    return false;
            }

            return true;
        }

        /**
         * @brief Checks for inequality between this instance and other.
         */
        constexpr bool operator!=(const VkcDescriptorLayoutInfo& other) const noexcept { return (*this) == other; }

        /**
         * @brief The vector of descriptor set layout bindings.
         */
        std::vector<VkDescriptorSetLayoutBinding> Bindings;
    };

    /**
     * @brief Creates, stores and caches descriptor set layouts for quick reuse.
     */
    class VkcDescriptorLayoutCache
    {
    public:
        VkcDescriptorLayoutCache() = default;
        ~VkcDescriptorLayoutCache() noexcept;

        // Allow copy-assignment by ilm::Ref only
        constexpr VkcDescriptorLayoutCache(const VkcDescriptorLayoutCache&)            = delete;
        constexpr VkcDescriptorLayoutCache& operator=(const VkcDescriptorLayoutCache&) = delete;

        // Allow move-assignment by ilm::Ref only
        constexpr VkcDescriptorLayoutCache(VkcDescriptorLayoutCache&&)            = delete;
        constexpr VkcDescriptorLayoutCache& operator=(VkcDescriptorLayoutCache&&) = delete;

        /**
         * @brief Creates a new descriptor set layout or returns the layout if it already exists.
         *
         * @param info The descriptor set creation info struct.
         * @param outDstSetLayout The reference to the returned descriptor set layout.
         *
         * @returns True if the descriptor set layout was found or created successfully, false otherwise.
         */
        bool CreateDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo& info, VkDescriptorSetLayout& outDstSetLayout) noexcept;

        /**
         * @brief Destroys this descriptor set layout cache.
         */
        void Destroy() noexcept;

        /**
         * @brief Checks if this descriptor set layout cache is valid.
         */
        constexpr bool Valid() const noexcept { return (bool)m_Device && m_Device->Valid() && !m_LayoutCache.empty(); }

    public:
        /**
         * @brief Creates a new descriptor set layout cache.
         */
        static ilm::Ref<VkcDescriptorLayoutCache> Create(const ilm::Ref<VkcDevice>& device) noexcept;

    private:
        // Hash struct for unordered_map
        struct VkcDescriptorLayoutHash
        {
            size_t operator()(const VkcDescriptorLayoutInfo& layout) const noexcept { return layout.Hash(); }
        };

    private:
        std::unordered_map<VkcDescriptorLayoutInfo, VkDescriptorSetLayout, VkcDescriptorLayoutHash> m_LayoutCache;
        ilm::Ref<VkcDevice> m_Device;
    };

    static_assert(VkcObject<VkcDescriptorLayoutCache>);
    static_assert(VkcCreatable<VkcDescriptorLayoutCache, const ilm::Ref<VkcDevice>&>);
}  // namespace vkc
