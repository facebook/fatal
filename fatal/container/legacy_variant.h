/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/container/optional.h>
#include <fatal/container/unitary_union.h>
#include <fatal/functional/functional.h>
#include <fatal/math/numerics.h>
#include <fatal/portability.h>
#include <fatal/type/conditional.h>
#include <fatal/type/deprecated/type_list.h>
#include <fatal/type/traits.h>

#include <functional>
#include <memory>
#include <utility>

#include <cassert>

FATAL_DIAGNOSTIC_PUSH
FATAL_GCC_DIAGNOSTIC_IGNORED_SHADOW_IF_BROKEN

/**
 * #################
 * # READ ME FIRST #
 * #################
 *
 * This file implements a memory efficient custom allocated variant.
 *
 * NOTE: this implementation is undergoing a major cleanup.
 * The interface should be considered mostly stable, though.
 *
 * This implementation is rather complex and involves lots of compile-time
 * template metaprogramming so. To keep it simple you're encouraged to start
 * with the variant declaration itself before anything else.
 *
 * To quickly get there, search for 'START HERE'.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */

namespace fatal {

/**
 * #######################
 * # 'ALLOCATION POLICY' #
 * #######################
 *
 * This is the default implementation for the allocation policy
 * given a type T it decides whether to allocate it automatically
 * or dynamically.
 *
 * A type T is automatically allocated if it is a scalar, or if its
 * size in bytes is smaller than ax + b, where x is the size in bytes
 * of a pointer to T, a is allocateMultiplier (defaults to 1) and
 * b is allocateIncrement (defaults to the size in bytes of size_t).
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
namespace detail {

template <typename std::size_t Threshold>
struct variant_automatic_policy_impl {
  template <
    typename T,
    typename = enable_when::any_true<
      std::is_scalar<T>,
      bool_constant<(sizeof(T) <= Threshold)>
    >
  >
  static std::true_type sfinae(T *);

  template <typename...>
  static std::false_type sfinae(...);

  template <typename T>
  using type = logical::all<
    is_complete<T>,
    decltype(sfinae(static_cast<T *>(nullptr)))
  >;
};

} // namespace detail {


template <
  std::size_t allocateMultiplier = 1,
  std::size_t allocateIncrement = sizeof(std::size_t)
>
class default_allocation_policy {
  template <typename T>
  using allocate_threshold = size_constant<
    allocateMultiplier * sizeof(void *) + allocateIncrement
  >;

public:
  template <typename T>
  using dynamic = bool_constant<!(
    detail::variant_automatic_policy_impl<allocate_threshold<T>::value>
      ::template type<T>::value
  )>;
};

template <bool AlwaysDynamic>
class fixed_allocation_policy {
  template <typename T>
  struct impl {
    static_assert(
      AlwaysDynamic || is_complete<T>::value,
      "can't allocate an incomplete type using automatic storage duration"
    );
    using type = bool_constant<AlwaysDynamic>;
  };

public:
  template <typename T>
  using dynamic = typename impl<T>::type;
};

using dynamic_allocation_policy = fixed_allocation_policy<true>;
using automatic_allocation_policy = fixed_allocation_policy<false>;

/**
 * ####################
 * # 'STORAGE POLICY' #
 * ####################
 *
 * This is the default implementation of the storage policy. Its goal is to
 * orchestrate the interactions with both the allocation policy and the
 * allocator, providing a uniform interface for the underlying storage
 * used to store the values.
 *
 * This policy provides the following methods:
 *
 *  - these methods are analogous to std::allocator_traits' ones:
 *
 *  static void allocate(Alloc const &, typename storage_type<T>::type &);
 *  static void deallocate(Alloc const &, typename storage_type<T>::type &);
 *  static T &construct(Alloc const &, typename storage_type<T>::type &, ...);
 *  static void destroy(Alloc const &, typename storage_type<T>::type &);
 *
 * - move_over() is provided to aid implementation of move construction, move
 *   assignment and swapping:
 *
 *  static void move_over(
 *    typename storage_type<T>::type &from
 *    typename storage_type<T>::type &to
 *  );
 *
 * - get() is provided to abstract the way the actual stored value is obtained,
 *   from its internal storage representation:
 *
 *  static T const &get(typename storage_type<T>::type const &);
 *  static T &get(typename storage_type<T>::type &);
 *
 * TAllocator is any STL-style allocator which will be rebound to whatever
 * type the storage policy must handle. It will only be used for dynamically
 * allocated types.
 *
 * TAllocationPolicy decides which allocation method will be used for a given
 * type. It needs to provide a single constant
 *
 *  template <typename T> using dynamic = std::integral_constant<bool, ???>;
 *
 * which returns true when dynamic allocation must be used to store the type
 * T, or false when automatic allocation must be used to store the same type.
 *
 * A default implementation of the allocation policy is provided. Search for
 * 'ALLOCATION POLICY' for more details.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  typename TAllocator = std::allocator<void>,
  typename TAllocationPolicy = default_allocation_policy<>,
  bool IsCopyable = true
>
struct legacy_storage_policy {
  using allocator_type = TAllocator;
  using allocator_traits = std::allocator_traits<allocator_type>;
  using allocation_policy = TAllocationPolicy;

  /**
   * Tells whether copy construction and copy assignment should be allowed by
   * the variant. Checked at compile time.
   */
  constexpr static bool is_copyable() { return IsCopyable; }

  template <typename T>
  using allocate_dynamically = typename allocation_policy::template dynamic<T>;

  /**
   * This class abstracts the interaction with TAllocationPolicy. It provides
   * a member type for the internal storage representation of the given type T,
   * and a boolean telling whether this representation uses automatic or
   * dynamic storage.
   */
  template <typename T>
  class storage_type {
    static_assert(
      !std::is_reference<T>::value,
      "default_storage_policy: unable to store references"
    );

  public:
    using dynamic = allocate_dynamically<T>;

    static_assert(
      is_complete<T>::value || dynamic::value,
      "can't allocate an incomplete type using automatic storage duration"
    );

    // A naked pointer is used for dynamically allocated types. No smart
    // pointers are used since we aim for the smallest possible footprint
    // (having a copy of the deleter for every pointer is undesired).
    using type = conditional<
      dynamic::value,
      T *, unitary_union<T, false>
    >;
  };

  // Methods for when T is stored using DYNAMIC ALLOCATION.
  //
  // The methods below are guaranteed to work on `T *` due to:
  //  typename std::enable_if<
  //    storage_type<T>::dynamic::value,
  //    ...

  template <typename T, typename std::enable_if<
    storage_type<T>::dynamic::value, int
  >::type = 0>
  static void allocate(
    allocator_type const &allocator,
    typename storage_type<T>::type &storage
  ) {
    using rebound_alloc = typename allocator_traits::template rebind_alloc<T>;
    storage = rebound_alloc(allocator).allocate(1);
  }

  template <typename T, typename std::enable_if<
    storage_type<T>::dynamic::value, int
  >::type = 0>
  static void deallocate(
    allocator_type const &allocator,
    typename storage_type<T>::type &storage
  ) {
    using rebound_alloc = typename allocator_traits::template rebind_alloc<T>;
    rebound_alloc(allocator).deallocate(storage, 1);
    storage = nullptr;
  }

