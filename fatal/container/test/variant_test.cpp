/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/container/variant.h>

#include <fatal/test/driver.h>

#include <folly/Arena.h>
#include <folly/Memory.h>
#include <folly/Optional.h>
#include <folly/String.h>
#include <folly/dynamic.h>

#include <initializer_list>
#include <map>
#include <queue>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fatal {

using namespace folly;

static unsigned long long allocated = 0;
static unsigned long long freed = 0;

template <typename Allocator, typename T>
struct MemoryLeakCheckerAllocator {
  typedef T value_type;
  typedef value_type *pointer;
  typedef value_type const *const_pointer;
  typedef value_type &reference;
  typedef value_type const *const_reference;

  typedef std::ptrdiff_t difference_type;
  typedef std::size_t size_type;

  static_assert(
    std::is_same<value_type, typename Allocator::value_type>::value,
    "wrapped allocator has a different value_type"
  );

  explicit MemoryLeakCheckerAllocator() {}

  explicit MemoryLeakCheckerAllocator(Allocator allocator):
    allocator_(allocator)
  {}

  template <typename UAllocator, typename U>
  MemoryLeakCheckerAllocator(
    MemoryLeakCheckerAllocator<UAllocator, U> const &other
  ):
    allocator_(other.allocator())
  {}

  value_type* allocate(size_t n, const void* hint = nullptr) {
    auto p = allocator_.allocate(n, hint);
    allocated += n * sizeof(value_type);
    return p;
  }

  void deallocate(value_type* p, size_t n) {
    allocator_.deallocate(p, n);
    freed += n * sizeof(value_type);
  }

  size_t max_size() const { return allocator_.max_size(); }

  template <typename... Args>
  void construct(value_type* p, Args&&... args) {
    allocator_.construct(p, std::forward<Args>(args)...);
  }

  void destroy(value_type* p) { allocator_.destroy(p); }

  template <typename U>
  struct rebind {
    typedef MemoryLeakCheckerAllocator<
      typename std::allocator_traits<Allocator>::template rebind_alloc<U>,
      U
    > other;
  };

  Allocator const &allocator() const { return allocator_; }

  bool operator !=(MemoryLeakCheckerAllocator const &other) const {
    return allocator_ != other.allocator_;
  }

  bool operator ==(MemoryLeakCheckerAllocator const &other) const {
    return allocator_ == other.allocator_;
  }

private:
  Allocator allocator_;
};

MemoryLeakCheckerAllocator<
  typename default_storage_policy<>::allocator_type::rebind<int>::other, int
> allocator{
  typename default_storage_policy<>::allocator_type::rebind<int>::other()
};

template <typename... Args>
using test_variant = variant<
  default_storage_policy<decltype(allocator)>,
  Args...
>;

template <typename T>
using basic_test_string = std::basic_string<
  char,
  std::char_traits<char>,
  typename decltype(allocator)::template rebind<char>::other
>;

typedef basic_test_string<char> test_string;

template <typename T>
using test_vector = std::vector<
  T,
  typename decltype(allocator)::template rebind<char>::other
>;

////////////////
// UNIT TESTS //
////////////////

TEST(variant, is_variant) {
  LOG(INFO) << "is_variant -> variant";
  EXPECT_TRUE((is_variant<test_variant<>>::value));
  EXPECT_TRUE((is_variant<test_variant<bool>>::value));
  EXPECT_TRUE((is_variant<test_variant<int>>::value));
  EXPECT_TRUE((is_variant<test_variant<double>>::value));
  EXPECT_TRUE((is_variant<test_variant<long>>::value));
  EXPECT_TRUE((is_variant<test_variant<test_string>>::value));
  EXPECT_TRUE((is_variant<test_variant<int, double, long>>::value));
  EXPECT_TRUE((is_variant<test_variant<int, double, test_string>>::value));

  LOG(INFO) << "is_variant -> single types";
  EXPECT_FALSE((is_variant<bool>::value));
  EXPECT_FALSE((is_variant<int>::value));
  EXPECT_FALSE((is_variant<double>::value));
  EXPECT_FALSE((is_variant<long>::value));
  EXPECT_FALSE((is_variant<test_string>::value));

  LOG(INFO) << "is_variant -> tuple";
  EXPECT_FALSE((is_variant<std::tuple<>>::value));
  EXPECT_FALSE((is_variant<std::tuple<bool>>::value));
  EXPECT_FALSE((is_variant<std::tuple<int>>::value));
  EXPECT_FALSE((is_variant<std::tuple<double>>::value));
  EXPECT_FALSE((is_variant<std::tuple<long>>::value));
  EXPECT_FALSE((is_variant<std::tuple<test_string>>::value));
  EXPECT_FALSE((is_variant<std::tuple<int, double, long>>::value));
  EXPECT_FALSE((is_variant<std::tuple<int, double, test_string>>::value));
}

TEST(variant, default_ctor) {
  test_variant<int, test_string, double> v(allocator);
  typedef decltype(v) var;
  EXPECT_EQ(var::no_tag(), v.tag());
  EXPECT_TRUE(v.empty());
}

TEST(variant, getters) {
  test_variant<int, test_string, double> v(allocator, 10);

  EXPECT_TRUE(v.is_of<int>());
  EXPECT_EQ(10, (v.unchecked_get<int>()));
  EXPECT_EQ(10, (v.get<int>()));
  EXPECT_THROW((v.get<double>()), std::invalid_argument);
}

struct FooCopyCtorTest {
  explicit FooCopyCtorTest(int) {}
  FooCopyCtorTest() = delete;
  FooCopyCtorTest(FooCopyCtorTest const &) {}
  FooCopyCtorTest(FooCopyCtorTest &&) = delete;
};

struct BarCopyCtorTest {
  explicit BarCopyCtorTest(int) {}
  BarCopyCtorTest() { EXPECT_TRUE(false); }
  BarCopyCtorTest(BarCopyCtorTest const &) {}
  /* may throw */ BarCopyCtorTest(BarCopyCtorTest &&) { EXPECT_TRUE(false); }
};

TEST(variant, copy_ctor) {
  test_variant<FooCopyCtorTest, BarCopyCtorTest> v(allocator);
  EXPECT_TRUE(v.empty());

  v.emplace<FooCopyCtorTest>(17);
  EXPECT_FALSE(v.empty());
  EXPECT_EQ(0, v.tag());

  decltype(v) u(static_cast<decltype(v) const &>(v));
  EXPECT_FALSE(v.empty());
  EXPECT_FALSE(u.empty());
  EXPECT_EQ(0, v.tag());
  EXPECT_EQ(0, u.tag());

  v.emplace<BarCopyCtorTest>(37);
  EXPECT_FALSE(v.empty());
  EXPECT_EQ(1, v.tag());

  decltype(v) z(static_cast<decltype(v) const &>(v));
  EXPECT_FALSE(v.empty());
  EXPECT_FALSE(z.empty());
  EXPECT_EQ(1, v.tag());
  EXPECT_EQ(1, z.tag());
}

struct FooMoveCtor {
  explicit FooMoveCtor(int) {}
  FooMoveCtor() = delete;
  FooMoveCtor(FooMoveCtor const &) = delete;
  FooMoveCtor(FooMoveCtor &&) noexcept {}
};

struct BarMoveCtor {
  explicit BarMoveCtor(int) {}
  BarMoveCtor() { EXPECT_TRUE(false); }
  BarMoveCtor(BarMoveCtor const &) { EXPECT_TRUE(false); }
  BarMoveCtor(BarMoveCtor &&) noexcept {}
};

TEST(variant, move_ctor) {
  test_variant<FooMoveCtor, BarMoveCtor> v(allocator);
  EXPECT_TRUE(v.empty());

  v.emplace<FooMoveCtor>(17);
  EXPECT_FALSE(v.empty());
  EXPECT_EQ(0, v.tag());

  decltype(v) u(std::move(v));
  EXPECT_TRUE(v.empty());
  EXPECT_FALSE(u.empty());
  EXPECT_EQ(0, u.tag());

  v.emplace<BarMoveCtor>(37);
  EXPECT_FALSE(v.empty());
  EXPECT_EQ(1, v.tag());

  decltype(v) z(std::move(v));
  EXPECT_TRUE(v.empty());
  EXPECT_FALSE(z.empty());
  EXPECT_EQ(1, z.tag());
}

TEST(variant, value_copy_ctor) {
  test_variant<int, test_string, double> i(
    allocator, static_cast<int const &>(10)
  );
  EXPECT_FALSE(i.empty());
  EXPECT_EQ(0, i.tag());

  test_variant<int, test_string, double> s(
    allocator, static_cast<test_string const &>(test_string(allocator))
  );
  EXPECT_FALSE(s.empty());
  EXPECT_EQ(1, s.tag());

  test_variant<int, test_string, double> d(
    allocator, static_cast<double const &>(1.0)
  );
  EXPECT_FALSE(d.empty());
  EXPECT_EQ(2, d.tag());
}

