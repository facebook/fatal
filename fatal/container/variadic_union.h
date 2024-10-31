/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/math/numerics.h>
#include <fatal/type/conditional.h>
#include <fatal/type/deprecated/type_list.h>
#include <fatal/type/deprecated/transform.h>

#include <memory>
#include <type_traits>
#include <utility>

namespace fatal {
namespace detail {

template <typename...> struct variadic_union_impl;

} // namespace detail {

////////////////////
// variadic_union //
////////////////////

/**
 * Wraps a union that holds a variadic number of types.
 *
 * This class can be used as a basis to implement optionals or variants.
 *
 * Regardless of the types it holds, the union is constructed
 * with all members uninitialized.
 *
 * Example:
 *
 *  variadic_union<int, double, bool, std::string> v;
 *
 *  // gets a reference to the member of type `int`
 *  auto &i = v.ref<int>();
 *
 *  // gets a pointer to the member of type `double`
 *  auto d = v.ptr<double>();
 *
 *  // constructs the member of type `std::string` in place
 *  // passing "hello, world!" as the argument.
 *  v.construct<std::string>("hello, world!");
 *
 *  // destroys the member of type `std::string`
 *  v.destroy<std::string>();
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename... Args>
struct variadic_union {
  /**
   * A `type_list` of all supported types. Duplicates are removed.
   *
   * Example:
   *
   *  using v1 = variadic_union<>;
   *
   *  // yields `type_list<>`
   *  using result1 = v1::list;
   *
   *  using v2 = variadic_union<std::string, int, std::string>;
   *
   *  // yields `type_list<std::string, int>`
   *  using result2 = v2::list;
   *
   *  using v3 = variadic_union<int, double, bool, int, double, bool, int>;
   *
   *  // yields `type_list<int, double, bool>`
   *  using result3 = v3::list;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using list = typename type_list<Args...>::template unique<>;

private:
  // since we filter out duplicates from Args, we need
  // to check if all of Args are still supported
  static_assert(
    logical::all<
      std::true_type,
      typename list::template contains<Args>...
    >::value,
    "list doesn't support all requested types"
  );

  using impl = typename list::template apply<detail::variadic_union_impl>;

  using union_type = typename impl::union_type;

  static_assert(
    std::is_same<list, typename impl::list>::value,
    "implementation doesn't support all requested types"
  );

public:
  /**
   * Tells if the given type `T` is supported.
   *
   * Returns a type equivalent to either `std::true_type` or `std::false_type`.
   *
   * Example:
   *
   *  using type = variadic_union<int, std::string>;
   *
   *  // yields `true`
   *  auto i = type::supports<int>::value
   *
   *  // yields `false`
   *  auto d = type::supports<std::string>::value
   *
   * @author: Marcelo Juchem
   */
  template <typename T>
  using supports = typename impl::list::template contains<T>;

