/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/functional/identity.h>
#include <fatal/functional/no_op.h>
#include <fatal/type/scalar.h>

#include <limits>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

namespace fatal {
namespace fn {

/**
 * Takes any number of arguments and returns a default constructed instance of
 * type `T`.
 *
 * Example:
 *
 *  default_constructer<std::string> f;
 *
 *  // yields an empty `std::string`
 *  auto result1 = f(10);
 *
 *  // yields an empty `std::string`
 *  auto result2 = f("hello");
 *
 *  // yields an empty `std::string`
 *  auto result3 = f(std::string("world"));
 *
 *  std::string s("test");
 *  // yields an empty `std::string`
 *  auto result4 = f(s);
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
struct default_constructed {
  template <typename... Args>
  constexpr T operator ()(Args &&...) const noexcept(noexcept(T())) {
    return T();
  }
};

/**
 * A callable that accepts any number of parameters, ignores them and always
 * returns the constant represented by `T::value`.
 *
 * Example:
 *
 *  constant<std::integral_constant<int, 56>> c;
 *
 *  // yields `56`
 *  auto result1 = c(10);
 *
 *  // yields `56`
 *  auto result2 = c("hello");
 *
 *  // yields `56`
 *  auto result3 = c("hello", "world", 12345);
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
struct constant {
  using value_type = typename T::value_type;

  template <typename... Args>
  constexpr value_type operator ()(Args &&...) const noexcept {
    return T::value;
  }
};

/**
 * A callable that receives any parameter and returns the result of calling
 * `std::get<Index>` on it.
 *
 * Example:
 *
 *  auto x = std::make_tuple(10, 20, 30);
 *  auto y = std::make_tuple(40, 50, 60);
 *
 *  std_get<1> g;
 *
 *  // yields `20`
 *  auto result1 = g(x);
 *
 *  // yields `50`
 *  auto result2 = g(y);
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <std::size_t Index>
struct std_get {
  template <typename T>
  constexpr auto operator ()(T &&value) const
    noexcept(noexcept(std::get<Index>(std::forward<T>(value))))
    -> decltype(std::get<Index>(std::forward<T>(value)))
  {
    return std::get<Index>(std::forward<T>(value));
  }
};

/**
 * A callable that receives any parameter and returns the result of calling
 * `static_cast<T>` on it.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
struct static_caster {
  template <typename U>
  constexpr T operator ()(U &&value) const
    noexcept(noexcept(static_cast<T>(std::forward<U>(value))))
  {
    return static_cast<T>(std::forward<U>(value));
  }
};

/**
 * A callable that receives any parameter and returns the result of calling
 * `reinterpret_cast<T>` on it.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
struct reinterpret_caster {
  template <typename U>
  constexpr T operator ()(U &&value) const
    noexcept(noexcept(reinterpret_cast<T>(std::forward<U>(value))))
  {
    return reinterpret_cast<T>(std::forward<U>(value));
  }
};

/**
 * A callable that receives any parameter and returns the result of calling
 * `dynamic_cast<T>` on it.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
struct dynamic_caster {
  template <typename U>
  constexpr T operator ()(U &&value) const
    noexcept(noexcept(dynamic_cast<T>(std::forward<U>(value))))
  {
    return dynamic_cast<T>(std::forward<U>(value));
  }
};

/**
 * A callable that receives any parameter and returns the result of calling
 * `operator *` on it.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct dereferencer {
  template <typename T>
  constexpr auto operator ()(T &&value) const
    noexcept(noexcept(*std::forward<T>(value)))
    -> decltype(*std::forward<T>(value))
  {
    return *std::forward<T>(value);
  }
};

/**
 * A callable that receives any parameter and returns the result of calling
 * `std::addressof` on it.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct address_of {
  template <typename T>
  constexpr T *operator ()(T &value) const noexcept {
    return std::addressof(value);
  }
};

/**
 * A callable that receives any parameter and returns the result of checking
 * whether the parameter is greater than 0.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct is_positive {
  template <typename T>
  constexpr bool operator ()(T &&value) const
    noexcept(noexcept(value > static_cast<T>(0)))
  {
    return std::forward<T>(value) > static_cast<T>(0);
  }
};

/**
 * A callable that receives any parameter and returns the result of checking
 * whether the parameter is less than or equal to 0.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct not_positive {
  template <typename T>
  constexpr bool operator ()(T &&value) const
    noexcept(noexcept(value <= static_cast<T>(0)))
  {
    return std::forward<T>(value) <= static_cast<T>(0);
  }
};

namespace detail {
namespace functional_impl {

template <typename T, bool = std::is_unsigned<T>::value>
struct is_negative {
  static constexpr bool impl(T const &value)
    noexcept(noexcept(value >= static_cast<T>(0)))
  {
    return value < static_cast<T>(0);
  }
};

template <typename T>
struct is_negative<T, true> {
  static constexpr bool impl(T const &) noexcept {
    return false;
  }
};

template <typename T, bool = std::is_unsigned<T>::value>
struct not_negative {
  static constexpr bool impl(T const &value)
    noexcept(noexcept(value >= static_cast<T>(0)))
  {
    return value >= static_cast<T>(0);
  }
};

template <typename T>
struct not_negative<T, true> {
  static constexpr bool impl(T const &) noexcept {
    return true;
  }
};

} // namespace functional_impl {
} // namespace detail {

/**
 * A callable that receives any parameter and returns the result of checking
 * whether the parameter is less than 0.
 *
 * This comparison won't trigger 'tautological comparison' warnings during
 * compilation.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct is_negative {
  template <typename T>
  constexpr bool operator ()(T &&value) const
    noexcept(noexcept(
      detail::functional_impl::is_negative<
        typename std::decay<T>::type
      >::impl(std::forward<T>(value))
    ))
  {
    return detail::functional_impl::is_negative<
      typename std::decay<T>::type
    >::impl(std::forward<T>(value));
  }
};
/**
 * A callable that receives any parameter and returns the result of checking
 * whether the parameter is greater than or equal to 0.
 *
 * This comparison won't trigger 'tautological comparison' warnings during
 * compilation.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct not_negative {
  template <typename T>
  constexpr bool operator ()(T &&value) const
    noexcept(noexcept(
      detail::functional_impl::not_negative<
        typename std::decay<T>::type
      >::impl(std::forward<T>(value))
    ))
  {
    return detail::functional_impl::not_negative<
      typename std::decay<T>::type
    >::impl(std::forward<T>(value));
  }
};

/**
 * A callable that receives any two parameters and returns the result of calling
 * `operator ==` on them.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct equal {
  template <typename LHS, typename RHS>
  constexpr auto operator ()(LHS &&lhs, RHS &&rhs) const
    noexcept(noexcept(std::forward<LHS>(lhs) == std::forward<RHS>(rhs)))
    -> decltype(std::forward<LHS>(lhs) == std::forward<RHS>(rhs))
  {
    return std::forward<LHS>(lhs) == std::forward<RHS>(rhs);
  }
};

/**
 * A callable that receives any two parameters and returns the result of calling
 * `operator !=` on them.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct not_equal {
  template <typename LHS, typename RHS>
  constexpr auto operator ()(LHS &&lhs, RHS &&rhs) const
    noexcept(noexcept(std::forward<LHS>(lhs) != std::forward<RHS>(rhs)))
    -> decltype(std::forward<LHS>(lhs) != std::forward<RHS>(rhs))
  {
    return std::forward<LHS>(lhs) != std::forward<RHS>(rhs);
  }
};

/**
 * A callable that receives any two parameters and returns the result of calling
 * `operator <` on them.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct less {
  template <typename LHS, typename RHS>
  constexpr auto operator ()(LHS &&lhs, RHS &&rhs) const
    noexcept(noexcept(std::forward<LHS>(lhs) < std::forward<RHS>(rhs)))
    -> decltype(std::forward<LHS>(lhs) < std::forward<RHS>(rhs))
  {
    return std::forward<LHS>(lhs) < std::forward<RHS>(rhs);
  }
};

/**
 * A callable that receives any two parameters and returns the result of calling
 * `operator <=` on them.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct less_equal {
  template <typename LHS, typename RHS>
  constexpr auto operator ()(LHS &&lhs, RHS &&rhs) const
    noexcept(noexcept(std::forward<LHS>(lhs) <= std::forward<RHS>(rhs)))
    -> decltype(std::forward<LHS>(lhs) <= std::forward<RHS>(rhs))
  {
    return std::forward<LHS>(lhs) <= std::forward<RHS>(rhs);
  }
};

/**
 * A callable that receives any two parameters and returns the result of calling
 * `operator >` on them.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct greater {
  template <typename LHS, typename RHS>
  constexpr auto operator ()(LHS &&lhs, RHS &&rhs) const
    noexcept(noexcept(std::forward<LHS>(lhs) > std::forward<RHS>(rhs)))
    -> decltype(std::forward<LHS>(lhs) > std::forward<RHS>(rhs))
  {
    return std::forward<LHS>(lhs) > std::forward<RHS>(rhs);
  }
};

/**
 * A callable that receives any two parameters and returns the result of calling
 * `operator >=` on them.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct greater_equal {
  template <typename LHS, typename RHS>
  constexpr auto operator ()(LHS &&lhs, RHS &&rhs) const
    noexcept(noexcept(std::forward<LHS>(lhs) >= std::forward<RHS>(rhs)))
    -> decltype(std::forward<LHS>(lhs) >= std::forward<RHS>(rhs))
  {
    return std::forward<LHS>(lhs) >= std::forward<RHS>(rhs);
  }
};

/**
 * A callable that receives any two parameter and returns the result of calling
 * `operator !` on it.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct negate {
  template <typename T>
  constexpr auto operator ()(T &&value) const
    noexcept(noexcept(!std::forward<T>(value)))
    -> decltype(!std::forward<T>(value))
  {
    return !std::forward<T>(value);
  }
};

/**
 * A callable that receives any number of parameters and returns the result of
 * applying the logical AND operation on them.
 *
 * When no parameters are given, the neutral operand `true` is returned.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct logical_and {
  constexpr bool operator ()() const noexcept {
    return true;
  }

  template <typename T>
  constexpr T &&operator ()(T &&value) const noexcept {
    return std::forward<T>(value);
  }

  template <typename LHS, typename... Args>
  constexpr auto operator ()(LHS &&lhs, Args &&...args) const
    noexcept(
      noexcept(
        std::forward<LHS>(lhs)
          && std::declval<logical_and const &>()(std::forward<Args>(args)...)
      )
    )
    -> decltype(
      std::forward<LHS>(lhs)
        && std::declval<logical_and const &>()(std::forward<Args>(args)...)
    )
  {
    return std::forward<LHS>(lhs) && (*this)(std::forward<Args>(args)...);
  }
};

/**
 * A callable that receives any number of parameters and returns the result of
 * applying the logical OR operation on them.
 *
 * When no parameters are given, the neutral operand `false` is returned.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct logical_or {
  constexpr bool operator ()() const noexcept {
    return false;
  }

  template <typename T>
  constexpr T &&operator ()(T &&value) const noexcept {
    return std::forward<T>(value);
  }

  template <typename LHS, typename... Args>
  constexpr auto operator ()(LHS &&lhs, Args &&...args) const
    noexcept(
      noexcept(
        std::forward<LHS>(lhs)
          || std::declval<logical_or const &>()(std::forward<Args>(args)...)
      )
    )
    -> decltype(
      std::forward<LHS>(lhs)
        || std::declval<logical_or const &>()(std::forward<Args>(args)...)
    )
  {
    return std::forward<LHS>(lhs) || (*this)(std::forward<Args>(args)...);
  }
};

/**
 * A callable that receives any three parameters `c`, `t` and `f`; and returns
 * the result of the ternary operator as in `c ? t : f`.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct ternary {
  template <typename C, typename T, typename F>
  constexpr auto operator ()(C &&c, T &&t, F &&f) const
    noexcept(
      noexcept(std::forward<C>(c) ? std::forward<T>(t) : std::forward<F>(f))
    )
    -> decltype(std::forward<C>(c) ? std::forward<T>(t) : std::forward<F>(f))
  {
    return std::forward<C>(c) ? std::forward<T>(t) : std::forward<F>(f);
  }
};

/**
 * A callable that receives any parameter and returns the result of calling
 * `operator ~` on it.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct complement {
  template <typename T>
  constexpr auto operator ()(T &&value) const
    noexcept(noexcept(~std::forward<T>(value)))
    -> decltype(~std::forward<T>(value))
  {
    return ~std::forward<T>(value);
  }
};

/**
 * A callable that receives one or more parameters and returns the result of
 * applying the bitwise AND operation on them.
 *
 * When only one parameter is given, this callable works like the identity
 * function.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct bitwise_and {
  template <typename T>
  constexpr T &&operator ()(T &&value) const noexcept {
    return std::forward<T>(value);
  }

  template <typename LHS, typename... Args>
  constexpr auto operator ()(LHS &&lhs, Args &&...args) const
    noexcept(
      noexcept(
        std::forward<LHS>(lhs)
          & std::declval<bitwise_and const &>()(std::forward<Args>(args)...)
      )
    )
    -> decltype(
      std::forward<LHS>(lhs)
        & std::declval<bitwise_and const &>()(std::forward<Args>(args)...)
    )
  {
    return std::forward<LHS>(lhs) & (*this)(std::forward<Args>(args)...);
  }
};

/**
 * A callable that receives one or more parameters and returns the result of
 * applying the bitwise OR operation on them.
 *
 * When only one parameter is given, this callable works like the identity
 * function.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct bitwise_or {
  template <typename T>
  constexpr T &&operator ()(T &&value) const noexcept {
    return std::forward<T>(value);
  }

  template <typename LHS, typename... Args>
  constexpr auto operator ()(LHS &&lhs, Args &&...args) const
    noexcept(
      noexcept(
        std::forward<LHS>(lhs)
          | std::declval<bitwise_or const &>()(std::forward<Args>(args)...)
      )
    )
    -> decltype(
      std::forward<LHS>(lhs)
        | std::declval<bitwise_or const &>()(std::forward<Args>(args)...)
    )
  {
    return std::forward<LHS>(lhs) | (*this)(std::forward<Args>(args)...);
  }
};

/**
 * A callable that receives one or more parameters and returns the result of
 * applying the bitwise XOR operation on them.
 *
 * When only one parameter is given, this callable works like the identity
 * function.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct bitwise_xor {
  template <typename T>
  constexpr T &&operator ()(T &&value) const noexcept {
    return std::forward<T>(value);
  }

  template <typename LHS, typename... Args>
  constexpr auto operator ()(LHS &&lhs, Args &&...args) const
    noexcept(
      noexcept(
        std::forward<LHS>(lhs)
          ^ std::declval<bitwise_xor const &>()(std::forward<Args>(args)...)
      )
    )
    -> decltype(
      std::forward<LHS>(lhs)
        ^ std::declval<bitwise_xor const &>()(std::forward<Args>(args)...)
    )
  {
    return std::forward<LHS>(lhs) ^ (*this)(std::forward<Args>(args)...);
  }
};

/**
 * A callable that receives a pointer of type `T` to uninitialized memory,
 * followed by any number of arguments. It then uses placement new to call the
 * constructor of `T`, perfectly forwarding the remaining arguments to it.
 *
 * Useful for perfect forward constructing using placement new.
 *
 * Returns the pointer to the now initialized memory (first argument).
 *
 * Example:
 *
 *  uninitialized<std::string, true> s; // from fatal/container/uninitialized.h
 *
 *  placement_forward c;
 *
 *  // same effect of calling `new (s.ptr()) T("hello, world")`;
 *  auto p = c(s.ptr(), "hello, world");
 *  assert(p == s.ptr());
 *
 *  s.destroy();
 *
 *  std::string x("test");
 *
 *  // same effect of copy constructing with `x`, using placement new
 *  c(s.ptr(), x);
 *
 *  s.destroy();
 *
 *  // same effect of move constructing with `x`, using placement new
 *  c(s.ptr(), std::move(x));
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct placement_forward {
  template <typename T, typename... Args>
  constexpr T *operator ()(T *to, Args &&...args) const
    noexcept(noexcept(new (to) T(std::forward<Args>(args)...)))
  {
    return new (to) T(std::forward<Args>(args)...);
  }
};

/**
 * A callable that receives a pointer of type T to uninitialized memory,
 * followed by any number of arguments. It then uses placement new to call the
 * constructor of `T`, passing the remaining arguments to it as reference to
 * `const`.
 *
 * Useful for copy constructing using placement new.
 *
 * Returns the pointer to the now initialized memory (first argument).
 *
 * Example:
 *
 *  uninitialized<std::string, true> s; // from fatal/container/uninitialized.h
 *
 *  placement_copy c;
 *
 *  // same effect of calling `new (s.ptr()) T("hello, world")`;
 *  auto p = c(s.ptr(), "hello, world");
 *  assert(p == s.ptr());
 *
 *  s.destroy();
 *
 *  std::string x("test");
 *
 *  // same effect of copy constructing with `x`, using placement new
 *  c(s.ptr(), x);
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct placement_copy {
  template <typename T, typename... Args>
  constexpr T *operator ()(T *to, Args const &...args) const
    noexcept(noexcept(new (to) T(args...)))
  {
    return new (to) T(args...);
  }
};

/**
 * A callable that receives a pointer of type `T` to uninitialized memory,
 * followed by any number of arguments. It then uses placement new to call the
 * constructor of `T`, passing the remaining arguments to it through
 * `std::move`, regardless of whether the arguments were received as r-value
 * references or not.
 *
 * Useful for move constructing using placement new.
 *
 * Returns the pointer to the now initialized memory (first argument).
 *
 * Example:
 *
 *  uninitialized<std::string, true> s; // from fatal/container/uninitialized.h
 *
 *  placement_move c;
 *
 *  // same effect of calling `new (s.ptr()) T(std::move("hello, world"))`;
 *  auto p = c(s.ptr(), "hello, world");
 *  assert(p == s.ptr());
 *
 *  s.destroy();
 *
 *  std::string x("test");
 *
 *  // same effect of move constructing with `x`, using placement new
 *  c(s.ptr(), x);
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct placement_move {
  template <typename T, typename... Args>
  constexpr T *operator ()(T *to, Args &&...args) const
    noexcept(noexcept(new (to) T(std::move(args)...)))
  {
    return new (to) T(std::move(args)...);
  }
};

/**
 * A callable that receives any two arguments and calls the assignment operator
 * on the first argument, perfectly forwarding the second argument as a
 * parameter.
 *
 * Example:
 *
 *  std::string lhs;
 *  std::string rhs;
 *  forward_assigner a;
 *
 *  // same effect of `lhs = rhs;`
 *  a(lhs, rhs);
 *
 *  // same effect of `lhs = std::move(rhs);`
 *  a(lhs, std::move(rhs));
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct forward_assigner {
  template <typename LHS, typename RHS>
  constexpr LHS &operator ()(LHS &lhs, RHS &&rhs) const
    noexcept(noexcept(lhs = std::forward<RHS>(rhs)))
  {
    return lhs = std::forward<RHS>(rhs);
  }
};

/**
 * A callable that receives any two arguments and calls the copy assignment
 * operator on the first argument, passing the second argument as a parameter.
 *
 * Example:
 *
 *  std::string lhs;
 *  std::string rhs;
 *  copy_assigner a;
 *
 *  // same effect of `lhs = rhs;`
 *  a(lhs, rhs);
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct copy_assigner {
  template <typename LHS, typename RHS>
  constexpr LHS &operator ()(LHS &lhs, RHS const &rhs) const
    noexcept(noexcept(lhs = rhs))
  {
    return lhs = rhs;
  }
};

/**
 * A callable that receives any two arguments and calls the move assignment
 * operator on the first argument, passing a `std::move` of the second argument
 * as a parameter.
 *
 * Example:
 *
 *  std::string lhs;
 *  std::string rhs;
 *  move_assigner a;
 *
 *  // same effect of `lhs = std::move(rhs);`
 *  a(lhs, rhs);
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct move_assigner {
  template <typename LHS, typename RHS>
  constexpr LHS &operator ()(LHS &lhs, RHS &&rhs) const
    noexcept(noexcept(lhs = std::move(rhs)))
  {
    return lhs = std::move(rhs);
  }
};

namespace detail {
namespace tuple_comparer_impl {

template <int Index>
struct index_traits {
  using reversed = bool_constant<(Index < 0)>;
  using index = size_constant<
    static_cast<std::size_t>(Index < 0 ? -(Index + 1) : Index)
  >;
};

} // namespace tuple_comparer_impl {
} // namespace detail {

/**
 * A comparer for tuples allowing for conveniently choosing the order with
 * which tuple elements are used for comparison, as well as reversing ordering
 * per element.
 *
 * It takes a list of indexes in the order they should be used for comparison.
 *
 * Default comparison uses the less than operator for each element. In order to
 * use the greater than operator instead, a negative 1-based index should be
 * passed instead. For instance, 0, 2 and 3 uses the less than operator for the
 * first, third and fourth element of the tuple; whereas -1, -3 and -4 uses the
 * greather than operator for the same elements, respectivelly.
 *
 * The proper formula for reversing the order of tuple element `n` is `-n - 1`.
 *
 * Example:
 *
 *  using tuple = std::tuple<int, int, int>;
 *  std::vector<tuple> const v{
 *    tuple{0, 2, 4},
 *    tuple{1, 3, 4},
 *    tuple{4, 2, 0},
 *    tuple{1, 3, 5},
 *    tuple{3, 5, 1},
 *    tuple{1, 3, 2},
 *    tuple{1, 1, 5}
 *  };
 *
 *  auto result1 = v;
 *  // yields `{
 *  //  {0, 2, 4},
 *  //  {1, 3, 2},
 *  //  {1, 3, 4},
 *  //  {1, 1, 5}
 *  //  {1, 3, 5},
 *  //  {3, 5, 1},
 *  //  {4, 2, 0},
 *  // };
 *  std::sort(result1.begin(), result1.end(), tuple_comparer<0, 2, 1>());
 *
 *  auto result2 = v;
 *  // yields `{
 *  //  {0, 2, 4},
 *  //  {1, 3, 5}
 *  //  {1, 3, 4},
 *  //  {1, 3, 2},
 *  //  {1, 1, 5},
 *  //  {3, 5, 1},
 *  //  {4, 2, 0},
 *  // };
 *  std::sort(result2.begin(), result2.end(), tuple_comparer<0, -2, -3>());
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <int...> class tuple_comparer;

template <int HeadIndex, int... TailIndexes>
class tuple_comparer<HeadIndex, TailIndexes...> {
  using traits = detail::tuple_comparer_impl::index_traits<HeadIndex>;
  using head = tuple_comparer<HeadIndex>;
  using tail = tuple_comparer<TailIndexes...>;

public:
  template <typename LHS, typename RHS>
  constexpr bool operator ()(LHS const &lhs, RHS const &rhs) const
    noexcept(
      noexcept(
        std::get<traits::index::value>(lhs)
          == std::get<traits::index::value>(rhs)
          ? tail()(lhs, rhs)
          : head()(lhs, rhs)
      )
    )
  {
    return std::get<traits::index::value>(lhs)
      == std::get<traits::index::value>(rhs)
      ? tail()(lhs, rhs)
      : head()(lhs, rhs);
  }
};

template <int Index>
class tuple_comparer<Index> {
  using traits = detail::tuple_comparer_impl::index_traits<Index>;

public:
  template <typename LHS, typename RHS>
  constexpr bool operator ()(LHS const &lhs, RHS const &rhs) const
    noexcept(
      noexcept(
        traits::reversed::value
          ? std::get<traits::index::value>(lhs)
            > std::get<traits::index::value>(rhs)
          : std::get<traits::index::value>(lhs)
            < std::get<traits::index::value>(rhs)
      )
    )
  {
    return traits::reversed::value
      ? std::get<traits::index::value>(lhs)
        > std::get<traits::index::value>(rhs)
      : std::get<traits::index::value>(lhs)
        < std::get<traits::index::value>(rhs);
  }
};

} // namespace fn {
} // namespace fatal {
