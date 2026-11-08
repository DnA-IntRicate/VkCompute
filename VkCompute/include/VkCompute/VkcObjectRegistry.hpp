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
    /**
     * @brief A compile-time collection of Vkc object types.
     *
     * VkcObjectGroup provides a constrained collection of types that satisfy
     * the VkcObject concept. It can be used to centrally register Vkc
     * object types and perform compile-time introspection on the registered types.
     *
     * Every type supplied to this template must satisfy the VkcObject concept.
     * Invalid types are rejected during template instantiation.
     *
     * @tparam Objects The Vkc object types belonging to this group.
     */
    template <VkcObject... Objects>
    struct VkcObjectGroup
    {
        /**
         * @brief Indicates whether this object group is valid.
         * Always true, as invalid types cannot be used to instantiate VkcObjectGroup.
         */
        static constexpr bool Valid = true;

        /**
         * @brief The number of object types registered in this group.
         */
        static constexpr std::size_t Count = sizeof...(Objects);

        /**
         * @brief Retrieves an object type by its compile-time index.
         *
         * @tparam Index Zero-based index of the requested object type.
         *
         * @note Index must be less than Count.
         */
        template <std::size_t Index>
        using Type = std::tuple_element_t<Index, std::tuple<Objects...>>;
    };

    /**
     * @brief A compile-time collection of device-owning Vkc object types.
     *
     * VkcDeviceOwningObjectGroup provides a constrained collection of types that satisfy
     * the VkcDeviceOwningObject concept. It can be used to centrally register Vkc
     * object types and perform compile-time introspection on the registered types.
     *
     * Every type supplied to this template must satisfy the VkcDeviceOwningObject concept.
     * Invalid types are rejected during template instantiation.
     *
     * @tparam Objects The Vkc object types belonging to this group.
     */
    template <VkcDeviceOwningObject... Objects>
    struct VkcDeviceOwningObjectGroup
    {
        /**
         * @brief Indicates whether this object group is valid.
         * Always true, as invalid types cannot be used to instantiate VkcDeviceOwningObjectGroup.
         */
        static constexpr bool Valid = true;

        /**
         * @brief The number of object types registered in this group.
         */
        static constexpr std::size_t Count = sizeof...(Objects);

        /**
         * @brief Retrieves an object type by its compile-time index.
         *
         * @tparam Index Zero-based index of the requested object type.
         *
         * @note Index must be less than Count.
         */
        template <std::size_t Index>
        using Type = std::tuple_element_t<Index, std::tuple<Objects...>>;
    };

    // clang-format off

    /**
     * @brief Registry of all Vkc object types provided by the library.
     *
     * Acts as the central compile-time registry for Vkc objects.
     * All registered types must satisfy the VkcObject concept.
     *
     * This registry can be used for compile-time validation, type enumeration,
     * generic resource management, and other template-based operations.
     */
    using VkcAllObjects = VkcObjectGroup<
        VkcDescriptorAllocator,
        VkcDescriptorBuilder,
        VkcDescriptorLayoutCache,
        VkcDevice,
        VkcShader
    >;

    /**
     * @brief Registry of all Vkc objects that own a logical device reference.
     *
     * Contains the Vkc object types that satisfy the VkcDeviceOwningObject
     * concept. These objects expose access to the logical device they are
     * associated with through their GetDevice() member function.
     */
    using VkcDeviceOwningObjects = VkcDeviceOwningObjectGroup<
        VkcDescriptorAllocator,
        VkcDescriptorLayoutCache,
        VkcShader
    >;

    // clang-format on
}  // namespace vkc
