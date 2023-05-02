#pragma once

#include <tr/detail/ebo.h>
#include <tr/detail/type_traits.h>
#include <tr/detail/utility.h>

#include <utility>

namespace tr {
namespace detail {
enum struct fold_side { left, right };

template <fold_side S>
struct apply_side_tag {};
static constexpr apply_side_tag<fold_side::left> apply_left{};
static constexpr apply_side_tag<fold_side::right> apply_right{};

template <std::size_t>
struct combinator_tag;

template <typename BinaryOp>
struct compressed_op : ebo<BinaryOp, combinator_tag<0>> {
    constexpr decltype(auto) get_operator() &noexcept {
        return compressed_op::get_operator_impl(*this);
    }

    constexpr decltype(auto) get_operator() const &noexcept {
        return compressed_op::get_operator_impl(*this);
    }

    constexpr decltype(auto) get_operator() &&noexcept {
        return compressed_op::get_operator_impl(std::move(*this));
    }

    constexpr decltype(auto) get_operator() const &&noexcept {
        return compressed_op::get_operator_impl(std::move(*this));
    }

  private:
    template <typename CompOp>
    static constexpr decltype(auto)
    get_operator_impl(CompOp &&compOp) noexcept {
        using ebo_t = ebo<BinaryOp, combinator_tag<0>>;
        return get_ebo_val(forward_as_base<ebo_t, CompOp>(compOp));
    }
};
} // namespace detail

template <typename BinaryOp, typename ValT>
struct combinator : detail::compressed_op<BinaryOp>,
                    detail::ebo<ValT, detail::combinator_tag<1>> {
  private:
    template <typename Comb, typename Arg, detail::fold_side Side>
    constexpr static decltype(auto) call_op(Comb &&comb, Arg &&arg,
                                            detail::apply_side_tag<Side>) {
        if constexpr (Side == detail::fold_side::left) {
            return comb.get_operator()(std::forward<Comb>(comb)(),
                                       std::forward<Arg>(arg));
        } else {
            return comb.get_operator()(std::forward<Arg>(arg),
                                       std::forward<Comb>(comb)());
        }
    }

    template <typename Comb>
    constexpr static decltype(auto) get_val(Comb &&comb) noexcept {
        using ebo_t = detail::ebo<ValT, detail::combinator_tag<1>>;
        return get_ebo_val(forward_as_base<ebo_t, Comb>(comb));
    }

  public:
    using detail::compressed_op<BinaryOp>::get_operator;

    constexpr decltype(auto) operator()() & {
        return combinator::get_val(*this);
    }

    constexpr decltype(auto) operator()() const & {
        return combinator::get_val(*this);
    }

    constexpr decltype(auto) operator()() && {
        return combinator::get_val(std::move(*this));
    }

    constexpr decltype(auto) operator()() const && {
        return combinator::get_val(std::move(*this));
    }

    template <typename Arg, detail::fold_side Side>
    constexpr decltype(auto) operator()(Arg &&arg,
                                        detail::apply_side_tag<Side> side) {
        return combinator::call_op(*this, std::forward<Arg>(arg), side);
    }

    template <typename Arg, detail::fold_side Side>
    constexpr decltype(auto)
    operator()(Arg &&arg, detail::apply_side_tag<Side> side) const {
        return combinator::call_op(*this, std::forward<Arg>(arg), side);
    }
};

template <typename BinaryOp>
struct combinator<BinaryOp, void> : detail::compressed_op<BinaryOp> {
    using detail::compressed_op<BinaryOp>::get_operator;

    // No value to get
    constexpr void operator()() const noexcept {}

    template <typename T, detail::fold_side S>
    constexpr T &&operator()(T &&t, detail::apply_side_tag<S>) const noexcept {
        return std::forward<T>(t);
    }
};

template <typename BinaryOp>
combinator(BinaryOp &&) -> combinator<BinaryOp, void>;

template <typename BinaryOp, typename Val>
combinator(BinaryOp &&, Val &&) -> combinator<BinaryOp, Val>;

namespace detail {
template <typename Comb, typename Arg, fold_side Side>
constexpr decltype(auto) pipe_impl(Comb &&comb, Arg &&arg,
                                   apply_side_tag<Side> side) {
    return combinator{std::forward<Comb>(comb).get_operator(),
                      std::forward<Comb>(comb)(std::forward<Arg>(arg), side)};
}
} // namespace detail

template <typename>
struct is_combinator : std::false_type {};

template <typename BinaryOp, typename ValT>
struct is_combinator<combinator<BinaryOp, ValT>> : std::true_type {};

template <typename Comb, typename Arg,
          typename = std::enable_if_t<
              is_combinator<detail::remove_cvref_t<Comb>>::value>>
constexpr decltype(auto) operator|(Comb &&comb, Arg &&arg) {
    return detail::pipe_impl(std::forward<Comb>(comb), std::forward<Arg>(arg),
                             detail::apply_left);
}

template <typename Comb, typename Arg,
          typename = std::enable_if_t<
              is_combinator<detail::remove_cvref_t<Comb>>::value>>
constexpr decltype(auto) operator|(Arg &&arg, Comb &&comb) {
    return detail::pipe_impl(std::forward<Comb>(comb), std::forward<Arg>(arg),
                             detail::apply_right);
}
} // namespace tr