TEST(variant, value_move_ctor) {
  test_variant<int, test_string, double> i(
    allocator, std::move(10)
  );
  EXPECT_FALSE(i.empty());
  EXPECT_EQ(0, i.tag());

  test_variant<int, test_string, double> s(
    allocator, std::move(test_string(allocator))
  );
  EXPECT_FALSE(s.empty());
  EXPECT_EQ(1, s.tag());

  test_variant<int, test_string, double> d(
    allocator, std::move(1.0)
  );
  EXPECT_FALSE(d.empty());
  EXPECT_EQ(2, d.tag());
}

TEST(variant, copy_ctor_different_variant) {
  typedef test_variant<bool, test_string, double, int> source_var;

  {
    source_var v1(allocator, 10);
    test_variant<int> v2(static_cast<source_var const &>(v1));
    ASSERT_TRUE(v2.is_of<int>());
    EXPECT_EQ(10, v2.get<int>());
  }

  {
    source_var v1(allocator, 5.0);
    test_variant<double> v2(static_cast<source_var const &>(v1));
    ASSERT_TRUE(v2.is_of<double>());
    EXPECT_EQ(5.0, v2.get<double>());
  }

  {
    source_var v1(allocator, test_string("test", allocator));
    test_variant<test_string> v2(static_cast<source_var const &>(v1));
    ASSERT_TRUE(v2.is_of<test_string>());
    EXPECT_EQ("test", v2.get<test_string>());
  }
}

TEST(variant, copy_ctor_different_variant_and_allocator) {
  typedef test_variant<bool, test_string, double, int> source_var;

  {
    source_var v1(allocator, 10);
    test_variant<int> v2(
      allocator,
      static_cast<source_var const &>(v1)
    );
    ASSERT_TRUE(v2.is_of<int>());
    EXPECT_EQ(10, v2.get<int>());
  }

  {
    source_var v1(allocator, 5.0);
    test_variant<double> v2(
      allocator,
      static_cast<source_var const &>(v1)
    );
    ASSERT_TRUE(v2.is_of<double>());
    EXPECT_EQ(5.0, v2.get<double>());
  }

  {
    source_var v1(allocator, test_string("test", allocator));
    test_variant<test_string> v2(
      allocator,
      static_cast<source_var const &>(v1)
    );
    ASSERT_TRUE(v2.is_of<test_string>());
    EXPECT_EQ("test", v2.get<test_string>());
  }
}

TEST(variant, copy_assignment) {
  typedef test_variant<int, double, test_string> VAR;

  VAR v1(allocator, 10);
  EXPECT_TRUE(v1.is_of<int>());
  EXPECT_EQ(10, (v1.get<int>()));

  VAR v2;
  EXPECT_TRUE(v2.empty());

  v2 = v1;
  EXPECT_TRUE(v1.is_of<int>());
  EXPECT_EQ(10, (v1.get<int>()));
  EXPECT_TRUE((v2.is_of<int>()));
  EXPECT_EQ(10, (v2.get<int>()));
}

TEST(variant, move_assignment) {
  typedef test_variant<int, double, test_string> VAR;

  VAR v1(allocator, 10);
  EXPECT_TRUE(v1.is_of<int>());
  EXPECT_EQ(10, (v1.get<int>()));

  VAR v2;
  EXPECT_TRUE(v2.empty());

  v2 = std::move(v1);
  EXPECT_TRUE(v1.empty());
  EXPECT_TRUE(v2.is_of<int>());
  EXPECT_EQ(10, (v2.get<int>()));
}

TEST(variant, self_assignment) {
  typedef test_variant<int, double, test_string> VAR;

  VAR v1(allocator, 10);
  EXPECT_TRUE(v1.is_of<int>());
  EXPECT_EQ(10, (v1.get<int>()));

  v1 = v1;
  EXPECT_TRUE(v1.is_of<int>());
  EXPECT_EQ(10, (v1.get<int>()));

  v1 = std::move(v1);
  EXPECT_TRUE(v1.is_of<int>());
  EXPECT_EQ(10, (v1.get<int>()));
}

template <typename T, typename TStoragePolicy, typename ...Args>
T const &copyset_constget_helper(
  variant<TStoragePolicy, Args...> &v, T const &value
) {
  typedef typename std::remove_reference<T>::type type;
  v.template set<type>(value);
  return static_cast<variant<TStoragePolicy, Args...> const &>(v)
    .template get<type>();
}

TEST(variant, copyset_get) {
  test_variant<int, test_string, double> v(allocator);
  EXPECT_EQ(10, copyset_constget_helper<int>(v, 10));
  EXPECT_EQ(
    "It's alive! ALIVE!",
    copyset_constget_helper<test_string>(
      v, test_string("It's alive! ALIVE!", allocator)
    )
  );
  EXPECT_EQ(1.2, copyset_constget_helper<double>(v, 1.2));
}

template <typename T, typename TStoragePolicy, typename ...Args>
T &moveset_get_helper(variant<TStoragePolicy, Args...> &v, T &&value) {
  static_assert(
    !std::is_const<typename std::remove_reference<T>::type>::value,
    "T can't be const"
  );

  typedef typename std::remove_reference<T>::type type;
  v.template set<type>(std::move(value));
  return v.template get<type>();
}

TEST(variant, moveset_get) {
  test_variant<int, test_string, double> v(allocator);
  EXPECT_EQ(10, moveset_get_helper<int>(v, 10));
  EXPECT_EQ(
    "It's alive! ALIVE!",
    moveset_get_helper<test_string>(
      v, test_string("It's alive! ALIVE!", allocator)
    )
  );
  EXPECT_EQ(1.2, moveset_get_helper<double>(v, 1.2));
}

template <typename T, typename V, typename ...Args>
T const &emplace_constget_helper(V &v, Args &&...args) {
  typedef typename std::remove_reference<T>::type type;
  v.template emplace<type>(std::forward<Args>(args)...);
  return static_cast<V const &>(v).template get<type>();
}

TEST(variant, emplace_constget) {
  test_variant<int, test_string, double> v(allocator);
  EXPECT_EQ(10, emplace_constget_helper<int>(v, 10));
  EXPECT_EQ(
    "It's alive! ALIVE!",
    emplace_constget_helper<test_string>(
      v, test_string("It's alive! ALIVE!", allocator)
    )
  );
  EXPECT_EQ(1.2, emplace_constget_helper<double>(v, 1.2));
}

TEST(variant, try_get) {
  test_variant<int, double> v(allocator);
  EXPECT_EQ(nullptr, v.try_get<int>());
  EXPECT_EQ(nullptr, v.try_get<double>());

  v = 10;
  EXPECT_NE(nullptr, v.try_get<int>());
  EXPECT_EQ(10, *v.try_get<int>());
  EXPECT_EQ(nullptr, v.try_get<double>());

  v = 5.0;
  EXPECT_EQ(nullptr, v.try_get<int>());
  EXPECT_NE(nullptr, v.try_get<double>());
  EXPECT_EQ(5.0, *v.try_get<double>());
}

TEST(variant, clear_empty) {
  test_variant<int, test_string, double> v(allocator);
  EXPECT_TRUE(v.empty());
  EXPECT_EQ(3, v.tag());

  v.clear();
  EXPECT_TRUE(v.empty());
  EXPECT_EQ(3, v.tag());

  v.set(10);
  EXPECT_FALSE(v.empty());
  EXPECT_EQ(0, v.tag());

  v.clear();
  EXPECT_TRUE(v.empty());
  EXPECT_EQ(3, v.tag());
}

TEST(variant, swap) {
  test_variant<int, test_string> v(allocator, 10);
  test_variant<int, test_string> u(allocator, test_string("5.0", allocator));
  test_variant<int, test_string> e(allocator);
  LOG(INFO) << "initial";
  EXPECT_FALSE(v.empty());
  EXPECT_EQ(10, v.get<int>());
  EXPECT_FALSE(u.empty());
  EXPECT_EQ(test_string("5.0", allocator), u.get<test_string>());
  EXPECT_TRUE(e.empty());

  LOG(INFO) << "v.swap(u);";
  v.swap(u);
  EXPECT_FALSE(u.empty());
  EXPECT_EQ(10, u.get<int>());
  EXPECT_FALSE(v.empty());
  EXPECT_EQ("5.0", v.get<test_string>());
  EXPECT_TRUE(e.empty());

  LOG(INFO) << "v.swap(u);";
  v.swap(u);
  EXPECT_FALSE(v.empty());
  EXPECT_EQ(10, v.get<int>());
  EXPECT_FALSE(u.empty());
  EXPECT_EQ("5.0", u.get<test_string>());
  EXPECT_TRUE(e.empty());

  LOG(INFO) << "u.swap(e);";
  u.swap(e);
  EXPECT_FALSE(v.empty());
  EXPECT_EQ(10, v.get<int>());
  EXPECT_FALSE(e.empty());
  EXPECT_EQ("5.0", e.get<test_string>());
  EXPECT_TRUE(u.empty());

  LOG(INFO) << "e.swap(v);";
  e.swap(v);
  EXPECT_FALSE(e.empty());
  EXPECT_EQ(10, e.get<int>());
  EXPECT_FALSE(v.empty());
  EXPECT_EQ("5.0", v.get<test_string>());
  EXPECT_TRUE(u.empty());
}

template <typename T>
struct visit_if_visitor {
  visit_if_visitor() = default;
  visit_if_visitor(T expected): expected_(std::move(expected)) {}

