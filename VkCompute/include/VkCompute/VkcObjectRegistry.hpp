#pragma once
#include "VkcDescriptorAllocator.hpp"
#include "VkcDescriptorBuilder.hpp"
#include "VkcDescriptorLayoutCache.hpp"
#include "VkcDevice.hpp"
#include "VkcShader.hpp"
#include "VkcObject.hpp"
#include <tuple>


namespace vkc
{
    template <VkcObject... Objects>
    struct VkcObjectGroup
    {
    };

    using VkcAllObjects = VkcObjectGroup<VkcDescriptorAllocator, VkcDescriptorBuilder, VkcDescriptorLayoutCache, VkcDevice, VkcShader>;

}  // namespace vkc
