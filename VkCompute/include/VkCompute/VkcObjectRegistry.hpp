#pragma once
#include "VkcDescriptorAllocator.hpp"
#include "VkcDescriptorBuilder.hpp"
#include "VkcDescriptorLayoutCache.hpp"
#include "VkcDevice.hpp"
#include "VkcShader.hpp"
#include "VkcObject.hpp"
#include <cstdint>
#include <tuple>


namespace vkc
{
    template <VkcObject... Objects>
    struct VkcObjectGroup
    {
        static constexpr bool Valid = true;

        static constexpr size_t Count = sizeof...(Objects);

        template <size_t Index>
        using Type = std::tuple_element<Index, std::tuple<Objects...>>;
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
