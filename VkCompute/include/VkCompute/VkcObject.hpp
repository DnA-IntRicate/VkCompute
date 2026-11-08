#pragma once
#include <Ilm/Ilm.hpp>
#include <concepts>
#include <string_view>
#include <type_traits>
#include <utility>


namespace vkc
{
    // clang-format off
    template<typename T>
    concept VkcObject = requires(T obj)
    {
        // All objects must define a static constexpr string called 'Name'.
        { T::Name } -> std::convertible_to<std::string_view>;
        typename std::bool_constant<(T::Name.size() > 0)>;

        // All objects must define a public void 'Destroy' function.
        { obj.Destroy() } noexcept -> std::same_as<void>;

        // All objects must define a public bool 'Valid' function.
        { obj.Valid() } noexcept -> std::same_as<bool>;
    };
    // clang-format on
}  // namespace vkc
