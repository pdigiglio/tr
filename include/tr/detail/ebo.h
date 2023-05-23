#pragma once

#include <tr/detail/type_traits.h>
#include <tr/detail/utility.h>
#include <tr/forward_as_base.h>

#include <type_traits>

namespace tr {
namespace detail {

template <typename T, typename Tag,
          bool IsCompressed =
              std::is_void_v<T> || (!std::is_reference_v<T> &&
                                    !std::is_final_v<T> && std::is_empty_v<T>)>
struct ebo : T {
    [[nodiscard]] constexpr auto value() &noexcept -> T & { return *this; }
    [[nodiscard]] constexpr auto value() const &noexcept -> T const & {
        return *this;
    }

    [[nodiscard]] constexpr auto value() &&noexcept -> T && {
        return std::move(*this);
    }
    [[nodiscard]] constexpr auto value() const &&noexcept -> T const && {
        return std::move(*this);
    }
};

template <typename Tag>
struct ebo<void, Tag, true> {};

template <typename T, typename Tag>
struct ebo<T, Tag, false> {
    T Val_;

    [[nodiscard]] constexpr auto value() &noexcept -> T & { return this->Val_; }
    [[nodiscard]] constexpr auto value() const &noexcept -> T const & {
        return this->Val_;
    }

    [[nodiscard]] constexpr auto value() &&noexcept -> T && {
        return std::move(*this).Val_;
    }
    [[nodiscard]] constexpr auto value() const &&noexcept -> T const && {
        return std::move(*this).Val_;
    }
};

template <typename T, typename Tag>
struct ebo<T &&, Tag, false> {
    T &&Val_;

    [[nodiscard]] constexpr auto value() const noexcept -> T && {
        return static_cast<T &&>(this->Val_);
    }
};

template <typename T, typename Tag>
struct ebo<T &, Tag, false> {
    T &Val_;

    [[nodiscard]] constexpr auto value() const noexcept -> T & {
        return this->Val_;
    }
};

template <typename>
struct is_ebo : std::false_type {};

template <typename T, typename Tag, bool IsCompressed>
struct is_ebo<ebo<T, Tag, IsCompressed>> : std::true_type {};

template <typename T>
static constexpr bool is_ebo_v{is_ebo<T>::value};

template <typename>
struct ebo_traits;

template <typename T, typename Tag, bool IsCompressed>
struct ebo_traits<ebo<T, Tag, IsCompressed>> {
    using type = T;
    using tag = Tag;
    static constexpr bool is_compressed{IsCompressed};
};

} // namespace detail
} // namespace tr