  template <typename T, typename... Args, typename std::enable_if<
    storage_type<T>::dynamic::value, int
  >::type = 0>
  static T &construct(
    allocator_type const &allocator,
    typename storage_type<T>::type &storage,
    Args &&...args
  ) {
    using rebound_traits = typename allocator_traits::template rebind_traits<T>;
    auto allocator_ = typename rebound_traits::allocator_type(allocator);
    rebound_traits::construct(allocator_, storage, std::forward<Args>(args)...);
    return *storage;
  }

  template <typename T, typename std::enable_if<
    storage_type<T>::dynamic::value, int
  >::type = 0>
  static void destroy(
    allocator_type const &allocator,
    typename storage_type<T>::type &storage
  ) {
    using rebound_traits = typename allocator_traits::template rebind_traits<T>;
    auto allocator_ = typename rebound_traits::allocator_type(allocator);
    rebound_traits::destroy(allocator_, storage);
  }

  template <typename T, typename std::enable_if<
    storage_type<T>::dynamic::value, int
  >::type = 0>
  static void move_over(
    typename storage_type<T>::type &from,
    typename storage_type<T>::type &to
  ) {
    to = from;
    from = nullptr;
  }

  template <typename T, typename std::enable_if<
    storage_type<T>::dynamic::value, int
  >::type = 0>
  static T const &get(
    typename storage_type<T>::type const &storage
  ) {
    return *storage;
  }

  template <typename T, typename std::enable_if<
    storage_type<T>::dynamic::value, int
  >::type = 0>
  static T &get(
    typename storage_type<T>::type &storage
  ) {
    return *storage;
  }

  // Methods for when T is stored using AUTOMATIC ALLOCATION.
  //
  // The methods below are guaranteed to work on `union { T value; };` due to:
  //  typename std::enable_if<
  //    !storage_type<T>::dynamic::value,
  //    ...

  template <typename T, typename std::enable_if<
    !storage_type<T>::dynamic::value, int
  >::type = 0>
  static void allocate(
    allocator_type const &,
    typename storage_type<T>::type &
  ) {}

  template <typename T, typename std::enable_if<
    !storage_type<T>::dynamic::value, int
  >::type = 0>
  static void deallocate(
    allocator_type const &,
    typename storage_type<T>::type &
  ) {}

  template <typename T, typename... Args, typename std::enable_if<
    !storage_type<T>::dynamic::value, int
  >::type = 0>
  static T &construct(
    allocator_type const &,
    typename storage_type<T>::type &storage,
    Args &&...args
  ) {
    return *new (std::addressof(storage.value)) T(std::forward<Args>(args)...);
  }

  template <typename T, typename std::enable_if<
    !storage_type<T>::dynamic::value, int
  >::type = 0>
  static void destroy(
    allocator_type const &,
    typename storage_type<T>::type &storage
  ) {
    storage.value.~T();
  }

  template <typename T, typename std::enable_if<
    !storage_type<T>::dynamic::value, int
  >::type = 0>
  static void move_over(
    typename storage_type<T>::type &from,
    typename storage_type<T>::type &to
  ) {
    new (std::addressof(to.value)) T(std::move(from.value));
    from.value.~T();
  }

  template <typename T, typename std::enable_if<
    !storage_type<T>::dynamic::value, int
  >::type = 0>
  static T const &get(
    typename storage_type<T>::type const &storage
  ) {
    return storage.value;
  }

  template <typename T, typename std::enable_if<
    !storage_type<T>::dynamic::value, int
  >::type = 0>
  static T &get(
    typename storage_type<T>::type &storage
  ) {
    return storage.value;
  }
};

template <
  typename Allocator = std::allocator<void>,
  typename AllocationPolicy = default_allocation_policy<>,
  bool IsCopyable = true
>
using default_storage_policy = legacy_storage_policy<
  Allocator, AllocationPolicy, IsCopyable
>;

