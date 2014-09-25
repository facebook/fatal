/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#pragma once

#include <fatal/type/tag.h>

#include <limits>
#include <type_traits>
#include <utility>

namespace fatal {

////////////////////////
// identity_transform //
////////////////////////

/**
 * Helper alias similar to std::decay_t, that resolves to the type
 * given as the template parameter.
 *
 * Example:
 *
 *  // yields `int`
 *  typedef identity_transform<int> result1;
 *
 *  // yields `std::string`
 *  typedef identity_transform<std::string> result2;
 *
 *  // yields `double`
 *  typedef identity_transform<identity_transform<double>>> result3;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
namespace detail {
// needed due to gcc bug https://gcc.gnu.org/bugzilla/show_bug.cgi?id=62276
template <typename T> struct identity_transform_impl { typedef T type; };
} // namespace detail {

template <typename T>
using identity_transform = typename detail::identity_transform_impl<T>::type;

/////////////////////
// fixed_transform //
/////////////////////

/**
 * Helper alias that always resolves to the same type regardless of parameters.
 *
 * It is useful, for instance, in situations where you have a variadic method
 * and you want to receive parameters of a given type in the same amount as the
 * variadic template types.
 *
 * Example:
 *
 *  template <typename... Args>
 *  void foo(fixed_transform<int>::apply<Args>... args) {
 *    // ...
 *  }
 *
 *  // calls foo passing 3 integers
 *  foo<bool, int, std::string>(0, 1, 2);
 *
 *  // compilation error: foo expects 3 integers as parameters
 *  foo<bool, int, std::string>(0, 1);
 *
 *  // compilation error: foo expects 3 integers as parameters
 *  foo<bool, int, std::string>(true, 0, std::string("hello"));
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
struct fixed_transform {
  template <typename...> using apply = T;
};

/**
 * A convenience specialization of `fixed_transform`
 * that always yields std::true_type.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename...> using true_predicate = std::true_type;

/**
 * A convenience specialization of `fixed_transform`
 * that always yields std::false_type.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename...> using false_predicate = std::false_type;

////////////////////////
// constant_transform //
////////////////////////

/**
 * Helper alias that always evaluates to `std::integral_constant<T, Value>`.
 *
 * This is very useful when initializing a variadic
 * container with the same type:
 *
 * Example:
 *
 *  template <typename... TTags>
 *  struct tag_counters {
 *    tag_counters():
 *      // initializes the `counters` tuple with as many 0s as there are TTags
 *      counters(constant_transform<int, 0>::template apply<TTags>::value...)
 *    {}
 *
 *  private:
 *    // creates a `std::tuple<int, int...>` with
 *    // as many `int`s as there are `TTag`s
 *    std::tuple<constant_transform<int, 0>::template apply<TTags>...> counters;
 *  };
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, T Value>
struct constant_transform {
  template <typename...> using apply = std::integral_constant<T, Value>;
};

///////////////////////////
// conditional_transform //
///////////////////////////

namespace detail {

template <
  bool,
  template <typename...> class TWhenTrueTransform,
  template <typename...> class,
  typename T
>
struct conditional_transform_impl {
  using type = TWhenTrueTransform<T>;
};

template <
  template <typename...> class TWhenTrueTransform,
  template <typename...> class TWhenFalseTransform,
  typename T
>
struct conditional_transform_impl<
  false, TWhenTrueTransform, TWhenFalseTransform, T
> {
  using type = TWhenFalseTransform<T>;
};

} // namespace detail {

/**
 * Applies either `TWhenTrueTransform` or `TWhenFalseTransform` to the type `T`
 * depending whether the predicate `TPredicate` returns, respectively, true or
 * false, when applied to to the type `T`.
 *
 * The default transform for `TWhenFalseTransform` is `identity_transform`.
 *
 * Example:
 *
 *  using transformA = conditional_transform<
 *    std::is_integral,
 *    std::make_unsigned_t
 *  >;
 *
 *  // yields `unsigned long`
 *  using resultA1 = transformA::apply<long>;
 *
 *  // yields `std::string`
 *  using resultA2 = transformA::apply<std::string>;
 *
 *  using transformB = conditional_transform<
 *    std::is_integral,
 *    std::tuple,
 *    std::add_pointer_t
 *  >;
 *
 *  // yields `std::tuple<long>`
 *  using resultB1 = transformB::apply<long>;
 *
 *  // yields `std::string *`
 *  using resultB2 = transformB::apply<std::string>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  template <typename...> class TPredicate,
  template <typename...> class TWhenTrueTransform,
  template <typename...> class TWhenFalseTransform = identity_transform
>
struct conditional_transform {
  template <typename T>
  using apply = typename detail::conditional_transform_impl<
    TPredicate<T>::value, TWhenTrueTransform, TWhenFalseTransform, T
  >::type;
};

///////////////////////////
// type_member_transform //
///////////////////////////

/**
 * An adapter that allows using a template class' member typedef `type`
 * as a transform.
 *
 * This is useful for using C++'s type modifiers (decay, make_signed, ...)
 * as transforms on a pre-C++14 compiler.
 *
 * `TTransform` is any class that receives a given type `T` as a template
 * parameter and provides a member typedef `type`, which is to be used as
 * the transform result.
 *
 * Example:
 *
 *  typedef type_member_transform<std::decay> dc;
 *
 *  // yields `int`
 *  typedef dc::apply<int const &> result1;
 *
 *  template <typename T, template <typename...> class MyTransform>
 *  using foo = MyTransform<T>;
 *
 *  // yields `double`
 *  typedef foo<double &&, dc::template apply> result2;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <template <typename...> class TTransform>
struct type_member_transform {
  template <typename... Args>
  using apply = typename TTransform<Args...>::type;
};

////////////////////////
// transform_sequence //
////////////////////////

/**
 * A convenience adapter that allows nesting multiple transforms into one.
 *
 * The transforms are applied in the order specified.
 *
 * If no transforms are given, it acts like `identity_transform`.
 *
 * Example:
 *
 *  template <typename> struct T1 {};
 *  template <typename> struct T2 {};
 *  template <typename> struct T3 {};
 *
 *  typedef transform_sequence<T1, T2, T3> tr;
 *
 *  // yields `T3<T2<T1<int>>>`
 *  typedef tr::apply<int> result;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
namespace detail {

template <template <typename...> class TTransform, typename... Args>
struct transform_sequence_impl { using type = TTransform<Args...>; };

template <template <typename> class TTransform, typename T>
struct transform_sequence_impl<TTransform, T> { using type = TTransform<T>; };

} // namespace detail {

template <template <typename...> class...> struct transform_sequence;

template <
  template <typename...> class TTransform,
  template <typename...> class... TTransforms
>
struct transform_sequence<TTransform, TTransforms...> {
  template <typename... Args>
  using apply = typename transform_sequence<TTransforms...>::template apply<
    typename detail::transform_sequence_impl<TTransform, Args...>::type
  >;
};

template <>
struct transform_sequence<> {
  template <typename T, typename...>
  using apply = T;
};

/////////////////////
// transform_alias //
/////////////////////

/**
 * Provides an alias for type transforms, so they can be handled as regular
 * types.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <template <typename...> class TTemplate, typename... Args>
struct transform_alias {
  /**
   * Applies the given arguments `UArgs...`, in addition
   * to the partially applied ones, to the original template.
   *
   * Example:
   *
   *  typedef transform_alias<std::tuple, int, double> c1;
   *
   *  // yields `std::tuple<int, double>`
   *  typedef c1::apply<> result1;
   *
   *  // yields `std::tuple<int, double, long, std::string, bool, float>`
   *  typedef c1::apply<long, std::string, bool, float> result2;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UArgs>
  using apply = TTemplate<Args..., UArgs...>;

  /**
   * Partially applies arguments `UArgs...` in addition
   * to those partially applied so far.
   *
   * Example:
   *
   *  typedef transform_alias<std::tuple, int, double> c1;
   *  typedef c1::curry<long, std::string> c2;
   *
   *  // yields `std::tuple<int, double, long, std::string>`
   *  typedef c2::apply<> result1;
   *
   *  // yields `std::tuple<int, double, long, std::string, bool, float>`
   *  typedef c2::apply<bool, float> result2;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UArgs>
  using curry = transform_alias<TTemplate, Args..., UArgs...>;

  /**
   * Replaces all arguments partially applied so far with `UArgs...`.
   *
   * Example:
   *
   *  typedef transform_alias<std::tuple, int, double> c1;
   *  typedef c1::rebind<long, std::string> c2;
   *
   *  // yields `std::tuple<long, std::string>`
   *  typedef c2::apply<> result1;
   *
   *  // yields `std::tuple<long, std::string, bool, float>`
   *  typedef c2::apply<bool, float> result2;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UArgs>
  using rebind = transform_alias<TTemplate, UArgs...>;

  /**
   * Discards any arguments partially applied so far.
   *
   * Example:
   *
   *  typedef transform_alias<std::tuple, int, double> c1;
   *
   *  typedef transform_alias<c1::template uncurry, long, std::string> c2;
   *
   *  // yields `std::tuple<long, std::string>`
   *  typedef c2::apply<> result1;
   *
   *  // yields `std::tuple<long, std::string, bool, float>`
   *  typedef c2::apply<bool, float> result2;
   *
   *  // yields `std::tuple<long, std::string>`
   *  typedef c1::uncurry<long, std::string> result2;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UArgs>
  using uncurry = TTemplate<UArgs...>;

  /**
   * Re-applies arguments `UArgs...`, in addition to all
   * partially applied arguments, to a separate template.
   *
   * Example:
   *
   *  typedef transform_alias<std::tuple, int, double> c1;
   *
   *  // yields `V1<int, double>`
   *  typedef c1::rebind_args<V1>::apply<> result1;
   *
   *  // yields `V1<int, double, bool, float>`
   *  typedef c1::rebind_args<V1, bool, float>::apply<> result2;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename...> class UTemplate, typename... UArgs>
  using rebind_args = transform_alias<UTemplate, Args..., UArgs...>;

  /**
   * Applies the partially applied arguments, in addition
   * to the arguments `UArgs...`, to another template.
   *
   * This acts as a shortcut to `rebind_args<UTemplate, UArgs...>::apply<>`
   *
   * Example:
   *
   *  typedef transform_alias<std::tuple, int, double> c1;
   *
   *  // yields `V1<int, double>`
   *  typedef c1::apply_args<V1> result1;
   *
   *  // yields `V1<int, double, bool, float>`
   *  typedef c1::apply_args<V1, bool, float> result2;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename...> class UTemplate, typename... UArgs>
  using apply_args = UTemplate<Args..., UArgs...>;
};

//////////////////////
// transform_switch //
//////////////////////

namespace detail {
namespace transform_switch_impl {

template <template <typename...> class, template <typename...> class...>
struct select;

template <
  bool,
  template <typename...> class,
  template <typename...> class TTransform,
  template <typename...> class...
>
struct branch { template <typename U> using apply = TTransform<U>; };

template <
  template <typename...> class TFallback,
  template <typename...> class TTransform,
  template <typename...> class... Args
>
struct branch<false, TFallback, TTransform, Args...> {
  template <typename U>
  using apply = typename select<TFallback, Args...>::template apply<U>;
};

template <
  template <typename...> class TFallback,
  template <typename...> class TPredicate,
  template <typename...> class TTransform,
  template <typename...> class... Args
>
struct select<TFallback, TPredicate, TTransform, Args...> {
  template <typename U>
  using apply = typename branch<
    TPredicate<U>::value, TFallback, TTransform, Args...
  >::template apply<U>;
};

template <template <typename...> class TFallback>
struct select<TFallback> {
  template <typename U> using apply = TFallback<U>;
};

} // namespace transform_switch_impl {
} // namespace detail {

// TODO: DOCUMENT AND TEST
template <
  template <typename...> class TFallback,
  template <typename...> class... Args
>
using transform_switch = detail::transform_switch_impl::select<
  TFallback, Args...
>;

/**
 * TODO: DOCUMENT AND TEST
 * TODO: review member_transform_stack now that we have this
 * TODO: review recursive_transform now that we have this
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  template <typename...> class TAggregator,
  template <typename...> class... TTransforms
>
struct transform_aggregator {
  template <typename T>
  using apply = TAggregator<TTransforms<T>...>;
};

/**
 * A convenience version of `transform_switch` that uses `identity_transform`
 * as the fallback transform.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <template <typename...> class... Args>
using identity_transform_switch = transform_switch<identity_transform, Args...>;

////////////////////////
// member_transformer //
////////////////////////

// TODO: DOCUMENT AND PROPERLY TEST
/**
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_MEMBER_TRANSFORMER(Name, Member) \
  template < \
    template <typename...> class TPreTransform = identity_transform, \
    template <typename...> class TPostTransform = identity_transform \
  > \
  struct Name { \
    template < \
      template <typename...> class TTransform, \
      typename... Args \
    > \
    struct bind { \
      template < \
        typename T, \
        typename... UArgs \
      > \
      using use = TPostTransform< \
        typename TPreTransform<T>::template Member< \
          TTransform, Args..., UArgs... \
        > \
      >; \
    }; \
    \
    template < \
      typename T, \
      template <typename...> class TTransform, \
      typename... Args \
    > \
    using use = typename bind<TTransform, Args...>::template use<T>; \
  }

// TODO: DOCUMENT AND PROPERLY TEST
/**
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct member_transformer {
# define FATAL_IMPL_BIND_MEMBER_TRANSFORMER(Name) \
    FATAL_MEMBER_TRANSFORMER(Name, Name)

  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(transform);
  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(apply);
  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(conditional);
  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(separate);
  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(filter);
  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(reject);
  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(search);
  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(combine);
  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(merge);
  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(sort);
  FATAL_IMPL_BIND_MEMBER_TRANSFORMER(unique);

#undef FATAL_IMPL_BIND_MEMBER_TRANSFORMER
};

//////////////////////////////
// member_transformer_stack //
//////////////////////////////

// TODO: DOCUMENT AND TEST
/**
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  template <typename, template <typename...> class, typename...> class TMember
>
struct member_transformer_stack {
  template <template <typename...> class... TPreTransforms>
  struct pre {
    template <template <typename...> class... TPostTransforms>
    struct post {
      template <
        typename T,
        template <typename...> class TTransform = identity_transform,
        typename... UArgs
      >
      using use = typename transform_sequence<
        TPostTransforms...
      >::template apply<
        TMember<
          typename transform_sequence<TPreTransforms...>::template apply<T>,
          TTransform,
          UArgs...
        >
      >;
    };

    template <
      typename T,
      template <typename...> class TTransform = identity_transform,
      typename... UArgs
    >
    using use = typename post<>::template use<T, TTransform, UArgs...>;
  };

  template <template <typename...> class... TPostTransforms>
  struct post {
    template <
      typename T,
      template <typename...> class TTransform = identity_transform,
      typename... UArgs
    >
    using use = typename transform_sequence<TPostTransforms...>::template apply<
      TMember<T, TTransform, UArgs...>
    >;
  };

  template <
    typename T,
    template <typename...> class TTransform = identity_transform,
    typename... UArgs
  >
  using use = typename pre<>::template post<>::template use<
    T, TTransform, UArgs...
  >;
};

/////////////////////////
// recursive_transform //
/////////////////////////

namespace detail {
namespace transform_recursively_impl {

template <
  bool, std::size_t,
  template <typename...> class, template <typename...> class,
  template <typename...> class,
  template <typename, template <typename...> class, typename...> class,
  template <typename...> class
>
struct recurse;

template <
  bool, std::size_t, typename,
  template <typename...> class, template <typename...> class,
  template <typename...> class,
  template <typename, template <typename...> class, typename...> class,
  template <typename...> class
>
struct branch;

template <
  std::size_t Depth,
  typename T,
  template <typename...> class TPre,
  template <typename...> class TPost,
  template <typename...> class TPredicate,
  template <
    typename, template <typename...> class, typename...
  > class TTransformer,
  template <typename...> class TTransform
>
struct branch<
  true, Depth, T, TPre, TPost, TPredicate, TTransformer, TTransform
> {
  using type = TPost<
    TTransformer<
      TPre<T>,
      recurse<true, Depth, TPre, TPost, TPredicate, TTransformer, TTransform>
        ::template apply
    >
  >;
};

template <
  std::size_t Depth,
  typename T,
  template <typename...> class TPre,
  template <typename...> class TPost,
  template <typename...> class TPredicate,
  template <
    typename, template <typename...> class, typename...
  > class TTransformer,
  template <typename...> class TTransform
>
struct branch<
  false, Depth, T, TPre, TPost, TPredicate, TTransformer, TTransform
> {
  using type = TTransform<T>;
};

template <
  std::size_t Depth,
  template <typename...> class TPre,
  template <typename...> class TPost,
  template <typename...> class TPredicate,
  template <
    typename, template <typename...> class, typename...
  > class TTransformer,
  template <typename...> class TTransform
>
struct recurse<true, Depth, TPre, TPost, TPredicate, TTransformer, TTransform> {
  static_assert(Depth > 0, "invalid recursion depth");

  template <typename T>
  using apply = typename branch<
    TPredicate<T>::value, Depth - 1, T,
    TPre, TPost, TPredicate, TTransformer, TTransform
  >::type;
};

template <
  template <typename...> class TPre,
  template <typename...> class TPost,
  template <typename...> class TPredicate,
  template <
    typename, template <typename...> class, typename...
  > class TTransformer,
  template <typename...> class TTransform
>
struct recurse<true, 0, TPre, TPost, TPredicate, TTransformer, TTransform> {
  template <typename T>
  using apply = typename conditional_transform<
    TPredicate,
    transform_sequence<TPre, TPost>::template apply,
    TTransform
  >::template apply<T>;
};

} // namespace transform_recursively_impl {
} // namespace detail {

// TODO: DOCUMENT & PROPERLY TEST
// TODO: REFINE THE INTERFACE

/**
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  template <typename...> class TPreTransform = identity_transform,
  template <typename...> class TPostTransform = identity_transform,
  std::size_t Depth = std::numeric_limits<std::size_t>::max()
>
struct recursive_transform {
  template <
    template <typename...> class TPredicate,
    template <
      typename, template <typename...> class, typename...
    > class TTransformer = member_transformer::transform<>::use,
    template <typename...> class... TTransforms
  >
  using with = detail::transform_recursively_impl::recurse<
    true,
    Depth,
    TPreTransform,
    TPostTransform,
    TPredicate,
    TTransformer,
    transform_sequence<TTransforms...>::template apply
  >;
};

/////////////////////////
// recursive_type_sort //
/////////////////////////

// TODO: write in terms of `recursive_transform`

template <typename T, std::size_t Depth>
struct recursive_type_sort_impl { typedef T type; };

template <std::size_t Depth = std::numeric_limits<std::size_t>::max()>
struct recursive_type_sort {
  template <typename T>
  using apply = typename recursive_type_sort_impl<T, Depth>::type;
};

template <typename T>
using full_recursive_type_sort = typename recursive_type_sort<>
  ::template apply<T>;

} // namespace fatal