  void operator ()(T const &actual) const {
    ASSERT_TRUE(expected_.hasValue());
    EXPECT_EQ(*expected_, actual);
  }

private:
  folly::Optional<T> const expected_;
};

template <typename T, typename TVariant, typename... Args>
void check_visit_if(TVariant &&variant, Args &&...args) {
  variant.template visit_if<transform::alias<std::is_same, T>::template  type>(
    visit_if_visitor<T>(std::forward<Args>(args)...)
  );
}

TEST(variant, visit_if) {
  test_variant<int, test_string, double> v(allocator);

  check_visit_if<int>(v);
  check_visit_if<test_string>(v);
  check_visit_if<double>(v);

  v = 10;
  check_visit_if<int>(v, 10);

  v = test_string(allocator);
  check_visit_if<test_string>(v, test_string(allocator));

  v = 5.0;
  check_visit_if<double>(v, 5.0);
}

template <typename T, bool fail = false>
struct type_checker_visitor {
  template <typename U, bool>
  struct comparer {
    static void compare(T const &expected, U const &actual) {
      EXPECT_EQ(expected, actual);
    }
  };

  template <typename U>
  struct comparer<U, false> {
    static void compare(T const &expected, U const &actual) {
      LOG(INFO) << "visited \"" << actual << "\" ["
        << demangle(typeid(U).name()) << ", expecting \""
        << expected << "\" [" << demangle(typeid(T).name()) << ']';
      EXPECT_TRUE(false);
    }
  };

  explicit type_checker_visitor(T expected):
    expected_(std::move(expected))
  {}

  template <typename U>
  void operator ()(U &&value) {
    comparer<
      U,
      std::is_same<
        typename std::remove_const<
          typename std::remove_reference<U>::type
        >::type,
        T
      >::value
    >::compare(expected_, value);
  }

private:
  T expected_;
};

template <>
struct type_checker_visitor<void, true> {
  template <typename U>
  void operator ()(U &&) { EXPECT_TRUE(false); }
};

TEST(variant, visit) {
  test_variant<int, test_string, double> v(allocator);
  v.visit(type_checker_visitor<void, true>());

  v = 10;
  v.visit(type_checker_visitor<int>(10));

  v = test_string(allocator);
  v.visit(type_checker_visitor<test_string>(test_string(allocator)));

  v = 5.0;
  v.visit(type_checker_visitor<double>(5.0));
}

TEST(variant, tag) {
  test_variant<int, test_string, double> v(allocator);
  EXPECT_EQ(3, v.tag());

  v.set(10);
  EXPECT_EQ(0, v.tag());

  v.set(test_string(allocator));
  EXPECT_EQ(1, v.tag());

  v.set(1.0);
  EXPECT_EQ(2, v.tag());

  v.clear();
  EXPECT_EQ(3, v.tag());
}

TEST(variant, is_of) {
  test_variant<int, double> v(allocator);
  EXPECT_FALSE(v.is_of<int>());
  EXPECT_FALSE(v.is_of<double>());

  v = 10;
  EXPECT_TRUE(v.is_of<int>());
  EXPECT_FALSE(v.is_of<double>());

  v = 5.0;
  EXPECT_FALSE(v.is_of<int>());
  EXPECT_TRUE(v.is_of<double>());
}

TEST(variant, is_supported) {
  typedef test_variant<int, double, int const *, test_string> var;

  EXPECT_TRUE((var::is_supported<int>()));
  EXPECT_TRUE((var::is_supported<double>()));
  EXPECT_TRUE((var::is_supported<int const *>()));
  EXPECT_TRUE((var::is_supported<test_string>()));

  EXPECT_FALSE((var::is_supported<int *>()));
  EXPECT_FALSE((var::is_supported<long *>()));
  EXPECT_FALSE((var::is_supported<test_vector<int>>()));
  EXPECT_FALSE((var::is_supported<bool>()));
  EXPECT_FALSE((var::is_supported<long>()));
  EXPECT_FALSE((var::is_supported<short>()));
  EXPECT_FALSE((var::is_supported<void>()));
  EXPECT_FALSE((var::is_supported<void *>()));
}

TEST(variant, operator_equals) {
  test_variant<int, double> const i10(allocator, 10);
  test_variant<int, double> const i5(allocator, 5);
  test_variant<int, double> const d10(allocator, 10.0);
  test_variant<int, double> const d5(allocator, 5.0);
  test_variant<int, double> const e(allocator);

  EXPECT_TRUE(i10 == i10);
  EXPECT_FALSE(i10 == i5);
  EXPECT_FALSE(i10 == d10);
  EXPECT_FALSE(d10 == i10);
  EXPECT_TRUE(d10 == d10);
  EXPECT_FALSE(d10 == d5);
  EXPECT_TRUE(e == e);
  EXPECT_FALSE(i10 == e);
  EXPECT_FALSE(e == i10);
  EXPECT_FALSE(d10 == e);
  EXPECT_FALSE(e == d10);
}

TEST(variant, operator_not_equals) {
  test_variant<int, double> const i10(allocator, 10);
  test_variant<int, double> const i5(allocator, 5);
  test_variant<int, double> const d10(allocator, 10.0);
  test_variant<int, double> const d5(allocator, 5.0);
  test_variant<int, double> const e(allocator);

  EXPECT_FALSE(i10 != i10);
  EXPECT_TRUE(i10 != i5);
  EXPECT_TRUE(i10 != d10);
  EXPECT_TRUE(d10 != i10);
  EXPECT_FALSE(d10 != d10);
  EXPECT_TRUE(d10 != d5);
  EXPECT_FALSE(e != e);
  EXPECT_TRUE(i10 != e);
  EXPECT_TRUE(e != i10);
  EXPECT_TRUE(d10 != e);
  EXPECT_TRUE(e != d10);
}

TEST(variant, operator_less_than) {
  test_variant<int, double> const i10(allocator, 10);
  test_variant<int, double> const i5(allocator, 5);
  test_variant<int, double> const d10(allocator, 10.0);
  test_variant<int, double> const d5(allocator, 5.0);
  test_variant<int, double> const e(allocator);

  EXPECT_FALSE(i10 < i10);
  EXPECT_FALSE(i10 < i5);
  EXPECT_TRUE(i5 < i10);
  EXPECT_TRUE(i10 < d10);
  EXPECT_FALSE(d10 < i10);
  EXPECT_FALSE(d10 < d10);
  EXPECT_FALSE(d10 < d5);
  EXPECT_TRUE(d5 < d10);
  EXPECT_FALSE(e < e);
  EXPECT_TRUE(i10 < e);
  EXPECT_FALSE(e < i10);
  EXPECT_TRUE(d10 < e);
  EXPECT_FALSE(e < d10);
}

TEST(variant, operator_less_than_or_equal) {
  test_variant<int, double> const i10(allocator, 10);
  test_variant<int, double> const i5(allocator, 5);
  test_variant<int, double> const d10(allocator, 10.0);
  test_variant<int, double> const d5(allocator, 5.0);
  test_variant<int, double> const e(allocator);

  EXPECT_TRUE(i10 <= i10);
  EXPECT_FALSE(i10 <= i5);
  EXPECT_TRUE(i5 <= i10);
  EXPECT_TRUE(i10 <= d10);
  EXPECT_FALSE(d10 <= i10);
  EXPECT_TRUE(d10 <= d10);
  EXPECT_FALSE(d10 <= d5);
  EXPECT_TRUE(d5 <= d10);
  EXPECT_TRUE(e <= e);
  EXPECT_TRUE(i10 <= e);
  EXPECT_FALSE(e <= i10);
  EXPECT_TRUE(d10 <= e);
  EXPECT_FALSE(e <= d10);
}

TEST(variant, operator_greater_than) {
  test_variant<int, double> const i10(allocator, 10);
  test_variant<int, double> const i5(allocator, 5);
  test_variant<int, double> const d10(allocator, 10.0);
  test_variant<int, double> const d5(allocator, 5.0);
  test_variant<int, double> const e(allocator);

  EXPECT_FALSE(i10 > i10);
  EXPECT_TRUE(i10 > i5);
  EXPECT_FALSE(i5 > i10);
  EXPECT_FALSE(i10 > d10);
  EXPECT_TRUE(d10 > i10);
  EXPECT_FALSE(d10 > d10);
  EXPECT_TRUE(d10 > d5);
  EXPECT_FALSE(d5 > d10);
  EXPECT_FALSE(e > e);
  EXPECT_FALSE(i10 > e);
  EXPECT_TRUE(e > i10);
  EXPECT_FALSE(d10 > e);
  EXPECT_TRUE(e > d10);
}

TEST(variant, operator_greater_than_or_equal) {
  test_variant<int, double> const i10(allocator, 10);
  test_variant<int, double> const i5(allocator, 5);
  test_variant<int, double> const d10(allocator, 10.0);
  test_variant<int, double> const d5(allocator, 5.0);
  test_variant<int, double> const e(allocator);

  EXPECT_TRUE(i10 >= i10);
  EXPECT_TRUE(i10 >= i5);
  EXPECT_FALSE(i5 >= i10);
  EXPECT_FALSE(i10 >= d10);
  EXPECT_TRUE(d10 >= i10);
  EXPECT_TRUE(d10 >= d10);
  EXPECT_TRUE(d10 >= d5);
  EXPECT_FALSE(d5 >= d10);
  EXPECT_TRUE(e >= e);
  EXPECT_FALSE(i10 >= e);
  EXPECT_TRUE(e >= i10);
  EXPECT_FALSE(d10 >= e);
  EXPECT_TRUE(e >= d10);
}

