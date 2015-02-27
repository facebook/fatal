/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_traits_h
#define FATAL_INCLUDE_fatal_type_traits_h

#include <fatal/preprocessor.h>
#include <fatal/type/string.h>
#include <fatal/type/transform.h>

#include <type_traits>
#include <utility>

namespace fatal {

/**
 * Removes any r-value references from a given type.
 * L-value references remain untouched.
 *
 * Example:
 *
 *  // yields `int`
 *  using result1 = remove_rvalue_reference<int &&>::type;
 *
 *  // yields `int const`
 *  using result2 = remove_rvalue_reference<int const &&>::type;
 *
 *  // yields `int *`
 *  using result3 = remove_rvalue_reference<int *>::type;
 *
 *  // yields `int *&`
 *  using result4 = remove_rvalue_reference<int *&>::type;
 *
 *  // yields `int const &`
 *  using result5 = remove_rvalue_reference<int const &>::type;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
struct remove_rvalue_reference { using type = T; };

template <typename T>
struct remove_rvalue_reference<T &&> { using type = T; };

/**
 * Given types `T` and `U`:
 * - if `U` is not a reference, yield `T`
 * - if `U` is an l-value reference, turn `T` into an l-value reference
 * - if `U` is an r-value reference, turn `T` into an r-value reference
 *
 * Example:
 *
 *  struct foo {};
 *
 *  // yields `foo`
 *  using result1 = same_reference_as<foo, int>::type;
 *
 *  // yields `foo &&`
 *  using result2 = same_reference_as<foo &&, int>::type;
 *
 *  // yields `foo &`
 *  using result3 = same_reference_as<foo, int &>::type;
 *
 *  // yields `foo &`
 *  using result4 = same_reference_as<foo &&, int &>::type;
 *
 *  // yields `foo &&`
 *  using result5 = same_reference_as<foo, int &&>::type;
 *
 *  // yields `foo &&`
 *  using result6 = same_reference_as<foo &, int &&>::type;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, typename>
struct same_reference_as {
  using type = T;
};

template <typename T, typename TFrom>
struct same_reference_as<T, TFrom &> {
  using type = typename std::add_lvalue_reference<
    typename std::remove_reference<T>::type
  >::type;
};

template <typename T, typename TFrom>
struct same_reference_as<T, TFrom &&> {
  using type = typename std::add_rvalue_reference<
    typename std::remove_reference<T>::type
  >::type;
};

/**
 * Given types `T` and `U`:
 * - if `U` is not a reference, yield `T`
 * - if `U` is an l-value reference, yield `std::add_lvalue_reference<T>::type`
 * - if `U` is an r-value reference, yield `std::add_rvalue_reference<T>::type`
 *
 * Example:
 *
 *  struct foo {};
 *
 *  // yields `foo`
 *  using result1 = add_reference_from<foo, int>::type;
 *
 *  // yields `foo &&`
 *  using result2 = add_reference_from<foo &&, int>::type;
 *
 *  // yields `foo &`
 *  using result3 = add_reference_from<foo, int &>::type;
 *
 *  // yields `foo &`
 *  using result4 = add_reference_from<foo &&, int &>::type;
 *
 *  // yields `foo &&`
 *  using result5 = add_reference_from<foo, int &&>::type;
 *
 *  // yields `foo &`
 *  using result6 = add_reference_from<foo &, int &&>::type;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, typename>
struct add_reference_from {
  using type = T;
};

template <typename T, typename TFrom>
struct add_reference_from<T, TFrom &> {
  using type = typename std::add_lvalue_reference<T>::type;
};

template <typename T, typename TFrom>
struct add_reference_from<T, TFrom &&> {
  using type = typename std::add_rvalue_reference<T>::type;
};

/**
 * Applies `std::add_const` to a type iff some other type is const.
 *
 * Example:
 *
 *  struct foo {};
 *
 *  // yields `foo`
 *  using result1 = add_const_from<foo, int>::type;
 *
 *  // yields `foo const`
 *  using result2 = add_const_from<foo, int const>::type;
 *
 *  // yields `foo const`
 *  using result3 = add_const_from<foo const, int const>::type;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, typename>
struct add_const_from {
  using type = T;
};

template <typename T, typename TFrom>
struct add_const_from<T, TFrom const> {
  using type = typename std::add_const<T>::type;
};

/**
 * Adds the const qualifier to a type.
 *
 * If the type is a reference, to some other type `U`,
 * make it the same kind of reference to `U const`.
 *
 * Example:
 *
 *  // yields `int const`
 *  using result1 = constify<int>::type;
 *
 *  // yields `int const &`
 *  using result2 = constify<int &>::type;
 *
 *  // yields `int const &&`
 *  using result3 = constify<int &&>::type;
 *
 *  // yields `int const &`
 *  using result4 = constify<int const &>::type;
 *
 *  // yields `int *const`
 *  using result4 = constify<int *>::type;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
struct constify {
  using type = typename add_reference_from<
    typename std::decay<T>::type const,
    T
  >::type;
};

/**
 * Applies `constify` to a type iff some other type is const.
 *
 * Example:
 *
 *  struct foo {};
 *
 *  // yields `foo const`
 *  using result1 = constify_from<foo, int const>::type;
 *
 *  // yields `foo const &`
 *  using result2 = constify_from<foo &, int const>::type;
 *
 *  // yields `foo &&`
 *  using result3 = constify_from<foo &&, int>::type;
 *
 *  // yields `foo const &`
 *  using result4 = constify_from<foo const &, int const>::type;
 *
 *  // yields `foo const &`
 *  using result5 = constify_from<foo const &, int>::type;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, typename>
struct constify_from {
  using type = T;
};

template <typename T, typename TFrom>
struct constify_from<T, TFrom const> {
  using type = typename constify<T>::type;
};

/**
 * Checks whether a given type is an instantiation of at least one of a list of
 * class templates.
 *
 * The template member `type` which evaluates to either `std::true_type` or
 * `std::false_type`.
 *
 * Example:
 *
 *  // yields `true` for std::string
 *  is_template<std::basic_string>::type<std::string>::value
 *
 *  // yields `true` for std::wstring
 *  is_template<std::basic_string>::type<std::wstring>::value
 *
 *  // yields `true` for std::basic_string<int>
 *  is_template<std::basic_string>::type<std::basic_string<int>>::value
 *
 *  // yields `false` for std::vector<int>
 *  is_template<std::basic_string>::type<std::vector<int>>::value
 *
 *  // yields `true` for std::vector<int>
 *  is_template<std::basic_string, std::vector>::type<std::string>::value
 *
 *  // yields `true` for std::vector<int>
 *  is_template<std::basic_string, std::vector>::type<std::vector<int>>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
namespace detail {

template <template <typename...> class, typename>
struct is_template_impl;

template <template <typename...> class T, typename... Args>
struct is_template_impl<T, T<Args...>> {};

} // namespace detail {

template <template <typename...> class... TTemplates>
class is_template {
  template <template <typename...> class TTemplate, typename T>
  using impl = is_complete<detail::is_template_impl<TTemplate, T>>;

public:
  template <typename T>
  using type = logical_transform::any<impl<TTemplates, T>...>;
};

/**
 * fast_pass_by_value: tells if pass-by-value is the fastest way of passing a
 * given type as a read-only argument or return value.
 *
 * Pointers, integers and whatnot are passed by value while user defined
 * types are passed by reference to const.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
using fast_pass_by_value = std::integral_constant<
  bool, std::is_scalar<typename std::decay<T>::type>::value
>;

/**
 * fast_pass: resolves to the fastest way of passing a given type as a read-only
 * argument or return value.
 *
 * See fast_pass_by_value for details.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T>
using fast_pass = typename std::conditional<
  fast_pass_by_value<
    typename std::decay<T>::type
  >::value,
  typename std::add_const<
    typename std::decay<T>::type
  >::type,
  typename std::add_lvalue_reference<
    typename std::add_const<
      typename std::decay<T>::type
    >::type
  >::type
>::type;

/**
 * Type traits to prevent the universal constructor from overloading
 * the copy/move constructor.
 *
 * For more information, see http://ericniebler.com/2013/08/07/ \
 *     universal-references-and-the-copy-constructo/
 *
 * Usage:
 *
 * // before
 * struct Foo {
 *   template <typename T>
 *   Foo(T &&value) { ... }
 * };
 *
 * // after
 * struct Foo {
 *   template <
 *     typename T,
 *     typename X = typename std::enable_if<
 *       safe_overload<Foo, T>::value, void
 *     >::type
 *   >
 *   Foo(T &&value) { ... }
 * };
 *
 * It also works with variadic templates:
 *
 * // before
 * struct Foo {
 *   template <typename... Args>
 *   Foo(Args &&...args) { ... }
 * };
 *
 * // after
 * struct Foo {
 *   template <
 *     typename... Args,
 *     typename X = typename std::enable_if<
 *       safe_overload<Foo, Args...>::value, void
 *     >::type
 *   >
 *   Foo(Args &&...args) { ... }
 * };
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename, typename...>
struct safe_overload: public std::true_type {};

template <typename Class, typename T>
struct safe_overload<Class, T>:
  public std::integral_constant<
    bool,
    !std::is_base_of<
      Class,
      typename std::remove_cv<
        typename std::remove_reference<T>::type
      >::type
    >::value
  >
{};

/**
 * Template alias for safe_overload above.
 *
 * Usage:
 *
 * struct Foo {
 *   template <typename T, typename X = safe_overload_t<Foo, T>>
 *   Foo(T &&value) { ... }
 * };
 *
 * It also works with variadic templates:
 *
 * struct Foo {
 *   template <
 *     typename... UArgs,
 *     typename X = safe_overload_t<Foo, UArgs...>
 *   >
 *   Foo(UArgs &&...args) { ... }
 * };
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename Class, typename... Args>
using safe_overload_t = typename std::enable_if<
  safe_overload<Class, Args...>::value
>::type;

/**
 * Tells whether an instance of a given type supports the
 * call operator, when passing arguments of given types.
 *
 * Example:
 *
 *  void noop();
 *
 *  int fn(std::string const &s, bool b);
 *
 *  struct non_callable {};
 *
 *  struct callable {
 *    void operator ()(int i, double d);
 *  };
 *
 *  // yields `true`
 *  is_callable<noop>::value
 *
 *  // yields `false`
 *  is_callable<decltype(fn), int, double>::value
 *
 *  // yields `true`
 *  is_callable<decltype(fn), char const *, bool>::value
 *
 *  // yields `false`
 *  is_callable<non_callable>::value
 *
 *  // yields `false`
 *  is_callable<non_callable, int>::value
 *
 *  // yields `true`
 *  is_callable<callable, int, double>::value
 *
 *  // yields `false`
 *  is_callable<callable, int, double, std::string>::value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
namespace detail {

template <typename... Args>
class is_callable_impl {
  struct impl {
    template <
      typename T,
      typename = decltype(
        std::declval<T>()(
          std::forward<Args>(std::declval<typename std::decay<Args>::type>())...
        )
      )
    >
    static std::true_type sfinae(T *);

    template <typename = void>
    static std::false_type sfinae(...);
  };

public:
  template <typename T>
  using type = decltype(impl::sfinae(static_cast<T *>(nullptr)));
};

} // namespace detail {

template <typename T, typename... Args>
using is_callable = typename detail::is_callable_impl<Args...>
  ::template type<T>;

/**
 * This macro creates a class named `Class` that can check whether some
 * type has a nested member type with name `Member`.
 *
 * The class created by this macro looks like this:
 *
 *  struct Class {
 *    template <typename T>
 *    using check = <either std::true_type or std::false_type>;
 *  };
 *
 * Example:
 *
 *  FATAL_HAS_MEMBER_TYPE(has_xyz, xyz);
 *
 *  struct foo { using xyz = int; };
 *  struct bar { typedef int xyz; };
 *  struct baz {};
 *
 *  // yields `std::true_type`
 *  using result1 = has_xyz::check<foo>;
 *
 *  // yields `std::true_type`
 *  using result2 = has_xyz::check<bar>;
 *
 *  // yields `std::false_type`
 *  using result3 = has_xyz::check<baz>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_HAS_MEMBER_TYPE(Class, ...) \
  struct Class { \
    template <typename T> \
    static std::true_type sfinae(typename T::__VA_ARGS__ *); \
    \
    template <typename> \
    static std::false_type sfinae(...); \
    \
    template <typename T> \
    using check = decltype(sfinae<T>(nullptr)); \
  }

struct has_member_type {
# define FATAL_IMPL_HAS_MEMBER_TYPE_DECL(Class, Name) \
  private: \
    FATAL_HAS_MEMBER_TYPE(Class, Name); \
    \
  public: \
    template <typename T> \
    using Name = typename Class::template check<T>

# define FATAL_IMPL_HAS_MEMBER_TYPE(Name) \
  FATAL_IMPL_HAS_MEMBER_TYPE_DECL( \
    FATAL_UID(FATAL_CAT(has_member_type_impl, Name)), \
    Name \
  )

  FATAL_IMPL_HAS_MEMBER_TYPE(char_type);
  FATAL_IMPL_HAS_MEMBER_TYPE(type);
  FATAL_IMPL_HAS_MEMBER_TYPE(types);

# define FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(Name) \
  FATAL_IMPL_HAS_MEMBER_TYPE(Name); \
  FATAL_IMPL_HAS_MEMBER_TYPE(Name##_type)

  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(allocator);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(args);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(array);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(category);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(config);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(const_iterator);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(const_pointer);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(const_ptr);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(const_ref);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(const_reference);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(const_reverse_iterator);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(data);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(decode);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(decoder);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(difference);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(element);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(encode);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(encoder);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(extension);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(first);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(flag);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(hash);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(id);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(ids);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(index);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(info);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(information);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(instance);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(item);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(iterator);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(key);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(list);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(map);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(mapped);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(mapping);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(mappings);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(member);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(members);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(name);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(names);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(pair);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(pointer);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(predicate);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(ptr);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(ref);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(reference);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(request);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(response);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(result);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(reverse);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(reverse_iterator);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(second);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(set);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(size);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(str);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(string);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(tag);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(traits);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(tuple);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(value);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(values);
  FATAL_IMPL_HAS_MEMBER_TYPE_TYPE(version);

# undef FATAL_IMPL_HAS_MEMBER_TYPE_TYPE
# undef FATAL_IMPL_HAS_MEMBER_TYPE
# undef FATAL_IMPL_HAS_MEMBER_TYPE_DECL
};

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_DATA_MEMBER_GETTER(Class, ...) \
  struct Class { \
    template <typename TOwner> \
    using type = typename ::fatal::add_reference_from< \
      typename ::fatal::constify_from< \
        decltype(::std::declval<TOwner>().__VA_ARGS__), \
        typename ::std::remove_reference<TOwner>::type \
      >::type, \
      TOwner \
    >::type; \
    \
  public: \
    FATAL_STR(name, FATAL_TO_STR(__VA_ARGS__)); \
    \
    template <typename TOwner> \
    static type<TOwner &&> ref(TOwner &&owner) { \
      return static_cast<type<TOwner &&>>( \
        ::std::forward<TOwner>(owner).__VA_ARGS__ \
      ); \
    } \
    \
    struct ref_getter { \
      template <typename TOwner> \
      type<TOwner &&> operator ()(TOwner &&owner) const { \
        return static_cast<type<TOwner &&>>( \
          ::std::forward<TOwner>(owner).__VA_ARGS__ \
        ); \
      } \
    }; \
    \
    template <typename TOwner> \
    static typename ::std::remove_reference<type<TOwner>>::type *ptr( \
      TOwner &owner \
    ) { \
      return ::std::addressof(owner.__VA_ARGS__); \
    } \
    \
    struct ptr_getter { \
      template <typename TOwner> \
      typename ::std::remove_reference<type<TOwner>>::type *operator ()( \
        TOwner &owner \
      ) const { \
        return ::std::addressof(owner.__VA_ARGS__); \
      } \
    }; \
  }

/**
 * Instantiations of FATAL_DATA_MEMBER_GETTER
 * for some popular names.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct data_member_getter {
# define FATAL_IMPL_DATA_MEMBER_GETTER_FOR_IMPL(Class, ...) \
  private: \
    FATAL_DATA_MEMBER_GETTER(Class, __VA_ARGS__); \
    \
  public: \
    using __VA_ARGS__ = Class

# define FATAL_IMPL_DATA_MEMBER_GETTER_FOR(...) \
  FATAL_IMPL_DATA_MEMBER_GETTER_FOR_IMPL(FATAL_UID(__VA_ARGS__), __VA_ARGS__)

# define FATAL_IMPL_DATA_MEMBER_GETTER(...) \
  FATAL_IMPL_DATA_MEMBER_GETTER_FOR(__VA_ARGS__); \
  FATAL_IMPL_DATA_MEMBER_GETTER_FOR(FATAL_CAT(__VA_ARGS__, _))

  FATAL_IMPL_DATA_MEMBER_GETTER(allocator);
  FATAL_IMPL_DATA_MEMBER_GETTER(args);
  FATAL_IMPL_DATA_MEMBER_GETTER(array);
  FATAL_IMPL_DATA_MEMBER_GETTER(category);
  FATAL_IMPL_DATA_MEMBER_GETTER(config);
  FATAL_IMPL_DATA_MEMBER_GETTER(data);
  FATAL_IMPL_DATA_MEMBER_GETTER(decoder);
  FATAL_IMPL_DATA_MEMBER_GETTER(difference);
  FATAL_IMPL_DATA_MEMBER_GETTER(element);
  FATAL_IMPL_DATA_MEMBER_GETTER(encoder);
  FATAL_IMPL_DATA_MEMBER_GETTER(extension);
  FATAL_IMPL_DATA_MEMBER_GETTER(first);
  FATAL_IMPL_DATA_MEMBER_GETTER(flag);
  FATAL_IMPL_DATA_MEMBER_GETTER(hash);
  FATAL_IMPL_DATA_MEMBER_GETTER(id);
  FATAL_IMPL_DATA_MEMBER_GETTER(ids);
  FATAL_IMPL_DATA_MEMBER_GETTER(index);
  FATAL_IMPL_DATA_MEMBER_GETTER(info);
  FATAL_IMPL_DATA_MEMBER_GETTER(information);
  FATAL_IMPL_DATA_MEMBER_GETTER(instance);
  FATAL_IMPL_DATA_MEMBER_GETTER(item);
  FATAL_IMPL_DATA_MEMBER_GETTER(iterator);
  FATAL_IMPL_DATA_MEMBER_GETTER(key);
  FATAL_IMPL_DATA_MEMBER_GETTER(list);
  FATAL_IMPL_DATA_MEMBER_GETTER(map);
  FATAL_IMPL_DATA_MEMBER_GETTER(mapped);
  FATAL_IMPL_DATA_MEMBER_GETTER(mapping);
  FATAL_IMPL_DATA_MEMBER_GETTER(mappings);
  FATAL_IMPL_DATA_MEMBER_GETTER(member);
  FATAL_IMPL_DATA_MEMBER_GETTER(members);
  FATAL_IMPL_DATA_MEMBER_GETTER(name);
  FATAL_IMPL_DATA_MEMBER_GETTER(names);
  FATAL_IMPL_DATA_MEMBER_GETTER(pair);
  FATAL_IMPL_DATA_MEMBER_GETTER(pointer);
  FATAL_IMPL_DATA_MEMBER_GETTER(predicate);
  FATAL_IMPL_DATA_MEMBER_GETTER(ptr);
  FATAL_IMPL_DATA_MEMBER_GETTER(ref);
  FATAL_IMPL_DATA_MEMBER_GETTER(reference);
  FATAL_IMPL_DATA_MEMBER_GETTER(request);
  FATAL_IMPL_DATA_MEMBER_GETTER(response);
  FATAL_IMPL_DATA_MEMBER_GETTER(result);
  FATAL_IMPL_DATA_MEMBER_GETTER(second);
  FATAL_IMPL_DATA_MEMBER_GETTER(set);
  FATAL_IMPL_DATA_MEMBER_GETTER(size);
  FATAL_IMPL_DATA_MEMBER_GETTER(str);
  FATAL_IMPL_DATA_MEMBER_GETTER(string);
  FATAL_IMPL_DATA_MEMBER_GETTER(tag);
  FATAL_IMPL_DATA_MEMBER_GETTER(tuple);
  FATAL_IMPL_DATA_MEMBER_GETTER(type);
  FATAL_IMPL_DATA_MEMBER_GETTER(types);
  FATAL_IMPL_DATA_MEMBER_GETTER(value);
  FATAL_IMPL_DATA_MEMBER_GETTER(values);
  FATAL_IMPL_DATA_MEMBER_GETTER(version);

# undef FATAL_IMPL_DATA_MEMBER_GETTER
# undef FATAL_IMPL_DATA_MEMBER_GETTER_FOR
# undef FATAL_IMPL_DATA_MEMBER_GETTER_FOR_IMPL
};

} // namespace fatal

#endif // FATAL_INCLUDE_fatal_type_traits_h
