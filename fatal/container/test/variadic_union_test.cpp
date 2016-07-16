/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/container/variadic_union.h>

#include <fatal/test/driver.h>
#include <fatal/test/ref_counter.h>

#include <memory>
#include <string>
#include <type_traits>

namespace fatal {

FATAL_TEST(variadic_union, sanity_check) {
  variadic_union<int, double, bool> v;

  v.construct<int>(5);
  FATAL_EXPECT_EQ(5, v.ref<int>());
  v.ref<int>() = 10;
  FATAL_EXPECT_EQ(10, v.ref<int>());
  v.destroy<int>();

  v.construct<double>(5.6);
  FATAL_EXPECT_EQ(5.6, v.ref<double>());
  v.ref<double>() = 7.2;
  FATAL_EXPECT_EQ(7.2, v.ref<double>());
  v.destroy<double>();

  v.construct<bool>(true);
  FATAL_EXPECT_EQ(true, v.ref<bool>());
  v.ref<bool>() = false;
  FATAL_EXPECT_EQ(false, v.ref<bool>());
  v.destroy<bool>();
}

template <typename> struct X {};

#define FATAL_IMPL_CALL(Fn) \
  do { \
    Fn(); \
    \
    Fn(int); \
    Fn(double); \
    Fn(bool); \
    Fn(std::string); \
    \
    Fn(int, int); \
    Fn(int, double); \
    Fn(int, bool); \
    Fn(int, std::string); \
    Fn(double, int); \
    Fn(double, double); \
    Fn(double, bool); \
    Fn(double, std::string); \
    Fn(bool, int); \
    Fn(bool, bool); \
    Fn(bool, double); \
    Fn(bool, std::string); \
    Fn(std::string, int); \
    Fn(std::string, double); \
    Fn(std::string, bool); \
    Fn(std::string, std::string); \
    \
    Fn(int, int, int); \
    Fn(int, double, bool); \
    Fn(int, bool, double); \
    Fn(double, double, double); \
    Fn(double, int, bool); \
    Fn(double, bool, int); \
    Fn(bool, bool, bool); \
    Fn(bool, int, double); \
    Fn(bool, double, int); \
    \
    Fn(int, double, bool, std::string); \
    Fn(std::string, bool, double, int); \
    \
    Fn(bool, std::uint8_t, float, double, bool); \
    Fn(bool, std::uint8_t, float, double, std::string); \
    \
    Fn(bool, std::string, std::uint16_t, float, double, std::string); \
    Fn(bool, std::uint8_t, std::uint16_t, float, double, std::string); \
    \
    Fn(int, double, bool, std::string, bool, double, int); \
    Fn( \
      bool, std::uint8_t, std::uint16_t, std::uint32_t, \
      float, double, std::string \
    ); \
    \
    Fn(int, double, bool, std::string, std::string, bool, double, int); \
    Fn( \
      bool, std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t, \
      float, double, std::string \
    ); \
  } while (false)

//////////////////////////////////
// variadic_union::default_ctor //
//////////////////////////////////

template <typename... Args>
void check_default_ctor(...) {
  auto fn = []() {
    variadic_union<Args...> v;
    return std::addressof(v) != nullptr;
  };

  FATAL_EXPECT_NO_THROW {
    fn();
  };
}

#define FATAL_IMPL_CHECK_DEFAULT_CTOR(...) \
  do { \
    check_default_ctor<__VA_ARGS__>(); \
  } while (false)

FATAL_TEST(variadic_union, default_ctor) {
  FATAL_IMPL_CALL(FATAL_IMPL_CHECK_DEFAULT_CTOR);
}

//////////////////////////
// variadic_union::list //
//////////////////////////

#define FATAL_IMPL_CHECK_LIST(...) \
  do { \
    using type = variadic_union<__VA_ARGS__>; \
    using expected = type_list<__VA_ARGS__>::unique<>; \
    FATAL_EXPECT_SAME<expected, typename type::list>(); \
  } while (false)

FATAL_TEST(variadic_union, list) {
  FATAL_IMPL_CALL(FATAL_IMPL_CHECK_LIST);
}

//////////////////////////////
// variadic_union::supports //
//////////////////////////////

template <typename... Args>
struct check_supports {
  using type = variadic_union<Args...>;

  using supported = logical::all<
    std::true_type,
    typename type::template supports<Args>...
  >;