TEST(variant, operator_copy_assignment_clvref) {
  test_variant<int, double> const idce;
  EXPECT_TRUE(idce.empty());

  test_variant<int, double> const idc50_1(allocator, 50.1);
  EXPECT_EQ(50.1, idc50_1.get<double>());

  test_variant<int, double> const idc10(allocator, 10);
  EXPECT_EQ(10, idc10.get<int>());

  test_variant<int> const ic11(allocator, 11);
  EXPECT_EQ(11, ic11.get<int>());

  test_variant<int> const ice;
  EXPECT_TRUE(ice.empty());

  test_variant<int, double> id(allocator, 7);
  EXPECT_EQ(7, id.get<int>());

  id = idce;
  EXPECT_TRUE(id.empty());

  id = idc50_1;
  EXPECT_EQ(50.1, id.get<double>());

  id = idc10;
  EXPECT_EQ(10, id.get<int>());

  id = ic11;
  EXPECT_EQ(11, id.get<int>());

  id = ice;
  EXPECT_TRUE(id.empty());

  test_variant<int> i(allocator, 9);
  EXPECT_EQ(9, i.get<int>());

  i = idce;
  EXPECT_TRUE(i.empty());

  i = idc10;
  EXPECT_EQ(10, i.get<int>());

  i = ic11;
  EXPECT_EQ(11, i.get<int>());

  i = ice;
  EXPECT_TRUE(i.empty());
}

TEST(variant, operator_copy_assignment_lvref) {
  test_variant<int, double> ide;
  EXPECT_TRUE(ide.empty());

  test_variant<int, double> id6_7(allocator, 6.7);
  EXPECT_EQ(6.7, id6_7.get<double>());

  test_variant<int, double> id5(allocator, 5);
  EXPECT_EQ(5, id5.get<int>());

  test_variant<int> i4(allocator, 4);
  EXPECT_EQ(4, i4.get<int>());

  test_variant<int> ie;
  EXPECT_TRUE(ie.empty());

  test_variant<int, double> id(allocator, 7);
  EXPECT_EQ(7, id.get<int>());

  id = ide;
  EXPECT_TRUE(id.empty());

  id = id6_7;
  EXPECT_EQ(6.7, id.get<double>());

  id = id5;
  EXPECT_EQ(5, id.get<int>());

  id = i4;
  EXPECT_EQ(4, id.get<int>());

  id = ie;
  EXPECT_TRUE(id.empty());

  test_variant<int> i(allocator, 9);
  EXPECT_EQ(9, i.get<int>());

  i = ide;
  EXPECT_TRUE(i.empty());

  i = id5;
  EXPECT_EQ(5, i.get<int>());

  i = i4;
  EXPECT_EQ(4, i.get<int>());

  i = ie;
  EXPECT_TRUE(i.empty());
}

TEST(variant, operator_copy_assignment_rvref) {
  test_variant<int, double> ide;
  EXPECT_TRUE(ide.empty());

  test_variant<int, double> id6_7(allocator, 6.7);
  EXPECT_EQ(6.7, id6_7.get<double>());

  test_variant<int, double> id5(allocator, 5);
  EXPECT_EQ(5, id5.get<int>());

  test_variant<int> i4(allocator, 4);
  EXPECT_EQ(4, i4.get<int>());

  test_variant<int, double> id3(nullptr, 3);
  EXPECT_EQ(3, id3.get<int>());

  test_variant<int> i2(nullptr, 2);
  EXPECT_EQ(2, i2.get<int>());

  test_variant<int> ie;
  EXPECT_TRUE(ie.empty());

  test_variant<int, double> id(allocator, 7);
  EXPECT_EQ(7, id.get<int>());

  id = std::move(ide);
  EXPECT_TRUE(id.empty());
  EXPECT_TRUE(ide.empty());

  id = std::move(id6_7);
  EXPECT_EQ(6.7, id.get<double>());
  EXPECT_TRUE(id6_7.empty());

  id = std::move(id5);
  EXPECT_EQ(5, id.get<int>());
  EXPECT_TRUE(id5.empty());

  id = std::move(i4);
  EXPECT_EQ(4, id.get<int>());
  EXPECT_TRUE(i4.empty());

  id = std::move(ie);
  EXPECT_TRUE(id.empty());
  EXPECT_TRUE(ie.empty());

  test_variant<int> i(allocator, 9);
  EXPECT_EQ(9, i.get<int>());

  i = std::move(ide);
  EXPECT_TRUE(i.empty());
  EXPECT_TRUE(ide.empty());

  i = std::move(id3);
  EXPECT_EQ(3, i.get<int>());
  EXPECT_TRUE(id3.empty());

  i = std::move(i2);
  EXPECT_EQ(2, i.get<int>());
  EXPECT_TRUE(i2.empty());

  i = std::move(ie);
  EXPECT_TRUE(i.empty());
  EXPECT_TRUE(ie.empty());
}

TEST(variant, operator_copy_assignment_heterogeneous_policy_clvref) {
  auto_variant<int, double> const idce;
  EXPECT_TRUE(idce.empty());

  auto_variant<int, double> const idc50_1(nullptr, 50.1);
  EXPECT_EQ(50.1, idc50_1.get<double>());

  auto_variant<int, double> const idc10(nullptr, 10);
  EXPECT_EQ(10, idc10.get<int>());

  auto_variant<int> const ic11(nullptr, 11);
  EXPECT_EQ(11, ic11.get<int>());

  auto_variant<int> const ice;
  EXPECT_TRUE(ice.empty());

  auto_variant<int, double> id(nullptr, 7);
  EXPECT_EQ(7, id.get<int>());

  id = idce;
  EXPECT_TRUE(id.empty());

  id = idc50_1;
  EXPECT_EQ(50.1, id.get<double>());

  id = idc10;
  EXPECT_EQ(10, id.get<int>());

  id = ic11;
  EXPECT_EQ(11, id.get<int>());

  id = ice;
  EXPECT_TRUE(id.empty());

  test_variant<int> i(allocator, 9);
  EXPECT_EQ(9, i.get<int>());

  i = idce;
  EXPECT_TRUE(i.empty());

  i = idc10;
  EXPECT_EQ(10, i.get<int>());

  i = ic11;
  EXPECT_EQ(11, i.get<int>());

  i = ice;
  EXPECT_TRUE(i.empty());
}

TEST(variant, operator_copy_assignment_heterogeneous_policy_lvref) {
  auto_variant<int, double> ide;
  EXPECT_TRUE(ide.empty());

  auto_variant<int, double> id6_7(nullptr, 6.7);
  EXPECT_EQ(6.7, id6_7.get<double>());

  auto_variant<int, double> id5(nullptr, 5);
  EXPECT_EQ(5, id5.get<int>());

  auto_variant<int> i4(nullptr, 4);
  EXPECT_EQ(4, i4.get<int>());

  auto_variant<int> ie;
  EXPECT_TRUE(ie.empty());

  auto_variant<int, double> id(nullptr, 7);
  EXPECT_EQ(7, id.get<int>());

  id = ide;
  EXPECT_TRUE(id.empty());

  id = id6_7;
  EXPECT_EQ(6.7, id.get<double>());

  id = id5;
  EXPECT_EQ(5, id.get<int>());

  id = i4;
  EXPECT_EQ(4, id.get<int>());

  id = ie;
  EXPECT_TRUE(id.empty());

  test_variant<int> i(allocator, 9);
  EXPECT_EQ(9, i.get<int>());

  i = ide;
  EXPECT_TRUE(i.empty());

  i = id5;
  EXPECT_EQ(5, i.get<int>());

  i = i4;
  EXPECT_EQ(4, i.get<int>());

  i = ie;
  EXPECT_TRUE(i.empty());
}