namespace detail {
namespace variant_impl {

/**
 * An internal traits class used by fatal::variant to handle variadic
 * recursion and runtime type erasure without the need for virtual calls.
 *
 * It basically provides a union comprised of all the types that need to
 * be stored and a uniform interface to handle such union in a type-safe
 * manner.
 *
 * Try to picture the goal of (some parameters omitted for simplicity)
 *
 *  variadic_union_traits<T1, T2, T3, ..., Tn-1, Tn>
 *
 * as providing something like
 *
 *  union {
 *    head: union { storage_policy::storage_type<T1>::type storage; };
 *    tail: union {
 *      head: union { storage_policy::storage_type<T2>::type storage; };
 *      tail: union {
 *        head: union { storage_policy::storage_type<T3>::type storage; };
 *        tail: union {
 *          ...
 *          head: union { storage_policy::storage_type<Tn-1>::type storage; };
 *          tail: union { storage_policy::storage_type<Tn>::type storage; };
 *        };
 *      };
 *    };
 *  };
 *
 * There are two specializations for this class. One of them is the general
 * variadic version (a.k.a. general case) and the other is the terminal
 * version (a.k.a. base case).
 *
 * - General case: it is responsible for checking whether the given type
 *   relates to the current recursion depth or not:
 *    - if it does, it delegates the call to a terminal (base case)
 *      representing the current recursion depth (head);
 *    - if not, it propagates the call further deep in the recursion (tail).
 *
 * - Base case: it is responsible for interfacing with the storage policy
 *   and effectivelly executing any necessary operations on the storage.
 *
 * Each of the recursion levels are identified by an unique number, a.k.a
 * the type_tag, which is the information used both at compile and run-time
 * by the general case to determine the right depth of the union that must
 * be operated upon.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename, typename TSize, TSize, typename...>
struct variadic_union_traits;

// empty case
template <typename TStoragePolicy, typename TSize, TSize Depth>
struct variadic_union_traits<TStoragePolicy, TSize, Depth> {
  using size_type = TSize;

  union union_type {};

  constexpr static size_type end_depth() { return Depth; }
};

// base case
template <typename TStoragePolicy, typename TSize, TSize Depth, typename T>
struct variadic_union_traits<TStoragePolicy, TSize, Depth, T> {
  static_assert(std::is_integral<TSize>::value, "TSize must be an integral");
  static_assert(!std::is_reference<T>::value, "T can't be a reference");
  static_assert(!std::is_const<T>::value, "T can't be const");

  using size_type = TSize;
  using value_type = T;

  using storage_policy = TStoragePolicy;
  using allocator_type = typename storage_policy::allocator_type;
  using storage_type = typename storage_policy
    ::template storage_type<value_type>::type;

  union union_type {
    storage_type storage;
    union_type() {}
    union_type(union_type const &) {}
    union_type(union_type &&) {}
    ~union_type() {}
  };

  // calls not dependent on value_type

  constexpr static size_type end_depth() { return Depth + 1; }

  template <template <typename> class... TTransforms>
  static std::type_info const &type(size_type const depth) {
    assert(depth == Depth), (void) depth;
    return typeid(
      typename fatal::try_transform<
        fatal::compose<TTransforms...>::template apply
      >::template apply<value_type>
    );
  }

  template <typename TVisitor, typename... UArgs>
  static void visit(
    size_type const depth, union_type const &u, TVisitor &&visitor,
    UArgs &&...args
  ) {
    assert(depth == Depth), (void) depth;
    visitor(
      storage_policy::template get<value_type>(u.storage),
      std::forward<UArgs>(args)...
    );
  }

  template <typename TVisitor, typename... UArgs>
  static void visit(
    size_type const depth, union_type &u, TVisitor &&visitor,
    UArgs &&...args
  ) {
    assert(depth == Depth), (void) depth;
    visitor(
      storage_policy::template get<value_type>(u.storage),
      std::forward<UArgs>(args)...
    );
  }

  static void allocate(
    allocator_type const &allocator, size_type const depth, union_type &u
  ) {
    assert(depth == Depth), (void) depth;
    storage_policy::template allocate<value_type>(allocator, u.storage);
  }

  static void deallocate(
    allocator_type const &allocator, size_type const depth, union_type &u
  ) {
    assert(depth == Depth), (void) depth;
    storage_policy::template deallocate<value_type>(allocator, u.storage);
  }

  static void copy_construct(
    allocator_type const &allocator, size_type const depth,
    union_type const &from, union_type &to
  ) {
    assert(depth == Depth), (void) depth;
    storage_policy::template construct<value_type>(
      allocator, to.storage,
      storage_policy::template get<value_type>(from.storage)
    );
  }

  static void destroy(
    allocator_type const &allocator, size_type const depth, union_type &u
  ) {
    assert(depth == Depth), (void) depth;
    storage_policy::template destroy<value_type>(allocator, u.storage);
  }

  static void move_over(
    size_type const depth, union_type &from, union_type &to
  ) {
    assert(depth == Depth), (void) depth;
    storage_policy::template move_over<value_type>(from.storage, to.storage);
  }

  static bool equals(
    size_type const depth, union_type const &lhs, union_type const &rhs
  ) {
    if (depth != Depth) {
      assert(depth == end_depth());
      return true;
    }

    return storage_policy::template get<value_type>(lhs.storage)
      == storage_policy::template get<value_type>(rhs.storage);
  }

  static bool less_than(
    size_type const depth, union_type const &lhs, union_type const &rhs
  ) {
    if (depth != Depth) {
      assert(depth == end_depth());
      return false;
    }

    return storage_policy::template get<value_type>(lhs.storage)
      < storage_policy::template get<value_type>(rhs.storage);
  }

  // templated calls dependent on value_type

  // returns end_depth() when U is not found
  template <typename U>
  constexpr static size_type get_depth() {
    return std::is_same<U, value_type>::value ? Depth : end_depth();
  }

  template <typename U, typename std::enable_if<
    std::is_same<U, value_type>::value, int
  >::type = 0>
  static storage_type const &get(union_type const &u) {
    return u.storage;
  }

  template <typename U, typename std::enable_if<
    std::is_same<U, value_type>::value, int
  >::type = 0>
  static storage_type &get(union_type &u) {
    return u.storage;
  }

  template <typename U, typename... UArgs, typename std::enable_if<
    std::is_same<U, value_type>::value, int
  >::type = 0>
  static value_type &construct(
    allocator_type const &allocator,
    size_type const depth,
    union_type &u,
    UArgs &&...args
  ) {
    assert(depth == Depth), (void) depth;
    return storage_policy::template construct<value_type>(
      allocator, u.storage, std::forward<UArgs>(args)...
    );
  }
};

// general case
template <
  typename TStoragePolicy,
  typename TSize, TSize Depth,
  typename T, typename Head, typename... Tail
>
struct variadic_union_traits<TStoragePolicy, TSize, Depth, T, Head, Tail...> {
  static_assert(std::is_integral<TSize>::value, "TSize must be an integral");
  static_assert(!std::is_reference<T>::value, "T can't be a reference");
  static_assert(!std::is_const<T>::value, "T can't be const");

  using size_type = TSize;
  using value_type = T;

  using storage_policy = TStoragePolicy;
  using allocator_type = typename storage_policy::allocator_type;
  using storage_type = typename storage_policy
    ::template storage_type<value_type>::type;

  using head_type = variant_impl::variadic_union_traits<
    storage_policy, size_type, Depth, T
  >;
  using tail_type = variant_impl::variadic_union_traits<
    storage_policy, size_type, Depth + 1, Head, Tail...
  >;

  union union_type {
    typename head_type::union_type head;
    typename tail_type::union_type tail;
    union_type() {}
    union_type(union_type const &) {}
    union_type(union_type &&) {}
    ~union_type() {}
  };

  // calls not dependent on value_type

  constexpr static size_type end_depth() { return tail_type::end_depth(); }

  template <template <typename> class... TTransforms>
  static std::type_info const &type(size_type const depth) {
    return depth == Depth
      ? head_type::template type<TTransforms...>(depth)
      : tail_type::template type<TTransforms...>(depth);
  }

  template <typename TVisitor, typename... UArgs>
  static void visit(
    size_type const depth, union_type const &u, TVisitor &&visitor,
    UArgs &&...args
  ) {
    if (depth == Depth) {
      head_type::template visit(
        depth, u.head, std::forward<TVisitor>(visitor),
        std::forward<UArgs>(args)...
      );
    } else {
      tail_type::template visit(
        depth, u.tail, std::forward<TVisitor>(visitor),
        std::forward<UArgs>(args)...
      );
    }
  }

  template <typename TVisitor, typename... UArgs>
  static void visit(
    size_type const depth, union_type &u, TVisitor &&visitor,
    UArgs &&...args
  ) {
    if (depth == Depth) {
      head_type::template visit(
        depth, u.head, std::forward<TVisitor>(visitor),
        std::forward<UArgs>(args)...
      );
    } else {
      tail_type::template visit(
        depth, u.tail, std::forward<TVisitor>(visitor),
        std::forward<UArgs>(args)...
      );
    }
  }

  static void allocate(
    allocator_type const &allocator, size_type const depth, union_type &u
  ) {
    if (depth == Depth) {
      head_type::allocate(allocator, depth, u.head);
    } else {
      tail_type::allocate(allocator, depth, u.tail);
    }
  }

  static void deallocate(
    allocator_type const &allocator, size_type const depth, union_type &u
  ) {
    if (depth == Depth) {
      head_type::deallocate(allocator, depth, u.head);
    } else {
      tail_type::deallocate(allocator, depth, u.tail);
    }
  }

  static void copy_construct(
    allocator_type const &allocator, size_type const depth,
    union_type const &from, union_type &to
  ) {
    if (depth == Depth) {
      head_type::copy_construct(allocator, depth, from.head, to.head);
    } else {
      tail_type::copy_construct(allocator, depth, from.tail, to.tail);
    }
  }

  static void destroy(
    allocator_type const &allocator, size_type const depth, union_type &u
  ) {
    if (depth == Depth) {
      head_type::destroy(allocator, depth, u.head);
    } else {
      tail_type::destroy(allocator, depth, u.tail);
    }
  }

  static void move_over(
    size_type const depth, union_type &from, union_type &to
  ) {
    if (depth == Depth) {
      head_type::move_over(depth, from.head, to.head);
    } else {
      tail_type::move_over(depth, from.tail, to.tail);
    }
  }

  static bool equals(
    size_type const depth, union_type const &lhs, union_type const &rhs
  ) {
    return depth == Depth
      ? head_type::equals(depth, lhs.head, rhs.head)
      : tail_type::equals(depth, lhs.tail, rhs.tail);
  }

  static bool less_than(
    size_type const depth, union_type const &lhs, union_type const &rhs
  ) {
    return depth == Depth
      ? head_type::less_than(depth, lhs.head, rhs.head)
      : tail_type::less_than(depth, lhs.tail, rhs.tail);
  }

  // templated calls dependent on value_type

  // returns end_depth() when U is not found
  template <typename U>
  constexpr static size_type get_depth() {
    return std::is_same<U, value_type>::value
      ? head_type::template get_depth<U>()
      : tail_type::template get_depth<U>();
  }

  template <typename U, typename std::enable_if<
    std::is_same<U, value_type>::value, int
  >::type = 0>
  static storage_type const &get(union_type const &u) {
    return head_type::template get<U>(u.head);
  }

  template <typename U, typename std::enable_if<
    std::is_same<U, value_type>::value, int
  >::type = 0>
  static storage_type &get(union_type &u) {
    return head_type::template get<U>(u.head);
  }

  template <typename U, typename std::enable_if<
    !std::is_same<U, value_type>::value, int
  >::type = 0>
  static typename storage_policy::template storage_type<U>::type const &get(
    union_type const &u
  ) {
    return tail_type::template get<U>(u.tail);
  }

  template <typename U, typename std::enable_if<
    !std::is_same<U, value_type>::value, int
  >::type = 0>
  static typename storage_policy::template storage_type<U>::type &get(
    union_type &u
  ) {
    return tail_type::template get<U>(u.tail);
  }

  template <typename U, typename... UArgs, typename std::enable_if<
    std::is_same<U, value_type>::value, int
  >::type = 0>
  static U &construct(
    allocator_type const &allocator,
    size_type const depth,
    union_type &u,
    UArgs &&...args
  ) {
    return head_type::template construct<U>(
      allocator, depth, u.head, std::forward<UArgs>(args)...
    );
  }

  template <typename U, typename... UArgs, typename std::enable_if<
    !std::is_same<U, value_type>::value, int
  >::type = 0>
  static U &construct(
    allocator_type const &allocator,
    size_type const depth,
    union_type &u,
    UArgs &&...args
  ) {
    return tail_type::template construct<U>(
      allocator, depth, u.tail, std::forward<UArgs>(args)...
    );
  }
};

template <template <typename...> class UCondition>
class visit_if_visitor {
  template <
    bool Condition,
    typename U,
    typename UVisitor,
    typename... UArgs,
    typename std::enable_if<Condition,  int>::type = 0
  >
  bool visit(
    U &&value, UVisitor &&visitor, UArgs &&...args
  ) const {
    visitor(std::forward<U>(value), std::forward<UArgs>(args)...);
    return true;
  }

  template <
    bool Condition,
    typename U,
    typename UVisitor,
    typename... UArgs,
    typename std::enable_if<!Condition,  int>::type = 0
  >
  bool visit(
    U &&, UVisitor &&, UArgs &&...
  ) const {
    return false;
  }

public:
  template <
    typename U, std::size_t Index, typename UTag,
    typename UVariant, typename UVisitor, typename... UArgs
  >
  void operator ()(
    indexed_type_tag<U, Index>, UTag &&tag, UVariant &&variant, bool &result,
    UVisitor &&visitor, UArgs &&...args
  ) {
    assert(tag == variant.tag()), (void) tag;
    assert(Index == variant.tag());

    using variant_type = typename std::decay<UVariant>::type;
    static_assert(sizeof(variant_type) >= 0, "");
    result = visit<UCondition<U>::value>(
      variant.variant_type::template get<U>(),
      std::forward<UVisitor>(visitor),
      std::forward<UArgs>(args)...
    );
  }
};

template <typename T, bool>
struct nothrow_cp_ctor {
  using type = std::is_nothrow_copy_constructible<T>;
};

template <typename T>
struct nothrow_cp_ctor<T, true> {
  using type = std::false_type;
};

template <typename T, bool>
struct nothrow_mv_ctor {
  using type = std::is_nothrow_move_constructible<T>;
};

template <typename T>
struct nothrow_mv_ctor<T, true> {
  using type = std::true_type;
};

template <typename T, bool>
struct nothrow_cp_assign {
  using type = std::is_nothrow_copy_assignable<T>;
};

template <typename T>
struct nothrow_cp_assign<T, true> {
  using type = std::false_type;
};

template <typename T, bool>
struct nothrow_mv_assign {
  using type = std::is_nothrow_move_assignable<T>;
};

template <typename T>
struct nothrow_mv_assign<T, true> {
  using type = std::true_type;
};

} // namespace variant_impl {
} // namespace detail {

template <typename, typename...>
struct legacy_variant;

namespace detail {
namespace variant_impl {

template <typename> struct is_variant_impl;
template <typename UStoragePolicy, typename... UArgs>
struct is_variant_impl<legacy_variant<UStoragePolicy, UArgs...>> {};

} // namespace variant_impl {
} // namespace detail {

template <typename T>
using is_variant = is_complete<detail::variant_impl::is_variant_impl<T>>;

/**
 * ################
 * # 'START HERE' #
 * ################
 *
 * This class represents a variant container which can store values of
 * different types, one at a time.
 *
 * The underlying storage used to hold these values can be fully customized
 * by means of a StoragePolicy. A default implementation for this policy is
 * provided (fatal::default_storage_policy) which allows the usage of
 * either automatic or dynamic allocation to store the values (this can be
 * further customized per type by means of an AllocationPolicy, search for
 * 'STORAGE POLICY' for more details).
 *
 * An Allocator must also be provided in order to handle dynamic allocation.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
// TODO: ADD MISSING noexcept DECLARATIONS
template <typename TStoragePolicy, typename... Args>
struct legacy_variant {
  using storage_policy = TStoragePolicy;
  using allocator_type = typename storage_policy::allocator_type;
  using types = type_list<Args...>;

private:
  template <typename UStoragePolicy, typename... UArgs>
  friend struct legacy_variant;

  /**
   * This is the control block used by the variant to hold both the allocator
   * and the type_tag of the type currently stored in the variant.
   */
  struct control_block {
    // bits_size is the amount of bits needed to represent the type_tag for
    // the given types passed as variadic template parameters (Args...)
    constexpr static std::size_t bits_size() {
      return most_significant_bit<sizeof...(Args)>::value;
    }

