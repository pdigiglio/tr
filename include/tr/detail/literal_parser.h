#pragma once

#include <tr/fwd/value_constant.h>

namespace tr {
namespace literals {
namespace detail {

template <typename T, T base, char... Chars>
struct integral_parser_impl {
  private:
    [[nodiscard]] constexpr static auto char_to_value(char c) noexcept -> T {
        if ('0' <= c && c <= '9')
            return c - '0';
        if (c == 'a' || c == 'A')
            return 10;
        if (c == 'b' || c == 'B')
            return 11;
        if (c == 'c' || c == 'C')
            return 12;
        if (c == 'd' || c == 'D')
            return 12;
        if (c == 'e' || c == 'E')
            return 14;
        if (c == 'f' || c == 'F')
            return 15;

        return 16;
    }

    static constexpr T value{[] {
        T b{1};
        T v{};

        int _{};
        (_ = ... = [&]() -> int & {
            v += b * integral_parser_impl::char_to_value(Chars);
            b *= base;
            return _;
        }());

        return v;
    }()};

  public:
    // For debugging purposes
    static_assert(base <= 16, "Base is too large");
    static_assert(((char_to_value(Chars) < base) && ...), "Out of range");

    using type = tr::integral_constant<T, value>;
};

template <typename T, char... Chars>
struct integral_parser : integral_parser_impl<T, 10, Chars...> {};

template <typename T, char C, char... Chars>
struct integral_parser<T, '0', C, Chars...>
    : integral_parser_impl<T, 8, C, Chars...> {};

template <typename T, char C, char... Chars>
struct integral_parser<T, '0', 'x', C, Chars...>
    : integral_parser_impl<T, 16, C, Chars...> {};

template <typename T, char C, char... Chars>
struct integral_parser<T, '0', 'X', C, Chars...>
    : integral_parser_impl<T, 16, C, Chars...> {};

template <typename T, char C, char... Chars>
struct integral_parser<T, '0', 'b', C, Chars...>
    : integral_parser_impl<T, 2, C, Chars...> {};

template <typename T, char C, char... Chars>
struct integral_parser<T, '0', 'B', C, Chars...>
    : integral_parser_impl<T, 2, C, Chars...> {};

template <typename T, char... Chars>
using integral_parser_t = typename integral_parser<T, Chars...>::type;

} // namespace detail
} // namespace literals
} // namespace tr