TEST(variant, operator_copy_assignment_heterogeneous_policy_rvref) {
  auto_variant<int, double> ide;
  EXPECT_TRUE(ide.empty());

  auto_variant<int, double> id6_7(nullptr, 6.7);
  EXPECT_EQ(6.7, id6_7.get<double>());

  auto_variant<int, double> id5(nullptr, 5);
  EXPECT_EQ(5, id5.get<int>());

  auto_variant<int> i4(nullptr, 4);
  EXPECT_EQ(4, i4.get<int>());

  auto_variant<int, double> id3(nullptr, 3);
  EXPECT_EQ(3, id3.get<int>());

  auto_variant<int> i2(nullptr, 2);
  EXPECT_EQ(2, i2.get<int>());

  auto_variant<int, double> id(nullptr, 7);
  EXPECT_EQ(7, id.get<int>());

  auto_variant<int> ie;
  EXPECT_TRUE(ie.empty());

  id = std::move(ide);
  EXPECT_TRUE(id.empty());
  EXPECT_TRUE(ide.empty());

  id = std::move(id6_7);
  EXPECT_EQ(6.7, id.get<double>());
  EXPECT_TRUE(id6_7.empty());

  id = std::move(id5);
  EXPECT_EQ(5, id.get<int>());
  EXPECT_TRUE(id5.empty());

  id = std::move(i4);
  EXPECT_EQ(4, id.get<int>());
  EXPECT_TRUE(i4.empty());

  id = std::move(ie);
  EXPECT_TRUE(id.empty());
  EXPECT_TRUE(ie.empty());

  test_variant<int> i(allocator, 9);
  EXPECT_EQ(9, i.get<int>());

  i = std::move(ide);
  EXPECT_TRUE(i.empty());
  EXPECT_TRUE(ide.empty());

  i = std::move(id3);
  EXPECT_EQ(3, i.get<int>());
  EXPECT_TRUE(id3.empty());

  i = std::move(i2);
  EXPECT_EQ(2, i.get<int>());
  EXPECT_TRUE(i2.empty());

  i = std::move(ie);
  EXPECT_TRUE(i.empty());
  EXPECT_TRUE(ie.empty());
}

template <typename Expected, typename ...Args>
void check_type_tag_size() {
  typedef variant<default_storage_policy<>, Args...> var;
  if(!std::is_same<Expected, typename var::type_tag>::value) {
    LOG(INFO) << "expected \"" << demangle(typeid(Expected).name())
      << "\", got \"" << demangle(typeid(typename var::type_tag).name())
      << "\" for " << sizeof...(Args) << " types";
    EXPECT_TRUE(false);
  }
}

template <int> struct X {};
TEST(variant, type_tag_size) {
#define TAG_SIZE_TEST(Expected,...) \
  check_type_tag_size<Expected, __VA_ARGS__>();

  TAG_SIZE_TEST(bool, X<1>);
  TAG_SIZE_TEST(uint8_t, X<1>, X<2>);
  TAG_SIZE_TEST(uint8_t, X<1>, X<2>, X<3>);
  TAG_SIZE_TEST(uint8_t, X<1>, X<2>, X<3>, X<4>, X<5>, X<6>, X<7>);
  TAG_SIZE_TEST(uint8_t, X<1>, X<2>, X<3>, X<4>, X<5>, X<6>, X<7>, X<8>);
  TAG_SIZE_TEST(uint8_t,
    X<1>, X<2>, X<3>, X<4>, X<5>, X<6>, X<7>, X<8>,
    X<9>, X<10>, X<11>, X<12>, X<13>, X<14>, X<15>
  );
  TAG_SIZE_TEST(uint8_t,
    X<1>, X<2>, X<3>, X<4>, X<5>, X<6>, X<7>, X<8>,
    X<9>, X<10>, X<11>, X<12>, X<13>, X<14>, X<15>, X<16>
  );
  TAG_SIZE_TEST(uint8_t,
    X<1>, X<2>, X<3>, X<4>, X<5>, X<6>, X<7>, X<8>,
    X<9>, X<10>, X<11>, X<12>, X<13>, X<14>, X<15>, X<16>,
    X<17>, X<18>, X<19>, X<20>, X<21>, X<22>, X<23>, X<24>,
    X<25>, X<26>, X<27>, X<28>, X<29>, X<30>, X<31>
  );
  TAG_SIZE_TEST(uint8_t,
    X<1>, X<2>, X<3>, X<4>, X<5>, X<6>, X<7>, X<8>,
    X<9>, X<10>, X<11>, X<12>, X<13>, X<14>, X<15>, X<16>,
    X<17>, X<18>, X<19>, X<20>, X<21>, X<22>, X<23>, X<24>,
    X<25>, X<26>, X<27>, X<28>, X<29>, X<30>, X<31>, X<32>
  );
  TAG_SIZE_TEST(uint8_t,
    X<1>, X<2>, X<3>, X<4>, X<5>, X<6>, X<7>, X<8>,
    X<9>, X<10>, X<11>, X<12>, X<13>, X<14>, X<15>, X<16>,
    X<17>, X<18>, X<19>, X<20>, X<21>, X<22>, X<23>, X<24>,
    X<25>, X<26>, X<27>, X<28>, X<29>, X<30>, X<31>, X<32>,
    X<33>, X<34>, X<35>, X<36>, X<37>, X<38>, X<39>, X<40>,
    X<41>, X<42>, X<43>, X<44>, X<45>, X<46>, X<47>, X<48>,
    X<49>, X<50>, X<51>, X<52>, X<53>, X<54>, X<55>, X<56>,
    X<57>, X<58>, X<59>, X<60>, X<61>, X<62>, X<63>
  );
  TAG_SIZE_TEST(uint8_t,
    X<1>, X<2>, X<3>, X<4>, X<5>, X<6>, X<7>, X<8>,
    X<9>, X<10>, X<11>, X<12>, X<13>, X<14>, X<15>, X<16>,
    X<17>, X<18>, X<19>, X<20>, X<21>, X<22>, X<23>, X<24>,
    X<25>, X<26>, X<27>, X<28>, X<29>, X<30>, X<31>, X<32>,
    X<33>, X<34>, X<35>, X<36>, X<37>, X<38>, X<39>, X<40>,
    X<41>, X<42>, X<43>, X<44>, X<45>, X<46>, X<47>, X<48>,
    X<49>, X<50>, X<51>, X<52>, X<53>, X<54>, X<55>, X<56>,
    X<57>, X<58>, X<59>, X<60>, X<61>, X<62>, X<63>, X<64>,
    X<65>, X<66>, X<67>, X<68>, X<69>, X<70>, X<71>, X<72>,
    X<73>, X<74>, X<75>, X<76>, X<77>, X<78>, X<79>, X<80>,
    X<81>, X<82>, X<83>, X<84>, X<85>, X<86>, X<87>, X<88>,
    X<89>, X<90>, X<91>, X<92>, X<93>, X<94>, X<95>, X<96>,
    X<97>, X<98>, X<99>, X<100>, X<101>, X<102>, X<103>, X<104>,
    X<105>, X<106>, X<107>, X<108>, X<109>, X<110>, X<111>, X<112>,
    X<113>, X<114>, X<115>, X<116>, X<117>, X<118>, X<119>, X<120>,
    X<121>, X<122>, X<123>, X<124>, X<125>, X<126>, X<127>
  );
#undef TAG_SIZE_TEST
}

TEST(variant, id) {
  typedef variant<
    default_storage_policy<>,
    int,
    short,
    test_string,
    test_vector<int>,
    test_vector<double>,
    test_vector<test_string>,
    std::map<test_string, test_string>
  > var;

  EXPECT_EQ(var::tag<int>(), 0);
  EXPECT_EQ(var::tag<short>(), 1);
  EXPECT_EQ(var::tag<test_string>(), 2);
  EXPECT_EQ(var::tag<test_vector<int>>(), 3);
  EXPECT_EQ(var::tag<test_vector<double>>(), 4);
  EXPECT_EQ(var::tag<test_vector<test_string>>(), 5);
  typedef std::map<
    test_string, test_string
  > map_string_string;
  EXPECT_EQ(var::tag<map_string_string>(), 6);
  typedef std::map<test_string, int> map_string_int;
  EXPECT_EQ(var::tag<map_string_int>(), 7);
  typedef std::map<short, bool> map_short_bool;
  EXPECT_EQ(var::tag<map_short_bool>(), 7);
  EXPECT_EQ(var::tag<bool>(), 7);
  EXPECT_EQ(var::tag<unsigned int>(), 7);
  EXPECT_EQ(var::tag<unsigned short>(), 7);
  EXPECT_EQ(var::no_tag(), 7);
}

TEST(variant, arena_allocator) {
  SysArena arena;
  MemoryLeakCheckerAllocator<StlAllocator<SysArena, int>, int> alloc{
    StlAllocator<SysArena, int>(&arena)
  };

  variant<
    default_storage_policy<decltype(alloc), dynamic_allocation_policy>,
    int, double, std::string, std::vector<int>
  > v(alloc);
  EXPECT_TRUE(v.empty());

  v = std::string("hello, world");
  EXPECT_FALSE(v.empty());
  EXPECT_EQ("hello, world", v.get<std::string>());

  v = 10;
  EXPECT_FALSE(v.empty());
  EXPECT_EQ(10, v.get<int>());

  v = 5.6;
  EXPECT_FALSE(v.empty());
  EXPECT_EQ(5.6, v.get<double>());

  v = std::vector<int>{1, 2, 3, 4, 5};
  EXPECT_FALSE(v.empty());
  EXPECT_EQ(5, v.get<std::vector<int>>().size());
  for(int i = 1; i <= 5; ++i) {
    EXPECT_EQ(i, v.get<std::vector<int>>()[i - 1]);
  }

  v.clear();
  EXPECT_TRUE(v.empty());
}

template <typename StoragePolicy = default_storage_policy<decltype(allocator)>>
struct t_nested_vector;
template <typename StoragePolicy = default_storage_policy<decltype(allocator)>>
using t_nested_variant = variant<StoragePolicy, int, t_nested_vector<>>;
template <typename StoragePolicy>
struct t_nested_vector: public test_vector<t_nested_variant<StoragePolicy>> {};

