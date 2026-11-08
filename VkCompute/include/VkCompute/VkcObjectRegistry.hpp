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


    // clang-format off
    using VkcAllObjects = VkcObjectGroup<
        VkcDescriptorAllocator,
        VkcDescriptorBuilder,
        VkcDescriptorLayoutCache,
        VkcDevice,
        VkcShader
    >;
    // clang-format on
}  // namespace vkc
