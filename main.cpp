#include "folder.h"

#include <memory>
#include <tuple>
#include <iostream>
#include <sstream>

template <typename...>
void foo();

int main() {

    using tr::Combinator;

    auto const toStr = [](auto i, auto j)
    {
        return (std::stringstream{} << '(' << i << ',' << j << ')').str();
    };

    std::tuple vals{ 0,1,2,3,4 };
    auto const str = std::apply(
        [toStr](auto const& ... args) { return (Combinator{ toStr, std::string{} } | ... | args)(); },
        vals);

    std::puts(str.c_str());


    //auto c1 = std::move(c0) | 1;

	//foo<decltype(c0)>();
	//foo<decltype(c1)>();

    //auto l = [](int, int) {};
    //auto& h = "hello";
    //foo<decltype(h)>();
    //foo<decltype(std::move(h))>();

    //tr::Combinator c0{l, std::ref(h)};
    //foo<decltype(c0)>();


    //tr::Sink w{[](int, int) {}};
    //foo<decltype(w)>();
    //w.getOperator();

    //std::unique_ptr<int, void (*)(int*)> p;

    //int i{};
    //Folder_ min{[](auto i, auto j) { return fmt::format("{} {}", i, j); }, i};
    //// foo<decltype(min)>();
    //// foo<decltype(min())>();
    //// foo<decltype(std::move(min)())>();
    //// foo<decltype(min(1, apply_left))>();
    //auto f = min(1, apply_left);
    //fmt::print("{}\n", f);
//
    //f = min(1, apply_right);
    //fmt::print("{}\n", f);
}

// template <typename Op, typename ValT>
// class Folder_;
//
//
// template <typename F>
// struct is_folder : std::false_type {};
//
// template <typename Op, typename ValT>
// struct is_folder<Folder_<Op, ValT>> : std::true_type {};
//
// template <typename Op>
// struct is_folder<Folder_<Op, void>> : std::true_type {};

// template <typename Op>
// Folder_<Op, void> folder(Op&& op) {
//     return {std::forward<Op>(op)};
// }
//
// namespace detail {
// template <typename F, typename T, FoldSide S>
// decltype(auto) pipe_impl(F&& f, T&& t, apply_side_t<S> side) {
//     return std::forward<F>(f)(std::forward<T>(t), side);
// }
// }  // namespace detail
//
//// template <typename Op, typename ValT>
//// Folder_<Op, ValT> folder(Op&& op, ValT&& val) {
////     return {std::forward<Op>(op), std::forward<ValT>(val)};
//// }
//
// template <typename Op, typename T>
// auto operator|(Folder_<Op, void> fold, T&& val) -> Folder_<Op, T> {
//    return {static_cast<wrapper<Op>&>(fold).val, val};
//}
//
// template <typename Op, typename ValT, typename T>
// auto operator|(Folder_<Op, ValT> fold, T&& val)
//    -> Folder_<Op, std::invoke_result_t<Op, ValT, T>> {
//    auto&& res = static_cast<wrapper<Op>&>(fold).val(
//        static_cast<wrapper<ValT>&>(fold).val, val);
//    return {static_cast<wrapper<Op>&>(fold).val, res};
//}
//
// template <typename... BinOps>
// struct overload : BinOps... {
//    using BinOps::operator()...;
//};
//
// template <typename... BinOps>
// overload(BinOps...) -> overload<BinOps...>;
//
// int main(int, char const*[]) {
//    auto printer = [](auto i, auto j) {
//        return fmt::format("({} / {})", i, j);
//    };
//
//    float i{2};
//    float j{3};
//    float k{4};
//    auto const strRes = (folder(printer) | i | j | k)();
//    auto const res = (folder(std::divides<>{}) | i | j | k)();
//
//    fmt::print("{} = {}\n", strRes, res);
//    // return res;
//}