TEST(variant, templated_nested_variant) {
  t_nested_variant<> v(allocator);
  EXPECT_TRUE(v.empty());

  v = 10;
  EXPECT_EQ(10, v.get<int>());

  t_nested_vector<> x;
  x.emplace_back(allocator, 5);
  v = std::move(x);
  EXPECT_EQ(1, v.get<decltype(x)>().size());
  EXPECT_EQ(5, v.get<decltype(x)>()[0].get<int>());
}

struct nested_vector;
typedef variant<
  default_storage_policy<decltype(allocator)>, int, nested_vector
> nested_variant;
struct nested_vector:
  public test_vector<nested_variant>
{
  nested_vector() = default;
  nested_vector(std::initializer_list<nested_variant> list):
    test_vector<nested_variant>(list, allocator)
  {}
};

TEST(variant, nested_variant) {
  nested_variant v(allocator);
  EXPECT_TRUE(v.empty());

  v = 10;
  EXPECT_EQ(10, v.get<int>());

  nested_vector x;
  x.emplace_back(allocator, 5);
  v = std::move(x);
  EXPECT_EQ(1, v.get<decltype(x)>().size());
  EXPECT_EQ(5, v.get<decltype(x)>()[0].get<int>());
}

/*
 * Takes an initializer list as input in the ctor, with the expected results
 * which is a list of integers. -1 means a vector is expected to be found
 * by the visitor, which will recurse on its contents; -2 means the visitor
 * is expected to return from a recursion. anything else means the visitor
 * is expected to find an integer.
 *
 * Say you have a variant containing a vector, which in turn contains the
 * values 6 and 7, this is the expected result:
 *
 *  -1, 6, 7, -2
 */
struct nested_visitor {
  nested_visitor(std::initializer_list<int> list): expected_(list) {}

  void operator ()(nested_vector const &v) {
    LOG(INFO) << "push" << " expected.size() = " << expected_.size()
      << " with front = " << expected_.front();
    ASSERT_NE(0, expected_.size());
    EXPECT_EQ(-1, expected_.front());
    expected_.pop();

    for(auto &i: v) {
      i.visit(*this);
    }

    LOG(INFO) << "pop" << " expected.size() = " << expected_.size()
      << " with front = " << expected_.front();
    ASSERT_NE(0, expected_.size());
    EXPECT_EQ(-2, expected_.front());
    expected_.pop();
  }

  void operator ()(int i) {
    LOG(INFO) << "int " << i << " expected.size() = " << expected_.size()
      << " with front = " << expected_.front();
    ASSERT_NE(0, expected_.size());
    EXPECT_EQ(expected_.front(), i);
    expected_.pop();
  }

  bool empty() const { return expected_.empty(); }
  std::size_t size() const { return expected_.size(); }

private:
  std::queue<int> expected_;
};

TEST(variant, nested_visitor_0) {
  nested_visitor visitor{5};
  nested_variant(allocator, 5).visit(visitor);
  EXPECT_TRUE(visitor.empty());
}

TEST(variant, nested_visitor_1) {
  nested_visitor visitor{-1, 9, -2};
  nested_variant(
    allocator,
    nested_vector{
      nested_variant(allocator, 9)
    }
  ).visit(visitor);
  EXPECT_TRUE(visitor.empty());
}

TEST(variant, nested_visitor_5) {
  nested_visitor visitor{
    -1, 10,
      -1, 20,
        -1, 30,
          -1, 40,
            -1, 50, 51, 52, 53,
            -2, 41,
          -2, 31,
        -2, 21,
      -2, 11,
        -1, 22,
      -2, 12,
    -2,
  };
  nested_variant(allocator, nested_vector{
    nested_variant(allocator, 10),
    nested_variant(allocator, nested_vector{
      nested_variant(allocator, 20),
      nested_variant(allocator, nested_vector{
        nested_variant(allocator, 30),
        nested_variant(allocator, nested_vector{
          nested_variant(allocator, 40),
          nested_variant(allocator, nested_vector{
            nested_variant(allocator, 50),
            nested_variant(allocator, 51),
            nested_variant(allocator, 52),
            nested_variant(allocator, 53)
          }),
          nested_variant(allocator, 41),
        }),
        nested_variant(allocator, 31),
      }),
      nested_variant(allocator, 21),
    }),
    nested_variant(allocator, 11),
    nested_variant(allocator, nested_vector{
      nested_variant(allocator, 22)
    }),
    nested_variant(allocator, 12)
  }).visit(visitor);
  EXPECT_TRUE(visitor.empty());
}

template <typename T, T returnValue>
struct visitor_that_returns {
  visitor_that_returns() {}

  template <typename U>
  T operator()(U&&) { return returnValue; }

  template <typename U>
  T operator()(U&&) const { return returnValue; }
};

TEST(variant, visitor_that_return) {
  typedef test_variant<int, double> var;

  typedef visitor_that_returns<int, 99> visitor_type;
  visitor_type visitor;
  visitor_type const cvisitor;

  EXPECT_THROW((visit<int>(var(), visitor_type())), std::logic_error);
  EXPECT_EQ(111, (visit_def<int>(var(), visitor_type(), 111)));
  EXPECT_THROW((visit<int>(var(), visitor)), std::logic_error);
  EXPECT_EQ(111, (visit_def<int>(var(), visitor, 111)));
  EXPECT_THROW((visit<int>(var(), cvisitor)), std::logic_error);
  EXPECT_EQ(111, (visit_def<int>(var(), cvisitor, 111)));

  EXPECT_EQ(99, (visit<int>(var(allocator, 10), visitor_type())));
  EXPECT_EQ(99, (visit<int>(var(allocator, 10), visitor)));
  EXPECT_EQ(99, (visit<int>(var(allocator, 10), cvisitor)));

  EXPECT_EQ(99, (visit<int>(var(allocator, 3.2), visitor_type())));
  EXPECT_EQ(99, (visit<int>(var(allocator, 3.2), visitor)));
  EXPECT_EQ(99, (visit<int>(var(allocator, 3.2), cvisitor)));

  var const c(allocator, 123);

  EXPECT_TRUE(c.is_of<int>());
  EXPECT_EQ(99, (visit<int>(c, visitor_type())));
  EXPECT_EQ(99, (visit_def<int>(c, visitor_type(), 111)));
  EXPECT_EQ(99, (visit<int>(c, visitor)));
  EXPECT_EQ(99, (visit_def<int>(c, visitor, 111)));
  EXPECT_EQ(99, (visit<int>(c, cvisitor)));
  EXPECT_EQ(99, (visit_def<int>(c, cvisitor, 111)));

  var v(allocator);

  EXPECT_TRUE(v.empty());
  EXPECT_THROW((visit<int>(v, visitor_type())), std::logic_error);
  EXPECT_EQ(111, (visit_def<int>(v, visitor_type(), 111)));
  EXPECT_THROW((visit<int>(v, visitor)), std::logic_error);
  EXPECT_EQ(111, (visit_def<int>(v, visitor, 111)));
  EXPECT_THROW((visit<int>(v, cvisitor)), std::logic_error);
  EXPECT_EQ(111, (visit_def<int>(v, cvisitor, 111)));

  v = 10;
  EXPECT_TRUE(v.is_of<int>());
  EXPECT_EQ(99, (visit<int>(v, visitor_type())));
  EXPECT_EQ(99, (visit<int>(v, visitor)));
  EXPECT_EQ(99, (visit<int>(v, cvisitor)));

  v = 3.2;
  EXPECT_TRUE(v.is_of<double>());
  EXPECT_EQ(99, (visit<int>(v, visitor_type())));
  EXPECT_EQ(99, (visit<int>(v, visitor)));
  EXPECT_EQ(99, (visit<int>(v, cvisitor)));
}

template <typename variant_type>
struct visitor_that_returns_type_tag {
  template <typename U>
  typename variant_type::type_tag operator()(U &&) {
    return variant_type::template tag<
      typename std::remove_reference<U>::type
    >();
  }
};

TEST(variant, visitor_that_return_on_arena_variant) {
  SysArena arena;
  MemoryLeakCheckerAllocator<StlAllocator<SysArena, int>, int> allocator{
    StlAllocator<SysArena, int>(&arena)
  };
  typedef variant<
    default_storage_policy<decltype(allocator), dynamic_allocation_policy>,
    int, double, std::string
  > var;
  typedef visitor_that_returns_type_tag<var> visitor_type;

  var v(allocator);
  EXPECT_TRUE(v.empty());
  EXPECT_THROW((visit<var::type_tag>(v, visitor_type())), std::logic_error);

  v = 10;
  EXPECT_TRUE(v.is_of<int>());
  EXPECT_EQ((var::tag<int>()), (visit<var::type_tag>(v, visitor_type())));

  v = 5.6;
  EXPECT_TRUE(v.is_of<double>());
  EXPECT_EQ((var::tag<double>()), (visit<var::type_tag>(v, visitor_type())));

  v = std::string();
  EXPECT_TRUE(v.is_of<std::string>());
  EXPECT_EQ(
    (var::tag<std::string>()),
    (visit<var::type_tag>(v, visitor_type()))
  );
}

