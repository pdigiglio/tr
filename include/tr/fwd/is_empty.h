#pragma once

#include <tr/unimplemented.h>

namespace tr {

template <typename, typename = void>
struct is_empty_impl : unimplemented {
    template <typename Tuple>
    static auto apply(Tuple &&) = delete;
};

struct is_empty_t {
    template <typename Tuple>
    [[nodiscard]] constexpr auto operator()(Tuple &&) const -> decltype(auto);
};

static constexpr is_empty_t is_empty{};

} // namespace tr