#pragma once

#include "fwd/all_of.h"

#include "../fwd/at.h"
#include "../fwd/length.h"

#include "../detail/type_traits.h"
#include "../indices_for.h"

#include <utility>

namespace tr {

template <typename T>
struct all_of_impl<T, std::enable_if_t<is_implemented_v<at_impl<T>> &&
                                       is_implemented_v<length_impl<T>>>> {
    template <typename Tuple, typename Predicate, std::size_t... Is>
    static constexpr auto apply(Tuple &&tuple, Predicate &&predicate,
                                std::index_sequence<Is...>) {

        return (std::forward<Predicate>(predicate)(
                    at_c<Is>(std::forward<Tuple>(tuple))) &&
                ...);
    }

    template <typename Tuple, typename Predicate>
    static constexpr auto apply(Tuple &&tuple, Predicate &&predicate) {
        return all_of_impl::apply(std::forward<Tuple>(tuple),
                                  std::forward<Predicate>(predicate),
                                  indices_for(tuple));
    }
};

template <typename Tuple, typename Predicate>
[[nodiscard]] constexpr auto all_of_t::operator()(Tuple &&tuple,
                                                  Predicate &&predicate) const
    -> decltype(auto) {
    using tuple_t = detail::remove_cvref_t<Tuple>;
    return all_of_impl<tuple_t>::apply(std::forward<Tuple>(tuple),
                                       std::forward<Predicate>(predicate));
}

} // namespace tr