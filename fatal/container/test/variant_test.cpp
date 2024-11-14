/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/container/variant.h>

#include <fatal/test/driver.h>

#include <fatal/container/optional.h>
#include <fatal/math/numerics.h>

#include <algorithm>
#include <initializer_list>
#include <map>
#include <memory>
#include <queue>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include <cassert>

namespace fatal {

static std::uintmax_t allocated = 0;
static std::uintmax_t freed = 0;

struct free_deleter {
  void operator ()(void *p) const {
    assert(p);
    std::free(p);
  }
};

template <typename T>
struct test_allocator {
  using value_type = T;
  using pointer = value_type *;
  using const_pointer = value_type const *;
  using reference = value_type &;
  using const_reference = value_type const *;

  using difference_type = std::ptrdiff_t;
  using size_type = std::size_t;

private:
  using chunk_t = std::unique_ptr<void, free_deleter>;
  using chunk_map = std::unordered_map<void *, chunk_t>;
  using shared_chunk = std::shared_ptr<chunk_map>;

public:
  test_allocator(): chunks_(std::make_shared<chunk_map>()) {}

  template <typename U>
  explicit test_allocator(test_allocator<U> const &rhs):
    chunks_(rhs.chunks())
  {}

  template <typename U>
  explicit test_allocator(test_allocator<U> &&rhs):
    chunks_(std::move(rhs.chunks()))
  {}

  pointer allocate(size_type n, void const * = nullptr) {
    for (auto tries = 3; tries--; ) {
      chunk_t buffer(std::malloc(sizeof(value_type) * n));
      auto p = buffer.get();

      // TODO: use std::align() when available
      if (!p || reinterpret_cast<std::uintptr_t>(p) % alignof(value_type)) {
        continue;
      }

      assert(chunks_->find(p) == chunks_->end());
      (*chunks_)[p] = std::move(buffer);
      return static_cast<pointer>(p);
    }

    return nullptr;
  }

  void deallocate(pointer p, size_type) {
    assert(chunks_->find(p) != chunks_->end());
    chunks_->erase(p);
  }

  size_type max_size() const { return std::numeric_limits<size_type>::max(); }

  template <typename... Args>
  void construct(pointer p, Args &&...args) {
    new (p) value_type(std::forward<Args>(args)...);
  }

  void destroy(pointer p) {
    assert(p);
    p->~value_type();
  }

  template <typename U>
  struct rebind {
    using other = test_allocator<U>;
  };

  bool operator !=(test_allocator const &rhs) const {
    return chunks_ != rhs.chunks_;
  }

  bool operator ==(test_allocator const &rhs) const {
    return chunks_ == rhs.chunks_;
  }

  shared_chunk const &chunks() const { return chunks_; }
  shared_chunk &chunks() { return chunks_; }

private:
  shared_chunk chunks_;
};

template <
  typename TAllocator,
  typename T = typename std::allocator_traits<TAllocator>::value_type
>
struct checked_allocator {
  using allocator_type = TAllocator;
  using allocator_traits = std::allocator_traits<allocator_type>;
  using value_type = T;
  using pointer = value_type *;
  using const_pointer = value_type const *;
  using reference = value_type &;
  using const_reference = value_type const *;

  using difference_type = std::ptrdiff_t;
  using size_type = std::size_t;

  static_assert(
    std::is_same<value_type, typename allocator_type::value_type>::value,
    "wrapped allocator has a different value_type"
  );

  checked_allocator() = default;

  checked_allocator(checked_allocator const &other) = default;

  explicit checked_allocator(allocator_type const &allocator) noexcept:
    allocator_(allocator)
  {}

  template <typename UAllocator, typename U>
  /* implicit */ checked_allocator(
    checked_allocator<UAllocator, U> const &other
  ) noexcept:
    allocator_(other.get_allocator())
  {}

  checked_allocator& operator =(checked_allocator const &other) = default;

  checked_allocator& operator =(allocator_type const &allocator) noexcept {
    allocator_ = allocator;
    return *this;
  }

  template <typename UAllocator, typename U>
  checked_allocator& operator =(
    checked_allocator<UAllocator, U> const &other
  ) noexcept {
    allocator_ = other.get_allocator();
    return *this;
  }

  pointer allocate(size_type n, void const *hint = nullptr) {
    auto p = allocator_traits::allocate(allocator_, n, hint);
    allocated += n * sizeof(value_type);
    return p;
  }

  void deallocate(pointer p, size_type n) {
    allocator_.deallocate(p, n);
    freed += n * sizeof(value_type);
  }

  size_type max_size() const {
    return allocator_traits::max_size(allocator_);
  }

  template <typename... Args>
  void construct(pointer p, Args &&...args) {
    allocator_traits::construct(allocator_, p, std::forward<Args>(args)...);
  }

  void destroy(pointer p) {
    allocator_traits::destroy(allocator_, p);
  }

  template <typename U>
  struct rebind {
    using other = checked_allocator<
      typename allocator_traits::template rebind_alloc<U>,
      U
    >;
  };

  allocator_type get_allocator() const { return allocator_; }

  bool operator !=(checked_allocator const &other) const {
    return allocator_ != other.allocator_;
  }

  bool operator ==(checked_allocator const &other) const {
    return allocator_ == other.allocator_;
  }

private:
  allocator_type allocator_;
};

checked_allocator<
  typename default_storage_policy<>::allocator_traits::rebind_alloc<int>
> allocator{
  typename default_storage_policy<>::allocator_traits::rebind_alloc<int>()
};

using test_policy = default_storage_policy<decltype(allocator)>;

template <typename... Args>
using test_variant = variant<test_policy, Args...>;

using empty_var = test_variant<>;

template <typename T>
using basic_test_string = std::basic_string<
  char,
  std::char_traits<char>,
  typename decltype(allocator)::template rebind<T>::other
>;

using test_string = basic_test_string<char>;

template <typename T>
using test_vector = std::vector<
  T,
  typename decltype(allocator)::template rebind<T>::other
>;

////////////////
// UNIT TESTS //
////////////////

FATAL_TEST(variant, is_variant) {
  FATAL_VLOG(1) << "is_variant -> variant";
  FATAL_EXPECT_TRUE((is_variant<test_variant<>>::value));
  FATAL_EXPECT_TRUE((is_variant<test_variant<bool>>::value));
  FATAL_EXPECT_TRUE((is_variant<test_variant<int>>::value));
  FATAL_EXPECT_TRUE((is_variant<test_variant<double>>::value));
  FATAL_EXPECT_TRUE((is_variant<test_variant<long>>::value));
  FATAL_EXPECT_TRUE((is_variant<test_variant<test_string>>::value));
  FATAL_EXPECT_TRUE((is_variant<test_variant<int, double, long>>::value));
  FATAL_EXPECT_TRUE(
    (is_variant<test_variant<int, double, test_string>>::value)
  );

  FATAL_VLOG(1) << "is_variant -> single types";
  FATAL_EXPECT_FALSE((is_variant<bool>::value));
  FATAL_EXPECT_FALSE((is_variant<int>::value));
  FATAL_EXPECT_FALSE((is_variant<double>::value));
  FATAL_EXPECT_FALSE((is_variant<long>::value));
  FATAL_EXPECT_FALSE((is_variant<test_string>::value));

  FATAL_VLOG(1) << "is_variant -> tuple";
  FATAL_EXPECT_FALSE((is_variant<std::tuple<>>::value));
  FATAL_EXPECT_FALSE((is_variant<std::tuple<bool>>::value));
  FATAL_EXPECT_FALSE((is_variant<std::tuple<int>>::value));
  FATAL_EXPECT_FALSE((is_variant<std::tuple<double>>::value));
  FATAL_EXPECT_FALSE((is_variant<std::tuple<long>>::value));
  FATAL_EXPECT_FALSE((is_variant<std::tuple<test_string>>::value));
  FATAL_EXPECT_FALSE((is_variant<std::tuple<int, double, long>>::value));
  FATAL_EXPECT_FALSE((is_variant<std::tuple<int, double, test_string>>::value));
}

FATAL_TEST(variant, default_ctor) {
  test_variant<int, test_string, double> v(allocator);
  using var = decltype(v);
  FATAL_EXPECT_EQ(var::no_tag(), v.tag());
  FATAL_EXPECT_TRUE(v.empty());
}

FATAL_TEST(variant, value_ctor) {
  test_variant<int, test_string, double> v(10);
  FATAL_EXPECT_FALSE(v.empty());
  FATAL_EXPECT_TRUE(v.is_of<int>());
  FATAL_EXPECT_EQ(10, (v.get<int>()));
  v = test_variant<int, test_string, double>(5.5);
  FATAL_EXPECT_FALSE(v.empty());
  FATAL_EXPECT_TRUE(v.is_of<double>());
  FATAL_EXPECT_EQ(5.5, (v.get<double>()));
  int i = 50;
  v = test_variant<int, test_string, double>(i);
  FATAL_EXPECT_FALSE(v.empty());
  FATAL_EXPECT_TRUE(v.is_of<int>());
  FATAL_EXPECT_EQ(50, (v.get<int>()));
}

test_variant<int, test_string, double> make_variant() {
  return test_variant<int, test_string, double>(allocator, 10);
}

FATAL_TEST(variant, getters) {
  test_variant<int, test_string, double> v(allocator, 10);

  FATAL_EXPECT_TRUE(v.is_of<int>());
  FATAL_EXPECT_EQ(10, (v.unchecked_get<int>()));
  FATAL_EXPECT_EQ(10, (make_variant().unchecked_get<int>()));
  FATAL_EXPECT_EQ(10, (v.get<int>()));
  FATAL_EXPECT_EQ(10, (make_variant().get<int>()));
  FATAL_EXPECT_THROW(std::invalid_argument) {
    v.get<double>();
  };
  FATAL_EXPECT_THROW(std::invalid_argument) {
    make_variant().get<double>();
  };
}

FATAL_TEST(variant, lvalue_setters) {
  test_variant<int> v;
  static_assert(std::is_lvalue_reference<decltype(v.set(0))>::value,
      "set must return a lvalue reference when the variant is a lvalue.");
  static_assert(std::is_lvalue_reference<decltype(v.emplace<int>(0))>::value,
      "emplace must return a lvalue reference when the variant is a lvalue.");
  static_assert(std::is_lvalue_reference<decltype(v = 0)>::value,
      "operator= must return a lvalue reference when the variant is a lvalue.");
  auto& x = v.set(10);
  FATAL_EXPECT_EQ(10, (v.get<int>()));
  x = 9;
  FATAL_EXPECT_EQ(9, (v.get<int>()));

  auto& y = v = 8;
  FATAL_EXPECT_EQ(8, (v.get<int>()));
  y = 7;
  FATAL_EXPECT_EQ(7, (v.get<int>()));

  auto& z = v.emplace<int>(6);
  FATAL_EXPECT_EQ(6, (v.get<int>()));
  z = 5;
  FATAL_EXPECT_EQ(5, (v.get<int>()));
}

FATAL_TEST(variant, rvalue_setters) {
  static_assert(std::is_rvalue_reference<
        decltype(make_variant().set(0))
      >::value,
      "set must return a rvalue reference when the variant is a rvalue.");
  static_assert(std::is_rvalue_reference<
        decltype(make_variant().emplace<int>(0))
      >::value,
      "emplace must return a rvalue reference when the variant is a rvalue.");
  static_assert(std::is_rvalue_reference<
        decltype(make_variant() = 0)
      >::value,
      "set must return a rvalue reference when the variant is a rvalue.");
  FATAL_EXPECT_EQ(10, (make_variant().set(10)));
  FATAL_EXPECT_EQ(10, (make_variant().emplace<int>(10)));
  FATAL_EXPECT_EQ(10, (make_variant() = 10));
}

struct copyable {
  copyable() {}