    using type_tag = smallest_uint_for_value<sizeof...(Args)>;

    control_block(allocator_type const &allocator, type_tag storedType):
      allocator_(allocator),
      tag_(std::move(storedType))
    {}

    fast_pass<type_tag> storedType() const { return tag_; }
    void setStoredType(type_tag tag) { tag_ = std::move(tag); }

    allocator_type const &get_allocator() const { return allocator_; }

    allocator_type allocator_;
    type_tag tag_;
  };

  template <typename T>
  using nothrow_cp_ctor = typename detail::variant_impl::nothrow_cp_ctor<
    T, storage_policy::template allocate_dynamically<T>::value
  >::type;

  template <typename T>
  using nothrow_mv_ctor = typename detail::variant_impl::nothrow_mv_ctor<
    T, storage_policy::template allocate_dynamically<T>::value
  >::type;

  template <typename T>
  using nothrow_cp_assign = typename detail::variant_impl::nothrow_cp_assign<
    T, storage_policy::template allocate_dynamically<T>::value
  >::type;

  template <typename T>
  using nothrow_mv_assign = typename detail::variant_impl::nothrow_mv_assign<
    T, storage_policy::template allocate_dynamically<T>::value
  >::type;

public:
  using type_tag = typename control_block::type_tag;
  using traits = detail::variant_impl::variadic_union_traits<
    storage_policy,
    type_tag, std::numeric_limits<type_tag>::min(),
    typename std::decay<Args>::type...
  >;
  using union_type = typename traits::union_type;

