#include <tr/detail/ebo.h>

#include <tr/type_constant.h>
#include <tr/value_constant.h>

using tr::type_c;
using tr::detail::ebo;
using tr::detail::ebo_traits;

namespace {

struct tag_t {};
struct S {};

template <typename Ebo>
static constexpr bool ebo_compressed_v{ebo_traits<Ebo>::is_compressed};

template <typename T, typename U,
          typename = std::enable_if_t<type_c<T> != type_c<U>>>
[[nodiscard]] constexpr auto same_type(T &&, U &&) noexcept {
    return tr::false_c;
}

template <typename T>
[[nodiscard]] constexpr auto same_type(T &&, T &&) noexcept {
    return tr::true_c;
}

template <typename T, typename U>
[[nodiscard]] constexpr auto same_value_type(T &&uncompressedEbo,
                                             U &&compressedEbo) noexcept {
    using type0 = decltype(std::forward<T>(uncompressedEbo).value());
    using type1 = decltype(std::forward<U>(compressedEbo).value());
    using type2 = decltype((std::forward<T>(uncompressedEbo).Val_));
    return type_c<type0> == type_c<type1> && type_c<type1> == type_c<type2>;
}

struct TestEbo {

    /// @brief Check `get_ebo_val` returns the same type regardles of whether
    /// the `ebo` is compressed or not.
    void value_categories() {
        {
            // Check that `.value()` and `get_ebo_val` returns the same type
            // regardles of whether the `ebo` is compressed or not.
            ebo<S, tag_t, true> ec{};
            ebo<S, tag_t, false> e{};

            static_assert(same_value_type(e, ec));
            static_assert(same_value_type(std::as_const(e), std::as_const(ec)));
            static_assert(same_value_type(std::move(e), std::move(ec)));
            static_assert(same_value_type(std::move(std::as_const(e)),
                                          std::move(std::as_const(ec))));
        }

        {
            ebo<S, tag_t> e{};
            static_assert(std::is_same_v<decltype(e.value()), S &>);
            static_assert(
                std::is_same_v<decltype(std::as_const(e).value()), S const &>);
            static_assert(std::is_same_v<decltype(std::move(e).value()), S &&>);
            static_assert(
                std::is_same_v<decltype(std::move(std::as_const(e)).value()),
                               S const &&>);
        }

        {
            S s{};
            ebo<S &, tag_t> e{s};
            static_assert(std::is_same_v<decltype(e.value()), S &>);
            static_assert(
                std::is_same_v<decltype(std::as_const(e).value()), S &>);
            static_assert(std::is_same_v<decltype(std::move(e).value()), S &>);
            static_assert(
                std::is_same_v<decltype(std::move(std::as_const(e)).value()),
                               S &>);
        }

        {
            S s{};
            ebo<S const &, tag_t> e{s};
            static_assert(std::is_same_v<decltype(e.value()), S const &>);
            static_assert(
                std::is_same_v<decltype(std::as_const(e).value()), S const &>);
            static_assert(
                std::is_same_v<decltype(std::move(e).value()), S const &>);
            static_assert(
                std::is_same_v<decltype(std::move(std::as_const(e)).value()),
                               S const &>);
        }

        {
            S s{};
            ebo<S &&, tag_t> e{std::move(s)};
            static_assert(std::is_same_v<decltype(e.value()), S &&>);
            static_assert(
                std::is_same_v<decltype(std::as_const(e).value()), S &&>);
            static_assert(std::is_same_v<decltype(std::move(e).value()), S &&>);
            static_assert(
                std::is_same_v<decltype(std::move(std::as_const(e)).value()),
                               S &&>);
        }
    }

    /// @brief Check `ebo` is compressed whenever possible.
    void compression() {
        struct S_final final {};
        using func_ptr_t = void (*)();

        static_assert(ebo_compressed_v<ebo<S, tag_t>>);
        static_assert(!ebo_compressed_v<ebo<S_final, tag_t>>);
        static_assert(!ebo_compressed_v<ebo<S &, tag_t>>);
        static_assert(!ebo_compressed_v<ebo<int, tag_t>>);
        static_assert(!ebo_compressed_v<ebo<func_ptr_t, tag_t>>);
    }
};

} // namespace
