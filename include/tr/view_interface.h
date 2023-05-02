#pragma once

#include <tr/fwd/view_interface.h>

#include <tr/at.h>
#include <tr/detail/type_traits.h>
#include <tr/length.h>
#include <tr/type_constant.h>
#include <tr/unimplemented.h>

#include <type_traits>
#include <utility>

namespace tr {

template <typename Derived>
struct view_interface {

    /// @brief Default constructor.
    constexpr explicit view_interface() {
        using derived_t = detail::remove_cvref_t<Derived>;
        static_assert(std::is_class_v<derived_t>,
                      "Derived must be a class type");

        static_assert(type_c<Derived> == type_c<derived_t>,
                      "Derived must not be cv qualified nor a reference");

        static_assert(is_implemented_v<length_impl<derived_t>> &&
                          is_implemented_v<at_impl<derived_t>>,
                      "Derived must be a tuple-like");
    }

    /// @brief Check if the view is empty.
    /// @returns `value_c<true>`, if it's empty; `value_c<false>`, otherwise.
    [[nodiscard]] constexpr auto empty() const {
        return value_c<length(static_cast<Derived const &>(*this)) != 0>;
    }

    [[nodiscard]] constexpr auto size() const {
        return length(static_cast<Derived const &>(*this));
    }

    template <typename T, T I>
    [[nodiscard]] constexpr decltype(auto)
    operator[](std::integral_constant<T, I> idx) & {
        return at(static_cast<Derived &>(*this), idx);
    }

    template <typename T, T I>
    [[nodiscard]] constexpr decltype(auto)
    operator[](std::integral_constant<T, I> idx) const & {
        return at(static_cast<Derived const &>(*this), idx);
    }

    template <typename T, T I>
    [[nodiscard]] constexpr decltype(auto)
    operator[](std::integral_constant<T, I> idx) && {
        return at(static_cast<Derived &&>(*this), idx);
    }

    template <typename T, T I>
    [[nodiscard]] constexpr decltype(auto)
    operator[](std::integral_constant<T, I> idx) const && {
        return at(static_cast<Derived const &&>(*this), idx);
    }
};

} // namespace tr