  // returns no_tag() when the given type is not part of the variant
  template <typename U>
  constexpr static type_tag tag() { return traits::template get_depth<U>(); }

  // the tag used when the variant is empty
  constexpr static type_tag no_tag() { return traits::end_depth(); }

  // returns true iff this type can be stored in the variant
  template <typename U>
  constexpr static bool is_supported() { return tag<U>() != no_tag(); }

  legacy_variant() noexcept:
    control_(allocator_type(), no_tag())
  {}

  explicit legacy_variant(allocator_type const &allocator) noexcept:
    control_(allocator, no_tag())
  {}

  legacy_variant(legacy_variant const &other)
    noexcept(logical::all<nothrow_cp_ctor<Args>...>::value):
    control_(copy_impl(other))
  {
    static_assert(
      storage_policy::is_copyable(),
      "copy construction disabled by the variant's policy"
    );
  }

  legacy_variant(legacy_variant &&other)
    noexcept(logical::all<nothrow_mv_ctor<Args>...>::value):
    control_(move_impl(std::move(other)))
  {}

  template <typename U>
  explicit legacy_variant(allocator_type const &allocator, U &&value):
    control_(allocator, no_tag())
  {
    set<typename std::decay<U>::type>(std::forward<U>(value));
  }

  template <
    typename U,
    typename = safe_overload<legacy_variant, U>,
    typename std::enable_if<
      !std::is_convertible<U&&, allocator_type const &>::value, int
    >::type = 0
  >
  explicit legacy_variant(U &&value) : control_(allocator_type(), no_tag()) {
    set(std::forward<U>(value));
  }

  template <typename... UArgs>
  legacy_variant(legacy_variant<storage_policy, UArgs...> const &other):
    legacy_variant(other.control_.get_allocator())
  {
    other.visit(copy_variant_visitor<true, false>(), *this);
  }

  template <typename UStoragePolicy, typename... UArgs>
  legacy_variant(
    allocator_type const &allocator,
    legacy_variant<UStoragePolicy, UArgs...> const &other
  ):
    legacy_variant(allocator)
  {
    other.visit(copy_variant_visitor<true, false>(), *this);
  }

  ~legacy_variant() { unset_impl(); }

  /**
   * The unchecked_get won't perform type checks. The user must ensure the
   * requested type matches the one stored in the variant. Provided for
   * performance sensitive scenarios.
   */
  template <typename U>
  U const &unchecked_get() const & {
    return storage_policy::template get<U>(
      traits::template get<U>(union_)
    );
  }

  template <typename U>
  U &unchecked_get() & {
    return storage_policy::template get<U>(
      traits::template get<U>(union_)
    );
  }

  template <typename U>
  U &&unchecked_get() && {
    return std::move(storage_policy::template get<U>(
      traits::template get<U>(union_)
    ));
  }

  template <typename U>
  U const &get() const & {
    if (tag<U>() != control_.storedType()) {
      throw std::invalid_argument(
        "requested type doesn't match the one contained in the variant"
      );
    }

    return storage_policy::template get<U>(
      traits::template get<U>(union_)
    );
  }

  template <typename U>
  U &get() & {
    if (tag<U>() != control_.storedType()) {
      throw std::invalid_argument(
        "requested type doesn't match the one contained in the variant"
      );
    }

    return storage_policy::template get<U>(
      traits::template get<U>(union_)
    );
  }

  template <typename U>
  U &&get() && {
    if (tag<U>() != control_.storedType()) {
      throw std::invalid_argument(
        "requested type doesn't match the one contained in the variant"
      );
    }

    return std::move(storage_policy::template get<U>(
      traits::template get<U>(union_)
    ));
  }

  template <typename U>
  U const *try_get() const & {
    if (tag<U>() != control_.storedType()) {
      return nullptr;
    }

    return std::addressof(
      storage_policy::template get<U>(
        traits::template get<U>(union_)
      )
    );
  }

  template <typename U>
  U *try_get() & {
    if (tag<U>() != control_.storedType()) {
      return nullptr;
    }

    return std::addressof(
      storage_policy::template get<U>(
        traits::template get<U>(union_)
      )
    );
  }

  template <typename U>
  U *try_get() && = delete;

  template <
    template <typename...> class UCondition,
    typename UVisitor, typename... UArgs
  >
  bool visit_if(UVisitor &&visitor, UArgs &&...args) const {
    bool result = false;

    return types::template binary_search<index_value_comparer>::exact(
      control_.storedType(),
      detail::variant_impl::visit_if_visitor<UCondition>(),
      *this, result,
      std::forward<UVisitor>(visitor),
      std::forward<UArgs>(args)...
    ) && result;
  }

  template <
    template <typename...> class UCondition,
    typename UVisitor, typename... UArgs
  >
  bool visit_if(UVisitor &&visitor, UArgs &&...args) {
    bool result = false;

    return types::template binary_search<index_value_comparer>::exact(
      control_.storedType(),
      detail::variant_impl::visit_if_visitor<UCondition>(),
      *this, result,
      std::forward<UVisitor>(visitor),
      std::forward<UArgs>(args)...
    ) && result;
  }

  template <typename TVisitor, typename... UArgs>
  bool visit(TVisitor &&visitor, UArgs &&...args) const {
    auto const storedType = control_.storedType();

    if (storedType == no_tag()) {
      return false;
    }

    traits::template visit(
      storedType, union_, std::forward<TVisitor>(visitor),
      std::forward<UArgs>(args)...
    );
    return true;
  }

  template <typename TVisitor, typename... UArgs>
  bool visit(TVisitor &&visitor, UArgs &&...args) {
    auto const storedType = control_.storedType();

    if (storedType == no_tag()) {
      return false;
    }

    traits::template visit(
      storedType, union_, std::forward<TVisitor>(visitor),
      std::forward<UArgs>(args)...
    );
    return true;
  }

  template <typename U, typename std::enable_if<
    is_supported<typename std::decay<U>::type>(), int
  >::type = 0>
  bool try_set(U &&value) {
    *this = std::forward<U>(value);
    return true;
  }

  template <typename U, typename std::enable_if<
    !is_supported<typename std::decay<U>::type>(), int
  >::type = 0>
  bool try_set(U &&) {
    return false;
  }