struct visitor_that_return_folly_dynamic {
  template <typename U>
  folly::dynamic operator()(U &&value) {
    return folly::dynamic(value);
  }
};

TEST(variant, visitor_that_return_folly_dynamic) {
  typedef test_variant<int, double> var;
  visitor_that_return_folly_dynamic visitor;
  var v(allocator);

  v = 10;
  auto i = visit<folly::dynamic>(v, visitor);
  EXPECT_TRUE(i.isInt());
  EXPECT_EQ(10, i.asInt());

  v = 5.6;
  auto d = visit<folly::dynamic>(v, visitor);
  EXPECT_TRUE(d.isDouble());
  EXPECT_EQ(5.6, d.asDouble());
}

template <typename return_type>
struct visitor_with_additional_parameters {
  return_type operator () () const { return return_type(0); }

  template <typename U, typename... UArgs>
  return_type operator () (U &&value, UArgs &&...args) const {
    return static_cast<return_type>(value)
      + (*this)(std::forward<UArgs>(args)...);
  }
};

TEST(variant, visitor_with_additional_parameters) {
  typedef test_variant<int, double> var;

  typedef visitor_with_additional_parameters<double> visitor_type;
  visitor_type visitor;

  EXPECT_THROW(
    (visit<double>(var(), visitor_type(), 1.9, 54)),
    std::logic_error
  );
  EXPECT_EQ(111, (visit_def<double>(var(), visitor_type(), 111, 1.9, 54)));
  EXPECT_THROW((visit<double>(var(), visitor, 1.9, 54)), std::logic_error);
  EXPECT_EQ(111, (visit_def<double>(var(), visitor, 111)));
  EXPECT_EQ(
    10 + 1.9 + 54,
    (visit<double>(var(allocator, 10), visitor_type(), 1.9, 54))
  );
  EXPECT_EQ(
    10 + 1.9 + 54,
    (visit<double>(var(allocator, 10), visitor, 1.9, 54))
  );
  EXPECT_EQ(
    3.2 + 1.9 + 54,
    (visit<double>(var(allocator, 3.2), visitor_type(), 1.9, 54))
  );
  EXPECT_EQ(
    3.2 + 1.9 + 54,
    (visit<double>(var(allocator, 3.2), visitor, 1.9, 54))
  );

  var const c(allocator, 99);

  EXPECT_TRUE(c.is_of<int>());
  EXPECT_EQ(99 + 1.9 + 54, (visit<double>(c, visitor_type(), 1.9, 54)));
  EXPECT_EQ(99 + 1.9 + 54, (visit<double>(c, visitor, 1.9, 54)));
  EXPECT_EQ(
    99 + 1.9 + 54,
    (visit_def<double>(c, visitor_type(), 111, 1.9, 54))
  );
  EXPECT_EQ(99 + 1.9 + 54, (visit_def<double>(c, visitor, 111, 1.9, 54)));

  var v(allocator);

  EXPECT_TRUE(v.empty());
  EXPECT_THROW((visit<double>(v, visitor_type())), std::logic_error);
  EXPECT_EQ(111, (visit_def<double>(v, visitor_type(), 111)));
  EXPECT_THROW((visit<double>(v, visitor)), std::logic_error);
  EXPECT_EQ(111, (visit_def<double>(v, visitor, 111)));

  v = 10;
  EXPECT_TRUE(v.is_of<int>());
  EXPECT_EQ(10 + 1.9 + 54, (visit<double>(v, visitor_type(), 1.9, 54)));
  EXPECT_EQ(10 + 1.9 + 54, (visit<double>(v, visitor, 1.9, 54)));

  v = 3.2;
  EXPECT_TRUE(v.is_of<double>());
  EXPECT_EQ(3.2 + 1.9 + 54, (visit<double>(v, visitor_type(), 1.9, 54)));
  EXPECT_EQ(3.2 + 1.9 + 54, (visit<double>(v, visitor, 1.9, 54)));
}

TEST(variant, variant_vector) {
  typedef test_variant<int, double, test_string> var;
  test_vector<var> v(allocator);

  v.reserve(1);
  EXPECT_EQ(0, v.size());

  v.push_back(var(allocator, 10));
  EXPECT_EQ(1, v.size());
  EXPECT_TRUE(v[0].is_of<int>());
  EXPECT_EQ(10, (v[0].get<int>()));

  v.emplace_back(allocator, 5.6);
  EXPECT_EQ(2, v.size());
  EXPECT_TRUE(v[0].is_of<int>());
  EXPECT_EQ(10, (v[0].get<int>()));
  EXPECT_TRUE(v[1].is_of<double>());
  EXPECT_EQ(5.6, (v[1].get<double>()));

  v.resize(3);
  EXPECT_EQ(3, v.size());
  EXPECT_TRUE(v[0].is_of<int>());
  EXPECT_EQ(10, (v[0].get<int>()));
  EXPECT_TRUE(v[1].is_of<double>());
  EXPECT_EQ(5.6, (v[1].get<double>()));
  EXPECT_TRUE(v[2].empty());

  v[2] = var(allocator, test_string("HELLO, WORLD", allocator));
  EXPECT_EQ(3, v.size());
  EXPECT_TRUE(v[0].is_of<int>());
  EXPECT_EQ(10, (v[0].get<int>()));
  EXPECT_TRUE(v[1].is_of<double>());
  EXPECT_EQ(5.6, (v[1].get<double>()));
  EXPECT_TRUE(v[2].is_of<test_string>());
  EXPECT_EQ(test_string("HELLO, WORLD", allocator), (v[2].get<test_string>()));

  v.resize(4);
  EXPECT_EQ(4, v.size());
  EXPECT_TRUE(v[0].is_of<int>());
  EXPECT_EQ(10, (v[0].get<int>()));
  EXPECT_TRUE(v[1].is_of<double>());
  EXPECT_EQ(5.6, (v[1].get<double>()));
  EXPECT_TRUE(v[2].is_of<test_string>());
  EXPECT_EQ(test_string("HELLO, WORLD", allocator), (v[2].get<test_string>()));
  EXPECT_TRUE(v[3].empty());

  var const t(allocator, test_string("THIS IS A TEST", allocator));
  v[3] = t;
  EXPECT_EQ(4, v.size());
  EXPECT_TRUE(v[0].is_of<int>());
  EXPECT_EQ(10, (v[0].get<int>()));
  EXPECT_TRUE(v[1].is_of<double>());
  EXPECT_EQ(5.6, (v[1].get<double>()));
  EXPECT_TRUE(v[2].is_of<test_string>());
  EXPECT_EQ(test_string("HELLO, WORLD", allocator), (v[2].get<test_string>()));
  EXPECT_EQ(t.tag(), v[3].tag());
  EXPECT_EQ((t.get<test_string>()), (v[3].get<test_string>()));
}

TEST(variant, variant_map_as_value) {
  typedef test_variant<int, double, test_string> var;
  std::map<test_string, var> m;
  typedef decltype(m)::value_type pair_type;

  m.insert(pair_type("int", var(allocator, 10)));
  m.insert(pair_type("double", var(allocator, 5.6)));
  m["string"] = var(allocator, test_string("HELLO, WORLD", allocator));
  var const t(allocator, test_string("THIS IS A TEST", allocator));
  m["t"] = t;

  EXPECT_EQ(4, m.size());

  FOR_EACH_KV(key, value, m) {
    if(key == "int") {
      EXPECT_TRUE(value.is_of<int>());
      EXPECT_EQ(10, (value.get<int>()));
    } else if(key == "double") {
      EXPECT_TRUE(value.is_of<double>());
      EXPECT_EQ(5.6, (value.get<double>()));
    } else if(key == "string") {
      EXPECT_TRUE(value.is_of<test_string>());
      EXPECT_EQ(
        test_string("HELLO, WORLD", allocator),
        (value.get<test_string>())
      );
    } else if(key == "t") {
      EXPECT_EQ(t.tag(), value.tag());
      EXPECT_EQ((t.get<test_string>()), (value.get<test_string>()));
    } else {
      EXPECT_TRUE(false);
    }
  }
}

