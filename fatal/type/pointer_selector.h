/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <memory>
#include <type_traits>
#include <utility>

namespace fatal {

/**
 * Classes of pointers supported by `pointer_selector`,
 * `pointer_selector_t` and `make_ptr`.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
namespace pointer_class {

struct raw;
struct unique;
struct shared;

} // namespace pointer_class {

/**
 * Selects among several classes of pointers based on a parameter
 * and provides the appropriate function to dynamically allocate
 * an instance the pointee into the proper pointer class.
 *
 * This is a convenient mechanism for factories that don't want to
 * force the user to commit to a single class of pointers.
 *
 * See also `pointer_selector_t` for a convenient alias and `make_ptr`
 * for a convenient way of allocating the pointee.
 *
 * Example:
 *
 *  struct Foo {
 *    Foo(int a, float b) { ... }
 *  };
 *
 *  template <typename PtrClass>
 *  typename pointer_selector<PtrClass, Foo>::type my_factory(int a, float b) {
 *    return pointer_selector<PtrClass, Foo>::make(a, b);
 *  }
 *
 *  // dynamically allocates `Foo` into an unique pointer
 *  auto instance = my_factory<pointer_class::unique>(a, b);
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename PtrClass, typename T, typename... Args>
class pointer_selector {
  using impl = typename PtrClass::template impl<T, Args...>;

public:
  /**
   * The type of the pointee.
   *
   * Example:
   *
   *  // yields `int`
   *  using result1 = pointer_selector<pointer_class::raw, int>::element_type;
   *
   *  // yields `int`
   *  using result2 = pointer_selector<
   *    pointer_class::unique, int
   *  >::element_type;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using element_type = typename impl::element_type;

  /**
   * The raw pointer for the pointee.
   *
   * Example:
   *
   *  // yields `int *`
   *  using result1 = pointer_selector<pointer_class::raw, int>::pointer;
   *
   *  // yields `int *`
   *  using result2 = pointer_selector<pointer_class::unique, int>::pointer;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using pointer = typename impl::pointer;

  /**
   * The pointer of the chosen class. This is either `element_type *`,
   * `std::unique_ptr<element_type>`, `std::shared_ptr<element_type>` or
   * `std::unique_ptr<element_type, CustomDeleter>`.
   *
   * Example:
   *
   *  // yields `int *`
   *  using raw = pointer_selector<pointer_class::raw, int>::type;
   *
   *  // yields `std::unique_ptr<int>`
   *  using unique = pointer_selector<pointer_class::unique, int>::type;
   *
   *  // yields `std::unique_ptr<int, MyDeleter>`
   *  using unique_d = pointer_selector<
   *    pointer_class::unique, int, MyDeleter
   *  >::type;
   *
   *  // yields `std::shared_ptr<int>`
   *  using shared = pointer_selector<pointer_class::shared, int>::type;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using type = typename impl::type;

  /**
   * A boolean `std::integral_constant` telling whether the
   * pointer of the chosen class automatically manages the
   * lifetime of the pointee.
   *
   * When false, the pointee must be destroyed using explicit
   * deletion.
   *
   * Example:
   *
   *  // yields `false`
   *  pointer_selector<pointer_class::raw, int>::managed::value;
   *
   *  // yields `true`
   *  pointer_selector<pointer_class::unique, int>::managed::value;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using managed = typename impl::managed;

  /**
   * A convenient way to obtain a raw pointer out of the
   * specified pointer class.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  static pointer get(type const &p) { return impl::get(p); }
  static pointer get(type const &&p) { return impl::get(p); }

  /**
   * Allocates a raw, unique or shared pointer according to
   * the specified pointer class.
   *
   * The parameters passed to this function are perfectly forwarded
   * to the constructor of the pointee.
   *
   * Example:
   *
   *  struct Foo {
   *    Foo(int a, float b) { ... }
   *  };
   *
   *  template <typename PtrClass>
   *  typename pointer_selector<PtrClass, Foo>::type my_factory(
   *    int a, float b
   *  ) {
   *    return pointer_selector<PtrClass, Foo>::make(a, b);
   *  }
   *
   *  // dynamically allocates `Foo` and returns a raw pointer to it
   *  auto raw = my_factory<pointer_class::raw>(a, b);
   *
   *  // dynamically allocates `Foo` into an unique pointer
   *  auto unique = my_factory<pointer_class::unique>(a, b);
   *
   *  // dynamically allocates `Foo` into a shared pointer
   *  auto shared = my_factory<pointer_class::shared>(a, b);
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UArgs>
  static type make(UArgs &&...args) {
    return impl::make(std::forward<UArgs>(args)...);
  }

  /**
   * Deletes the pointee stored in the specified pointer class.
   *
   * NOTE:
   * Unique and shared pointers don't need to be explicitly deleted.
   * Raw pointers can be deleted using the `delete` keyword.
   * Calling destroy on a shared pointer doesn't guarantee that the
   * pointee will actually be deleted since there can be other
   * shared pointers referencing it.
   *
   * This method is offered for convenience purposes for situations
   * where explicit deletion is desired.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  static void destroy(type &p) { impl::destroy(p); }
  static void destroy(type &&p) { impl::destroy(p); }
};

/**
 * Alias for `pointer_selector<args>::type`.
 *
 * Selects among several classes of pointers based on a parameter.
 *
 * This is a convenient mechanism for factories that don't want to
 * force the user to commit to a single class of pointers.
 *
 * See also `make_ptr` for a convenient way of allocating the pointee.
 *
 * Example:
 *
 *  struct Foo {
 *    Foo(int a, float b) { ... }
 *  };
 *
 *  template <typename PtrClass>
 *  pointer_selector_t<PtrClass, Foo> my_factory(int a, double b) {
 *    return make_ptr<PtrClass, Foo>(a, b);
 *  }
 *
 *  // dynamically allocates `Foo` and returns a raw pointer to it
 *  auto raw = my_factory<pointer_class::raw>(a, b);
 *
 *  // dynamically allocates `Foo` into an unique pointer
 *  auto unique = my_factory<pointer_class::unique>(a, b);
 *
 *  // dynamically allocates `Foo` into a shared pointer
 *  auto shared = my_factory<pointer_class::shared>(a, b);
 *
 *  // yields a `std::unique_ptr` to `Foo` using a custom deleter `MyDeleter`
 *  using my_ptr = pointer_selector_t<pointer_class::unique, Foo, MyDeleter>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename PtrClass, typename T, typename... Args>
using pointer_selector_t = typename pointer_selector<
  PtrClass, T, Args...
>::type;

/**
 * Allocates a raw, unique or shared pointer of type `T` according
 * to the specified `PtrClass` template parameter.
 *
 * The non-template parameters passed to this function are
 * perfectly forwarded to the constructor of the pointee.
 *
 * See also `pointer_selector_t`.
 *
 * Example:
 *
 *  struct Foo {
 *    Foo(int a, double b) { ... }
 *  };
 *
 *  template <typename PtrClass>
 *  pointer_selector_t<PtrClass, Foo> my_factory(int a, double b) {
 *    return make_ptr<PtrClass, Foo>(a, b);
 *  }
 *
 *  // dynamically allocates `Foo` and returns a raw pointer to it
 *  auto raw = my_factory<pointer_class::raw>(a, b);
 *
 *  // dynamically allocates `Foo` into an unique pointer
 *  auto unique = my_factory<pointer_class::unique>(a, b);
 *
 *  // dynamically allocates `Foo` into a shared pointer
 *  auto shared = my_factory<pointer_class::shared>(a, b);
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename PtrClass, typename T, typename... Args, typename... UArgs>
static pointer_selector_t<PtrClass, T, Args...> make_ptr(UArgs &&...args) {
  return pointer_selector<PtrClass, T, Args...>::make(
    std::forward<UArgs>(args)...
  );
}

////////////////////////////
// IMPLEMENTATION DETAILS //
////////////////////////////

namespace pointer_class {

struct raw {
  template <typename T>
  struct impl {
    using element_type = T;
    using pointer = element_type *;
    using type = pointer;
    using managed = std::false_type;

    static pointer get(type const &p) { return p; }

    template <typename... UArgs>
    static type make(UArgs &&...args) {
      return new element_type(std::forward<UArgs>(args)...);
    }

    static void destroy(type &p) { delete p; }
  };
};

struct unique {
  template <typename T, typename... Args>
  struct impl {
    using element_type = T;
    using pointer = element_type *;
    using type = std::unique_ptr<element_type, Args...>;
    using managed = std::true_type;

    static pointer get(type const &p) { return p.get(); }

    template <typename... UArgs>
    static type make(UArgs &&...args) {
      return type(new element_type(std::forward<UArgs>(args)...));
    }

    static void destroy(type &p) { p.reset(); }
  };
};

struct shared {
  template <typename T>
  struct impl {
    using element_type = T;
    using pointer = element_type *;
    using type = std::shared_ptr<element_type>;
    using managed = std::true_type;

    static pointer get(type const &p) { return p.get(); }

    template <typename... UArgs>
    static type make(UArgs &&...args) {
      return std::make_shared<element_type>(std::forward<UArgs>(args)...);
    }

    static void destroy(type &p) { p.reset(); }
  };
};

} // namespace pointer_class {
} // namespace fatal {