  copyable(copyable const &) = default;
  copyable(copyable &&) = delete;

  copyable &operator =(copyable const &) { return *this; }
  copyable &operator =(copyable &&) = delete;
};

FATAL_TEST(variant, universal_lvalue_setters) {
  test_variant<copyable> v(allocator);

  copyable x;
  v.set(x);

  const copyable y;
  v.set(y);

  test_variant<copyable>().set(x);
  test_variant<copyable>().set(y);

  v = x;
  v = y;

  test_variant<copyable>() = x;
  test_variant<copyable>() = y;
}

struct movable {
  movable() = default;

  movable(movable const &) = delete;
  movable(movable &&) = default;

  movable &operator =(movable const &) = delete;
  movable &operator =(movable &&) { return *this; }
};

FATAL_TEST(variant, universal_rvalue_setters) {
  test_variant<movable, copyable> v(allocator);

  movable a;
  v.set(std::move(a));

  const copyable ca;
  v.set(std::move(ca));

  v.set(movable());

  movable b;
  test_variant<movable, copyable>().set(std::move(b));

  const copyable cb;
  test_variant<movable, copyable>().set(std::move(cb));

  test_variant<movable, copyable>().set(movable());

  movable c;
  v = std::move(c);

  const copyable cc;
  v = std::move(cc);

  v = movable();

  movable d;
  test_variant<movable, copyable>() = std::move(d);

  const copyable cd;
  test_variant<movable, copyable>() = std::move(cd);

  test_variant<movable, copyable>() = movable();
}

struct FooCopyCtorTest {
  explicit FooCopyCtorTest(int) {}
  FooCopyCtorTest() = delete;
  FooCopyCtorTest(FooCopyCtorTest const &) {}
  FooCopyCtorTest(FooCopyCtorTest &&) = delete;
};

struct BarCopyCtorTest {
  explicit BarCopyCtorTest(int) {}
  BarCopyCtorTest() { FATAL_EXPECT_TRUE(false); }
  BarCopyCtorTest(BarCopyCtorTest const &) {}
  /* may throw */ BarCopyCtorTest(BarCopyCtorTest &&) {
    FATAL_EXPECT_TRUE(false);
  }
};

FATAL_TEST(variant, copy_ctor) {
  test_variant<FooCopyCtorTest, BarCopyCtorTest> v(allocator);
  FATAL_EXPECT_TRUE(v.empty());

  v.emplace<FooCopyCtorTest>(17);
  FATAL_EXPECT_FALSE(v.empty());
  FATAL_EXPECT_EQ(0, v.tag());

  decltype(v) u(static_cast<decltype(v) const &>(v));
  FATAL_EXPECT_FALSE(v.empty());
  FATAL_EXPECT_FALSE(u.empty());
  FATAL_EXPECT_EQ(0, v.tag());
  FATAL_EXPECT_EQ(0, u.tag());

  v.emplace<BarCopyCtorTest>(37);
  FATAL_EXPECT_FALSE(v.empty());
  FATAL_EXPECT_EQ(1, v.tag());

  decltype(v) z(static_cast<decltype(v) const &>(v));
  FATAL_EXPECT_FALSE(v.empty());
  FATAL_EXPECT_FALSE(z.empty());
  FATAL_EXPECT_EQ(1, v.tag());
  FATAL_EXPECT_EQ(1, z.tag());
}

struct FooMoveCtor {
  explicit FooMoveCtor(int) {}
  FooMoveCtor() = delete;
  FooMoveCtor(FooMoveCtor const &) = delete;
  FooMoveCtor(FooMoveCtor &&) noexcept {}
};

struct BarMoveCtor {
  explicit BarMoveCtor(int) {}
  BarMoveCtor() { FATAL_EXPECT_TRUE(false); }
  BarMoveCtor(BarMoveCtor const &) { FATAL_EXPECT_TRUE(false); }
  BarMoveCtor(BarMoveCtor &&) noexcept {}
};

FATAL_TEST(variant, move_ctor) {
  test_variant<FooMoveCtor, BarMoveCtor> v(allocator);
  FATAL_EXPECT_TRUE(v.empty());

  v.emplace<FooMoveCtor>(17);
  FATAL_EXPECT_FALSE(v.empty());
  FATAL_EXPECT_EQ(0, v.tag());

  decltype(v) u(std::move(v));
  FATAL_EXPECT_TRUE(v.empty());
  FATAL_EXPECT_FALSE(u.empty());
  FATAL_EXPECT_EQ(0, u.tag());

  v.emplace<BarMoveCtor>(37);
  FATAL_EXPECT_FALSE(v.empty());
  FATAL_EXPECT_EQ(1, v.tag());

  decltype(v) z(std::move(v));
  FATAL_EXPECT_TRUE(v.empty());
  FATAL_EXPECT_FALSE(z.empty());
  FATAL_EXPECT_EQ(1, z.tag());
}

struct throw_foo {
  [[noreturn]] throw_foo() { throw nullptr; }
  [[noreturn]] throw_foo(throw_foo const &) { throw nullptr; }
  [[noreturn]] throw_foo(throw_foo &&) { throw nullptr; }
  throw_foo &operator =(throw_foo const &) { throw nullptr; }
  throw_foo &operator =(throw_foo &&) { throw nullptr; }
};

static_assert(!std::is_nothrow_default_constructible<throw_foo>::value, "");
static_assert(!std::is_nothrow_copy_constructible<throw_foo>::value, "");
static_assert(!std::is_nothrow_move_constructible<throw_foo>::value, "");
static_assert(!std::is_nothrow_copy_assignable<throw_foo>::value, "");
static_assert(!std::is_nothrow_move_assignable<throw_foo>::value, "");

struct throw_large {
  [[noreturn]] throw_large() { throw nullptr; }
  [[noreturn]] throw_large(throw_large const &) { throw nullptr; }
  [[noreturn]] throw_large(throw_large &&) { throw nullptr; }
  throw_large &operator =(throw_large const &) { throw nullptr; }
  throw_large &operator =(throw_large &&) { throw nullptr; }
  std::array<char, 8192> payload;
};

static_assert(!std::is_nothrow_default_constructible<throw_large>::value, "");
static_assert(!std::is_nothrow_copy_constructible<throw_large>::value, "");
static_assert(!std::is_nothrow_move_constructible<throw_large>::value, "");
static_assert(!std::is_nothrow_copy_assignable<throw_large>::value, "");
static_assert(!std::is_nothrow_move_assignable<throw_large>::value, "");

struct nothrow_foo {
  nothrow_foo() noexcept {}
  nothrow_foo(nothrow_foo const &) noexcept {}
  nothrow_foo(nothrow_foo &&) noexcept {}
  nothrow_foo &operator =(nothrow_foo const &) noexcept { return *this; }
  nothrow_foo &operator =(nothrow_foo &&) noexcept { return *this; }
};

static_assert(std::is_nothrow_default_constructible<nothrow_foo>::value, "");
static_assert(std::is_nothrow_copy_constructible<nothrow_foo>::value, "");
static_assert(std::is_nothrow_move_constructible<nothrow_foo>::value, "");
static_assert(std::is_nothrow_copy_assignable<nothrow_foo>::value, "");
static_assert(std::is_nothrow_move_assignable<nothrow_foo>::value, "");

FATAL_TEST(variant, noexcept) {
  static_assert(std::is_nothrow_default_constructible<empty_var>::value, "");
  static_assert(std::is_nothrow_copy_constructible<empty_var>::value, "");
  static_assert(std::is_nothrow_move_constructible<empty_var>::value, "");
  static_assert(std::is_nothrow_copy_assignable<empty_var>::value, "");
  static_assert(std::is_nothrow_move_assignable<empty_var>::value, "");

# define TEST_IMPL(MayThrow, DefaultMoveMayThrow, ...) \
  do { \
    using default_var = default_variant<__VA_ARGS__>; \
    \
    default_var default_v; \
    FATAL_EXPECT_NOT_NULL(std::addressof(default_v)); \
    \
    static_assert( \
      !MayThrow == std::is_nothrow_copy_constructible<default_var>::value, \
      "unexpected noexcept declaration for copy constructor (default)" \
    ); \
    static_assert( \
      !DefaultMoveMayThrow == std::is_nothrow_move_constructible< \
        default_var \
      >::value, \
      "unexpected noexcept declaration for move constructor (default)" \
    ); \
    static_assert( \
      !MayThrow == std::is_nothrow_copy_assignable<default_var>::value, \
      "unexpected noexcept declaration for copy assignment operator (default)" \
    ); \
    static_assert( \
      !DefaultMoveMayThrow == std::is_nothrow_move_assignable< \
        default_var \
      >::value, \
      "unexpected noexcept declaration for move assignment operator (default)" \
    ); \
    \
    using auto_var = auto_variant<__VA_ARGS__>; \
    \
    auto_var auto_v; \
    FATAL_EXPECT_NOT_NULL(std::addressof(auto_v)); \
    \
    static_assert( \
      !MayThrow == std::is_nothrow_copy_constructible<auto_var>::value, \
      "unexpected noexcept declaration for copy constructor (auto)" \
    ); \
    static_assert( \
      !MayThrow == std::is_nothrow_move_constructible<auto_var>::value, \
      "unexpected noexcept declaration for move constructor (auto)" \
    ); \
    static_assert( \
      !MayThrow == std::is_nothrow_copy_assignable<auto_var>::value, \
      "unexpected noexcept declaration for copy assignment operator (auto)" \
    ); \
    static_assert( \
      !MayThrow == std::is_nothrow_move_assignable<auto_var>::value, \
      "unexpected noexcept declaration for move assignment operator (auto)" \
    ); \
    \
    using dynamic_var = default_dynamic_variant<__VA_ARGS__>; \
    \
    dynamic_var dynamic_v; \
    FATAL_EXPECT_NOT_NULL(std::addressof(dynamic_v)); \
    \
    static_assert( \
      !std::is_nothrow_copy_constructible<dynamic_var>::value, \
      "unexpected noexcept declaration for copy constructor (dynamic)" \
    ); \
    static_assert( \
      std::is_nothrow_move_constructible<dynamic_var>::value, \
      "unexpected noexcept declaration for move constructor (dynamic)" \
    ); \
    static_assert( \
      !std::is_nothrow_copy_assignable<dynamic_var>::value, \
      "unexpected noexcept declaration for copy assignment operator (dynamic)" \
    ); \
    static_assert( \
      std::is_nothrow_move_assignable<dynamic_var>::value, \
      "unexpected noexcept declaration for move assignment operator (dynamic)" \
    ); \
  } while (false)

  TEST_IMPL(false, false, int);
  TEST_IMPL(true,  true,  throw_foo);
  TEST_IMPL(true,  false, throw_large);
  TEST_IMPL(false, false, nothrow_foo);

  TEST_IMPL(true,  true,  int, throw_foo);
  TEST_IMPL(true,  false, int, throw_large);
  TEST_IMPL(false, false, int, nothrow_foo);
  TEST_IMPL(true,  true,  throw_foo, int);
  TEST_IMPL(true,  true,  throw_foo, throw_large);
  TEST_IMPL(true,  true,  throw_foo, nothrow_foo);
  TEST_IMPL(true,  false, throw_large, int);
  TEST_IMPL(true,  true,  throw_large, throw_foo);
  TEST_IMPL(true,  false, throw_large, nothrow_foo);
  TEST_IMPL(false, false, nothrow_foo, int);
  TEST_IMPL(true,  true,  nothrow_foo, throw_foo);
  TEST_IMPL(true,  false, nothrow_foo, throw_large);

  TEST_IMPL(true,  true,  int, throw_foo, throw_large);
  TEST_IMPL(true,  true,  int, throw_foo, nothrow_foo);
  TEST_IMPL(true,  false, int, throw_large, nothrow_foo);
  TEST_IMPL(true,  true,  throw_foo, throw_large, nothrow_foo);

  TEST_IMPL(true,  true,  int, throw_foo, throw_large, nothrow_foo);

# undef TEST_IMPL
}

FATAL_TEST(variant, value_copy_ctor) {
  test_variant<int, test_string, double> i(
    allocator, static_cast<int const &>(10)
  );
  FATAL_EXPECT_FALSE(i.empty());
  FATAL_EXPECT_EQ(0, i.tag());

  test_variant<int, test_string, double> s(
    allocator, static_cast<test_string const &>(test_string(allocator))
  );
  FATAL_EXPECT_FALSE(s.empty());
  FATAL_EXPECT_EQ(1, s.tag());

  test_variant<int, test_string, double> d(
    allocator, static_cast<double const &>(1.0)
  );
  FATAL_EXPECT_FALSE(d.empty());
  FATAL_EXPECT_EQ(2, d.tag());
}

FATAL_TEST(variant, value_move_ctor) {
  test_variant<int, test_string, double> i(
    allocator, std::move(10)
  );
  FATAL_EXPECT_FALSE(i.empty());
  FATAL_EXPECT_EQ(0, i.tag());

  test_variant<int, test_string, double> s(
    allocator, test_string(allocator)
  );
  FATAL_EXPECT_FALSE(s.empty());
  FATAL_EXPECT_EQ(1, s.tag());

  test_variant<int, test_string, double> d(
    allocator, std::move(1.0)
  );
  FATAL_EXPECT_FALSE(d.empty());
  FATAL_EXPECT_EQ(2, d.tag());
}

FATAL_TEST(variant, copy_ctor_different_variant) {
  using source_var = test_variant<bool, test_string, double, int>;

  {
    source_var v1(allocator, 10);
    test_variant<int> v2(static_cast<source_var const &>(v1));
    FATAL_ASSERT_TRUE(v2.is_of<int>());
    FATAL_EXPECT_EQ(10, v2.get<int>());
  }

  {
    source_var v1(allocator, 5.0);
    test_variant<double> v2(static_cast<source_var const &>(v1));
    FATAL_ASSERT_TRUE(v2.is_of<double>());
    FATAL_EXPECT_EQ(5.0, v2.get<double>());
  }

  {
    source_var v1(allocator, test_string("test", allocator));
    test_variant<test_string> v2(static_cast<source_var const &>(v1));
    FATAL_ASSERT_TRUE(v2.is_of<test_string>());
    FATAL_EXPECT_EQ("test", v2.get<test_string>());
  }
}

FATAL_TEST(variant, copy_ctor_different_variant_and_allocator) {
  using source_var = test_variant<bool, test_string, double, int>;

  {
    source_var v1(allocator, 10);
    test_variant<int> v2(
      allocator,
      static_cast<source_var const &>(v1)
    );
    FATAL_ASSERT_TRUE(v2.is_of<int>());
    FATAL_EXPECT_EQ(10, v2.get<int>());
  }

  {
    source_var v1(allocator, 5.0);
    test_variant<double> v2(
      allocator,
      static_cast<source_var const &>(v1)
    );
    FATAL_ASSERT_TRUE(v2.is_of<double>());
    FATAL_EXPECT_EQ(5.0, v2.get<double>());
  }

  {
    source_var v1(allocator, test_string("test", allocator));
    test_variant<test_string> v2(
      allocator,
      static_cast<source_var const &>(v1)
    );
    FATAL_ASSERT_TRUE(v2.is_of<test_string>());
    FATAL_EXPECT_EQ("test", v2.get<test_string>());
  }
}

FATAL_TEST(variant, copy_assignment) {
  using VAR = test_variant<int, double, test_string>;

  VAR v1(allocator, 10);
  FATAL_EXPECT_TRUE(v1.is_of<int>());
  FATAL_EXPECT_EQ(10, (v1.get<int>()));

  VAR v2;
  FATAL_EXPECT_TRUE(v2.empty());

  v2 = v1;
  FATAL_EXPECT_TRUE(v1.is_of<int>());
  FATAL_EXPECT_EQ(10, (v1.get<int>()));
  FATAL_EXPECT_TRUE((v2.is_of<int>()));
  FATAL_EXPECT_EQ(10, (v2.get<int>()));
}

FATAL_TEST(variant, move_assignment) {
  using VAR = test_variant<int, double, test_string>;

  VAR v1(allocator, 10);
  FATAL_EXPECT_TRUE(v1.is_of<int>());
  FATAL_EXPECT_EQ(10, (v1.get<int>()));

  VAR v2;
  FATAL_EXPECT_TRUE(v2.empty());

  v2 = std::move(v1);
  FATAL_EXPECT_TRUE(v1.empty());
  FATAL_EXPECT_TRUE(v2.is_of<int>());
  FATAL_EXPECT_EQ(10, (v2.get<int>()));
}

FATAL_TEST(variant, self_assignment) {
  using VAR = test_variant<int, double, test_string>;

  VAR v1(allocator, 10);
  auto p = std::addressof(v1);
  FATAL_EXPECT_TRUE(v1.is_of<int>());
  FATAL_EXPECT_EQ(10, (v1.get<int>()));

  *p = v1;
  FATAL_EXPECT_TRUE(v1.is_of<int>());
  FATAL_EXPECT_EQ(10, (v1.get<int>()));

  *p = std::move(v1);
  FATAL_EXPECT_TRUE(v1.is_of<int>());
  FATAL_EXPECT_EQ(10, (v1.get<int>()));
}

template <typename T, typename TStoragePolicy, typename ...Args>
T const &copyset_constget_helper(
  variant<TStoragePolicy, Args...> &v, T const &value
) {
  using type = typename std::remove_reference<T>::type;
  v.template set<type>(value);
  return static_cast<variant<TStoragePolicy, Args...> const &>(v)
    .variant<TStoragePolicy, Args...>::template get<type>();
}

FATAL_TEST(variant, copyset_get) {
  test_variant<int, test_string, double> v(allocator);
  FATAL_EXPECT_EQ(10, copyset_constget_helper<int>(v, 10));
  FATAL_EXPECT_EQ(
    "It's alive! ALIVE!",
    copyset_constget_helper<test_string>(
      v, test_string("It's alive! ALIVE!", allocator)
    )
  );
  FATAL_EXPECT_EQ(1.2, copyset_constget_helper<double>(v, 1.2));
}

template <typename T, typename TStoragePolicy, typename ...Args>
T &moveset_get_helper(variant<TStoragePolicy, Args...> &v, T &&value) {
  static_assert(
    !std::is_const<typename std::remove_reference<T>::type>::value,
    "T can't be const"
  );

  using type = typename std::remove_reference<T>::type;
  v.template set<type>(std::move(value));
  return v.variant<TStoragePolicy, Args...>::template get<type>();
}

FATAL_TEST(variant, moveset_get) {
  test_variant<int, test_string, double> v(allocator);
  FATAL_EXPECT_EQ(10, moveset_get_helper<int>(v, 10));
  FATAL_EXPECT_EQ(
    "It's alive! ALIVE!",
    moveset_get_helper<test_string>(
      v, test_string("It's alive! ALIVE!", allocator)
    )
  );
  FATAL_EXPECT_EQ(1.2, moveset_get_helper<double>(v, 1.2));
}

template <typename T, typename V, typename ...Args>
T const &emplace_constget_helper(V &v, Args &&...args) {
  using type = typename std::remove_reference<T>::type;
  v.template emplace<type>(std::forward<Args>(args)...);
  return static_cast<V const &>(v).V::template get<type>();
}

FATAL_TEST(variant, emplace_constget) {
  test_variant<int, test_string, double> v(allocator);
  FATAL_EXPECT_EQ(10, emplace_constget_helper<int>(v, 10));
  FATAL_EXPECT_EQ(
    "It's alive! ALIVE!",
    emplace_constget_helper<test_string>(
      v, test_string("It's alive! ALIVE!", allocator)
    )
  );
  FATAL_EXPECT_EQ(1.2, emplace_constget_helper<double>(v, 1.2));
}

FATAL_TEST(variant, try_get) {
  test_variant<int, double> v(allocator);
  FATAL_EXPECT_EQ(nullptr, v.try_get<int>());
  FATAL_EXPECT_EQ(nullptr, v.try_get<double>());

  v = 10;
  FATAL_EXPECT_NE(nullptr, v.try_get<int>());
  FATAL_EXPECT_EQ(10, *v.try_get<int>());
  FATAL_EXPECT_EQ(nullptr, v.try_get<double>());

  v = 5.0;
  FATAL_EXPECT_EQ(nullptr, v.try_get<int>());
  FATAL_EXPECT_NE(nullptr, v.try_get<double>());
  FATAL_EXPECT_EQ(5.0, *v.try_get<double>());
}

FATAL_TEST(variant, clear_empty) {
  test_variant<int, test_string, double> v(allocator);
  FATAL_EXPECT_TRUE(v.empty());
  FATAL_EXPECT_EQ(3, v.tag());

  v.clear();
  FATAL_EXPECT_TRUE(v.empty());
  FATAL_EXPECT_EQ(3, v.tag());

  v.set(10);
  FATAL_EXPECT_FALSE(v.empty());
  FATAL_EXPECT_EQ(0, v.tag());

  v.clear();
  FATAL_EXPECT_TRUE(v.empty());
  FATAL_EXPECT_EQ(3, v.tag());
}

FATAL_TEST(variant, swap) {
  test_variant<int, test_string> v(allocator, 10);
  test_variant<int, test_string> u(allocator, test_string("5.0", allocator));
  test_variant<int, test_string> e(allocator);
  FATAL_VLOG(1) << "initial";
  FATAL_EXPECT_FALSE(v.empty());
  FATAL_EXPECT_EQ(10, v.get<int>());
  FATAL_EXPECT_FALSE(u.empty());
  FATAL_EXPECT_EQ(test_string("5.0", allocator), u.get<test_string>());
  FATAL_EXPECT_TRUE(e.empty());

  FATAL_VLOG(1) << "v.swap(u);";
  v.swap(u);
  FATAL_EXPECT_FALSE(u.empty());
  FATAL_EXPECT_EQ(10, u.get<int>());
  FATAL_EXPECT_FALSE(v.empty());
  FATAL_EXPECT_EQ("5.0", v.get<test_string>());
  FATAL_EXPECT_TRUE(e.empty());

  FATAL_VLOG(1) << "v.swap(u);";
  v.swap(u);
  FATAL_EXPECT_FALSE(v.empty());
  FATAL_EXPECT_EQ(10, v.get<int>());
  FATAL_EXPECT_FALSE(u.empty());
  FATAL_EXPECT_EQ("5.0", u.get<test_string>());
  FATAL_EXPECT_TRUE(e.empty());

  FATAL_VLOG(1) << "u.swap(e);";
  u.swap(e);
  FATAL_EXPECT_FALSE(v.empty());
  FATAL_EXPECT_EQ(10, v.get<int>());
  FATAL_EXPECT_FALSE(e.empty());
  FATAL_EXPECT_EQ("5.0", e.get<test_string>());
  FATAL_EXPECT_TRUE(u.empty());

  FATAL_VLOG(1) << "e.swap(v);";
  e.swap(v);
  FATAL_EXPECT_FALSE(e.empty());
  FATAL_EXPECT_EQ(10, e.get<int>());
  FATAL_EXPECT_FALSE(v.empty());
  FATAL_EXPECT_EQ("5.0", v.get<test_string>());
  FATAL_EXPECT_TRUE(u.empty());
}

template <typename T>
struct visit_if_visitor {
  visit_if_visitor() = default;
  explicit visit_if_visitor(T expected): expected_(std::move(expected)) {}