  template <typename UCallable, typename... UArgs, typename std::enable_if<
    is_supported<
      typename std::decay<
        typename std::invoke_result<UCallable&, UArgs...>::type
      >::type
    >(), int
  >::type = 0>
  bool set_result_of(UCallable &&callable, UArgs &&...args) {
    *this = callable(std::forward<UArgs>(args)...);
    return true;
  }

  template <typename UCallable, typename... UArgs, typename std::enable_if<
    !is_supported<
      typename std::decay<
        typename std::invoke_result<UCallable&, UArgs...>::type
      >::type
    >(), int
  >::type = 0>
  bool set_result_of(UCallable &&callable, UArgs &&...args) {
    callable(std::forward<UArgs>(args)...);
    return false;
  }

  template <typename U>
  U &set(U const &value) & {
    return set_impl<typename std::decay<U>::type>(value);
  }
  template <typename U, typename = enable_when::movable<U>>
  U &set(U &&value) & {
    return set_impl<typename std::decay<U>::type>(
      std::forward<U>(value)
    );
  }

  template <typename U>
  U &&set(U const &value) && {
    return std::move(set_impl<typename std::decay<U>::type>(value));
  }
  template <typename U, typename = enable_when::movable<U>>
  U &&set(U &&value) && {
    return std::move(set_impl<typename std::decay<U>::type>(
      std::forward<U>(value)
    ));
  }

  template <typename U, typename... UArgs>
  U &emplace(UArgs &&...args) & {
    return set_impl<typename std::decay<U>::type>(
      std::forward<UArgs>(args)...
    );
  }

  template <typename U, typename... UArgs>
  U &&emplace(UArgs &&...args) && {
    return std::move(set_impl<typename std::decay<U>::type>(
      std::forward<UArgs>(args)...
    ));
  }

  void clear() { unset_impl(); }
  bool empty() const { return control_.storedType() == no_tag(); }

  void swap(legacy_variant &other) {
    if (this == std::addressof(other)) {
      return;
    }

    auto const storedType = control_.storedType();
    auto const otherStoredType = other.control_.storedType();
    union_type tmp;

    if (storedType != no_tag()) {
      traits::move_over(storedType, union_, tmp);
    }

    if (otherStoredType != no_tag()) {
      traits::move_over(otherStoredType, other.union_, union_);
    }

    if (storedType != no_tag()) {
      traits::move_over(storedType, tmp, other.union_);
    }

    std::swap(control_, other.control_);
  }

  type_tag tag() const { return control_.storedType(); }

  template <typename U>
  bool is_of() const { return tag() == tag<U>(); }

  template <typename U>
  using tag_for = std::integral_constant<type_tag, legacy_variant::tag<U>()>;

  template <typename UList>
  using tags_for = typename UList::template transform<tag_for>;

  template <typename... U>
  bool is_any_of() const {
    return tags_for<type_list<U...>>::template binary_search<>::exact(
      tag(), fn::no_op()
    );
  }

  template <typename UList>
  bool is_any_from() const {
    return tags_for<UList>::template binary_search<>::exact(tag(), fn::no_op());
  }

  // for compatibility with boost::variant
  typename std::enable_if<
    traits::end_depth() <= std::numeric_limits<int>::max(), int
  >::type which() const {
    return static_cast<int>(tag());
  }

  // for compatibility with boost::variant
  template <template <typename> class... TTransforms>
  std::type_info const &type() const {
    return traits::template type<TTransforms...>(control_.storedType());
  }

  allocator_type get_allocator() const { return control_.get_allocator(); }

  legacy_variant &operator =(legacy_variant const &other)
    noexcept(
      logical::all<std::true_type, nothrow_cp_assign<Args>...>::value
    )
  {
    static_assert(
      storage_policy::is_copyable(),
      "copy assignment disabled by the variant's policy"
    );

    if (this != std::addressof(other)) {
      unset_impl();
      control_ = copy_impl(other);
    }

    return *this;
  }

  legacy_variant &operator =(legacy_variant &other)
    noexcept(logical::all<nothrow_cp_assign<Args>...>::value)
  {
    return (*this = static_cast<legacy_variant const &>(other));
  }

  legacy_variant &operator =(legacy_variant &&other)
    noexcept(logical::all<nothrow_mv_assign<Args>...>::value)
  {
    if (this != std::addressof(other)) {
      unset_impl();
      control_ = move_impl(std::move(other));
    }

    return *this;
  }

  template <typename U>
  U &operator =(U const &value) & {
    return set(value);
  }

  template <
    typename U,
    typename = safe_overload<legacy_variant, U>,
    typename = enable_when::movable<U>
  >
  U &operator =(U &&value) & {
    return set(std::forward<U>(value));
  }

  template <typename U>
  U &&operator =(U const &value) && {
    return std::move(set(value));
  }

  template <
    typename U,
    typename = safe_overload<legacy_variant, U>,
    typename = enable_when::movable<U>
  >
  U &&operator =(U &&value) && {
    return std::move(set(std::forward<U>(value)));
  }

  template <typename UStoragePolicy, typename... UArgs>
  legacy_variant &operator =(
    legacy_variant<UStoragePolicy, UArgs...> const &other
  ) {
    if (!other.visit(copy_variant_visitor<false, true>(), *this)) {
      clear();
    }

    return *this;
  }

  template <typename UStoragePolicy, typename... UArgs>
  legacy_variant &operator =(legacy_variant<UStoragePolicy, UArgs...> &other) {
    if (!other.visit(copy_variant_visitor<false, true>(), *this)) {
      clear();
    }

    return *this;
  }

  template <typename... UArgs>
  legacy_variant &operator =(legacy_variant<storage_policy, UArgs...> &&other) {
    if (control_.get_allocator() == other.control_.get_allocator()) {
      if (!other.visit(copy_variant_visitor<false, true>(), *this)) {
        clear();
      }
      other.clear();
    } else {
      // can't move when different storage policies
      // or different allocators are employed

      if (storage_policy::is_copyable()) {
        if (!other.visit(copy_variant_visitor<false, true>(), *this)) {
          clear();
        }
        other.clear();
      } else {
        throw std::invalid_argument(
          "can't move values allocated by different allocators"
          " and storage policy doesn't allow copies"
        );
      }
    }

    return *this;
  }

  template <typename UStoragePolicy, typename... UArgs>
  legacy_variant &operator =(legacy_variant<UStoragePolicy, UArgs...> &&other) {
    if (!other.visit(copy_variant_visitor<false, true>(), *this)) {
      clear();
    }
    other.clear();

    return *this;
  }

  bool operator ==(legacy_variant const &other) const {
    auto const storedType = control_.storedType();

    if (storedType != other.control_.storedType()) {
      return false;
    }

    return traits::equals(storedType, union_, other.union_);
  }

  bool operator <(legacy_variant const &other) const {
    auto const storedType = control_.storedType();
    auto const otherStoredType = other.control_.storedType();

    if (storedType != otherStoredType) {
      return storedType < otherStoredType;
    }

    return traits::less_than(storedType, union_, other.union_);
  }

  bool operator !=(legacy_variant const &other) const {
    return !(*this == other);
  }

  bool operator >(legacy_variant const &other) const {
    return other < *this;
  }

  bool operator <=(legacy_variant const &other) const {
    return !(other < *this);
  }

