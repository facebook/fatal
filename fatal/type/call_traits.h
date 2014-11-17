/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_call_traits_h
#define FATAL_INCLUDE_fatal_type_call_traits_h

#include <fatal/type/tag.h>

#include <utility>

namespace fatal {

#define FATAL_CALL_TRAITS(Name, ...) \
  struct Name { \
    class member_function { \
      template <typename U, typename... UArgs> \
      struct supported_impl { \
        template <typename> struct dummy {}; \
        template <typename V> \
        static std::true_type sfinae( \
          dummy< \
            decltype(std::declval<V>().__VA_ARGS__( \
              std::forward<UArgs>(std::declval<UArgs>())... \
            )) \
          > * \
        ); \
        template <typename> static std::false_type sfinae(...); \
      }; \
      \
    public: \
      constexpr member_function() {} \
      \
      template <typename U, typename... UArgs> \
      constexpr static auto call(U &&subject, UArgs &&...args) \
        -> decltype(subject.__VA_ARGS__(std::forward<UArgs>(args)...)) \
      { return subject.__VA_ARGS__(std::forward<UArgs>(args)...); } \
      \
      template <typename U, typename... UArgs> \
      constexpr auto operator ()(U &&subject, UArgs &&...args) const \
        -> decltype( \
          call(std::forward<U>(subject), std::forward<UArgs>(args)...) \
        ) \
      { return call(std::forward<U>(subject), std::forward<UArgs>(args)...); } \
      \
      template <typename U, typename... UArgs> \
      using supported = decltype(supported_impl<U, UArgs...> \
        ::template sfinae<U>(nullptr)); \
    }; \
    \
    template <typename TSubject> \
    struct static_member { \
      using type = TSubject; \
      \
      constexpr static_member() {} \
      \
      template <typename... UArgs> \
      constexpr static auto call(UArgs &&...args) \
        -> decltype(TSubject::__VA_ARGS__(std::forward<UArgs>(args)...)) \
      { return TSubject::__VA_ARGS__(std::forward<UArgs>(args)...); } \
      \
      template <typename... UArgs> \
      constexpr auto operator ()(UArgs &&...args) const \
        -> decltype(call(std::forward<UArgs>(args)...)) \
      { return call(std::forward<UArgs>(args)...); } \
    }; \
    \
    struct free_function { \
      constexpr free_function() {} \
      \
      template <typename... UArgs> \
      constexpr static auto call(UArgs &&...args) \
        -> decltype(__VA_ARGS__(std::forward<UArgs>(args)...)) \
      { return __VA_ARGS__(std::forward<UArgs>(args)...); } \
      \
      template <typename... UArgs> \
      constexpr auto operator ()(UArgs &&...args) const \
        -> decltype(call(std::forward<UArgs>(args)...)) \
      { return call(std::forward<UArgs>(args)...); } \
    }; \
  }

struct ctor_call_traits {
  template <typename T>
  struct automatic {
    using type = T;

    constexpr automatic() {}

    template <typename... UArgs>
    constexpr static T construct(UArgs &&...args) {
      return T(std::forward<UArgs>(args)...);
    }

    template <typename... UArgs>
    constexpr T operator ()(UArgs &&...args) const {
      return construct(std::forward<UArgs>(args)...);
    }
  };

  template <typename T>
  struct dynamic {
    using type = T;

    constexpr dynamic() {}

    template <typename... UArgs>
    constexpr static T *construct(UArgs &&...args) {
      return new T(std::forward<UArgs>(args)...);
    }

    template <typename... UArgs>
    constexpr T *operator ()(UArgs &&...args) const {
      return construct(std::forward<UArgs>(args)...);
    }
  };

  template <typename T>
  struct placement {
    using type = T;

    constexpr placement() {}

    template <typename... UArgs>
    constexpr static T *construct(T *pointer, UArgs &&...args) {
      return new (pointer) T(std::forward<UArgs>(args)...);
    }

    template <typename... UArgs>
    constexpr T *operator ()(T *pointer, UArgs &&...args) const {
      return construct(pointer, std::forward<UArgs>(args)...);
    }
  };
};

class call_operator_traits {
  template <typename... Args>
  struct is_impl {
    template <typename T>
    static std::true_type sfinae(
      type_tag<decltype(std::declval<T>().operator()(
        std::forward<Args>(std::declval<typename std::decay<Args>::type>())...
      ))> *
    );
    template <typename> static std::false_type sfinae(...);
  };

public:
  constexpr call_operator_traits() {}

  template <typename T, typename... UArgs>
  constexpr static auto call(T &&subject, UArgs &&...args)
    -> decltype(subject(std::forward<UArgs>(args)...))
  { return subject(std::forward<UArgs>(args)...); }

  template <typename T, typename... UArgs>
  constexpr auto operator ()(T &&subject, UArgs &&...args) const
    -> decltype(call(std::forward<T>(subject), std::forward<UArgs>(args)...))
  { return call(std::forward<T>(subject), std::forward<UArgs>(args)...); }

  /**
   * TODO: DOCUMENT
   *
   * Example:
   *
   *  struct Foo {
   *    void operator ()() {}
   *    void operator ()(int i, std::string s) {}
   *  };
   *
   *  auto const lambda_is = [](int, std::string) {};
   *  using lambda = decltype(lambda_is);
   *
   *  cout << std::boolalpha
   *    << call_operator_traits::supported<Foo>::value
   *    << ' ' << std::boolalpha
   *    << call_operator_traits::supported<Foo, int>::value
   *    << ' ' << std::boolalpha
   *    << call_operator_traits::supported<Foo, int, double>::value
   *    << ' ' << std::boolalpha
   *    << call_operator_traits::supported<Foo, int, std::string>::value
   *    << std::endl
   *    << ' ' << std::boolalpha
   *    << call_operator_traits::supported<lambda>::value
   *    << ' ' << std::boolalpha
   *    << call_operator_traits::supported<lambda, int>::value
   *    << ' ' << std::boolalpha
   *    << call_operator_traits::supported<lambda, int, double>::value
   *    << ' ' << std::boolalpha
   *    << call_operator_traits::supported<lambda, int, std::string>::value;
   *
   * Outputs:
   *  true false false true
   *  false false false true
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename T, typename... Args>
  using supported = decltype(is_impl<Args...>::template sfinae<T>(nullptr));
};

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_call_traits_h