  /**
   * Gets a reference to the member with the specified type `T`.
   *
   * Example:
   *
   *  variadic_union<int, double, bool> v;
   *
   *  // gets a valid reference of type `int &`
   *  auto &i = v.ref<int>();
   *
   *  // gets a valid reference of type `bool &&`
   *  auto &&b = std::move(v).ref<bool>();
   *
   *  // fails to compile
   *  auto &f = v.ref<float>();
   *
   *  variadic_union<int, double, bool> const c;
   *
   *  // gets a valid reference of type `double const &`
   *  auto &d = v.ref<double>();
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
# define FATAL_IMPL_VARIADIC_UNION_GET(Modifier, ...) \
  template <typename T> \
  T Modifier ref() __VA_ARGS__ noexcept { \
    static_assert(list::template contains<T>::value, "unsupported type"); \
    return impl::template get<T>(container_); \
  }

  FATAL_IMPL_VARIADIC_UNION_GET(const &, const)
  FATAL_IMPL_VARIADIC_UNION_GET(&)
  FATAL_IMPL_VARIADIC_UNION_GET(&&, &&)

# undef FATAL_IMPL_VARIADIC_UNION_GET

  /**
   * Gets a pointer to the member with the specified type `T`.
   *
   * Example:
   *
   *  variadic_union<int, double, bool> v;
   *
   *  // gets a valid pointer of type `int *`
   *  auto i = v.ptr<int>();
   *
   *  // fails to compile
   *  auto f = v.ptr<float>();
   *
   *  variadic_union<int, double, bool> const c;
   *
   *  // gets a valid pointer of type `double const *`
   *  auto d = v.ptr<double>();
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename T>
  T const *ptr() const noexcept { return std::addressof(ref<T>()); }

  template <typename T>
  T *ptr() noexcept { return std::addressof(ref<T>()); }

  /**
   * Calls the constructor for the member of type `T` in place,
   * perfectly forwarding all of the given arguments `args`.
   *
   * No checks are performed to avoid constructing when some member
   * has already been initialized.
   *
   * The member of type `T` must be manually destroyed in order to
   * avoid memory leaks.
   *
   * Example:
   *
   *  variadic_union<int, double, bool, std::string> v;
   *
   *  // constructs the member of type `std::string` in place
   *  // passing "hello, world!" as the argument.
   *  v.construct<std::string>("hello, world!");
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename T, typename... TArgs>
  T &construct(TArgs &&...args)
    noexcept(noexcept(T(std::forward<TArgs>(args)...)))
  {
    return *new (ptr<T>()) T(std::forward<TArgs>(args)...);
  }

  /**
   * Calls the destructor for the member of type `T`.
   *
   * No checks are performed to avoid destroying an uninitialized member.
   *
   * Example:
   *
   *  variadic_union<int, double, bool, std::string> v;
   *
   *  v.construct<std::string>("hello, world!");
   *
   *  // destroys the member of type `std::string`
   *  v.destroy<std::string>();
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename T>
  T *destroy() noexcept {
    auto p = ptr<T>();
    p->~T();
    return p;
  }

private:
  union_type container_;
};

////////////////////////////
// IMPLEMENTATION DETAILS //
////////////////////////////

namespace detail {

template <>
struct variadic_union_impl<> {
  using list = type_list<>;

  union union_type {};
};

#define FATAL_IMPL_VARIADIC_UNION_CTOR \
  union_type() noexcept {} \
  union_type(union_type const &) = delete; \
  union_type(union_type &&) = delete; \
  ~union_type() noexcept {}

#define FATAL_IMPL_VARIADIC_UNION_GET(Type, Member, Modifier) \
  template <typename U> \
  static typename std::enable_if< \
    std::is_same<U, Type>::value, \
    Type \
  >::type Modifier get(union_type Modifier container) noexcept { \
    return container.Member; \
  }

#define FATAL_IMPL_VARIADIC_UNION_GET_COMBO(Type, Member) \
  FATAL_IMPL_VARIADIC_UNION_GET(Type, Member, const &) \
  FATAL_IMPL_VARIADIC_UNION_GET(Type, Member, &) \
  FATAL_IMPL_VARIADIC_UNION_GET(Type, Member, &&) \
  static_assert(list::template contains<Type>::value, "unsupported type")

template <typename T, typename... Args>
struct variadic_union_impl<T, Args...> {
  using list = type_list<T, Args...>;

  using halves = typename list::template split<>;
  static_assert(!halves::second::empty, "invalid specialization");

  using left = typename halves::first::template apply<
    detail::variadic_union_impl
  >;
  using pivot = typename halves::second::template at<0>;
  using right = typename halves::second::template tail<1>
    ::template apply<detail::variadic_union_impl>;

  static_assert(
    std::is_same<
      list,
      typename left::list
        ::template push_back<pivot>
        ::template concat<typename right::list>
    >::value,
    "invalid division of elements"
  );

  static_assert(
    !type_list<std::false_type>::concat<
      typename left::list::template transform<right::list::template contains>,
      typename right::list::template transform<left::list::template contains>
    >::template apply<logical::any>::value,
    "there are types supported by both the left and right side"
  );

  union union_type {
    FATAL_IMPL_VARIADIC_UNION_CTOR
    pivot p;
    typename left::union_type lhs;
    typename right::union_type rhs;
  };

  FATAL_IMPL_VARIADIC_UNION_GET_COMBO(pivot, p);

# define FATAL_IMPL_VARIADIC_UNION_GET_SIDE(Suffix, Modifier) \
  struct left_getter_##Suffix { \
    template <typename U> \
    static U Modifier get(union_type Modifier container) { \
      return left::template get<U>(container.lhs); \
    } \
  }; \
  \
  struct right_getter_##Suffix { \
    template <typename U> \
    static U Modifier get(union_type Modifier container) { \
      return right::template get<U>(container.rhs); \
    } \
  }; \
  \
  template <typename U> \
  static typename std::enable_if< \
    left::list::template contains<U>::value \
      || right::list::template contains<U>::value, \
    U \
  >::type Modifier get(union_type Modifier container) noexcept { \
    using getter = conditional< \
      left::list::template contains<U>::value, \
      left_getter_##Suffix, \
      right_getter_##Suffix \
    >; \
    return getter::template get<U>(container); \
  }

  FATAL_IMPL_VARIADIC_UNION_GET_SIDE(clv, const &)
  FATAL_IMPL_VARIADIC_UNION_GET_SIDE(lv, &)
  FATAL_IMPL_VARIADIC_UNION_GET_SIDE(rv, &&)

# undef FATAL_IMPL_VARIADIC_UNION_GET_SIDE
};

////////////////////////////////////////////////////
// OPTIMIZATIONS FOR MOST USED NUMBER OF ELEMENTS //
////////////////////////////////////////////////////

template <typename T0>
struct variadic_union_impl<T0> {
  using list = type_list<T0>;

  union union_type {
    FATAL_IMPL_VARIADIC_UNION_CTOR
    T0 value_0;
  };

  FATAL_IMPL_VARIADIC_UNION_GET_COMBO(T0, value_0);
};

template <typename T0, typename T1>
struct variadic_union_impl<T0, T1> {
  using list = type_list<T0, T1>;

  union union_type {
    FATAL_IMPL_VARIADIC_UNION_CTOR
    T0 value_0;
    T1 value_1;
  };

  FATAL_IMPL_VARIADIC_UNION_GET_COMBO(T0, value_0);
  FATAL_IMPL_VARIADIC_UNION_GET_COMBO(T1, value_1);
};

template <typename T0, typename T1, typename T2>
struct variadic_union_impl<T0, T1, T2> {
  using list = type_list<T0, T1, T2>;

  union union_type {
    FATAL_IMPL_VARIADIC_UNION_CTOR
    T0 value_0;
    T1 value_1;
    T2 value_2;
  };

  FATAL_IMPL_VARIADIC_UNION_GET_COMBO(T0, value_0);
  FATAL_IMPL_VARIADIC_UNION_GET_COMBO(T1, value_1);
  FATAL_IMPL_VARIADIC_UNION_GET_COMBO(T2, value_2);
};

template <typename T0, typename T1, typename T2, typename T3>
struct variadic_union_impl<T0, T1, T2, T3> {
  using list = type_list<T0, T1, T2, T3>;

  union union_type {
    FATAL_IMPL_VARIADIC_UNION_CTOR
    T0 value_0;
    T1 value_1;
    T2 value_2;
    T3 value_3;
  };

  FATAL_IMPL_VARIADIC_UNION_GET_COMBO(T0, value_0);
  FATAL_IMPL_VARIADIC_UNION_GET_COMBO(T1, value_1);
  FATAL_IMPL_VARIADIC_UNION_GET_COMBO(T2, value_2);
  FATAL_IMPL_VARIADIC_UNION_GET_COMBO(T3, value_3);
};

template <typename T0, typename T1, typename T2, typename T3, typename T4>
struct variadic_union_impl<T0, T1, T2, T3, T4> {
  using list = type_list<T0, T1, T2, T3, T4>;

  union union_type {
    FATAL_IMPL_VARIADIC_UNION_CTOR
    T0 value_0;
    T1 value_1;
    T2 value_2;
    T3 value_3;
    T4 value_4;
  };

  FATAL_IMPL_VARIADIC_UNION_GET_COMBO(T0, value_0);
  FATAL_IMPL_VARIADIC_UNION_GET_COMBO(T1, value_1);
  FATAL_IMPL_VARIADIC_UNION_GET_COMBO(T2, value_2);
  FATAL_IMPL_VARIADIC_UNION_GET_COMBO(T3, value_3);
  FATAL_IMPL_VARIADIC_UNION_GET_COMBO(T4, value_4);
};

#undef FATAL_IMPL_VARIADIC_UNION_GET_COMBO
#undef FATAL_IMPL_VARIADIC_UNION_GET
#undef FATAL_IMPL_VARIADIC_UNION_CTOR

} // namespace detail {
} // namespace fatal {