  bool operator >=(legacy_variant const &other) const {
    return !(*this < other);
  }

private:
  union_type union_;
  control_block control_;

  template <typename U, typename... UArgs>
  U &set_impl(UArgs &&...args) {
    auto const storedType = control_.storedType();
    auto const &allocator = control_.get_allocator();

    static_assert((is_supported<U>()), "can't set an unsupported type");

    if (storedType == tag<U>()) {
      assert(storedType != no_tag());

      auto &storage = traits::template get<U>(union_);
      storage_policy::template destroy<U>(allocator, storage);
      try {
        return storage_policy::template construct<U>(
          allocator, storage, std::forward<UArgs>(args)...
        );
      } catch(...) {
        control_.setStoredType(no_tag());
        storage_policy::template deallocate<U>(allocator, storage);
        throw;
      }
    }

    unset_impl();

    traits::allocate(allocator, tag<U>(), union_);
    try {
      auto &result = traits::template construct<U>(
        allocator, tag<U>(), union_, std::forward<UArgs>(args)...
      );
      control_.setStoredType(tag<U>());
      return result;
    } catch(...) {
      traits::deallocate(allocator, tag<U>(), union_);
      throw;
    }
  }

  void unset_impl() {
    auto const storedType = control_.storedType();
    auto const &allocator = control_.get_allocator();

    if (storedType == no_tag()) {
      return;
    }

    traits::destroy(allocator, storedType, union_);
    traits::deallocate(allocator, storedType, union_);
    control_.setStoredType(no_tag());
  }

  // assumes already unset
  control_block copy_impl(legacy_variant const &other) {
    auto const otherStoredType = other.control_.storedType();
    auto otherAllocator = other.control_.get_allocator();

    if (otherStoredType == no_tag()) {
      return other.control_;
    }

    traits::allocate(otherAllocator, otherStoredType, union_);
    try {
      traits::copy_construct(
        otherAllocator, otherStoredType, other.union_, union_
      );
    } catch(...) {
      traits::deallocate(otherAllocator, otherStoredType, union_);
      throw;
    }

    return other.control_;
  }

  // assumes already unset
  control_block move_impl(legacy_variant &&other) {
    auto const otherStoredType = other.control_.storedType();

    if (otherStoredType != no_tag()) {
      traits::move_over(otherStoredType, other.union_, union_);
    }

    control_block control(
      other.control_.get_allocator(),
      other.control_.storedType()
    );
    other.control_.setStoredType(no_tag());
    return control;
  }

  template <bool ThrowIfUnsupported, bool ClearIfUnsupported>
  class copy_variant_visitor {
    template <typename U, bool>
    struct setter {
      static void set(legacy_variant &v, U const &value) { v.set(value); }
    };

    template <typename U>
    struct setter<U, false> {
      static void set(legacy_variant &v, U const &) {
        if (ClearIfUnsupported) {
          v.clear();
        }

        if (ThrowIfUnsupported) {
          throw std::invalid_argument("type not supported");
        }
      }
    };

