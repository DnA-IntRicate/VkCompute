#pragma once
#include "VkcDevice.hpp"
#include <Ilm/Ilm.hpp>
#include <concepts>
#include <string_view>
#include <type_traits>
#include <utility>


namespace vkc
{
    // clang-format off

    /**
     * @brief Defines the requirements for a named Vkc object.
     *
     * A named Vkc object must expose a public, static constexpr member
     * called `Name` that is convertible to std::string_view.
     *
     * The name must be non-empty and usable in a constant expression.
     *
     * @tparam T The type being evaluated.
     */
    template<typename T>
    concept VkcNamedObject = requires
    {
        // All objects must define a static constexpr string called 'Name'.
        { T::Name } -> std::convertible_to<std::string_view>;
        typename std::bool_constant<(T::Name.size() > 0)>;
    };

    /**
     * @brief Defines the fundamental interface requirements for a Vkc object.
     *
     * A Vkc object must satisfy VkcNamedObject and provide the following
     * public member functions:
     *
     * - `Destroy()` must return void and be noexcept.
     * - `Valid()` must return bool, be noexcept, and be callable on a const object.
     *
     * Additionally, Vkc objects must have non-throwing destructors and
     * must not satisfy the standard copy-constructible or move-constructible
     * concepts. Resource ownership and transfer are handled through the
     * ilm smart pointer types.
     *
     * @tparam T The type being evaluated.
     */
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

    /**
     * @brief Defines the requirements for a Vkc object that owns a device reference.
     *
     * A device-owning Vkc object must satisfy VkcObject and expose a public
     * `GetDevice()` member function returning exactly an ilm::Ref<VkcDevice>.
     *
     * This concept is intended for objects whose lifetime or operations
     * are associated with a logical Vulkan device.
     *
     * @tparam T The type being evaluated.
     */
    template<typename T>
    concept VkcDeviceOwningObject = requires(const T & obj)
    {
        { obj.GetDevice() } -> std::same_as<ilm::Ref<VkcDevice>>;
    } && VkcObject<T>;

    // clang-format on
}  // namespace vkc
