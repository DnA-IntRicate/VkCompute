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
    concept VkcNamedObject = requires
    {
        // All objects must define a static constexpr string called 'Name'.
        { T::Name } -> std::convertible_to<std::string_view>;
        typename std::bool_constant<(T::Name.size() > 0)>;
    };

    template<typename T>
    concept VkcObject = requires(T& obj, const T& constObj)
    {
        // All objects must define a public void 'Destroy' function.
        { obj.Destroy() } noexcept -> std::same_as<void>;

        // All objects must define a public bool 'Valid' function.
        { constObj.Valid() } noexcept -> std::same_as<bool>;

        // Destructors must never throw.
        requires std::is_nothrow_destructible_v<T>;

        // Objects must not be directly copyable or movable (only through ilm types).
        requires (!std::copy_constructible<T>);
        requires (!std::move_constructible<T>);
    } && VkcNamedObject<T>;
    // clang-format on
}  // namespace vkc
