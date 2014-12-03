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

struct call_traits {
# define FATAL_CALL_TRAITS_IMPL(Name) FATAL_CALL_TRAITS(Name, Name)

  FATAL_CALL_TRAITS_IMPL(append);
  FATAL_CALL_TRAITS_IMPL(assign);
  FATAL_CALL_TRAITS_IMPL(at);
  FATAL_CALL_TRAITS_IMPL(back);
  FATAL_CALL_TRAITS_IMPL(begin);
  FATAL_CALL_TRAITS_IMPL(binary_search);
  FATAL_CALL_TRAITS_IMPL(capacity);
  FATAL_CALL_TRAITS_IMPL(cbegin);
  FATAL_CALL_TRAITS_IMPL(cend);
  FATAL_CALL_TRAITS_IMPL(clear);
  FATAL_CALL_TRAITS_IMPL(compare);
  FATAL_CALL_TRAITS_IMPL(copy);
  FATAL_CALL_TRAITS_IMPL(count);
  FATAL_CALL_TRAITS_IMPL(crbegin);
  FATAL_CALL_TRAITS_IMPL(crend);
  FATAL_CALL_TRAITS_IMPL(c_str);
  FATAL_CALL_TRAITS_IMPL(data);
  FATAL_CALL_TRAITS_IMPL(decode);
  FATAL_CALL_TRAITS_IMPL(dequeue);
  FATAL_CALL_TRAITS_IMPL(deserialize);
  FATAL_CALL_TRAITS_IMPL(emplace);
  FATAL_CALL_TRAITS_IMPL(emplace_back);
  FATAL_CALL_TRAITS_IMPL(emplace_front);
  FATAL_CALL_TRAITS_IMPL(empty);
  FATAL_CALL_TRAITS_IMPL(encode);
  FATAL_CALL_TRAITS_IMPL(end);
  FATAL_CALL_TRAITS_IMPL(enqueue);
  FATAL_CALL_TRAITS_IMPL(equal_range);
  FATAL_CALL_TRAITS_IMPL(erase);
  FATAL_CALL_TRAITS_IMPL(find);
  FATAL_CALL_TRAITS_IMPL(find_first_not_of);
  FATAL_CALL_TRAITS_IMPL(find_first_of);
  FATAL_CALL_TRAITS_IMPL(find_last_not_of);
  FATAL_CALL_TRAITS_IMPL(find_last_of);
  FATAL_CALL_TRAITS_IMPL(for_each);
  FATAL_CALL_TRAITS_IMPL(foreach);
  FATAL_CALL_TRAITS_IMPL(forward);
  FATAL_CALL_TRAITS_IMPL(front);
  FATAL_CALL_TRAITS_IMPL(get);
  FATAL_CALL_TRAITS_IMPL(getline);
  FATAL_CALL_TRAITS_IMPL(get_pointer);
  FATAL_CALL_TRAITS_IMPL(get_reference);
  FATAL_CALL_TRAITS_IMPL(hash);
  FATAL_CALL_TRAITS_IMPL(id);
  FATAL_CALL_TRAITS_IMPL(index);
  FATAL_CALL_TRAITS_IMPL(index_of);
  FATAL_CALL_TRAITS_IMPL(insert);
  FATAL_CALL_TRAITS_IMPL(length);
  FATAL_CALL_TRAITS_IMPL(lower_bound);
  FATAL_CALL_TRAITS_IMPL(max);
  FATAL_CALL_TRAITS_IMPL(max_size);
  FATAL_CALL_TRAITS_IMPL(median);
  FATAL_CALL_TRAITS_IMPL(merge);
  FATAL_CALL_TRAITS_IMPL(min);
  FATAL_CALL_TRAITS_IMPL(move);
  FATAL_CALL_TRAITS_IMPL(name);
  FATAL_CALL_TRAITS_IMPL(next);
  FATAL_CALL_TRAITS_IMPL(next_as);
  FATAL_CALL_TRAITS_IMPL(nth_element);
  FATAL_CALL_TRAITS_IMPL(partial_sort);
  FATAL_CALL_TRAITS_IMPL(partition);
  FATAL_CALL_TRAITS_IMPL(pointer);
  FATAL_CALL_TRAITS_IMPL(pop);
  FATAL_CALL_TRAITS_IMPL(pop_back);
  FATAL_CALL_TRAITS_IMPL(pop_front);
  FATAL_CALL_TRAITS_IMPL(prev);
  FATAL_CALL_TRAITS_IMPL(previous);
  FATAL_CALL_TRAITS_IMPL(push);
  FATAL_CALL_TRAITS_IMPL(push_back);
  FATAL_CALL_TRAITS_IMPL(push_front);
  FATAL_CALL_TRAITS_IMPL(rbegin);
  FATAL_CALL_TRAITS_IMPL(read);
  FATAL_CALL_TRAITS_IMPL(reference);
  FATAL_CALL_TRAITS_IMPL(rehash);
  FATAL_CALL_TRAITS_IMPL(remove);
  FATAL_CALL_TRAITS_IMPL(rend);
  FATAL_CALL_TRAITS_IMPL(replace);
  FATAL_CALL_TRAITS_IMPL(reserve);
  FATAL_CALL_TRAITS_IMPL(resize);
  FATAL_CALL_TRAITS_IMPL(reverse);
  FATAL_CALL_TRAITS_IMPL(rfind);
  FATAL_CALL_TRAITS_IMPL(rng);
  FATAL_CALL_TRAITS_IMPL(rotate);
  FATAL_CALL_TRAITS_IMPL(search);
  FATAL_CALL_TRAITS_IMPL(select);
  FATAL_CALL_TRAITS_IMPL(serialize);
  FATAL_CALL_TRAITS_IMPL(set);
  FATAL_CALL_TRAITS_IMPL(shrink_to_fit);
  FATAL_CALL_TRAITS_IMPL(shuffle);
  FATAL_CALL_TRAITS_IMPL(size);
  FATAL_CALL_TRAITS_IMPL(sort);
  FATAL_CALL_TRAITS_IMPL(stable_sort);
  FATAL_CALL_TRAITS_IMPL(substr);
  FATAL_CALL_TRAITS_IMPL(swap);
  FATAL_CALL_TRAITS_IMPL(to);
  FATAL_CALL_TRAITS_IMPL(to_str);
  FATAL_CALL_TRAITS_IMPL(to_string);
  FATAL_CALL_TRAITS_IMPL(to_wstring);
  FATAL_CALL_TRAITS_IMPL(transform);
  FATAL_CALL_TRAITS_IMPL(try_emplace);
  FATAL_CALL_TRAITS_IMPL(try_get);
  FATAL_CALL_TRAITS_IMPL(try_set);
  FATAL_CALL_TRAITS_IMPL(try_to);
  FATAL_CALL_TRAITS_IMPL(unique);
  FATAL_CALL_TRAITS_IMPL(upper_bound);
  FATAL_CALL_TRAITS_IMPL(value);
  FATAL_CALL_TRAITS_IMPL(visit);
  FATAL_CALL_TRAITS_IMPL(visit_if);
  FATAL_CALL_TRAITS_IMPL(write);

# undef FATAL_CALL_TRAITS_IMPL
};

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
