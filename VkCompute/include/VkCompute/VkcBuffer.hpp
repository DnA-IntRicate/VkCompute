#pragma once
#include "VkcBufferFactory.hpp"
#include "VkcDevice.hpp"
#include <Ilm/Ilm.hpp>
#include <vulkan/vulkan_core.h>
#include <string_view>
#include <vector>


namespace vkc
{
    // Should we template this out too?
    class VkcBuffer
    {
    public:
        static constexpr std::string_view Name = "VkcBuffer";

    public:
        // Usual methods here....
        // SetData goes here! NOT in the factory
        constexpr VkcBuffer() = default;

    private:
        VkBuffer m_Buffer          = nullptr;
        VkDescriptorSet m_DstSet   = nullptr;
        VmaAllocation m_Allocation = nullptr;
        VmaAllocationInfo m_AllocationInfo{};

        // The buffer should probably also contain a device reference?
        // That MUST be populated from the buffer factory!

        std::string m_DebugName;
        VkcBufferKind m_Kind = VkcBufferKind::None;

    private:
        template<VkcBufferKind Kind>
        friend class VkcBufferFactory;
    };
}  // namespace vkc