  void operator ()(T const &actual) const {
    FATAL_ASSERT_FALSE(expected_.empty());
    FATAL_EXPECT_EQ(*expected_, actual);
  }

private:
  optional<T> const expected_;
};

template <typename T, typename TVariant, typename... Args>
void check_visit_if (TVariant &&variant, Args &&...args) {
  variant.template visit_if<transform_alias<std::is_same, T>::template apply>(
    visit_if_visitor<T>(std::forward<Args>(args)...)
  );
}

FATAL_TEST(variant, visit_if) {
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
      FATAL_EXPECT_EQ(expected, actual);
    }
  };

  template <typename U>
  struct comparer<U, false> {
    static void compare(T const &expected, U const &actual) {
      FATAL_VLOG(1) << "visited \"" << actual << "\" ["
        << type_str<U>() << ", expecting \""
        << expected << "\" [" << type_str<T>() << ']';
      FATAL_EXPECT_TRUE(false);
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
  void operator ()(U &&) { FATAL_EXPECT_TRUE(false); }
};

FATAL_TEST(variant, visit) {
  test_variant<int, test_string, double> v(allocator);
  v.visit(type_checker_visitor<void, true>());

  v = 10;
  v.visit(type_checker_visitor<int>(10));

  v = test_string(allocator);
  v.visit(type_checker_visitor<test_string>(test_string(allocator)));

  v = 5.0;
  v.visit(type_checker_visitor<double>(5.0));
}

FATAL_TEST(variant, tag) {
  test_variant<int, test_string, double> v(allocator);
  FATAL_EXPECT_EQ(3, v.tag());

  v.set(10);
  FATAL_EXPECT_EQ(0, v.tag());

  v.set(test_string(allocator));
  FATAL_EXPECT_EQ(1, v.tag());

  v.set(1.0);
  FATAL_EXPECT_EQ(2, v.tag());

  v.clear();
  FATAL_EXPECT_EQ(3, v.tag());
}

FATAL_TEST(variant, is_of) {
  test_variant<int, double> v(allocator);
  FATAL_EXPECT_FALSE(v.is_of<int>());
  FATAL_EXPECT_FALSE(v.is_of<double>());

  v = 10;
  FATAL_EXPECT_TRUE(v.is_of<int>());
  FATAL_EXPECT_FALSE(v.is_of<double>());

  v = 5.0;
  FATAL_EXPECT_FALSE(v.is_of<int>());
  FATAL_EXPECT_TRUE(v.is_of<double>());
}

FATAL_TEST(variant, is_supported) {
  using var = test_variant<int, double, int const *, test_string>;

  FATAL_EXPECT_TRUE((var::is_supported<int>()));
  FATAL_EXPECT_TRUE((var::is_supported<double>()));
  FATAL_EXPECT_TRUE((var::is_supported<int const *>()));
  FATAL_EXPECT_TRUE((var::is_supported<test_string>()));

  FATAL_EXPECT_FALSE((var::is_supported<int *>()));
  FATAL_EXPECT_FALSE((var::is_supported<long *>()));
  FATAL_EXPECT_FALSE((var::is_supported<test_vector<int>>()));
  FATAL_EXPECT_FALSE((var::is_supported<bool>()));
  FATAL_EXPECT_FALSE((var::is_supported<long>()));
  FATAL_EXPECT_FALSE((var::is_supported<short>()));
  FATAL_EXPECT_FALSE((var::is_supported<void>()));
  FATAL_EXPECT_FALSE((var::is_supported<void *>()));
}

FATAL_TEST(variant, operator_equals) {
  test_variant<int, double> const i10(allocator, 10);
  test_variant<int, double> const i5(allocator, 5);
  test_variant<int, double> const d10(allocator, 10.0);
  test_variant<int, double> const d5(allocator, 5.0);
  test_variant<int, double> const e(allocator);

  FATAL_EXPECT_TRUE(i10 == i10);
  FATAL_EXPECT_FALSE(i10 == i5);
  FATAL_EXPECT_FALSE(i10 == d10);
  FATAL_EXPECT_FALSE(d10 == i10);
  FATAL_EXPECT_TRUE(d10 == d10);
  FATAL_EXPECT_FALSE(d10 == d5);
  FATAL_EXPECT_TRUE(e == e);
  FATAL_EXPECT_FALSE(i10 == e);
  FATAL_EXPECT_FALSE(e == i10);
  FATAL_EXPECT_FALSE(d10 == e);
  FATAL_EXPECT_FALSE(e == d10);
}

FATAL_TEST(variant, operator_not_equals) {
  test_variant<int, double> const i10(allocator, 10);
  test_variant<int, double> const i5(allocator, 5);
  test_variant<int, double> const d10(allocator, 10.0);
  test_variant<int, double> const d5(allocator, 5.0);
  test_variant<int, double> const e(allocator);

  FATAL_EXPECT_FALSE(i10 != i10);
  FATAL_EXPECT_TRUE(i10 != i5);
  FATAL_EXPECT_TRUE(i10 != d10);
  FATAL_EXPECT_TRUE(d10 != i10);
  FATAL_EXPECT_FALSE(d10 != d10);
  FATAL_EXPECT_TRUE(d10 != d5);
  FATAL_EXPECT_FALSE(e != e);
  FATAL_EXPECT_TRUE(i10 != e);
  FATAL_EXPECT_TRUE(e != i10);
  FATAL_EXPECT_TRUE(d10 != e);
  FATAL_EXPECT_TRUE(e != d10);
}

FATAL_TEST(variant, operator_less_than) {
  test_variant<int, double> const i10(allocator, 10);
  test_variant<int, double> const i5(allocator, 5);
  test_variant<int, double> const d10(allocator, 10.0);
  test_variant<int, double> const d5(allocator, 5.0);
  test_variant<int, double> const e(allocator);

  FATAL_EXPECT_FALSE(i10 < i10);
  FATAL_EXPECT_FALSE(i10 < i5);
  FATAL_EXPECT_TRUE(i5 < i10);
  FATAL_EXPECT_TRUE(i10 < d10);
  FATAL_EXPECT_FALSE(d10 < i10);
  FATAL_EXPECT_FALSE(d10 < d10);
  FATAL_EXPECT_FALSE(d10 < d5);
  FATAL_EXPECT_TRUE(d5 < d10);
  FATAL_EXPECT_FALSE(e < e);
  FATAL_EXPECT_TRUE(i10 < e);
  FATAL_EXPECT_FALSE(e < i10);
  FATAL_EXPECT_TRUE(d10 < e);
  FATAL_EXPECT_FALSE(e < d10);
}

FATAL_TEST(variant, operator_less_than_or_equal) {
  test_variant<int, double> const i10(allocator, 10);
  test_variant<int, double> const i5(allocator, 5);
  test_variant<int, double> const d10(allocator, 10.0);
  test_variant<int, double> const d5(allocator, 5.0);
  test_variant<int, double> const e(allocator);

  FATAL_EXPECT_TRUE(i10 <= i10);
  FATAL_EXPECT_FALSE(i10 <= i5);
  FATAL_EXPECT_TRUE(i5 <= i10);
  FATAL_EXPECT_TRUE(i10 <= d10);
  FATAL_EXPECT_FALSE(d10 <= i10);
  FATAL_EXPECT_TRUE(d10 <= d10);
  FATAL_EXPECT_FALSE(d10 <= d5);
  FATAL_EXPECT_TRUE(d5 <= d10);
  FATAL_EXPECT_TRUE(e <= e);
  FATAL_EXPECT_TRUE(i10 <= e);
  FATAL_EXPECT_FALSE(e <= i10);
  FATAL_EXPECT_TRUE(d10 <= e);
  FATAL_EXPECT_FALSE(e <= d10);
}

FATAL_TEST(variant, operator_greater_than) {
  test_variant<int, double> const i10(allocator, 10);
  test_variant<int, double> const i5(allocator, 5);
  test_variant<int, double> const d10(allocator, 10.0);
  test_variant<int, double> const d5(allocator, 5.0);
  test_variant<int, double> const e(allocator);

  FATAL_EXPECT_FALSE(i10 > i10);
  FATAL_EXPECT_TRUE(i10 > i5);
  FATAL_EXPECT_FALSE(i5 > i10);
  FATAL_EXPECT_FALSE(i10 > d10);
  FATAL_EXPECT_TRUE(d10 > i10);
  FATAL_EXPECT_FALSE(d10 > d10);
  FATAL_EXPECT_TRUE(d10 > d5);
  FATAL_EXPECT_FALSE(d5 > d10);
  FATAL_EXPECT_FALSE(e > e);
  FATAL_EXPECT_FALSE(i10 > e);
  FATAL_EXPECT_TRUE(e > i10);
  FATAL_EXPECT_FALSE(d10 > e);
  FATAL_EXPECT_TRUE(e > d10);
}

FATAL_TEST(variant, operator_greater_than_or_equal) {
  test_variant<int, double> const i10(allocator, 10);
  test_variant<int, double> const i5(allocator, 5);
  test_variant<int, double> const d10(allocator, 10.0);
  test_variant<int, double> const d5(allocator, 5.0);
  test_variant<int, double> const e(allocator);

  FATAL_EXPECT_TRUE(i10 >= i10);
  FATAL_EXPECT_TRUE(i10 >= i5);
  FATAL_EXPECT_FALSE(i5 >= i10);
  FATAL_EXPECT_FALSE(i10 >= d10);
  FATAL_EXPECT_TRUE(d10 >= i10);
  FATAL_EXPECT_TRUE(d10 >= d10);
  FATAL_EXPECT_TRUE(d10 >= d5);
  FATAL_EXPECT_FALSE(d5 >= d10);
  FATAL_EXPECT_TRUE(e >= e);
  FATAL_EXPECT_FALSE(i10 >= e);
  FATAL_EXPECT_TRUE(e >= i10);
  FATAL_EXPECT_FALSE(d10 >= e);
  FATAL_EXPECT_TRUE(e >= d10);
}

FATAL_TEST(variant, operator_copy_assignment_clvref) {
  test_variant<int, double> const idce;
  FATAL_EXPECT_TRUE(idce.empty());

  test_variant<int, double> const idc50_1(allocator, 50.1);
  FATAL_EXPECT_EQ(50.1, idc50_1.get<double>());

  test_variant<int, double> const idc10(allocator, 10);
  FATAL_EXPECT_EQ(10, idc10.get<int>());

  test_variant<int> const ic11(allocator, 11);
  FATAL_EXPECT_EQ(11, ic11.get<int>());

  test_variant<int> const ice;
  FATAL_EXPECT_TRUE(ice.empty());

  test_variant<int, double> id(allocator, 7);
  FATAL_EXPECT_EQ(7, id.get<int>());

  id = idce;
  FATAL_EXPECT_TRUE(id.empty());

  id = idc50_1;
  FATAL_EXPECT_EQ(50.1, id.get<double>());

  id = idc10;
  FATAL_EXPECT_EQ(10, id.get<int>());

  id = ic11;
  FATAL_EXPECT_EQ(11, id.get<int>());

  id = ice;
  FATAL_EXPECT_TRUE(id.empty());

  test_variant<int> i(allocator, 9);
  FATAL_EXPECT_EQ(9, i.get<int>());

  i = idce;
  FATAL_EXPECT_TRUE(i.empty());

  i = idc10;
  FATAL_EXPECT_EQ(10, i.get<int>());

  i = ic11;
  FATAL_EXPECT_EQ(11, i.get<int>());

  i = ice;
  FATAL_EXPECT_TRUE(i.empty());
}

FATAL_TEST(variant, operator_copy_assignment_lvref) {
  test_variant<int, double> ide;
  FATAL_EXPECT_TRUE(ide.empty());

  test_variant<int, double> id6_7(allocator, 6.7);
  FATAL_EXPECT_EQ(6.7, id6_7.get<double>());

  test_variant<int, double> id5(allocator, 5);
  FATAL_EXPECT_EQ(5, id5.get<int>());

  test_variant<int> i4(allocator, 4);
  FATAL_EXPECT_EQ(4, i4.get<int>());

  test_variant<int> ie;
  FATAL_EXPECT_TRUE(ie.empty());

  test_variant<int, double> id(allocator, 7);
  FATAL_EXPECT_EQ(7, id.get<int>());

  id = ide;
  FATAL_EXPECT_TRUE(id.empty());

  id = id6_7;
  FATAL_EXPECT_EQ(6.7, id.get<double>());

  id = id5;
  FATAL_EXPECT_EQ(5, id.get<int>());

  id = i4;
  FATAL_EXPECT_EQ(4, id.get<int>());

  id = ie;
  FATAL_EXPECT_TRUE(id.empty());

  test_variant<int> i(allocator, 9);
  FATAL_EXPECT_EQ(9, i.get<int>());

  i = ide;
  FATAL_EXPECT_TRUE(i.empty());

  i = id5;
  FATAL_EXPECT_EQ(5, i.get<int>());

  i = i4;
  FATAL_EXPECT_EQ(4, i.get<int>());

  i = ie;
  FATAL_EXPECT_TRUE(i.empty());
}

FATAL_TEST(variant, operator_copy_assignment_rvref) {
  test_variant<int, double> ide;
  FATAL_EXPECT_TRUE(ide.empty());

  test_variant<int, double> id6_7(allocator, 6.7);
  FATAL_EXPECT_EQ(6.7, id6_7.get<double>());

  test_variant<int, double> id5(allocator, 5);
  FATAL_EXPECT_EQ(5, id5.get<int>());

  test_variant<int> i4(allocator, 4);
  FATAL_EXPECT_EQ(4, i4.get<int>());

  test_variant<int, double> id3(3);
  FATAL_EXPECT_EQ(3, id3.get<int>());

  test_variant<int> i2(2);
  FATAL_EXPECT_EQ(2, i2.get<int>());

  test_variant<int> ie;
  FATAL_EXPECT_TRUE(ie.empty());

  test_variant<int, double> id(allocator, 7);
  FATAL_EXPECT_EQ(7, id.get<int>());

  id = std::move(ide);
  FATAL_EXPECT_TRUE(id.empty());
  FATAL_EXPECT_TRUE(ide.empty());

  id = std::move(id6_7);
  FATAL_EXPECT_EQ(6.7, id.get<double>());
  FATAL_EXPECT_TRUE(id6_7.empty());

  id = std::move(id5);
  FATAL_EXPECT_EQ(5, id.get<int>());
  FATAL_EXPECT_TRUE(id5.empty());

  id = std::move(i4);
  FATAL_EXPECT_EQ(4, id.get<int>());
  FATAL_EXPECT_TRUE(i4.empty());

  id = std::move(ie);
  FATAL_EXPECT_TRUE(id.empty());
  FATAL_EXPECT_TRUE(ie.empty());

  test_variant<int> i(allocator, 9);
  FATAL_EXPECT_EQ(9, i.get<int>());

  i = std::move(ide);
  FATAL_EXPECT_TRUE(i.empty());
  FATAL_EXPECT_TRUE(ide.empty());

  i = std::move(id3);
  FATAL_EXPECT_EQ(3, i.get<int>());
  FATAL_EXPECT_TRUE(id3.empty());

  i = std::move(i2);
  FATAL_EXPECT_EQ(2, i.get<int>());
  FATAL_EXPECT_TRUE(i2.empty());

  i = std::move(ie);
  FATAL_EXPECT_TRUE(i.empty());
  FATAL_EXPECT_TRUE(ie.empty());
}

FATAL_TEST(variant, operator_copy_assignment_heterogeneous_policy_clvref) {
  auto_variant<int, double> const idce;
  FATAL_EXPECT_TRUE(idce.empty());

  auto_variant<int, double> const idc50_1(50.1);
  FATAL_EXPECT_EQ(50.1, idc50_1.get<double>());

  auto_variant<int, double> const idc10(10);
  FATAL_EXPECT_EQ(10, idc10.get<int>());

  auto_variant<int> const ic11(11);
  FATAL_EXPECT_EQ(11, ic11.get<int>());

  auto_variant<int> const ice;
  FATAL_EXPECT_TRUE(ice.empty());

  auto_variant<int, double> id(7);
  FATAL_EXPECT_EQ(7, id.get<int>());

  id = idce;
  FATAL_EXPECT_TRUE(id.empty());

  id = idc50_1;
  FATAL_EXPECT_EQ(50.1, id.get<double>());

  id = idc10;
  FATAL_EXPECT_EQ(10, id.get<int>());

  id = ic11;
  FATAL_EXPECT_EQ(11, id.get<int>());

  id = ice;
  FATAL_EXPECT_TRUE(id.empty());

  test_variant<int> i(allocator, 9);
  FATAL_EXPECT_EQ(9, i.get<int>());

  i = idce;
  FATAL_EXPECT_TRUE(i.empty());

  i = idc10;
  FATAL_EXPECT_EQ(10, i.get<int>());

  i = ic11;
  FATAL_EXPECT_EQ(11, i.get<int>());

  i = ice;
  FATAL_EXPECT_TRUE(i.empty());
}

FATAL_TEST(variant, operator_copy_assignment_heterogeneous_policy_lvref) {
  auto_variant<int, double> ide;
  FATAL_EXPECT_TRUE(ide.empty());

  auto_variant<int, double> id6_7(6.7);
  FATAL_EXPECT_EQ(6.7, id6_7.get<double>());

  auto_variant<int, double> id5(5);
  FATAL_EXPECT_EQ(5, id5.get<int>());

  auto_variant<int> i4(4);
  FATAL_EXPECT_EQ(4, i4.get<int>());

  auto_variant<int> ie;
  FATAL_EXPECT_TRUE(ie.empty());

  auto_variant<int, double> id(7);
  FATAL_EXPECT_EQ(7, id.get<int>());

  id = ide;
  FATAL_EXPECT_TRUE(id.empty());

  id = id6_7;
  FATAL_EXPECT_EQ(6.7, id.get<double>());

  id = id5;
  FATAL_EXPECT_EQ(5, id.get<int>());

  id = i4;
  FATAL_EXPECT_EQ(4, id.get<int>());

  id = ie;
  FATAL_EXPECT_TRUE(id.empty());

  test_variant<int> i(allocator, 9);
  FATAL_EXPECT_EQ(9, i.get<int>());

  i = ide;
  FATAL_EXPECT_TRUE(i.empty());

  i = id5;
  FATAL_EXPECT_EQ(5, i.get<int>());

  i = i4;
  FATAL_EXPECT_EQ(4, i.get<int>());

  i = ie;
  FATAL_EXPECT_TRUE(i.empty());
}

FATAL_TEST(variant, operator_copy_assignment_heterogeneous_policy_rvref) {
  auto_variant<int, double> ide;
  FATAL_EXPECT_TRUE(ide.empty());

  auto_variant<int, double> id6_7(6.7);
  FATAL_EXPECT_EQ(6.7, id6_7.get<double>());

  auto_variant<int, double> id5(5);
  FATAL_EXPECT_EQ(5, id5.get<int>());

  auto_variant<int> i4(4);
  FATAL_EXPECT_EQ(4, i4.get<int>());

  auto_variant<int, double> id3(3);
  FATAL_EXPECT_EQ(3, id3.get<int>());

  auto_variant<int> i2(2);
  FATAL_EXPECT_EQ(2, i2.get<int>());

  auto_variant<int, double> id(7);
  FATAL_EXPECT_EQ(7, id.get<int>());

  auto_variant<int> ie;
  FATAL_EXPECT_TRUE(ie.empty());

  id = std::move(ide);
  FATAL_EXPECT_TRUE(id.empty());
  FATAL_EXPECT_TRUE(ide.empty());

  id = std::move(id6_7);
  FATAL_EXPECT_EQ(6.7, id.get<double>());
  FATAL_EXPECT_TRUE(id6_7.empty());

  id = std::move(id5);
  FATAL_EXPECT_EQ(5, id.get<int>());
  FATAL_EXPECT_TRUE(id5.empty());

  id = std::move(i4);
  FATAL_EXPECT_EQ(4, id.get<int>());
  FATAL_EXPECT_TRUE(i4.empty());

  id = std::move(ie);
  FATAL_EXPECT_TRUE(id.empty());
  FATAL_EXPECT_TRUE(ie.empty());

  test_variant<int> i(allocator, 9);
  FATAL_EXPECT_EQ(9, i.get<int>());

  i = std::move(ide);
  FATAL_EXPECT_TRUE(i.empty());
  FATAL_EXPECT_TRUE(ide.empty());

  i = std::move(id3);
  FATAL_EXPECT_EQ(3, i.get<int>());
  FATAL_EXPECT_TRUE(id3.empty());

  i = std::move(i2);
  FATAL_EXPECT_EQ(2, i.get<int>());
  FATAL_EXPECT_TRUE(i2.empty());

  i = std::move(ie);
  FATAL_EXPECT_TRUE(i.empty());
  FATAL_EXPECT_TRUE(ie.empty());
}

template <typename Expected, typename ...Args>
void check_type_tag_size() {
  using var = variant<default_storage_policy<>, Args...>;
  if (!std::is_same<Expected, typename var::type_tag>::value) {
    FATAL_VLOG(1) << "expected \"" << type_str<Expected>()
      << "\", got \"" << type_str<typename var::type_tag>()
      << "\" for " << sizeof...(Args) << " types";
    FATAL_EXPECT_TRUE(false);
  }
}

template <int> struct X {};
FATAL_TEST(variant, type_tag_size) {
#define TAG_SIZE_TEST(Expected,...) \
  check_type_tag_size<Expected, __VA_ARGS__>()

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

FATAL_TEST(variant, id) {
  using var = variant<
    default_storage_policy<>,
    int,
    short,
    test_string,
    test_vector<int>,
    test_vector<double>,
    test_vector<test_string>,
    std::map<test_string, test_string>
  >;

  FATAL_EXPECT_EQ(var::tag<int>(), 0);
  FATAL_EXPECT_EQ(var::tag<short>(), 1);
  FATAL_EXPECT_EQ(var::tag<test_string>(), 2);
  FATAL_EXPECT_EQ(var::tag<test_vector<int>>(), 3);
  FATAL_EXPECT_EQ(var::tag<test_vector<double>>(), 4);
  FATAL_EXPECT_EQ(var::tag<test_vector<test_string>>(), 5);
  using map_string_string = std::map<test_string, test_string>;
  FATAL_EXPECT_EQ(var::tag<map_string_string>(), 6);
  using map_string_int = std::map<test_string, int>;
  FATAL_EXPECT_EQ(var::tag<map_string_int>(), 7);
  using map_short_bool = std::map<short, bool>;
  FATAL_EXPECT_EQ(var::tag<map_short_bool>(), 7);
  FATAL_EXPECT_EQ(var::tag<bool>(), 7);
  FATAL_EXPECT_EQ(var::tag<unsigned int>(), 7);
  FATAL_EXPECT_EQ(var::tag<unsigned short>(), 7);
  FATAL_EXPECT_EQ(var::no_tag(), 7);
}

FATAL_TEST(variant, arena_allocator) {
  test_allocator<int> arena;
  checked_allocator<test_allocator<int>> alloc{arena};

  variant<
    default_storage_policy<decltype(alloc), dynamic_allocation_policy>,
    int, double, std::string, std::vector<int>
  > v(alloc);
  FATAL_EXPECT_TRUE(v.empty());

  v = std::string("hello, world");
  FATAL_EXPECT_FALSE(v.empty());
  FATAL_EXPECT_EQ("hello, world", v.get<std::string>());

  v = 10;
  FATAL_EXPECT_FALSE(v.empty());
  FATAL_EXPECT_EQ(10, v.get<int>());

  v = 5.6;
  FATAL_EXPECT_FALSE(v.empty());
  FATAL_EXPECT_EQ(5.6, v.get<double>());

  v = std::vector<int>{1, 2, 3, 4, 5};
  FATAL_EXPECT_FALSE(v.empty());
  FATAL_EXPECT_EQ(5, v.get<std::vector<int>>().size());
  for (int i = 1; i <= 5; ++i) {
    FATAL_EXPECT_EQ(i, v.get<std::vector<int>>()[unsigned_cast(i - 1)]);
  }

  v.clear();
  FATAL_EXPECT_TRUE(v.empty());
}

template <typename StoragePolicy = test_policy> struct t_nested_vector;
template <typename StoragePolicy = test_policy>
using t_nested_variant = variant<StoragePolicy, int, t_nested_vector<>>;
template <typename StoragePolicy>
struct t_nested_vector: public test_vector<t_nested_variant<StoragePolicy>> {};

FATAL_TEST(variant, templated_nested_variant) {
  t_nested_variant<> v(allocator);
  FATAL_EXPECT_TRUE(v.empty());

  v = 10;
  FATAL_EXPECT_EQ(10, v.get<int>());

  t_nested_vector<> x;
  x.emplace_back(allocator, 5);
  v = std::move(x);
  FATAL_EXPECT_EQ(1, v.get<decltype(x)>().size());
  FATAL_EXPECT_EQ(5, v.get<decltype(x)>()[0].get<int>());
}

struct nested_vector;
using nested_variant = variant<test_policy, int, nested_vector>;
struct nested_vector:
  public test_vector<nested_variant>
{
  nested_vector() = default;
  nested_vector(std::initializer_list<nested_variant> list):
    test_vector<nested_variant>(list, allocator)
  {}
};

FATAL_TEST(variant, nested_variant) {
  nested_variant v(allocator);
  FATAL_EXPECT_TRUE(v.empty());

  v = 10;
  FATAL_EXPECT_EQ(10, v.get<int>());

  nested_vector x;
  x.emplace_back(allocator, 5);
  v = std::move(x);
  FATAL_EXPECT_EQ(1, v.get<decltype(x)>().size());
  FATAL_EXPECT_EQ(5, v.get<decltype(x)>()[0].get<int>());
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
    FATAL_VLOG(1) << "push" << " expected.size() = " << expected_.size()
      << " with front = " << expected_.front();
    FATAL_ASSERT_NE(0, expected_.size());
    FATAL_EXPECT_EQ(-1, expected_.front());
    expected_.pop();

    for (auto &i: v) {
      i.visit(*this);
    }

    FATAL_VLOG(1) << "pop" << " expected.size() = " << expected_.size()
      << " with front = " << expected_.front();
    FATAL_ASSERT_NE(0, expected_.size());
    FATAL_EXPECT_EQ(-2, expected_.front());
    expected_.pop();
  }

  void operator ()(int i) {
    FATAL_VLOG(1) << "int " << i << " expected.size() = " << expected_.size()
      << " with front = " << expected_.front();
    FATAL_ASSERT_NE(0, expected_.size());
    FATAL_EXPECT_EQ(expected_.front(), i);
    expected_.pop();
  }

  bool empty() const { return expected_.empty(); }
  std::size_t size() const { return expected_.size(); }

private:
  std::queue<int> expected_;
};

FATAL_TEST(variant, nested_visitor_0) {
  nested_visitor visitor{5};
  nested_variant(allocator, 5).visit(visitor);
  FATAL_EXPECT_TRUE(visitor.empty());
}

FATAL_TEST(variant, nested_visitor_1) {
  nested_visitor visitor{-1, 9, -2};
  nested_variant(
    allocator,
    nested_vector{
      nested_variant(allocator, 9)
    }
  ).visit(visitor);
  FATAL_EXPECT_TRUE(visitor.empty());
}

FATAL_TEST(variant, nested_visitor_5) {
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
  FATAL_EXPECT_TRUE(visitor.empty());
}

template <typename T, T returnValue>
struct visitor_that_returns {
  visitor_that_returns() {}

  template <typename U>
  T operator()(U&&) { return returnValue; }

  template <typename U>
  T operator()(U&&) const { return returnValue; }
};

FATAL_TEST(variant, visitor_that_return) {
  using var = test_variant<int, double>;

  using visitor_type = visitor_that_returns<int, 99>;
  visitor_type visitor;
  visitor_type const cvisitor;

  FATAL_EXPECT_THROW(std::logic_error) {
    visit<int>(var(), visitor_type());
  };
  FATAL_EXPECT_EQ(111, (visit_def<int>(var(), visitor_type(), 111)));
  FATAL_EXPECT_THROW(std::logic_error) {
    visit<int>(var(), visitor);
  };
  FATAL_EXPECT_EQ(111, (visit_def<int>(var(), visitor, 111)));
  FATAL_EXPECT_THROW(std::logic_error) {
    visit<int>(var(), cvisitor);
  };
  FATAL_EXPECT_EQ(111, (visit_def<int>(var(), cvisitor, 111)));

  FATAL_EXPECT_EQ(99, (visit<int>(var(allocator, 10), visitor_type())));
  FATAL_EXPECT_EQ(99, (visit<int>(var(allocator, 10), visitor)));
  FATAL_EXPECT_EQ(99, (visit<int>(var(allocator, 10), cvisitor)));

  FATAL_EXPECT_EQ(99, (visit<int>(var(allocator, 3.2), visitor_type())));
  FATAL_EXPECT_EQ(99, (visit<int>(var(allocator, 3.2), visitor)));
  FATAL_EXPECT_EQ(99, (visit<int>(var(allocator, 3.2), cvisitor)));

  var const c(allocator, 123);

  FATAL_EXPECT_TRUE(c.is_of<int>());
  FATAL_EXPECT_EQ(99, (visit<int>(c, visitor_type())));
  FATAL_EXPECT_EQ(99, (visit_def<int>(c, visitor_type(), 111)));
  FATAL_EXPECT_EQ(99, (visit<int>(c, visitor)));
  FATAL_EXPECT_EQ(99, (visit_def<int>(c, visitor, 111)));
  FATAL_EXPECT_EQ(99, (visit<int>(c, cvisitor)));
  FATAL_EXPECT_EQ(99, (visit_def<int>(c, cvisitor, 111)));

  var v(allocator);

  FATAL_EXPECT_TRUE(v.empty());
  FATAL_EXPECT_THROW(std::logic_error) {
    visit<int>(v, visitor_type());
  };
  FATAL_EXPECT_EQ(111, (visit_def<int>(v, visitor_type(), 111)));
  FATAL_EXPECT_THROW(std::logic_error) {
    visit<int>(v, visitor);
  };
  FATAL_EXPECT_EQ(111, (visit_def<int>(v, visitor, 111)));
  FATAL_EXPECT_THROW(std::logic_error) {
    visit<int>(v, cvisitor);
  };
  FATAL_EXPECT_EQ(111, (visit_def<int>(v, cvisitor, 111)));

  v = 10;
  FATAL_EXPECT_TRUE(v.is_of<int>());
  FATAL_EXPECT_EQ(99, (visit<int>(v, visitor_type())));
  FATAL_EXPECT_EQ(99, (visit<int>(v, visitor)));
  FATAL_EXPECT_EQ(99, (visit<int>(v, cvisitor)));

  v = 3.2;
  FATAL_EXPECT_TRUE(v.is_of<double>());
  FATAL_EXPECT_EQ(99, (visit<int>(v, visitor_type())));
  FATAL_EXPECT_EQ(99, (visit<int>(v, visitor)));
  FATAL_EXPECT_EQ(99, (visit<int>(v, cvisitor)));
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

FATAL_TEST(variant, visitor_that_return_on_arena_variant) {
  test_allocator<int> arena;
  checked_allocator<test_allocator<int>> alloc{arena};

  using var = variant<
    default_storage_policy<decltype(allocator), dynamic_allocation_policy>,
    int, double, std::string
  >;
  using visitor_type = visitor_that_returns_type_tag<var>;

  var v(allocator);
  FATAL_EXPECT_TRUE(v.empty());
  FATAL_EXPECT_THROW(std::logic_error) {
    visit<var::type_tag>(v, visitor_type());
  };

  v = 10;
  FATAL_EXPECT_TRUE(v.is_of<int>());
  FATAL_EXPECT_EQ((var::tag<int>()), (visit<var::type_tag>(v, visitor_type())));

  v = 5.6;
  FATAL_EXPECT_TRUE(v.is_of<double>());
  FATAL_EXPECT_EQ(
    (var::tag<double>()),
    (visit<var::type_tag>(v, visitor_type()))
  );

  v = std::string();
  FATAL_EXPECT_TRUE(v.is_of<std::string>());
  FATAL_EXPECT_EQ(
    (var::tag<std::string>()),
    (visit<var::type_tag>(v, visitor_type()))
  );
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

FATAL_TEST(variant, visitor_with_additional_parameters) {
  using var = test_variant<int, double>;

  using visitor_type = visitor_with_additional_parameters<double>;
  visitor_type visitor;

  FATAL_EXPECT_THROW(std::logic_error) {
    visit<double>(var(), visitor_type(), 1.9, 54);
  };
  FATAL_EXPECT_EQ(
    111,
    (visit_def<double>(var(), visitor_type(), 111, 1.9, 54))
  );
  FATAL_EXPECT_THROW(std::logic_error) {
    visit<double>(var(), visitor, 1.9, 54);
  };
  FATAL_EXPECT_EQ(111, (visit_def<double>(var(), visitor, 111)));
  FATAL_EXPECT_EQ(
    10 + 1.9 + 54,
    (visit<double>(var(allocator, 10), visitor_type(), 1.9, 54))
  );
  FATAL_EXPECT_EQ(
    10 + 1.9 + 54,
    (visit<double>(var(allocator, 10), visitor, 1.9, 54))
  );
  FATAL_EXPECT_EQ(
    3.2 + 1.9 + 54,
    (visit<double>(var(allocator, 3.2), visitor_type(), 1.9, 54))
  );
  FATAL_EXPECT_EQ(
    3.2 + 1.9 + 54,
    (visit<double>(var(allocator, 3.2), visitor, 1.9, 54))
  );

  var const c(allocator, 99);

  FATAL_EXPECT_TRUE(c.is_of<int>());
  FATAL_EXPECT_EQ(99 + 1.9 + 54, (visit<double>(c, visitor_type(), 1.9, 54)));
  FATAL_EXPECT_EQ(99 + 1.9 + 54, (visit<double>(c, visitor, 1.9, 54)));
  FATAL_EXPECT_EQ(
    99 + 1.9 + 54,
    (visit_def<double>(c, visitor_type(), 111, 1.9, 54))
  );
  FATAL_EXPECT_EQ(99 + 1.9 + 54, (visit_def<double>(c, visitor, 111, 1.9, 54)));

  var v(allocator);

  FATAL_EXPECT_TRUE(v.empty());
  FATAL_EXPECT_THROW(std::logic_error) {
    visit<double>(v, visitor_type());
  };
  FATAL_EXPECT_EQ(111, (visit_def<double>(v, visitor_type(), 111)));
  FATAL_EXPECT_THROW(std::logic_error) {
    visit<double>(v, visitor);
  };
  FATAL_EXPECT_EQ(111, (visit_def<double>(v, visitor, 111)));

  v = 10;
  FATAL_EXPECT_TRUE(v.is_of<int>());
  FATAL_EXPECT_EQ(10 + 1.9 + 54, (visit<double>(v, visitor_type(), 1.9, 54)));
  FATAL_EXPECT_EQ(10 + 1.9 + 54, (visit<double>(v, visitor, 1.9, 54)));

  v = 3.2;
  FATAL_EXPECT_TRUE(v.is_of<double>());
  FATAL_EXPECT_EQ(3.2 + 1.9 + 54, (visit<double>(v, visitor_type(), 1.9, 54)));
  FATAL_EXPECT_EQ(3.2 + 1.9 + 54, (visit<double>(v, visitor, 1.9, 54)));
}

FATAL_TEST(variant, variant_vector) {
  using var = test_variant<int, double, test_string>;
  test_vector<var> v(allocator);

  v.reserve(1);
  FATAL_EXPECT_EQ(0, v.size());

  v.push_back(var(allocator, 10));
  FATAL_EXPECT_EQ(1, v.size());
  FATAL_EXPECT_TRUE(v[0].is_of<int>());
  FATAL_EXPECT_EQ(10, (v[0].get<int>()));

  v.emplace_back(allocator, 5.6);
  FATAL_EXPECT_EQ(2, v.size());
  FATAL_EXPECT_TRUE(v[0].is_of<int>());
  FATAL_EXPECT_EQ(10, (v[0].get<int>()));
  FATAL_EXPECT_TRUE(v[1].is_of<double>());
  FATAL_EXPECT_EQ(5.6, (v[1].get<double>()));

  v.resize(3);
  FATAL_EXPECT_EQ(3, v.size());
  FATAL_EXPECT_TRUE(v[0].is_of<int>());
  FATAL_EXPECT_EQ(10, (v[0].get<int>()));
  FATAL_EXPECT_TRUE(v[1].is_of<double>());
  FATAL_EXPECT_EQ(5.6, (v[1].get<double>()));
  FATAL_EXPECT_TRUE(v[2].empty());

  v[2] = var(allocator, test_string("HELLO, WORLD", allocator));
  FATAL_EXPECT_EQ(3, v.size());
  FATAL_EXPECT_TRUE(v[0].is_of<int>());
  FATAL_EXPECT_EQ(10, (v[0].get<int>()));
  FATAL_EXPECT_TRUE(v[1].is_of<double>());
  FATAL_EXPECT_EQ(5.6, (v[1].get<double>()));
  FATAL_EXPECT_TRUE(v[2].is_of<test_string>());
  FATAL_EXPECT_EQ(
    test_string("HELLO, WORLD", allocator),
    (v[2].get<test_string>())
  );

  v.resize(4);
  FATAL_EXPECT_EQ(4, v.size());
  FATAL_EXPECT_TRUE(v[0].is_of<int>());
  FATAL_EXPECT_EQ(10, (v[0].get<int>()));
  FATAL_EXPECT_TRUE(v[1].is_of<double>());
  FATAL_EXPECT_EQ(5.6, (v[1].get<double>()));
  FATAL_EXPECT_TRUE(v[2].is_of<test_string>());
  FATAL_EXPECT_EQ(
    test_string("HELLO, WORLD", allocator),
    (v[2].get<test_string>())
  );
  FATAL_EXPECT_TRUE(v[3].empty());

  var const t(allocator, test_string("THIS IS A TEST", allocator));
  v[3] = t;
  FATAL_EXPECT_EQ(4, v.size());
  FATAL_EXPECT_TRUE(v[0].is_of<int>());
  FATAL_EXPECT_EQ(10, (v[0].get<int>()));
  FATAL_EXPECT_TRUE(v[1].is_of<double>());
  FATAL_EXPECT_EQ(5.6, (v[1].get<double>()));
  FATAL_EXPECT_TRUE(v[2].is_of<test_string>());
  FATAL_EXPECT_EQ(
    test_string("HELLO, WORLD", allocator),
    (v[2].get<test_string>())
  );
  FATAL_EXPECT_EQ(t.tag(), v[3].tag());
  FATAL_EXPECT_EQ((t.get<test_string>()), (v[3].get<test_string>()));
}

FATAL_TEST(variant, variant_map_as_value) {
  using var = test_variant<int, double, test_string>;
  std::map<test_string, var> m;
  using pair_type = decltype(m)::value_type;

  m.insert(pair_type("int", var(allocator, 10)));
  m.insert(pair_type("double", var(allocator, 5.6)));
  m["string"] = var(allocator, test_string("HELLO, WORLD", allocator));
  var const t(allocator, test_string("THIS IS A TEST", allocator));
  m["t"] = t;

  FATAL_EXPECT_EQ(4, m.size());

  for (auto const &i: m) {
    if (i.first == "int") {
      FATAL_EXPECT_TRUE(i.second.is_of<int>());
      FATAL_EXPECT_EQ(10, (i.second.get<int>()));
    } else if (i.first == "double") {
      FATAL_EXPECT_TRUE(i.second.is_of<double>());
      FATAL_EXPECT_EQ(5.6, (i.second.get<double>()));
    } else if (i.first == "string") {
      FATAL_EXPECT_TRUE(i.second.is_of<test_string>());
      FATAL_EXPECT_EQ(
        test_string("HELLO, WORLD", allocator),
        (i.second.get<test_string>())
      );
    } else if (i.first == "t") {
      FATAL_EXPECT_EQ(t.tag(), i.second.tag());
      FATAL_EXPECT_EQ((t.get<test_string>()), (i.second.get<test_string>()));
    } else {
      FATAL_EXPECT_TRUE(false);
    }
  }
}

FATAL_TEST(variant, variant_map_as_key_value) {
  using var = test_variant<int, double, test_string>;
  std::map<var, var> m;
  using pair_type = decltype(m)::value_type;

  m.insert(std::make_pair(var(allocator, 10), var(allocator, 5.6)));
  var const t(allocator, test_string("THIS IS A TEST", allocator));
  m.insert(
    pair_type(var(allocator, test_string("HELLO, WORLD", allocator)), t)
  );

  FATAL_EXPECT_EQ(2, m.size());

  for (auto const &i: m) {
    switch (i.first.tag()) {
      case var::tag<int>():
        FATAL_EXPECT_EQ(10, (i.first.get<int>()));
        FATAL_EXPECT_TRUE(i.second.is_of<double>());
        FATAL_EXPECT_EQ(5.6, (i.second.get<double>()));
        break;

      case var::tag<test_string>():
        FATAL_EXPECT_EQ(
          test_string("HELLO, WORLD", allocator),
          (i.first.get<test_string>())
        );
        FATAL_EXPECT_EQ(t.tag(), i.second.tag());
        FATAL_EXPECT_EQ((t.get<test_string>()), (i.second.get<test_string>()));
        break;

      default:
        FATAL_EXPECT_TRUE(false);
    }
  }

  var const k1(allocator, 10);
  var const k2(allocator, test_string("HELLO, WORLD", allocator));

  auto v1 = m.find(k1);
  FATAL_ASSERT_NE(v1, m.end());
  FATAL_EXPECT_TRUE(v1->second.is_of<double>());
  FATAL_EXPECT_EQ(5.6, (v1->second.get<double>()));

  auto v2 = m.find(var(allocator, test_string("HELLO, WORLD", allocator)));
  FATAL_ASSERT_NE(v2, m.end());
  FATAL_EXPECT_EQ(t.tag(), v2->second.tag());
  FATAL_EXPECT_EQ((t.get<test_string>()), (v2->second.get<test_string>()));

  m[var(allocator, 10)] = var(
    allocator,
    test_string("so you didn't misspell cat prophet, my bad", allocator)
  );
  m[k2] = var(allocator, 456789);

  for (auto const &i: m) {
    switch (i.first.tag()) {
      case var::tag<int>():
        FATAL_EXPECT_EQ(10, (i.first.get<int>()));
        FATAL_EXPECT_TRUE(i.second.is_of<test_string>());
        FATAL_EXPECT_EQ(
          "so you didn't misspell cat prophet, my bad",
          (i.second.get<test_string>())
        );
        break;

      case var::tag<test_string>():
        FATAL_EXPECT_EQ(
          test_string("HELLO, WORLD", allocator),
          (i.first.get<test_string>())
        );
        FATAL_EXPECT_TRUE(i.second.is_of<int>());
        FATAL_EXPECT_EQ(456789, (i.second.get<int>()));
        break;

      default:
        FATAL_EXPECT_TRUE(false);
    }
  }

  v1 = m.find(var(allocator, 10));
  FATAL_ASSERT_NE(v1, m.end());
  FATAL_EXPECT_TRUE(v1->second.is_of<test_string>());
  FATAL_EXPECT_EQ(
    "so you didn't misspell cat prophet, my bad",
    (v1->second.get<test_string>())
  );

  v2 = m.find(k2);
  FATAL_ASSERT_NE(v2, m.end());
  FATAL_EXPECT_TRUE(v2->second.is_of<int>());
  FATAL_EXPECT_EQ(456789, (v2->second.get<int>()));
}

FATAL_TEST(variant, variant_unordered_map_as_key_value) {
  using var = test_variant<int, double, std::string>;
  std::unordered_map<var, var> m;

  m.emplace(var(allocator, 10), var(allocator, 5.6));
  var const t(allocator, std::string("THIS IS A TEST"));
  m.insert(
    std::make_pair(var(allocator, std::string("HELLO, WORLD")), t)
  );

  FATAL_EXPECT_EQ(2, m.size());

  for (auto const &i: m) {
    switch (i.first.tag()) {
      case var::tag<int>():
        FATAL_EXPECT_EQ(10, (i.first.get<int>()));
        FATAL_EXPECT_TRUE(i.second.is_of<double>());
        FATAL_EXPECT_EQ(5.6, (i.second.get<double>()));
        break;

      case var::tag<std::string>():
        FATAL_EXPECT_EQ(
          std::string("HELLO, WORLD"),
          (i.first.get<std::string>())
        );
        FATAL_EXPECT_EQ(t.tag(), i.second.tag());
        FATAL_EXPECT_EQ((t.get<std::string>()), (i.second.get<std::string>()));
        break;

      default:
        FATAL_EXPECT_TRUE(false);
    }
  }

  var const k1(allocator, 10);
  var const k2(allocator, std::string("HELLO, WORLD"));

  auto v1 = m.find(k1);
  FATAL_ASSERT_NE(v1, m.end());
  FATAL_EXPECT_TRUE(v1->second.is_of<double>());
  FATAL_EXPECT_EQ(5.6, (v1->second.get<double>()));

  auto v2 = m.find(var(allocator, std::string("HELLO, WORLD")));
  FATAL_ASSERT_NE(v2, m.end());
  FATAL_EXPECT_EQ(t.tag(), v2->second.tag());
  FATAL_EXPECT_EQ((t.get<std::string>()), (v2->second.get<std::string>()));

  m[var(allocator, 10)] = var(
    allocator,
    std::string("so you didn't misspell cat prophet, my bad")
  );
  m[k2] = var(allocator, 456789);

  for (auto const &i: m) {
    switch (i.first.tag()) {
      case var::tag<int>():
        FATAL_EXPECT_EQ(10, (i.first.get<int>()));
        FATAL_EXPECT_TRUE(i.second.is_of<std::string>());
        FATAL_EXPECT_EQ(
          "so you didn't misspell cat prophet, my bad",
          (i.second.get<std::string>())
        );
        break;

      case var::tag<std::string>():
        FATAL_EXPECT_EQ(
          std::string("HELLO, WORLD"),
          (i.first.get<std::string>())
        );
        FATAL_EXPECT_TRUE(i.second.is_of<int>());
        FATAL_EXPECT_EQ(456789, (i.second.get<int>()));
        break;

      default:
        FATAL_EXPECT_TRUE(false);
    }
  }

  v1 = m.find(var(allocator, 10));
  FATAL_ASSERT_NE(v1, m.end());
  FATAL_EXPECT_TRUE(v1->second.is_of<std::string>());
  FATAL_EXPECT_EQ(
    "so you didn't misspell cat prophet, my bad",
    (v1->second.get<std::string>())
  );

  v2 = m.find(k2);
  FATAL_ASSERT_NE(v2, m.end());
  FATAL_EXPECT_TRUE(v2->second.is_of<int>());
  FATAL_EXPECT_EQ(456789, (v2->second.get<int>()));
}

FATAL_TEST(variant, rebind_storage_policy) {
  using custom_storage_policy = default_storage_policy<
    decltype(allocator), automatic_allocation_policy
  >;

  using original_var = test_variant<int, double, test_string>;
  using rebound_var = original_var::types::apply_front<
    variant, custom_storage_policy
  >;

  FATAL_EXPECT_TRUE((
    std::is_same<
      rebound_var,
      variant<custom_storage_policy, int, double, test_string>
    >::value
  ));

  rebound_var v(allocator, 123456789);

  FATAL_EXPECT_TRUE(v.is_of<int>());
  FATAL_EXPECT_EQ(123456789, (v.get<int>()));
}

FATAL_TEST(variant, container_assignment) {
  using VAR = test_variant<int, double, test_string>;
  using VECTOR = test_vector<VAR>;
  VECTOR v1(allocator);
  v1.reserve(1);

  v1.push_back(VAR(allocator, 10));
  v1.emplace_back(allocator, 5.6);
  v1.resize(3);
  v1[2] = VAR(allocator, test_string("HELLO, WORLD", allocator));

  auto check = [](VECTOR const &v) {
    FATAL_ASSERT_EQ(3, v.size());
    FATAL_EXPECT_TRUE(v[0].is_of<int>());
    FATAL_EXPECT_EQ(10, (v[0].get<int>()));
    FATAL_EXPECT_TRUE(v[1].is_of<double>());
    FATAL_EXPECT_EQ(5.6, (v[1].get<double>()));
    FATAL_EXPECT_TRUE(v[2].is_of<test_string>());
    FATAL_EXPECT_EQ("HELLO, WORLD", (v[2].get<test_string>()));
  };
  check(v1);

  VECTOR v2(allocator);
  v2 = v1;
  check(v1);
  check(v2);

  VECTOR v3(allocator);
  v3 = std::move(v1);
  FATAL_EXPECT_EQ(0, v1.size());
  check(v2);
  check(v3);
}

FATAL_TEST(variant, set_difference) {
  using VAR = test_variant<int, double, test_string>;
  using VECTOR = test_vector<VAR>;

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

  for (auto &i: diff) {
    FATAL_VLOG(1) << "result: " << i.get<int>();
  }

  VECTOR const expected({
    VAR(allocator, 1),
    VAR(allocator, 2),
    VAR(allocator, 3)
  }, allocator);
  FATAL_EXPECT_EQ(expected.size(), diff.size());
  FATAL_EXPECT_EQ(expected, diff);
}

FATAL_TEST(variant, set_difference_inplace) {
  using VAR = test_variant<int, double, test_string>;
  using VECTOR = test_vector<VAR>;

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

  for (auto &i: lhs) {
    FATAL_VLOG(1) << "result: " << i.get<int>();
  }

  VECTOR const expected({
    VAR(allocator, 1),
    VAR(allocator, 2),
    VAR(allocator, 3)
  }, allocator);
  FATAL_EXPECT_EQ(expected.size(), lhs.size());
  FATAL_EXPECT_EQ(expected, lhs);
}

struct incomplete;
struct below_threshold { void *a; };
struct at_threshold { void * a; std::size_t b; };
struct above_threshold { void * a; std::size_t b; int c; double d; };
struct default_recursive;
struct dynamic_recursive;

using default_policy_test_variant = variant<
  default_storage_policy<decltype(allocator)>,
  incomplete, below_threshold, at_threshold, above_threshold, default_recursive
>;

using dynamic_policy_test_variant = dynamic_variant<
  decltype(allocator),
  incomplete, below_threshold, at_threshold, above_threshold, dynamic_recursive
>;

using automatic_policy_test_variant = auto_variant<
  below_threshold, at_threshold, above_threshold
>;

struct default_recursive { default_policy_test_variant field; };
struct dynamic_recursive { dynamic_policy_test_variant field; };

template <typename T>
using default_policy_test = default_policy_test_variant::storage_policy
  ::allocate_dynamically<T>;

template <typename T>
using dynamic_policy_test = dynamic_policy_test_variant::storage_policy
  ::allocate_dynamically<T>;

template <typename T>
using automatic_policy_test = automatic_policy_test_variant::storage_policy
  ::allocate_dynamically<T>;

FATAL_TEST(default_allocation_policy, storage_duration) {
  FATAL_EXPECT_FALSE(default_policy_test<char>::value);
  FATAL_EXPECT_FALSE(default_policy_test<int>::value);
  FATAL_EXPECT_FALSE(default_policy_test<below_threshold>::value);
  FATAL_EXPECT_FALSE(default_policy_test<at_threshold>::value);
  FATAL_EXPECT_TRUE(default_policy_test<above_threshold>::value);
  FATAL_EXPECT_TRUE(default_policy_test<incomplete>::value);
  FATAL_EXPECT_TRUE(default_policy_test<default_recursive>::value);
}

FATAL_TEST(dynamic_allocation_policy, storage_duration) {
  FATAL_EXPECT_TRUE(dynamic_policy_test<char>::value);
  FATAL_EXPECT_TRUE(dynamic_policy_test<int>::value);
  FATAL_EXPECT_TRUE(dynamic_policy_test<below_threshold>::value);
  FATAL_EXPECT_TRUE(dynamic_policy_test<at_threshold>::value);
  FATAL_EXPECT_TRUE(dynamic_policy_test<above_threshold>::value);
  FATAL_EXPECT_TRUE(dynamic_policy_test<incomplete>::value);
  FATAL_EXPECT_TRUE(dynamic_policy_test<dynamic_recursive>::value);
}

FATAL_TEST(automatic_allocation_policy, storage_duration) {
  FATAL_EXPECT_FALSE(automatic_policy_test<char>::value);
  FATAL_EXPECT_FALSE(automatic_policy_test<int>::value);
  FATAL_EXPECT_FALSE(automatic_policy_test<below_threshold>::value);
  FATAL_EXPECT_FALSE(automatic_policy_test<at_threshold>::value);
  FATAL_EXPECT_FALSE(automatic_policy_test<above_threshold>::value);
}

// This MUST be the LAST test.
FATAL_TEST(variant, memory_leak) {
  auto balance = allocated < freed
    ? freed - allocated
    : allocated - freed;

  FATAL_VLOG(1) << "allocated: " << allocated
    << " freed: " << freed
    << " balance: " << balance
    << (
      allocated < freed
        ? " negative (huh?)"
        : freed < allocated
          ? " positive (leak)" : ""
    );

  FATAL_EXPECT_EQ(allocated, freed);
}

} // namespace fatal {
