/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_call_traits_h
#define FATAL_INCLUDE_fatal_type_call_traits_h

#include <fatal/type/transform.h>

#include <utility>

namespace fatal {
namespace detail {
namespace call_traits_impl {

template <typename T> T arg();

template <bool> struct call_if;

} // namespace call_traits_impl {
} // namespace detail {

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
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

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
class call_operator_traits {
  template <typename... Args>
  struct is_impl {
    template <
      typename T,
      typename = decltype(
        detail::call_traits_impl::arg<T>()(
          detail::call_traits_impl::arg<Args>()...
        )
      )
    >
    static std::true_type sfinae(T *);

    template <typename...>
    static std::false_type sfinae(...);
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
   *    << call_operator_traits::supports<Foo>::value
   *    << ' ' << std::boolalpha
   *    << call_operator_traits::supports<Foo, int>::value
   *    << ' ' << std::boolalpha
   *    << call_operator_traits::supports<Foo, int, double>::value
   *    << ' ' << std::boolalpha
   *    << call_operator_traits::supports<Foo, int, std::string>::value
   *    << std::endl
   *    << ' ' << std::boolalpha
   *    << call_operator_traits::supports<lambda>::value
   *    << ' ' << std::boolalpha
   *    << call_operator_traits::supports<lambda, int>::value
   *    << ' ' << std::boolalpha
   *    << call_operator_traits::supports<lambda, int, double>::value
   *    << ' ' << std::boolalpha
   *    << call_operator_traits::supports<lambda, int, std::string>::value;
   *
   * Outputs:
   *  true false false true
   *  false false false true
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename T, typename... Args>
  using supports = decltype(
    is_impl<Args...>::template sfinae(static_cast<T *>(nullptr))
  );
};

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_CALL_TRAITS(Name, ...) \
  class Name { \
    template <typename... UArgs> \
    struct member_fn_supports_impl { \
      template < \
        typename U, \
        typename = decltype( \
          ::fatal::detail::call_traits_impl::arg<U>().__VA_ARGS__( \
            ::fatal::detail::call_traits_impl::arg<UArgs>()... \
          ) \
        ) \
      > \
      static ::std::true_type sfinae(U *); \
      \
      template <typename...> \
      static ::std::false_type sfinae(...); \
    }; \
    \
    template <typename... UArgs> \
    struct static_member_supports_impl { \
      template < \
        typename U, \
        typename = decltype( \
          U::__VA_ARGS__( \
            ::fatal::detail::call_traits_impl::arg<UArgs>()... \
          ) \
        ) \
      > \
      static ::std::true_type sfinae(U *); \
      \
      template <typename...> \
      static ::std::false_type sfinae(...); \
    }; \
  \
  public: \
    struct member_function { \
      constexpr member_function() {} \
      \
      template <typename U> \
      struct bind { \
        template <typename... UArgs> \
        using supports = decltype( \
          member_fn_supports_impl<UArgs...>::template sfinae( \
            static_cast<typename ::std::remove_reference<U>::type *>(nullptr) \
          ) \
        ); \
      }; \
      \
      template <typename U, typename... UArgs> \
      using supports = typename bind<U>::template supports<UArgs...>; \
      \
      template <typename U, typename... UArgs> \
      constexpr static auto call(U &&subject, UArgs &&...args) \
        -> decltype(subject.__VA_ARGS__(::std::forward<UArgs>(args)...)) \
      { return subject.__VA_ARGS__(::std::forward<UArgs>(args)...); } \
      \
      template <typename U, typename... UArgs> \
      constexpr auto operator ()(U &&subject, UArgs &&...args) const \
        -> decltype( \
          call(::std::forward<U>(subject), ::std::forward<UArgs>(args)...) \
        ) \
      { \
        return call( \
          ::std::forward<U>(subject), \
          ::std::forward<UArgs>(args)... \
        ); \
      } \
    }; \
    \
    struct static_member { \
      template <typename U> \
      struct bind { \
        constexpr bind() {} \
        \
        using type = U; \
        \
        template <typename... UArgs> \
        constexpr static auto call(UArgs &&...args) \
          -> decltype(type::__VA_ARGS__(::std::forward<UArgs>(args)...)) \
        { return type::__VA_ARGS__(::std::forward<UArgs>(args)...); } \
        \
        template <typename... UArgs> \
        constexpr auto operator ()(UArgs &&...args) const \
          -> decltype(call(::std::forward<UArgs>(args)...)) \
        { return call(::std::forward<UArgs>(args)...); } \
        \
        template <typename... UArgs> \
        using supports = decltype( \
          static_member_supports_impl<UArgs...>::sfinae( \
            static_cast<U *>(nullptr) \
          ) \
        ); \
      }; \
      \
      template <typename U, typename... UArgs> \
      constexpr static auto call(UArgs &&...args) \
        -> decltype(bind<U>::call(::std::forward<UArgs>(args)...)) \
      { return bind<U>::call(::std::forward<UArgs>(args)...); } \
      \
      template <typename U, typename... UArgs> \
      using supports = decltype( \
        static_member_supports_impl<UArgs...>::sfinae( \
          static_cast<U *>(nullptr) \
        ) \
      ); \
    }; \
  }

/**
 * Some pre-instantiated call traits for common method names.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
struct call_traits {
# define FATAL_CALL_TRAITS_IMPL(Name) FATAL_CALL_TRAITS(Name, Name)

  FATAL_CALL_TRAITS_IMPL(abort);
  FATAL_CALL_TRAITS_IMPL(absolute);
  FATAL_CALL_TRAITS_IMPL(add);
  FATAL_CALL_TRAITS_IMPL(advance);
  FATAL_CALL_TRAITS_IMPL(append);
  FATAL_CALL_TRAITS_IMPL(append_to);
  FATAL_CALL_TRAITS_IMPL(apply);
  FATAL_CALL_TRAITS_IMPL(assign);
  FATAL_CALL_TRAITS_IMPL(at);
  FATAL_CALL_TRAITS_IMPL(back);
  FATAL_CALL_TRAITS_IMPL(begin);
  FATAL_CALL_TRAITS_IMPL(binary_search);
  FATAL_CALL_TRAITS_IMPL(bind);
  FATAL_CALL_TRAITS_IMPL(call);
  FATAL_CALL_TRAITS_IMPL(cancel);
  FATAL_CALL_TRAITS_IMPL(capacity);
  FATAL_CALL_TRAITS_IMPL(cbegin);
  FATAL_CALL_TRAITS_IMPL(cend);
  FATAL_CALL_TRAITS_IMPL(center);
  FATAL_CALL_TRAITS_IMPL(check);
  FATAL_CALL_TRAITS_IMPL(child);
  FATAL_CALL_TRAITS_IMPL(children);
  FATAL_CALL_TRAITS_IMPL(clear);
  FATAL_CALL_TRAITS_IMPL(clone);
  FATAL_CALL_TRAITS_IMPL(compare);
  FATAL_CALL_TRAITS_IMPL(compress);
  FATAL_CALL_TRAITS_IMPL(concat);
  FATAL_CALL_TRAITS_IMPL(concatenate);
  FATAL_CALL_TRAITS_IMPL(condition);
  FATAL_CALL_TRAITS_IMPL(config);
  FATAL_CALL_TRAITS_IMPL(configuration);
  FATAL_CALL_TRAITS_IMPL(configure);
  FATAL_CALL_TRAITS_IMPL(construct);
  FATAL_CALL_TRAITS_IMPL(contains);
  FATAL_CALL_TRAITS_IMPL(convert);
  FATAL_CALL_TRAITS_IMPL(copy);
  FATAL_CALL_TRAITS_IMPL(count);
  FATAL_CALL_TRAITS_IMPL(cptr);
  FATAL_CALL_TRAITS_IMPL(crbegin);
  FATAL_CALL_TRAITS_IMPL(cref);
  FATAL_CALL_TRAITS_IMPL(crend);
  FATAL_CALL_TRAITS_IMPL(c_str);
  FATAL_CALL_TRAITS_IMPL(data);
  FATAL_CALL_TRAITS_IMPL(decode);
  FATAL_CALL_TRAITS_IMPL(dequeue);
  FATAL_CALL_TRAITS_IMPL(deserialize);
  FATAL_CALL_TRAITS_IMPL(destroy);
  FATAL_CALL_TRAITS_IMPL(emplace);
  FATAL_CALL_TRAITS_IMPL(emplace_back);
  FATAL_CALL_TRAITS_IMPL(emplace_front);
  FATAL_CALL_TRAITS_IMPL(empty);
  FATAL_CALL_TRAITS_IMPL(encode);
  FATAL_CALL_TRAITS_IMPL(end);
  FATAL_CALL_TRAITS_IMPL(enqueue);
  FATAL_CALL_TRAITS_IMPL(equal);
  FATAL_CALL_TRAITS_IMPL(equal_range);
  FATAL_CALL_TRAITS_IMPL(erase);
  FATAL_CALL_TRAITS_IMPL(exact);
  FATAL_CALL_TRAITS_IMPL(exit);
  FATAL_CALL_TRAITS_IMPL(extension);
  FATAL_CALL_TRAITS_IMPL(find);
  FATAL_CALL_TRAITS_IMPL(find_first_not_of);
  FATAL_CALL_TRAITS_IMPL(find_first_of);
  FATAL_CALL_TRAITS_IMPL(find_last_not_of);
  FATAL_CALL_TRAITS_IMPL(find_last_of);
  FATAL_CALL_TRAITS_IMPL(finish);
  FATAL_CALL_TRAITS_IMPL(finished);
  FATAL_CALL_TRAITS_IMPL(first);
  FATAL_CALL_TRAITS_IMPL(for_each);
  FATAL_CALL_TRAITS_IMPL(foreach);
  FATAL_CALL_TRAITS_IMPL(foreach_if);
  FATAL_CALL_TRAITS_IMPL(fork);
  FATAL_CALL_TRAITS_IMPL(forward);
  FATAL_CALL_TRAITS_IMPL(fourth);
  FATAL_CALL_TRAITS_IMPL(freeze);
  FATAL_CALL_TRAITS_IMPL(from);
  FATAL_CALL_TRAITS_IMPL(front);
  FATAL_CALL_TRAITS_IMPL(get);
  FATAL_CALL_TRAITS_IMPL(getline);
  FATAL_CALL_TRAITS_IMPL(get_pointer);
  FATAL_CALL_TRAITS_IMPL(get_reference);
  FATAL_CALL_TRAITS_IMPL(go);
  FATAL_CALL_TRAITS_IMPL(halt);
  FATAL_CALL_TRAITS_IMPL(hash);
  FATAL_CALL_TRAITS_IMPL(id);
  FATAL_CALL_TRAITS_IMPL(index);
  FATAL_CALL_TRAITS_IMPL(index_of);
  FATAL_CALL_TRAITS_IMPL(init);
  FATAL_CALL_TRAITS_IMPL(initialize);
  FATAL_CALL_TRAITS_IMPL(inject);
  FATAL_CALL_TRAITS_IMPL(insert);
  FATAL_CALL_TRAITS_IMPL(join);
  FATAL_CALL_TRAITS_IMPL(joinable);
  FATAL_CALL_TRAITS_IMPL(last);
  FATAL_CALL_TRAITS_IMPL(left);
  FATAL_CALL_TRAITS_IMPL(length);
  FATAL_CALL_TRAITS_IMPL(lift);
  FATAL_CALL_TRAITS_IMPL(limit);
  FATAL_CALL_TRAITS_IMPL(list);
  FATAL_CALL_TRAITS_IMPL(lock);
  FATAL_CALL_TRAITS_IMPL(longest);
  FATAL_CALL_TRAITS_IMPL(loop);
  FATAL_CALL_TRAITS_IMPL(lower_bound);
  FATAL_CALL_TRAITS_IMPL(map);
  FATAL_CALL_TRAITS_IMPL(match);
  FATAL_CALL_TRAITS_IMPL(max);
  FATAL_CALL_TRAITS_IMPL(max_size);
  FATAL_CALL_TRAITS_IMPL(mean);
  FATAL_CALL_TRAITS_IMPL(median);
  FATAL_CALL_TRAITS_IMPL(merge);
  FATAL_CALL_TRAITS_IMPL(mid);
  FATAL_CALL_TRAITS_IMPL(middle);
  FATAL_CALL_TRAITS_IMPL(mimic);
  FATAL_CALL_TRAITS_IMPL(min);
  FATAL_CALL_TRAITS_IMPL(move);
  FATAL_CALL_TRAITS_IMPL(multi_append);
  FATAL_CALL_TRAITS_IMPL(name);
  FATAL_CALL_TRAITS_IMPL(next);
  FATAL_CALL_TRAITS_IMPL(next_as);
  FATAL_CALL_TRAITS_IMPL(nth_element);
  FATAL_CALL_TRAITS_IMPL(observe);
  FATAL_CALL_TRAITS_IMPL(off);
  FATAL_CALL_TRAITS_IMPL(offset);
  FATAL_CALL_TRAITS_IMPL(on);
  FATAL_CALL_TRAITS_IMPL(partial);
  FATAL_CALL_TRAITS_IMPL(partial_sort);
  FATAL_CALL_TRAITS_IMPL(partition);
  FATAL_CALL_TRAITS_IMPL(pause);
  FATAL_CALL_TRAITS_IMPL(payload);
  FATAL_CALL_TRAITS_IMPL(pending);
  FATAL_CALL_TRAITS_IMPL(piece);
  FATAL_CALL_TRAITS_IMPL(pieces);
  FATAL_CALL_TRAITS_IMPL(pinpoint);
  FATAL_CALL_TRAITS_IMPL(pointer);
  FATAL_CALL_TRAITS_IMPL(poll);
  FATAL_CALL_TRAITS_IMPL(pop);
  FATAL_CALL_TRAITS_IMPL(pop_back);
  FATAL_CALL_TRAITS_IMPL(pop_front);
  FATAL_CALL_TRAITS_IMPL(predicate);
  FATAL_CALL_TRAITS_IMPL(prefix);
  FATAL_CALL_TRAITS_IMPL(prefixes);
  FATAL_CALL_TRAITS_IMPL(prev);
  FATAL_CALL_TRAITS_IMPL(previous);
  FATAL_CALL_TRAITS_IMPL(properties);
  FATAL_CALL_TRAITS_IMPL(property);
  FATAL_CALL_TRAITS_IMPL(ptr);
  FATAL_CALL_TRAITS_IMPL(push);
  FATAL_CALL_TRAITS_IMPL(push_back);
  FATAL_CALL_TRAITS_IMPL(push_front);
  FATAL_CALL_TRAITS_IMPL(put);
  FATAL_CALL_TRAITS_IMPL(queue);
  FATAL_CALL_TRAITS_IMPL(quit);
  FATAL_CALL_TRAITS_IMPL(rbegin);
  FATAL_CALL_TRAITS_IMPL(read);
  FATAL_CALL_TRAITS_IMPL(ref);
  FATAL_CALL_TRAITS_IMPL(reference);
  FATAL_CALL_TRAITS_IMPL(rehash);
  FATAL_CALL_TRAITS_IMPL(relative);
  FATAL_CALL_TRAITS_IMPL(release);
  FATAL_CALL_TRAITS_IMPL(remove);
  FATAL_CALL_TRAITS_IMPL(rend);
  FATAL_CALL_TRAITS_IMPL(replace);
  FATAL_CALL_TRAITS_IMPL(reserve);
  FATAL_CALL_TRAITS_IMPL(reset);
  FATAL_CALL_TRAITS_IMPL(resize);
  FATAL_CALL_TRAITS_IMPL(resume);
  FATAL_CALL_TRAITS_IMPL(reverse);
  FATAL_CALL_TRAITS_IMPL(rfind);
  FATAL_CALL_TRAITS_IMPL(right);
  FATAL_CALL_TRAITS_IMPL(rng);
  FATAL_CALL_TRAITS_IMPL(rotate);
  FATAL_CALL_TRAITS_IMPL(run);
  FATAL_CALL_TRAITS_IMPL(search);
  FATAL_CALL_TRAITS_IMPL(second);
  FATAL_CALL_TRAITS_IMPL(seek);
  FATAL_CALL_TRAITS_IMPL(select);
  FATAL_CALL_TRAITS_IMPL(serialize);
  FATAL_CALL_TRAITS_IMPL(set);
  FATAL_CALL_TRAITS_IMPL(setup);
  FATAL_CALL_TRAITS_IMPL(shortest);
  FATAL_CALL_TRAITS_IMPL(shrink_to_fit);
  FATAL_CALL_TRAITS_IMPL(shuffle);
  FATAL_CALL_TRAITS_IMPL(shutdown);
  FATAL_CALL_TRAITS_IMPL(size);
  FATAL_CALL_TRAITS_IMPL(slice);
  FATAL_CALL_TRAITS_IMPL(sort);
  FATAL_CALL_TRAITS_IMPL(spawn);
  FATAL_CALL_TRAITS_IMPL(split);
  FATAL_CALL_TRAITS_IMPL(split_step);
  FATAL_CALL_TRAITS_IMPL(stable_sort);
  FATAL_CALL_TRAITS_IMPL(stack);
  FATAL_CALL_TRAITS_IMPL(standard_deviation);
  FATAL_CALL_TRAITS_IMPL(start);
  FATAL_CALL_TRAITS_IMPL(stddev);
  FATAL_CALL_TRAITS_IMPL(steal);
  FATAL_CALL_TRAITS_IMPL(stop);
  FATAL_CALL_TRAITS_IMPL(str);
  FATAL_CALL_TRAITS_IMPL(string);
  FATAL_CALL_TRAITS_IMPL(substr);
  FATAL_CALL_TRAITS_IMPL(suspend);
  FATAL_CALL_TRAITS_IMPL(swap);
  FATAL_CALL_TRAITS_IMPL(thaw);
  FATAL_CALL_TRAITS_IMPL(third);
  FATAL_CALL_TRAITS_IMPL(to);
  FATAL_CALL_TRAITS_IMPL(to_str);
  FATAL_CALL_TRAITS_IMPL(to_string);
  FATAL_CALL_TRAITS_IMPL(to_wstring);
  FATAL_CALL_TRAITS_IMPL(transcode);
  FATAL_CALL_TRAITS_IMPL(transform);
  FATAL_CALL_TRAITS_IMPL(try_emplace);
  FATAL_CALL_TRAITS_IMPL(try_get);
  FATAL_CALL_TRAITS_IMPL(try_lock);
  FATAL_CALL_TRAITS_IMPL(try_set);
  FATAL_CALL_TRAITS_IMPL(try_to);
  FATAL_CALL_TRAITS_IMPL(turn);
  FATAL_CALL_TRAITS_IMPL(type);
  FATAL_CALL_TRAITS_IMPL(unique);
  FATAL_CALL_TRAITS_IMPL(unlock);
  FATAL_CALL_TRAITS_IMPL(update);
  FATAL_CALL_TRAITS_IMPL(upper_bound);
  FATAL_CALL_TRAITS_IMPL(value);
  FATAL_CALL_TRAITS_IMPL(variance);
  FATAL_CALL_TRAITS_IMPL(visit);
  FATAL_CALL_TRAITS_IMPL(visit_if);
  FATAL_CALL_TRAITS_IMPL(which);
  FATAL_CALL_TRAITS_IMPL(with);
  FATAL_CALL_TRAITS_IMPL(write);

# undef FATAL_CALL_TRAITS_IMPL
};

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_FREE_FUNCTION_CALL_TRAITS(Name, ...) \
  struct Name { \
    constexpr Name() {} \
    \
    template <typename... UArgs> \
    constexpr static auto call(UArgs &&...args) \
      -> decltype(__VA_ARGS__(::std::forward<UArgs>(args)...)) \
    { return __VA_ARGS__(::std::forward<UArgs>(args)...); } \
    \
    template <typename... UArgs> \
    constexpr auto operator ()(UArgs &&...args) const \
      -> decltype(call(::std::forward<UArgs>(args)...)) \
    { return call(::std::forward<UArgs>(args)...); } \
  }; \

/**
 * Conditionally calls a function according to the given predicate.
 *
 * The provided call traits is used to decide which function to call, perfectly
 * forwarding the given parameters to it.
 *
 * When the predicate evaluates to false, `Fallback` is instantiated and its
 * call operator is called, perfectly forwarding the parameters to it.
 *
 * An optional list of template parameters `Args` can also be passed to the
 * function.
 *
 * The predicate is a type template. It receives the optional list of template
 * parameters and the types of the arguments given to `call_if`, and must
 * evaluate to a type similar to `std::integral_constant` of type `bool`.
 * In other words, `Predicate<Args..., UArgs...>::value` is used to decide
 * whether to call the function, through the call traits, or the fallback.
 *
 * Default predicate calls the function if it exists and supported the given
 * parameters, otherwise it calls the fallback.
 *
 * Example:
 *
 *  struct fallback {
 *    template <typename... Args>
 *    int operator ()(Args &&...) { return 98765; }
 *  };
 *
 *  struct foo {
 *    int bar() { return 12345; }
 *    double bar(char const *, bool) { return 5.6; }
 *  };
 *
 *  FATAL_CALL_TRAITS(bar_traits, bar);
 *
 *  using traits = bar_traits::member_function;
 *
 *  // yields `12345`
 *  auto result1 = call_if<traits, fallback>();
 *
 *  // yields `98765`
 *  auto result2 = call_if<traits, fallback>("hello", "world", 143);
 *
 *  // yields `5.6`
 *  auto result3 = call_if<traits, fallback>("test", true);
 *
 *  // yields `98765`
 *  auto result4 = call_if<traits, fallback>(10);
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  typename CallTraits,
  typename Fallback,
  template <typename...> class Predicate = CallTraits::template supports,
  typename... Args,
  typename... UArgs
>
constexpr auto call_if(UArgs &&...args)
  -> decltype(
    detail::call_traits_impl::call_if<
      fatal::apply<Predicate, Args..., UArgs...>::value
    >::template call<CallTraits, Fallback, Args...>(
      std::forward<UArgs>(args)...
    )
  )
{
  return detail::call_traits_impl::call_if<
    fatal::apply<Predicate, Args..., UArgs...>::value
  >::template call<CallTraits, Fallback, Args...>(
    std::forward<UArgs>(args)...
  );
}

////////////////////////////
// IMPLEMENTATION DETAILS //
////////////////////////////

namespace detail {
namespace call_traits_impl {

template <bool>
struct call_if {
  template <typename Traits, typename, typename... Args, typename... UArgs>
  constexpr static auto call(UArgs &&...args)
    -> decltype(Traits::template call<Args...>(std::forward<UArgs>(args)...))
  {
    return Traits::template call<Args...>(std::forward<UArgs>(args)...);
  }
};

template <>
struct call_if<false> {
  template <typename, typename Fn, typename... Args, typename... UArgs>
  constexpr static auto call(UArgs &&...args)
    -> decltype(Fn()(std::forward<UArgs>(args)...))
  {
    return Fn()(std::forward<UArgs>(args)...);
  }
};

} // namespace call_traits_impl {
} // namespace detail {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_call_traits_h
