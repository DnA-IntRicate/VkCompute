#pragma once
#include <Ilm/Ilm.hpp>
#include <concepts>
#include <utility>


namespace vkc
{
    // clang-format off
    template<typename T>
    concept VkcObject = requires(T obj)
    {
        { obj.Destroy() } noexcept -> std::same_as<void>;
        { obj.Valid() } noexcept -> std::same_as<bool>;
    };
    // clang-format on
}  // namespace vkc
