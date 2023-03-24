#include <tr/detail/ebo.h>

#include <tr/type_constant.h>
#include <tr/value_constant.h>

using tr::detail::ebo;
using tr::detail::ebo_traits;
using tr::type_c;

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

template <typename...>
class type_displayer;

template <typename T, typename U>
[[nodiscard]] constexpr auto same_ebo_val_type(T &&uncompressedEbo,
                                               U &&compressedEbo) noexcept {
    using type0 = decltype(get_ebo_val(std::forward<T>(uncompressedEbo)));
    using type1 = decltype(get_ebo_val(std::forward<U>(compressedEbo)));
    using type2 = decltype((std::forward<T>(uncompressedEbo).Val_));
    return type_c<type0> == type_c<type1> && type_c<type1> == type_c<type2>;
}

struct TestEbo {

    /// @brief Check `get_ebo_val` returns the same type regardles of whether
    /// the `ebo` is compressed or not.
    void value_categories() {
        ebo<S, tag_t, true> ec{};
        ebo<S, tag_t, false> e{};

        static_assert(same_ebo_val_type(e, ec));
        static_assert(same_ebo_val_type(std::as_const(e), std::as_const(ec)));
        static_assert(same_ebo_val_type(std::move(e), std::move(ec)));
        static_assert(same_ebo_val_type(std::move(std::as_const(e)),
                                        std::move(std::as_const(ec))));
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