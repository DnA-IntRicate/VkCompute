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
        { obj.Destroy() } -> std::same_as<void>;
        { obj.Valid() } -> std::same_as<bool>;
    };

    template<typename T, typename... Args>
    concept VkcCreatable = requires(Args&&... args)
    {
        { T::Create(std::forward<Args>(args)...) } -> std::same_as<ilm::Ref<T>>;
    };
    // clang-format on
}  // namespace vkc
