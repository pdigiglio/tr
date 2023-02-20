#pragma once 

#include "./detail/type_traits.h"
#include "./detail/utility.h"

#include <utility>

namespace tr
{
	namespace detail
	{
		enum struct FoldSide
		{
			Left,
			Right
		};

		template <FoldSide S>
		struct ApplySideTag {};
		static constexpr ApplySideTag<FoldSide::Left> apply_left{};
		static constexpr ApplySideTag<FoldSide::Right> apply_right{};

		/// @brief A wrapper class to hold either a value type (e.g. T == int)
		/// or a reference type (e.g. T == int&).
		///
		/// @tparam T The held type.
		template <typename T>
		struct Sink {
			T val;

			//template <
			//	typename Self,
			//	typename = std::enable_if_t<std::is_same_v<remove_cvref_t<Self>, Sink<T>>>>
			//static decltype(auto) fw_get(Self&& self) {
			//	// If T is not a reference type, you can also do:
			//	//return std::invoke(&Sink<T>::val, std::forward<Self>(self));
			//	//
			//	// If T is a reference type, the code above won't compile.

			//	return (std::forward<Self>(self).val);
			//	//     ^ note the parenthesis
			//	//
			//	// When T is a value type, I need the parenthesis to return
			//	// `val` with the same value category as `Self`. E.g., when T
			//	// is int:
			//	//  * decltype(std::forward<Self>(self).val) is always int
			//	//  * decltype((std::forward<Self>(self).val)) is:
			//	//    - int& if Self is lvalue to non-const;
			//	//    - int const& if Self is lvalue to const;
			//	//    - int&& if Self is rvalue to non-const;
			//	//    - int const&& if Self is rvalue to const;
			//	//
			//	// When T is a reference type, the returned type will be T.
			//	//
			//}
		};

		template <
			typename BinaryOp,
			typename Value,
			bool = !std::is_reference_v<BinaryOp> && !std::is_final_v<BinaryOp> && std::is_empty_v<BinaryOp>>
		struct CombinatorStorage : BinaryOp
		{
			Value Value_;
		};

		template <typename BinaryOp, typename Value>
		struct CombinatorStorage<BinaryOp, Value, false>
		{
			BinaryOp Operator_;
			Value Value_;
		};

		template <typename BinaryOp, typename Value, bool IsCompressed>
		constexpr decltype(auto) get_operator(CombinatorStorage<BinaryOp, Value, IsCompressed>& storage) noexcept
		{
			if constexpr (IsCompressed)
			{
				return static_cast<BinaryOp&>(storage);
			}
			else
			{
				return (storage.Operator_);
			}
		}

		template <typename BinaryOp, typename Value, bool IsCompressed>
		constexpr decltype(auto) get_operator(CombinatorStorage<BinaryOp, Value, IsCompressed> const& storage) noexcept
		{
			if constexpr (IsCompressed)
			{
				return static_cast<BinaryOp const&>(storage);
			}
			else
			{
				return (storage.Operator_);
			}
		}
	}

	template <typename BinaryOp, typename ValT>
	struct Combinator : detail::CombinatorStorage<BinaryOp, ValT>
	{
	private:
		using base_t = detail::CombinatorStorage<BinaryOp, ValT>;

		template <typename Comb, typename Arg, detail::FoldSide Side>
		static decltype(auto) call_op(Comb&& comb, Arg&& arg, detail::ApplySideTag<Side>)
		{
			if constexpr (Side == detail::FoldSide::Left)
			{
				return get_operator(comb)(std::forward<Comb>(comb)(), std::forward<Arg>(arg));
			}
			else
			{
				return get_operator(comb)(std::forward<Arg>(arg), std::forward<Comb>(comb)());
			}
		}

		template <typename Comb>
		static decltype(auto) get_val(Comb&& comb)
		{
			return std::forward<Comb>(comb).Value_;
		}

	public:

		decltype(auto) operator()()&
		{
			return Combinator::get_val(*this);
		}

		decltype(auto) operator()() const&
		{
			return Combinator::get_val(*this);
		}

		decltype(auto) operator()()&&
		{
			return Combinator::get_val(std::move(*this));
		}

		decltype(auto) operator()() const&&
		{
			return Combinator::get_val(std::move(*this));
		}

		template <typename Arg, detail::FoldSide Side>
		decltype(auto) operator()(Arg&& arg, detail::ApplySideTag<Side> side)
		{
			return Combinator::call_op(*this, std::forward<Arg>(arg), side);
		}

		template <typename Arg, detail::FoldSide Side>
		decltype(auto) operator()(Arg&& arg, detail::ApplySideTag<Side> side) const
		{
			return Combinator::call_op(*this, std::forward<Arg>(arg), side);
		}
	};

	template <typename BinaryOp>
	struct Combinator<BinaryOp, void> : detail::Sink<BinaryOp>
	{
		// No value to get
		constexpr void operator()() const noexcept {}

		template <typename T, detail::FoldSide S>
		constexpr T&& operator()(T&& t, detail::ApplySideTag<S>) const noexcept
		{
			return std::forward<T>(t);
		}
	};

	template <typename BinaryOp>
	Combinator(BinaryOp&&) -> Combinator<BinaryOp, void>;

	template <typename BinaryOp, typename Val>
	Combinator(BinaryOp&&, Val&&) -> Combinator<BinaryOp, Val>;

	namespace detail
	{
		template <typename Comb, typename Arg, FoldSide Side>
		constexpr decltype(auto) pipe_impl(Comb&& comb, Arg&& arg, ApplySideTag<Side> side)
		{
			return Combinator
			{
				forward_like<Comb>(get_operator(comb)),
				std::forward<Comb>(comb)(std::forward<Arg>(arg), side)
			};
		}
	}

	template <typename BinaryOp, typename ValT>
	struct Combinator;

	template <typename>
	struct IsCombinator : std::false_type {};

	template <typename BinaryOp, typename ValT>
	struct IsCombinator<Combinator<BinaryOp, ValT>> : std::true_type {};

	template <
		typename Comb,
		typename Arg,
		typename = std::enable_if_t<IsCombinator<detail::remove_cvref_t<Comb>>::value>>
	constexpr decltype(auto) operator|(Comb&& comb, Arg&& arg)
	{
		return detail::pipe_impl(std::forward<Comb>(comb), std::forward<Arg>(arg), detail::apply_left);
	}

	template <
		typename Comb,
		typename Arg,
		typename = std::enable_if_t<IsCombinator<detail::remove_cvref_t<Comb>>::value>>
	constexpr decltype(auto) operator|(Arg&& arg, Comb&& comb)
	{
		return detail::pipe_impl(std::forward<Comb>(comb), std::forward<Arg>(arg), detail::apply_right);
	}
}