  using not_supported = typename logical::any<
    std::false_type,
    typename type::template supports<X<Args>>...,
    typename type::template supports<Args const>...,
    typename type::template supports<Args const &>...,
    typename type::template supports<Args &>...,
    typename type::template supports<Args const &&>...,
    typename type::template supports<Args &&>...,
    typename type::template supports<Args const *>...,
    typename type::template supports<Args *>...
  >;

  using result = typename logical::all<
    supported,
    logical::negate<not_supported>
  >;
};

#define FATAL_IMPL_CHECK_SUPPORTS(...) \
  do { \
    using check = check_supports<__VA_ARGS__>; \
    FATAL_EXPECT_TRUE(check::result::value); \
  } while (false)

FATAL_TEST(variadic_union, supports) {
  FATAL_IMPL_CALL(FATAL_IMPL_CHECK_SUPPORTS);
}

//////////////////////////
// variadic_union::size //
//////////////////////////

template <typename... Args>
struct check_size {
  using expected = typename type_list<Args...>
    ::template transform<sizeof_transform>
    ::template choose<comparison_transform::greater_than>;

  using result = typename comparison_transform::equal<
    expected,
    sizeof_transform<variadic_union<Args...>>
  >;
};

template <>
struct check_size<> {
  using result = std::integral_constant<bool, sizeof(variadic_union<>) == 1>;
};

#define FATAL_IMPL_CHECK_SIZE(...) \
  do { \
    using check = check_size<__VA_ARGS__>; \
    FATAL_EXPECT_TRUE(check::result::value); \
  } while (false)

FATAL_TEST(variadic_union, size) {
  FATAL_IMPL_CALL(FATAL_IMPL_CHECK_SIZE);
}

/////////////////////////
// variadic_union::ref //
/////////////////////////

template <typename T>
struct reference_pointer_value {
  static_assert(std::is_scalar<T>::value, "scalar expected");

  static T value(bool b) {
    return static_cast<T>(b);
  }
};

template <>
struct reference_pointer_value<std::string> {
  static std::string value(bool b) {
    return b ? "hello" : "world";
  }
};

template <typename...> struct reference_pointer_test;

template <typename T, typename... Args>
struct reference_pointer_test<T, Args...> {
  template <typename U>
  static std::size_t test_reference(U &u, std::size_t count = 0) {
    U const &c = u;

    using type = decltype(u.template ref<T>());
    using c_type = decltype(c.template ref<T>());

    static_assert(
      std::is_same<T &, type>::value,
      "unexpected reference type"
    );
    static_assert(
      std::is_same<T const &, c_type>::value,
      "unexpected constant reference type"
    );

    {
      auto const value_0 = reference_pointer_value<T>::value(false);
      new (std::addressof(u.template ref<T>())) T(value_0);
      FATAL_EXPECT_EQ(value_0, u.template ref<T>());
      FATAL_EXPECT_EQ(value_0, c.template ref<T>());
      (u.template ref<T>()).~T();
    }

    {
      auto const value_1 = reference_pointer_value<T>::value(true);
      new (std::addressof(u.template ref<T>())) T(value_1);
      FATAL_EXPECT_EQ(value_1, u.template ref<T>());
      FATAL_EXPECT_EQ(value_1, c.template ref<T>());
      (u.template ref<T>()).~T();
    }

    {
      T const *p = reinterpret_cast<T *>(std::addressof(u));
      FATAL_EXPECT_EQ(p, std::addressof(u.template ref<T>()));
      FATAL_EXPECT_EQ(p, std::addressof(c.template ref<T>()));
    }

    return reference_pointer_test<Args...>::test_reference(
      u, count + 1
    );
  }

  template <typename U>
  static std::size_t test_pointer(U &u, std::size_t count = 0) {
    U const &c = u;

    using type = decltype(u.template ptr<T>());
    using c_type = decltype(c.template ptr<T>());

    static_assert(
      std::is_same<T *, type>::value,
      "unexpected pointer type"
    );
    static_assert(
      std::is_same<T const *, c_type>::value,
      "unexpected constant pointer type"
    );

    {
      auto const value_0 = reference_pointer_value<T>::value(false);
      new (u.template ptr<T>()) T(value_0);
      FATAL_EXPECT_EQ(value_0, *u.template ptr<T>());
      FATAL_EXPECT_EQ(value_0, *c.template ptr<T>());
      (u.template ptr<T>())->~T();
    }

    {
      auto const value_1 = reference_pointer_value<T>::value(true);
      new (u.template ptr<T>()) T(value_1);
      FATAL_EXPECT_EQ(value_1, *u.template ptr<T>());
      FATAL_EXPECT_EQ(value_1, *c.template ptr<T>());
      (u.template ptr<T>())->~T();
    }

    {
      T const *p = reinterpret_cast<T *>(std::addressof(u));
      FATAL_EXPECT_EQ(p, u.template ptr<T>());
      FATAL_EXPECT_EQ(p, c.template ptr<T>());
    }

    return reference_pointer_test<Args...>::test_pointer(
      u, count + 1
    );
  }
};

template <>
struct reference_pointer_test<> {
  template <typename U>
  static std::size_t test_reference(U &, std::size_t count = 0) {
    return count;
  }

