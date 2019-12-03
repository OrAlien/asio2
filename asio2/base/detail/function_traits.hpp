/*
 * COPYRIGHT (C) 2017-2019, zhllxt
 *
 * author   : zhllxt
 * email    : 37792738@qq.com
 * 
 * Distributed under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
 * (See accompanying file LICENSE or see <http://www.gnu.org/licenses/>)
 *
 * refrenced from : https://github.com/qicosmos/rest_rpc/blob/master/include/meta_util.hpp
 */

#ifndef __ASIO2_FUNCTION_TRAITS_HPP__
#define __ASIO2_FUNCTION_TRAITS_HPP__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <functional>
#include <type_traits>

namespace asio2::detail
{
	/*
	 * 1. function type								==>	Ret(Args...)
	 * 2. function pointer							==>	Ret(*)(Args...)
	 * 3. function reference						==>	Ret(&)(Args...)
	 * 4. pointer to non-static member function		==> Ret(T::*)(Args...)
	 * 5. function object and functor				==> &T::operator()
	 * 6. function with generic operator call		==> template <typeanme ... Args> &T::operator()
	 */
	template<typename, typename = void>
	struct function_traits { static constexpr bool is_callable = false; };

	template<typename Ret, typename... Args>
	struct function_traits<Ret(Args...)>
	{
	public:
		static constexpr std::size_t argc = sizeof...(Args);
		static constexpr bool is_callable = true;

		typedef Ret function_type(Args...);
		typedef Ret return_type;
		using stl_function_type = std::function<function_type>;
		typedef Ret(*pointer)(Args...);

		template<std::size_t I>
		struct args
		{
			static_assert(I < argc, "index is out of range, index must less than sizeof Args");
			using type = typename std::tuple_element<I, std::tuple<Args...>>::type;
		};

		typedef std::tuple<Args...> tuple_type;
		typedef std::tuple<std::remove_cv_t<std::remove_reference_t<Args>>...> pod_tuple_type;
	};

	// regular function pointer
	template<typename Ret, typename... Args>
	struct function_traits<Ret(*)(Args...)> : function_traits<Ret(Args...)> {};

	// non-static member function pointer
	template <typename Ret, typename Class, typename... Args>
	struct function_traits<Ret(Class::*)(Args...)> : function_traits<Ret(Args...)> {};

	template <typename Ret, typename Class, typename... Args>
	struct function_traits<Ret(Class::*)(Args...) const> : function_traits<Ret(Args...)> {};

	template <typename Ret, typename Class, typename... Args>
	struct function_traits<Ret(Class::*)(Args...) volatile> : function_traits<Ret(Args...)> {};

	template <typename Ret, typename Class, typename... Args>
	struct function_traits<Ret(Class::*)(Args...) const volatile> : function_traits<Ret(Args...)> {};

	template <typename Ret, typename Class, typename... Args>
	struct function_traits<Ret(Class::*)(Args...) &> : function_traits<Ret(Args...)> {};

	template <typename Ret, typename Class, typename... Args>
	struct function_traits<Ret(Class::*)(Args...) const &> : function_traits<Ret(Args...)> {};

	template <typename Ret, typename Class, typename... Args>
	struct function_traits<Ret(Class::*)(Args...) volatile &> : function_traits<Ret(Args...)> {};

	template <typename Ret, typename Class, typename... Args>
	struct function_traits<Ret(Class::*)(Args...) const volatile &> : function_traits<Ret(Args...)> {};

	template <typename Ret, typename Class, typename... Args>
	struct function_traits<Ret(Class::*)(Args...) && > : function_traits<Ret(Args...)> {};

	template <typename Ret, typename Class, typename... Args>
	struct function_traits<Ret(Class::*)(Args...) const &&> : function_traits<Ret(Args...)> {};

	template <typename Ret, typename Class, typename... Args>
	struct function_traits<Ret(Class::*)(Args...) volatile &&> : function_traits<Ret(Args...)> {};

	template <typename Ret, typename Class, typename... Args>
	struct function_traits<Ret(Class::*)(Args...) const volatile &&> : function_traits<Ret(Args...)> {};

	// non-static member function pointer -- noexcept versions for (C++17 and later)
	template <typename Ret, typename Class, typename... Args>
	struct function_traits<Ret(Class::*)(Args...) noexcept> : function_traits<Ret(Args...)> {};

	template <typename Ret, typename Class, typename... Args>
	struct function_traits<Ret(Class::*)(Args...) const noexcept> : function_traits<Ret(Args...)> {};

	template <typename Ret, typename Class, typename... Args>
	struct function_traits<Ret(Class::*)(Args...) volatile noexcept> : function_traits<Ret(Args...)> {};

	template <typename Ret, typename Class, typename... Args>
	struct function_traits<Ret(Class::*)(Args...) const volatile noexcept> : function_traits<Ret(Args...)> {};

	template <typename Ret, typename Class, typename... Args>
	struct function_traits<Ret(Class::*)(Args...) & noexcept> : function_traits<Ret(Args...)> {};

	template <typename Ret, typename Class, typename... Args>
	struct function_traits<Ret(Class::*)(Args...) const & noexcept> : function_traits<Ret(Args...)> {};

	template <typename Ret, typename Class, typename... Args>
	struct function_traits<Ret(Class::*)(Args...) volatile & noexcept> : function_traits<Ret(Args...)> {};

	template <typename Ret, typename Class, typename... Args>
	struct function_traits<Ret(Class::*)(Args...) const volatile & noexcept> : function_traits<Ret(Args...)> {};

	template <typename Ret, typename Class, typename... Args>
	struct function_traits<Ret(Class::*)(Args...) && noexcept> : function_traits<Ret(Args...)> {};

	template <typename Ret, typename Class, typename... Args>
	struct function_traits<Ret(Class::*)(Args...) const && noexcept> : function_traits<Ret(Args...)> {};

	template <typename Ret, typename Class, typename... Args>
	struct function_traits<Ret(Class::*)(Args...) volatile && noexcept> : function_traits<Ret(Args...)> {};

	template <typename Ret, typename Class, typename... Args>
	struct function_traits<Ret(Class::*)(Args...) const volatile && noexcept> : function_traits<Ret(Args...)> {};

	// functor lambda
	template<typename Callable>
	struct function_traits<Callable, std::void_t<decltype(&Callable::operator()), char>> : function_traits<decltype(&Callable::operator())> {};

	// std::function
	template <typename Ret, typename... Args>
	struct function_traits<std::function<Ret(Args...)>> : function_traits<Ret(Args...)> {};


	template <typename F>
	typename function_traits<F>::stl_function_type to_function(F&& lambda)
	{
		return static_cast<typename function_traits<F>::stl_function_type>(std::forward<F>(lambda));
	}

	template <typename F>
	typename function_traits<F>::pointer to_function_pointer(const F& lambda)
	{
		return static_cast<typename function_traits<F>::pointer>(lambda);
	}


	template< class T >
	inline constexpr bool is_callable_v = function_traits<std::decay_t<T>>::is_callable;
}

#endif // !__ASIO2_FUNCTION_TRAITS_HPP__