  public:
    template <typename U>
    void operator ()(U const &value, legacy_variant &v) {
      static_assert(
        storage_policy::is_copyable(),
        "copy disabled by the variant's policy"
      );

      using type = typename std::decay<U>::type;

      setter<type, legacy_variant::template is_supported<type>()>::set(
        v, value
      );
    }
  };
};

template <typename TStoragePolicy, typename... Args>
using variant = legacy_variant<TStoragePolicy, Args...>;

template <typename... Args>
using legacy_default_variant = legacy_variant<
  legacy_storage_policy<>,
  Args...
>;

template <typename... Args>
using default_variant = legacy_default_variant<Args...>;

template <typename... Args>
using auto_variant = variant<
  default_storage_policy<
    std::allocator<void>,
    automatic_allocation_policy
  >,
  Args...
>;

template <typename Allocator, typename... Args>
using dynamic_variant = variant<
  default_storage_policy<Allocator, dynamic_allocation_policy>,
  Args...
>;

template <typename... Args>
using default_dynamic_variant = dynamic_variant<std::allocator<void>, Args...>;

/**
 * Wraps non-derived visitors that return a value on operator()
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename TVisitor, typename TResult>
struct visitor_wrapper {
  using visitor_type = TVisitor;
  using result_type = TResult;

  explicit visitor_wrapper(visitor_type &visitor):
    visitor_(visitor)
  {}

  template <typename... UArgs>
  void operator()(UArgs &&...args) {
    result_.emplace(visitor_(std::forward<UArgs>(args)...));
  }

  bool has_value() const { return !result_.empty(); }

  fast_pass<result_type> value() const { return *result_; }

  result_type &value() { return result_.ref(); }

private:
  visitor_type &visitor_;
  optional<result_type> result_;
};

/**
 * A handy function to build visitor_wrapper instances given a visitor
 * whose operator() returns a value
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename result_type, typename visitor_type>
visitor_wrapper<visitor_type const, result_type> wrap_visitor(
  visitor_type const &visitor
) {
  return visitor_wrapper<visitor_type const, result_type>(visitor);
}

template <typename result_type, typename visitor_type>
visitor_wrapper<visitor_type, result_type> wrap_visitor(
  visitor_type &visitor
) {
  return visitor_wrapper<visitor_type, result_type>(visitor);
}

/**
 * Convenience function to visit a variant using a visitor that returns values.
 *
 * If the variant holds no values (the visitor is not actually called), this
 * function throws std::logic_error.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  typename result_type, typename visitor_type,
  typename TStoragePolicy, typename... Args,
  typename... UArgs
>
result_type visit(
  variant<TStoragePolicy, Args...> const &variant,
  visitor_type &&visitor,
  UArgs &&...args
) {
  auto wrapper = wrap_visitor<result_type>(visitor);
  variant.visit(wrapper, std::forward<UArgs>(args)...);

  if (!wrapper.has_value()) {
    throw std::logic_error("there's no value returned by the visitor");
  }

  return std::move(wrapper.value());
}

/**
 * Convenience function to visit a variant using a visitor that returns values.
 *
 * If the variant holds no values (the visitor is not actually called), this
 * function throws std::logic_error.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  typename result_type, typename visitor_type,
  typename TStoragePolicy, typename... Args,
  typename... UArgs
>
result_type visit(
  variant<TStoragePolicy, Args...> &variant,
  visitor_type &&visitor,
  UArgs &&...args
) {
  auto wrapper = wrap_visitor<result_type>(visitor);
  variant.visit(wrapper, std::forward<UArgs>(args)...);

  if (!wrapper.has_value()) {
    throw std::logic_error("there's no value returned by the visitor");
  }

  return std::move(wrapper.value());
}

/**
 * Convenience function to visit a variant using a visitor that returns values.
 *
 * If the variant holds no values (the visitor is not actually called), this
 * function throws std::logic_error.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  typename result_type, typename visitor_type,
  typename TStoragePolicy, typename... Args,
  typename... UArgs
>
result_type visit(
  variant<TStoragePolicy, Args...> &&variant,
  visitor_type &&visitor,
  UArgs &&...args
) {
  auto wrapper = wrap_visitor<result_type>(visitor);
  variant.visit(wrapper, std::forward<UArgs>(args)...);

  if (!wrapper.has_value()) {
    throw std::logic_error("there's no value returned by the visitor");
  }

  return std::move(wrapper.value());
}

/**
 * Convenience function to visit a variant using a visitor that returns values.
 *
 * If the variant holds no values (the visitor is not actually called), this
 * function throws std::logic_error.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  typename result_type, typename visitor_type,
  typename TStoragePolicy, typename... Args,
  typename... UArgs
>
result_type legacy_visit(
  variant<TStoragePolicy, Args...> const &variant,
  visitor_type &&visitor,
  UArgs &&...args
) {
  auto wrapper = wrap_visitor<result_type>(visitor);
  variant.visit(wrapper, std::forward<UArgs>(args)...);

  if (!wrapper.has_value()) {
    throw std::logic_error("there's no value returned by the visitor");
  }

  return std::move(wrapper.value());
}

/**
 * Convenience function to visit a variant using a visitor that returns values.
 *
 * If the variant holds no values (the visitor is not actually called), this
 * function throws std::logic_error.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  typename result_type, typename visitor_type,
  typename TStoragePolicy, typename... Args,
  typename... UArgs
>
result_type legacy_visit(
  variant<TStoragePolicy, Args...> &variant,
  visitor_type &&visitor,
  UArgs &&...args
) {
  auto wrapper = wrap_visitor<result_type>(visitor);
  variant.visit(wrapper, std::forward<UArgs>(args)...);

  if (!wrapper.has_value()) {
    throw std::logic_error("there's no value returned by the visitor");
  }

  return std::move(wrapper.value());
}

/**
 * Convenience function to visit a variant using a visitor that returns values.
 *
 * If the variant holds no values (the visitor is not actually called), this
 * function throws std::logic_error.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  typename result_type, typename visitor_type,
  typename TStoragePolicy, typename... Args,
  typename... UArgs
>
result_type legacy_visit(
  variant<TStoragePolicy, Args...> &&variant,
  visitor_type &&visitor,
  UArgs &&...args
) {
  auto wrapper = wrap_visitor<result_type>(visitor);
  variant.visit(wrapper, std::forward<UArgs>(args)...);

  if (!wrapper.has_value()) {
    throw std::logic_error("there's no value returned by the visitor");
  }

  return std::move(wrapper.value());
}

/**
 * Convenience function to visit a variant using a visitor that returns values.
 *
 * If the variant holds no values (the visitor is not actually called), this
 * function returns `defaultValue`.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  typename result_type, typename visitor_type,
  typename TStoragePolicy, typename... Args,
  typename... UArgs
>
result_type visit_def(
  variant<TStoragePolicy, Args...> const &variant,
  visitor_type &&visitor,
  result_type defaultValue,
  UArgs &&...args
) {
  auto wrapper = wrap_visitor<result_type>(visitor);
  variant.visit(wrapper, std::forward<UArgs>(args)...);

  return wrapper.has_value()
    ? std::move(wrapper.value())
    : defaultValue;
}

/**
 * Convenience function to visit a variant using a visitor that returns values.
 *
 * If the variant holds no values (the visitor is not actually called), this
 * function returns `defaultValue`.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  typename result_type, typename visitor_type,
  typename TStoragePolicy, typename... Args,
  typename... UArgs
>
result_type visit_def(
  variant<TStoragePolicy, Args...> &variant,
  visitor_type &&visitor,
  result_type defaultValue,
  UArgs &&...args
) {
  auto wrapper = wrap_visitor<result_type>(visitor);
  variant.visit(wrapper, std::forward<UArgs>(args)...);

  return wrapper.has_value()
    ? std::move(wrapper.value())
    : defaultValue;
}

/**
 * Convenience function to visit a variant using a visitor that returns values.
 *
 * If the variant holds no values (the visitor is not actually called), this
 * function returns `defaultValue`.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  typename result_type, typename visitor_type,
  typename TStoragePolicy, typename... Args,
  typename... UArgs
>
result_type visit_def(
  variant<TStoragePolicy, Args...> &&variant,
  visitor_type &&visitor,
  result_type defaultValue,
  UArgs &&...args
) {
  auto wrapper = wrap_visitor<result_type>(visitor);
  variant.visit(wrapper, std::forward<UArgs>(args)...);

  return wrapper.has_value()
    ? std::move(wrapper.value())
    : defaultValue;
}

/**
 * Convenience function to visit a variant using a visitor that returns values.
 *
 * If the variant holds no values (the visitor is not actually called), this
 * function returns `defaultValue`.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  typename result_type, typename visitor_type,
  typename TStoragePolicy, typename... Args,
  typename... UArgs
>
result_type legacy_visit_def(
  variant<TStoragePolicy, Args...> const &variant,
  visitor_type &&visitor,
  result_type defaultValue,
  UArgs &&...args
) {
  auto wrapper = wrap_visitor<result_type>(visitor);
  variant.visit(wrapper, std::forward<UArgs>(args)...);

  return wrapper.has_value()
    ? std::move(wrapper.value())
    : defaultValue;
}

/**
 * Convenience function to visit a variant using a visitor that returns values.
 *
 * If the variant holds no values (the visitor is not actually called), this
 * function returns `defaultValue`.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  typename result_type, typename visitor_type,
  typename TStoragePolicy, typename... Args,
  typename... UArgs
>
result_type legacy_visit_def(
  variant<TStoragePolicy, Args...> &variant,
  visitor_type &&visitor,
  result_type defaultValue,
  UArgs &&...args
) {
  auto wrapper = wrap_visitor<result_type>(visitor);
  variant.visit(wrapper, std::forward<UArgs>(args)...);

  return wrapper.has_value()
    ? std::move(wrapper.value())
    : defaultValue;
}

/**
 * Convenience function to visit a variant using a visitor that returns values.
 *
 * If the variant holds no values (the visitor is not actually called), this
 * function returns `defaultValue`.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <
  typename result_type, typename visitor_type,
  typename TStoragePolicy, typename... Args,
  typename... UArgs
>
result_type legacy_visit_def(
  variant<TStoragePolicy, Args...> &&variant,
  visitor_type &&visitor,
  result_type defaultValue,
  UArgs &&...args
) {
  auto wrapper = wrap_visitor<result_type>(visitor);
  variant.visit(wrapper, std::forward<UArgs>(args)...);

  return wrapper.has_value()
    ? std::move(wrapper.value())
    : defaultValue;
}

} // namespace fatal {

namespace std {

template <typename TStoragePolicy, typename... Args>
struct hash<fatal::variant<TStoragePolicy, Args...>> {
  using result_type = std::size_t;

  result_type operator ()(
    fatal::variant<TStoragePolicy, Args...> const &v
  ) const {
    hash_visitor visitor(std::hash<decltype(v.tag())>()(v.tag()));

    v.visit(visitor);

    return visitor.result;
  }

private:
  struct hash_visitor {
    explicit hash_visitor(result_type seed): result(std::move(seed)) {}

    template <typename U>
    void operator ()(U const &value) {
      result ^= std::hash<typename std::decay<U>::type>()(value);
    }

    result_type result;
  };
};

} // namespace std {

FATAL_DIAGNOSTIC_POP