  template <typename U>
  static std::size_t test_pointer(U &, std::size_t count = 0) {
    return count;
  }
};

#define FATAL_IMPL_CHECK_REFERENCE_POINTER(Which, ...) \
  do { \
    variadic_union<__VA_ARGS__> v; \
    using list = type_list<__VA_ARGS__>; \
    auto const count = reference_pointer_test<__VA_ARGS__>::test_##Which(v); \
    FATAL_EXPECT_EQ(list::size, count); \
  } while (false)

#define FATAL_IMPL_CHECK_REFERENCE(...) \
  FATAL_IMPL_CHECK_REFERENCE_POINTER(reference, __VA_ARGS__)

FATAL_TEST(variadic_union, ref) {
  FATAL_IMPL_CALL(FATAL_IMPL_CHECK_REFERENCE);
}

/////////////////////////
// variadic_union::ptr //
/////////////////////////

#define FATAL_IMPL_CHECK_POINTER(...) \
  FATAL_IMPL_CHECK_REFERENCE_POINTER(pointer, __VA_ARGS__)

FATAL_TEST(variadic_union, ptr) {
  FATAL_IMPL_CALL(FATAL_IMPL_CHECK_POINTER);
}

///////////////////////////////
// variadic_union::construct //
///////////////////////////////

template <typename... Args>
using variadic_union_ctor_test = variadic_union<ref_counter<Args>...>;

struct construct_test {
  template <typename T, std::size_t Index, typename V>
  void operator ()(indexed_type_tag<T, Index>, V &&v) const {
    FATAL_ASSERT_EQ(0, ref_counter_alive());
    using type = ref_counter<T>;
    new (v.template ptr<type>()) type();
    FATAL_EXPECT_EQ(1, ref_counter_alive());
    v.template ref<type>().~type();
    FATAL_EXPECT_EQ(0, ref_counter_alive());
  }
};

struct destroy_test {
  template <typename T, std::size_t Index, typename V>
  void operator ()(indexed_type_tag<T, Index>, V &&v) const {
    FATAL_ASSERT_EQ(0, ref_counter_alive());
    using type = ref_counter<T>;
    v.template construct<type>();
    FATAL_EXPECT_EQ(1, ref_counter_alive());
    v.template destroy<type>();
    FATAL_EXPECT_EQ(0, ref_counter_alive());
  }
};

#define FATAL_IMPL_CHECK_CONSTRUCT_DESTROY(Which, ...) \
  do { \
    FATAL_ASSERT_EQ(0, ref_counter_alive()); \
    variadic_union_ctor_test<__VA_ARGS__> v; \
    using list = type_list<__VA_ARGS__>; \
    list::foreach(Which##_test(), v); \
    FATAL_EXPECT_EQ(0, ref_counter_alive()); \
  } while (false)

#define FATAL_IMPL_CHECK_CONSTRUCT(...) \
  FATAL_IMPL_CHECK_CONSTRUCT_DESTROY(construct, __VA_ARGS__)

FATAL_TEST(variadic_union, construct) {
  ref_counter_guard guard;
  FATAL_IMPL_CALL(FATAL_IMPL_CHECK_CONSTRUCT);
}

/////////////////////////////
// variadic_union::destroy //
/////////////////////////////

#define FATAL_IMPL_CHECK_DESTROY(...) \
  FATAL_IMPL_CHECK_CONSTRUCT_DESTROY(destroy, __VA_ARGS__)

FATAL_TEST(variadic_union, destroy) {
  ref_counter_guard guard;
  FATAL_IMPL_CALL(FATAL_IMPL_CHECK_DESTROY);
}

} // namespace fatal {
