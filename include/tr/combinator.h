#pragma once

#include <tr/fwd/combinator.h>

#include <tr/detail/ebo.h>
#include <tr/forward_as_base.h>
#include <tr/macros.h>
#include <tr/overloaded.h>

#include <type_traits>

namespace tr {

namespace detail {
struct combinator_tag /*unimplemented*/;
} // namespace detail

template <typename BinaryOp, typename ValT>
struct TR_EMPTY_BASES combinator : detail::CallableWrapper<BinaryOp>,
                                   detail::ebo<ValT, detail::combinator_tag> {

  private:
    using callable_base_t = detail::CallableWrapper<BinaryOp>;
    using ebo_base_t = detail::ebo<ValT, detail::combinator_tag>;

    template <typename Comb, typename Arg, bool IsLeft>
    constexpr static auto apply(Comb &&, Arg &&arg, std::true_type /*isVoid*/,
                                std::bool_constant<IsLeft> /*isLeft*/) noexcept
        -> Arg && {
        return static_cast<Arg &&>(arg);
    }

    template <typename Comb, typename Arg>
    constexpr static auto apply(Comb &&comb, Arg &&arg,
                                std::false_type /*isVoid*/,
                                std::true_type /*isLeft*/) -> decltype(auto) {
        return static_cast<Comb &&>(comb).callable_base_t::operator()(
            static_cast<Comb &&>(comb).ebo_base_t::value(),
            static_cast<Arg &&>(arg));
    }

    template <typename Comb, typename Arg, typename Val_>
    constexpr static auto apply(Comb &&comb, Arg &&arg,
                                std::false_type /*isVoid*/,
                                std::false_type /*isLeft*/) -> decltype(auto) {
        return static_cast<Comb &&>(comb).callable_base_t::operator()(
            static_cast<Arg &&>(arg),
            static_cast<Comb &&>(comb).ebo_base_t::value());
    }

    template <typename Comb, typename Arg, bool IsLeft>
    constexpr static auto pipe_impl(Comb &&comb, Arg &&arg,
                                    std::integral_constant<bool, IsLeft> isLeft)
        -> decltype(auto) {

        typename std::is_void<ValT>::type isVoid{};
        using res_t = decltype(combinator::apply(static_cast<Comb &&>(comb),
                                                 static_cast<Arg &&>(arg),
                                                 isVoid, isLeft));
        return combinator<BinaryOp, res_t>{
            forward_as_base<BinaryOp, Comb>(static_cast<Comb &&>(comb)),
            combinator::apply(static_cast<Comb &&>(comb),
                              static_cast<Arg &&>(arg), isVoid, isLeft)};
    }

    template <typename Comb, typename Arg>
    constexpr static auto left_pipe_impl(Comb &&comb, Arg &&arg)
        -> decltype(auto) {
        std::true_type isLeft{};
        return combinator::pipe_impl(static_cast<Comb &&>(comb),
                                     static_cast<Arg &&>(arg), isLeft);
    }

    template <typename Comb, typename Arg>
    constexpr static auto right_pipe_impl(Comb &&comb, Arg &&arg)
        -> decltype(auto) {
        std::false_type isLeft{};
        return combinator::pipe_impl(static_cast<Comb &&>(comb),
                                     static_cast<Arg &&>(arg), isLeft);
    }

  public:
#if defined(__GNUC__)
    // These constructors are only needed as a GCC work-around
    template <typename Op, typename V, typename Val_ = ValT,
              typename = std::enable_if_t<!std::is_void_v<Val_>>>
    constexpr explicit combinator(Op &&op_, V &&val_)
        : callable_base_t{static_cast<Op &&>(op_)},
          ebo_base_t{static_cast<V &&>(val_)} {}

    template <typename Op, typename Val_ = ValT,
              typename = std::enable_if_t<std::is_void_v<Val_>>>
    constexpr explicit combinator(Op &&op_)
        : callable_base_t{static_cast<Op &&>(op_)}, ebo_base_t{} {}

#endif // defined(__GNUC__)

    template <typename Arg>
    friend constexpr auto operator|(combinator const &comb, Arg &&arg) {
        return left_pipe_impl(comb, static_cast<Arg &&>(arg));
    }

    template <typename Arg>
    friend constexpr auto operator|(combinator &comb, Arg &&arg) {
        return left_pipe_impl(comb, static_cast<Arg &&>(arg));
    }

    template <typename Arg>
    friend constexpr auto operator|(combinator &&comb, Arg &&arg) {
        return left_pipe_impl(std::move(comb), static_cast<Arg &&>(arg));
    }

    template <typename Arg>
    friend constexpr auto operator|(combinator const &&comb, Arg &&arg) {
        return left_pipe_impl(std::move(comb), static_cast<Arg &&>(arg));
    }

    template <typename Arg>
    friend constexpr auto operator|(Arg &&arg, combinator const &comb) {
        return right_pipe_impl(comb, static_cast<Arg &&>(arg));
    }

    template <typename Arg>
    friend constexpr auto operator|(Arg &&arg, combinator &comb) {
        return right_pipe_impl(comb, static_cast<Arg &&>(arg));
    }

    template <typename Arg>
    friend constexpr auto operator|(Arg &&arg, combinator &&comb) {
        return right_pipe_impl(std::move(comb), static_cast<Arg &&>(arg));
    }

    template <typename Arg>
    friend constexpr auto operator|(Arg &&arg, combinator const &&comb) {
        return right_pipe_impl(std::move(comb), static_cast<Arg &&>(arg));
    }
};

template <typename BinaryOp>
combinator(BinaryOp) -> combinator<BinaryOp, void>;

template <typename BinaryOp, typename Val>
combinator(BinaryOp, Val &&) -> combinator<BinaryOp, Val>;

} // namespace tr