TEST(variant, variant_map_as_key_value) {
  typedef test_variant<int, double, test_string> var;
  std::map<var, var> m;
  typedef decltype(m)::value_type pair_type;

  m.insert(std::make_pair(var(allocator, 10), var(allocator, 5.6)));
  var const t(allocator, test_string("THIS IS A TEST", allocator));
  m.insert(
    pair_type(var(allocator, test_string("HELLO, WORLD", allocator)), t)
  );

  EXPECT_EQ(2, m.size());

  FOR_EACH_KV(key, value, m) {
    switch(key.tag()) {
      case var::tag<int>():
        EXPECT_EQ(10, (key.get<int>()));
        EXPECT_TRUE(value.is_of<double>());
        EXPECT_EQ(5.6, (value.get<double>()));
        break;

      case var::tag<test_string>():
        EXPECT_EQ(
          test_string("HELLO, WORLD", allocator),
          (key.get<test_string>())
        );
        EXPECT_EQ(t.tag(), value.tag());
        EXPECT_EQ((t.get<test_string>()), (value.get<test_string>()));
        break;

      default:
        EXPECT_TRUE(false);
    }
  }

  var const k1(allocator, 10);
  var const k2(allocator, test_string("HELLO, WORLD", allocator));

  auto v1 = m.find(k1);
  ASSERT_NE(v1, m.end());
  EXPECT_TRUE(v1->second.is_of<double>());
  EXPECT_EQ(5.6, (v1->second.get<double>()));

  auto v2 = m.find(var(allocator, test_string("HELLO, WORLD", allocator)));
  ASSERT_NE(v2, m.end());
  EXPECT_EQ(t.tag(), v2->second.tag());
  EXPECT_EQ((t.get<test_string>()), (v2->second.get<test_string>()));

  m[var(allocator, 10)] = var(
    allocator,
    test_string("so you didn't misspell cat prophet, my bad", allocator)
  );
  m[k2] = var(allocator, 456789);

  FOR_EACH_KV(key, value, m) {
    switch(key.tag()) {
      case var::tag<int>():
        EXPECT_EQ(10, (key.get<int>()));
        EXPECT_TRUE(value.is_of<test_string>());
        EXPECT_EQ(
          "so you didn't misspell cat prophet, my bad",
          (value.get<test_string>())
        );
        break;

      case var::tag<test_string>():
        EXPECT_EQ(
          test_string("HELLO, WORLD", allocator),
          (key.get<test_string>())
        );
        EXPECT_TRUE(value.is_of<int>());
        EXPECT_EQ(456789, (value.get<int>()));
        break;

      default:
        EXPECT_TRUE(false);
    }
  }

  v1 = m.find(var(allocator, 10));
  ASSERT_NE(v1, m.end());
  EXPECT_TRUE(v1->second.is_of<test_string>());
  EXPECT_EQ(
    "so you didn't misspell cat prophet, my bad",
    (v1->second.get<test_string>())
  );

  v2 = m.find(k2);
  ASSERT_NE(v2, m.end());
  EXPECT_TRUE(v2->second.is_of<int>());
  EXPECT_EQ(456789, (v2->second.get<int>()));
}

TEST(variant, variant_unordered_map_as_key_value) {
  typedef test_variant<int, double, std::string> var;
  std::unordered_map<var, var> m;

  m.emplace(var(allocator, 10), var(allocator, 5.6));
  var const t(allocator, std::string("THIS IS A TEST"));
  m.insert(
    std::make_pair(var(allocator, std::string("HELLO, WORLD")), t)
  );

  EXPECT_EQ(2, m.size());

  FOR_EACH_KV(key, value, m) {
    switch(key.tag()) {
      case var::tag<int>():
        EXPECT_EQ(10, (key.get<int>()));
        EXPECT_TRUE(value.is_of<double>());
        EXPECT_EQ(5.6, (value.get<double>()));
        break;

      case var::tag<std::string>():
        EXPECT_EQ(std::string("HELLO, WORLD"), (key.get<std::string>()));
        EXPECT_EQ(t.tag(), value.tag());
        EXPECT_EQ((t.get<std::string>()), (value.get<std::string>()));
        break;

      default:
        EXPECT_TRUE(false);
    }
  }

  var const k1(allocator, 10);
  var const k2(allocator, std::string("HELLO, WORLD"));

  auto v1 = m.find(k1);
  ASSERT_NE(v1, m.end());
  EXPECT_TRUE(v1->second.is_of<double>());
  EXPECT_EQ(5.6, (v1->second.get<double>()));

  auto v2 = m.find(var(allocator, std::string("HELLO, WORLD")));
  ASSERT_NE(v2, m.end());
  EXPECT_EQ(t.tag(), v2->second.tag());
  EXPECT_EQ((t.get<std::string>()), (v2->second.get<std::string>()));

  m[var(allocator, 10)] = var(
    allocator,
    std::string("so you didn't misspell cat prophet, my bad")
  );
  m[k2] = var(allocator, 456789);

  FOR_EACH_KV(key, value, m) {
    switch(key.tag()) {
      case var::tag<int>():
        EXPECT_EQ(10, (key.get<int>()));
        EXPECT_TRUE(value.is_of<std::string>());
        EXPECT_EQ(
          "so you didn't misspell cat prophet, my bad",
          (value.get<std::string>())
        );
        break;

      case var::tag<std::string>():
        EXPECT_EQ(std::string("HELLO, WORLD"), (key.get<std::string>()));
        EXPECT_TRUE(value.is_of<int>());
        EXPECT_EQ(456789, (value.get<int>()));
        break;

      default:
        EXPECT_TRUE(false);
    }
  }

  v1 = m.find(var(allocator, 10));
  ASSERT_NE(v1, m.end());
  EXPECT_TRUE(v1->second.is_of<std::string>());
  EXPECT_EQ(
    "so you didn't misspell cat prophet, my bad",
    (v1->second.get<std::string>())
  );

  v2 = m.find(k2);
  ASSERT_NE(v2, m.end());
  EXPECT_TRUE(v2->second.is_of<int>());
  EXPECT_EQ(456789, (v2->second.get<int>()));
}

TEST(variant, rebind_storage_policy) {
  typedef default_storage_policy<
    decltype(allocator), automatic_allocation_policy
  > custom_storage_policy;

  typedef test_variant<int, double, test_string> original_var;
  typedef original_var::types::apply_front<
    variant, custom_storage_policy
  > rebound_var;

  EXPECT_TRUE((
    std::is_same<
      rebound_var,
      variant<custom_storage_policy, int, double, test_string>
    >::value
  ));

  rebound_var v(allocator, 123456789);

  EXPECT_TRUE(v.is_of<int>());
  EXPECT_EQ(123456789, (v.get<int>()));
}

TEST(variant, container_assignment) {
  typedef test_variant<int, double, test_string> VAR;
  typedef test_vector<VAR> VECTOR;
  VECTOR v1(allocator);
  v1.reserve(1);

  v1.push_back(VAR(allocator, 10));
  v1.emplace_back(allocator, 5.6);
  v1.resize(3);
  v1[2] = VAR(allocator, test_string("HELLO, WORLD", allocator));

  auto check = [](VECTOR const &v) {
    ASSERT_EQ(3, v.size());
    EXPECT_TRUE(v[0].is_of<int>());
    EXPECT_EQ(10, (v[0].get<int>()));
    EXPECT_TRUE(v[1].is_of<double>());
    EXPECT_EQ(5.6, (v[1].get<double>()));
    EXPECT_TRUE(v[2].is_of<test_string>());
    EXPECT_EQ("HELLO, WORLD", (v[2].get<test_string>()));
  };
  check(v1);

  VECTOR v2(allocator);
  v2 = v1;
  check(v1);
  check(v2);

  VECTOR v3(allocator);
  v3 = std::move(v1);
  EXPECT_EQ(0, v1.size());
  check(v2);
  check(v3);
}

TEST(variant, set_difference) {
  typedef test_variant<int, double, test_string> VAR;
  typedef test_vector<VAR> VECTOR;

  VECTOR lhs({
    VAR(allocator, 1),
    VAR(allocator, 2),
    VAR(allocator, 3)
  }, allocator);
  std::sort(lhs.begin(), lhs.end());

  VECTOR rhs(allocator);
  std::sort(rhs.begin(), rhs.end());

  VECTOR diff(allocator);

  std::set_difference(
    lhs.begin(), lhs.end(),
    rhs.begin(), rhs.end(),
    std::back_inserter(diff)
  );

  for(auto &i: diff) {
    LOG(INFO) << "result: " << i.get<int>();
  }

  VECTOR const expected({
    VAR(allocator, 1),
    VAR(allocator, 2),
    VAR(allocator, 3)
  }, allocator);
  EXPECT_EQ(expected.size(), diff.size());
  EXPECT_EQ(expected, diff);
}

TEST(variant, set_difference_inplace) {
  typedef test_variant<int, double, test_string> VAR;
  typedef test_vector<VAR> VECTOR;

  VECTOR lhs({
    VAR(allocator, 1),
    VAR(allocator, 2),
    VAR(allocator, 3)
  }, allocator);
  std::sort(lhs.begin(), lhs.end());

  VECTOR rhs(allocator);
  std::sort(rhs.begin(), rhs.end());

  auto end = std::set_difference(
    lhs.begin(), lhs.end(),
    rhs.begin(), rhs.end(),
    lhs.begin()
  );
  lhs.erase(end, lhs.end());

  for(auto &i: lhs) {
    LOG(INFO) << "result: " << i.get<int>();
  }

  VECTOR const expected({
    VAR(allocator, 1),
    VAR(allocator, 2),
    VAR(allocator, 3)
  }, allocator);
  EXPECT_EQ(expected.size(), lhs.size());
  EXPECT_EQ(expected, lhs);
}

// This MUST be the LAST test.
TEST(variant, memory_leak) {
  auto balance = allocated < freed
    ? freed - allocated
    : allocated - freed;

  LOG(INFO) << "allocated: " << allocated
    << " freed: " << freed
    << " balance: " << balance
    << (
      allocated < freed
        ? " negative (huh?)"
        : freed < allocated
          ? " positive (leak)" : ""
    );

  EXPECT_EQ(allocated, freed);
}

} // namespace fatal {
