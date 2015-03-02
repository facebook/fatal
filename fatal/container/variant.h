/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_container_variant_h
#define FATAL_INCLUDE_fatal_container_variant_h

#include <fatal/math/numerics.h>
#include <fatal/type/traits.h>
#include <fatal/type/list.h>

#include <memory>
#include <utility>
#include <functional>

#include <cassert>

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
template <
  std::size_t allocateMultiplier = 1,
  std::size_t allocateIncrement = sizeof(std::size_t)
>
class default_allocation_policy {
  template <typename T>
  constexpr static std::size_t allocateThreshold() {
    return allocateMultiplier * sizeof(T*) + allocateIncrement;
  }

public:
  template <typename T>
  constexpr static bool allocate_dynamically() {
    static_assert(!std::is_reference<T>::value, "T can't be a reference");
    return !std::is_scalar<T>::value && allocateThreshold<T>() < sizeof(T);
  }
};

template <bool alwaysDynamic>
struct fixed_allocation_policy {
  template <typename T>
  constexpr static bool allocate_dynamically() { return alwaysDynamic; }
};

typedef fixed_allocation_policy<true> dynamic_allocation_policy;
typedef fixed_allocation_policy<false> automatic_allocation_policy;

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
 *  static void allocate(TAllocator &, typename storage_type<T>::type &);
 *  static void deallocate(TAllocator &, typename storage_type<T>::type &);
 *  static T &construct(TAllocator &, typename storage_type<T>::type &, ...);
 *  static void destroy(TAllocator &, typename storage_type<T>::type &);
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
 * type. It needs to provide a single method
 *
 *  template <typename T> constexpr static bool allocate_dynamically();
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
struct default_storage_policy {
  typedef TAllocator allocator_type;
  typedef std::allocator_traits<allocator_type> allocator_traits;
  typedef TAllocationPolicy allocation_policy;

  /**
   * Tells whether copy construction and copy assignment should be allowed by
   * the variant. Checked at compile time.
   */
  constexpr static bool is_copyable() { return IsCopyable; }

  /**
   * This class abstracts the interaction with TAllocationPolicy and
   * adds support for incomplete types. It provides a typedef for the
   * internal storage representation of the given type T, and a boolean
   * telling whether this representation uses automatic or dynamic storage.
   */
  template <typename T>
  class storage_type {
    static_assert(
      !std::is_reference<T>::value,
      "default_storage_policy: unable to store references"
    );

    // Helper intended for complete types.
    template <typename U, bool>
    struct storage_type_impl:
      public std::integral_constant<
        bool,
        allocation_policy::template allocate_dynamically<U>()
      >
    {
      // A union is used in order to facilitate uninitialized automatic storage.
      union automatically_allocated{
        automatically_allocated() {}
        automatically_allocated(automatically_allocated const &) {}
        automatically_allocated(automatically_allocated &&) {}
        ~automatically_allocated() {}
        U value;
      };

      // A naked pointer is used for dynamically allocated types. no smart
      // pointers are used since we aim for the smallest possible footprint
      // (having a copy of the deleter for every pointer is undesired).
      typedef typename std::conditional<
        allocation_policy::template allocate_dynamically<U>(),
        U *, automatically_allocated
      >::type type;
    };

    // Template partial specialization for incomplete types
    // dynamic allocation will always be used since there's
    // no layout information for the type.
    template <typename U>
    struct storage_type_impl<U, false>:
      public std::true_type
    {
      typedef U *type;
    };

  public:
    typedef typename storage_type_impl<T, is_complete<T>::value>::type type;

    constexpr static bool allocate_dynamically() {
      return storage_type_impl<T, is_complete<T>::value>::value;
    }
  };

  // Methods for when T is stored using DYNAMIC ALLOCATION.
  //
  // The methods below are guaranteed to work on `T *` due to:
  //  typename std::enable_if<
  //    storage_type<T>::allocate_dynamically(),
  //    ...

  template <typename T, typename... Args>
  static void allocate(
    allocator_type *allocator,
    typename std::enable_if<
      storage_type<T>::allocate_dynamically(), typename storage_type<T>::type
    >::type &storage
  ) {
    typedef typename allocator_traits::template rebind_alloc<T> rebound_alloc;
    storage = rebound_alloc(*allocator).allocate(1);
  }

  template <typename T>
  static void deallocate(
    allocator_type *allocator,
    typename std::enable_if<
      storage_type<T>::allocate_dynamically(), typename storage_type<T>::type
    >::type &storage
  ) {
    typedef typename allocator_traits::template rebind_alloc<T> rebound_alloc;
    rebound_alloc(*allocator).deallocate(storage, 1);
    storage = nullptr;
  }

  template <typename T, typename... Args>
  static T &construct(
    allocator_type *allocator,
    typename std::enable_if<
      storage_type<T>::allocate_dynamically(), typename storage_type<T>::type
    >::type &storage,
    Args &&...args
  ) {
    typedef typename allocator_traits::template rebind_alloc<T> rebound_alloc;
    rebound_alloc(*allocator).construct(storage, std::forward<Args>(args)...);
    return *storage;
  }

  template <typename T>
  static void destroy(
    allocator_type *allocator,
    typename std::enable_if<
      storage_type<T>::allocate_dynamically(), typename storage_type<T>::type
    >::type &storage
  ) {
    typedef typename allocator_traits::template rebind_alloc<T> rebound_alloc;
    rebound_alloc(*allocator).destroy(storage);
  }

  template <typename T>
  static void move_over(
    typename std::enable_if<
      storage_type<T>::allocate_dynamically(), typename storage_type<T>::type
    >::type &from,
    typename storage_type<T>::type &to
  ) {
    to = from;
    from = nullptr;
  }

  template <typename T>
  static T const &get(
    typename std::enable_if<
      storage_type<T>::allocate_dynamically(), typename storage_type<T>::type
    >::type const &storage
  ) {
    return *storage;
  }

  template <typename T>
  static T &get(
    typename std::enable_if<
      storage_type<T>::allocate_dynamically(), typename storage_type<T>::type
    >::type &storage
  ) {
    return *storage;
  }

  // Methods for when T is stored using AUTOMATIC ALLOCATION.
  //
  // The methods below are guaranteed to work on `union { T value; };` due to:
  //  typename std::enable_if<
  //    !storage_type<T>::allocate_dynamically(),
  //    ...

  template <typename T, typename... Args>
  static void allocate(
    allocator_type *,
    typename std::enable_if<
      !storage_type<T>::allocate_dynamically(), typename storage_type<T>::type
    >::type &
  ) {}

  template <typename T>
  static void deallocate(
    allocator_type *,
    typename std::enable_if<
      !storage_type<T>::allocate_dynamically(), typename storage_type<T>::type
    >::type &
  ) {}

  template <typename T, typename... Args>
  static T &construct(
    allocator_type *,
    typename std::enable_if<
      !storage_type<T>::allocate_dynamically(), typename storage_type<T>::type
    >::type &storage,
    Args &&...args
  ) {
    return *new (std::addressof(storage.value)) T(std::forward<Args>(args)...);
  }

  template <typename T>
  static void destroy(
    allocator_type *,
    typename std::enable_if<
      !storage_type<T>::allocate_dynamically(), typename storage_type<T>::type
    >::type &storage
  ) {
    storage.value.~T();
  }

  template <typename T>
  static void move_over(
    typename std::enable_if<
      !storage_type<T>::allocate_dynamically(), typename storage_type<T>::type
    >::type &from,
    typename storage_type<T>::type &to
  ) {
    new (std::addressof(to.value)) T(std::move(from.value));
    from.value.~T();
  }

  template <typename T>
  static T const &get(
    typename std::enable_if<
      !storage_type<T>::allocate_dynamically(), typename storage_type<T>::type
    >::type const &storage
  ) {
    return storage.value;
  }

  template <typename T>
  static T &get(
    typename std::enable_if<
      !storage_type<T>::allocate_dynamically(), typename storage_type<T>::type
    >::type &storage
  ) {
    return storage.value;
  }
};

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

// base case
template <
  typename TStoragePolicy,
  typename TSize, TSize Depth, typename T
>
struct variadic_union_traits<TStoragePolicy, TSize, Depth, T> {
  static_assert(std::is_integral<TSize>::value, "TSize must be an integral");
  static_assert(!std::is_reference<T>::value, "T can't be a reference");
  static_assert(!std::is_const<T>::value, "T can't be const");

  typedef TSize size_type;
  typedef T value_type;

  typedef TStoragePolicy storage_policy;
  typedef typename storage_policy::allocator_type allocator_type;
  typedef typename storage_policy::template storage_type<value_type>::type
    storage_type;

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
    assert(depth == Depth);
    return typeid(
      typename fatal::try_transform<
        fatal::transform_sequence<TTransforms...>::template apply
      >::template apply<value_type>
    );
  }

  template <typename TVisitor, typename... UArgs>
  static void visit(
    size_type const depth, union_type const &u, TVisitor &&visitor,
    UArgs &&...args
  ) {
    assert(depth == Depth);
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
    assert(depth == Depth);
    visitor(
      storage_policy::template get<value_type>(u.storage),
      std::forward<UArgs>(args)...
    );
  }

  static void allocate(
    allocator_type *allocator, size_type const depth, union_type &u
  ) {
    assert(depth == Depth);
    storage_policy::template allocate<value_type>(allocator, u.storage);
  }

  static void deallocate(
    allocator_type *allocator, size_type const depth, union_type &u
  ) {
    assert(depth == Depth);
    storage_policy::template deallocate<value_type>(allocator, u.storage);
  }

  static void copy_construct(
    allocator_type *allocator, size_type const depth,
    union_type const &from, union_type &to
  ) {
    assert(depth == Depth);
    storage_policy::template construct<value_type>(
      allocator, to.storage,
      storage_policy::template get<value_type>(from.storage)
    );
  }

  static void destroy(
    allocator_type *allocator, size_type const depth, union_type &u
  ) {
    assert(depth == Depth);
    storage_policy::template destroy<value_type>(allocator, u.storage);
  }

  static void move_over(
    size_type const depth, union_type &from, union_type &to
  ) {
    assert(depth == Depth);
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

  template <typename U>
  static storage_type const &get(
    typename std::enable_if<
      std::is_same<U, value_type>::value, union_type
    >::type const &u
  ) {
    return u.storage;
  }

  template <typename U>
  static storage_type &get(
    typename std::enable_if<
      std::is_same<U, value_type>::value, union_type
    >::type &u
  ) {
    return u.storage;
  }

  template <typename U, typename... UArgs>
  static value_type &construct(
    allocator_type *allocator,
    typename std::enable_if<
      std::is_same<U, value_type>::value,
      size_type
    >::type const depth,
    union_type &u,
    UArgs &&...args
  ) {
    assert(depth == Depth);
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

  typedef TSize size_type;
  typedef T value_type;

  typedef TStoragePolicy storage_policy;
  typedef typename storage_policy::allocator_type allocator_type;
  typedef typename storage_policy::template storage_type<value_type>::type
    storage_type;

  typedef variant_impl::variadic_union_traits<
    storage_policy, size_type, Depth, T
  > head_type;
  typedef variant_impl::variadic_union_traits<
    storage_policy, size_type, Depth + 1, Head, Tail...
  > tail_type;

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
    allocator_type *allocator, size_type const depth, union_type &u
  ) {
    if (depth == Depth) {
      head_type::allocate(allocator, depth, u.head);
    } else {
      tail_type::allocate(allocator, depth, u.tail);
    }
  }

  static void deallocate(
    allocator_type *allocator, size_type const depth, union_type &u
  ) {
    if (depth == Depth) {
      head_type::deallocate(allocator, depth, u.head);
    } else {
      tail_type::deallocate(allocator, depth, u.tail);
    }
  }

  static void copy_construct(
    allocator_type *allocator, size_type const depth,
    union_type const &from, union_type &to
  ) {
    if (depth == Depth) {
      head_type::copy_construct(allocator, depth, from.head, to.head);
    } else {
      tail_type::copy_construct(allocator, depth, from.tail, to.tail);
    }
  }

  static void destroy(
    allocator_type *allocator, size_type const depth, union_type &u
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

  template <typename U>
  static storage_type const &get(
    typename std::enable_if<
      std::is_same<U, value_type>::value, union_type
    >::type const &u
  ) {
    return head_type::template get<U>(u.head);
  }

  template <typename U>
  static storage_type &get(
    typename std::enable_if<
      std::is_same<U, value_type>::value, union_type
    >::type &u
  ) {
    return head_type::template get<U>(u.head);
  }

  template <typename U>
  static typename storage_policy::template storage_type<U>::type const &get(
    typename std::enable_if<
      !std::is_same<U, value_type>::value, union_type
    >::type const &u
  ) {
    return tail_type::template get<U>(u.tail);
  }

  template <typename U>
  static typename storage_policy::template storage_type<U>::type &get(
    typename std::enable_if<
      !std::is_same<U, value_type>::value, union_type
    >::type &u
  ) {
    return tail_type::template get<U>(u.tail);
  }

  template <typename U, typename... UArgs>
  static U &construct(
    allocator_type *allocator,
    typename std::enable_if<
      std::is_same<U, value_type>::value,
      size_type
    >::type const depth,
    union_type &u,
    UArgs &&...args
  ) {
    return head_type::template construct<U>(
      allocator, depth, u.head, std::forward<UArgs>(args)...
    );
  }

  template <typename U, typename... UArgs>
  static U &construct(
    allocator_type *allocator,
    typename std::enable_if<
      !std::is_same<U, value_type>::value,
      size_type
    >::type const depth,
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
  template <bool Condition, typename U, typename UVisitor, typename... UArgs>
  typename std::enable_if<Condition,  bool>::type visit(
    U &&value, UVisitor &&visitor, UArgs &&...args
  ) const {
    visitor(std::forward<U>(value), std::forward<UArgs>(args)...);
    return true;
  }

  template <bool Condition, typename U, typename UVisitor, typename... UArgs>
  typename std::enable_if<!Condition,  bool>::type visit(
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
    assert(tag == variant.tag());
    assert(Index == variant.tag());

    result = visit<UCondition<U>::value>(
      variant.template get<U>(),
      std::forward<UVisitor>(visitor),
      std::forward<UArgs>(args)...
    );
  }
};

} // namespace variant_impl {
} // namespace detail {

template <typename, typename...>
struct variant;

template <typename, typename...>
struct is_variant:
  public std::false_type
{};

template <typename UStoragePolicy, typename... UArgs>
struct is_variant<variant<UStoragePolicy, UArgs...>>:
  public std::true_type
{};

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
template <typename TStoragePolicy, typename... Args>
struct variant {
  typedef TStoragePolicy storage_policy;
  typedef typename storage_policy::allocator_type allocator_type;
  typedef type_list<Args...> types;

private:
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

    typedef smallest_uint_for_value<sizeof...(Args)> type_tag;

    control_block(allocator_type *allocator, type_tag storedType):
      allocator_(std::move(allocator)),
      tag_(std::move(storedType))
    {}

    fast_pass<type_tag> storedType() const { return tag_; }
    void setStoredType(type_tag tag) { tag_ = std::move(tag); }

    allocator_type *allocator() const { return allocator_; }

    void set_allocator(allocator_type *allocator) {
      allocator_ = std::move(allocator);
    }

    allocator_type *allocator_;
    type_tag tag_;
  };

public:
  typedef typename control_block::type_tag type_tag;
  typedef detail::variant_impl::variadic_union_traits<
    storage_policy,
    type_tag, std::numeric_limits<type_tag>::min(),
    typename std::decay<Args>::type...
  > traits;
  typedef typename traits::union_type union_type;

  // returns no_tag() when the given type is not part of the variant
  template <typename U>
  constexpr static type_tag tag() { return traits::template get_depth<U>(); }

  // the tag used when the variant is empty
  constexpr static type_tag no_tag() { return traits::end_depth(); }

  // returns true iff this type can be stored in the variant
  template <typename U>
  constexpr static bool is_supported() { return tag<U>() != no_tag(); }

  /**
   * The allocator starts uninitialized (nullptr)
   * That's fine when dealing with types that will use automatic
   * storage, but for dynamically allocated ones set_allocator()
   * must be called before setting anything.
   *
   * This method is provided mainly for compatibility with some
   * standard containers.
   */
  explicit variant():
    control_(nullptr, no_tag())
  {}

  explicit variant(allocator_type &allocator):
    control_(std::addressof(allocator), no_tag())
  {}

  variant(variant const &other):
    control_(copy_impl(other))
  {
    static_assert(
      storage_policy::is_copyable(),
      "copy construction disabled by the variant's policy"
    );
  }

  /* may throw */
  variant(variant &&other):
    control_(move_impl(std::move(other)))
  {}

  template <typename U>
  explicit variant(allocator_type *allocator, U &&value):
    control_(allocator, no_tag())
  {
    set<typename std::decay<U>::type>(std::forward<U>(value));
  }

  template <typename U>
  explicit variant(allocator_type &allocator, U &&value):
    variant(std::addressof(allocator), std::forward<U>(value))
  {}

  template <typename... UArgs>
  variant(variant<storage_policy, UArgs...> const &other):
    variant(other.allocator())
  {
    other.visit(copy_variant_visitor<true, false>(), *this);
  }

  template <typename UStoragePolicy, typename... UArgs>
  variant(
    allocator_type &allocator,
    variant<UStoragePolicy, UArgs...> const &other
  ):
    variant(allocator)
  {
    other.visit(copy_variant_visitor<true, false>(), *this);
  }

  ~variant() { unset_impl(control_.allocator()); }

  /**
   * The unchecked_get won't perform type checks. The user must ensure the
   * requested type matches the one stored in the variant. Provided for
   * performance sensitive scenarios.
   */
  template <typename U>
  U const &unchecked_get() const {
    return storage_policy::template get<U>(
      traits::template get<U>(union_)
    );
  }

  template <typename U>
  U &unchecked_get() {
    return storage_policy::template get<U>(
      traits::template get<U>(union_)
    );
  }

  template <typename U>
  U const &get() const {
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
  U &get() {
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
  U const *try_get() const {
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
  U *try_get() {
    if (tag<U>() != control_.storedType()) {
      return nullptr;
    }

    return std::addressof(
      storage_policy::template get<U>(
        traits::template get<U>(union_)
      )
    );
  }

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

  template <typename U>
  auto try_set(U &&value)
    -> typename std::enable_if<
      is_supported<typename std::decay<U>::type>(), bool
    >::type
  {
    *this = std::forward<U>(value);
    return true;
  }

  template <typename U>
  auto try_set(U &&value)
    -> typename std::enable_if<
      !is_supported<typename std::decay<U>::type>(), bool
    >::type
  {
    return false;
  }

  template <typename UCallable, typename... UArgs>
  auto set_result_of(UCallable &&callable, UArgs &&...args)
    -> typename std::enable_if<
      is_supported<
        typename std::decay<
          decltype(callable(std::forward<UArgs>(args)...))
        >::type
      >(), bool
    >::type
  {
    *this = callable(std::forward<UArgs>(args)...);
    return true;
  }

  template <typename UCallable, typename... UArgs>
  auto set_result_of(UCallable &&callable, UArgs &&...args)
    -> typename std::enable_if<
      !is_supported<
        typename std::decay<
          decltype(callable(std::forward<UArgs>(args)...))
        >::type
      >(), bool
    >::type
  {
    callable(std::forward<UArgs>(args)...);
    return false;
  }

  template <typename U>
  U &set(U const &value) {
    return set_impl<typename std::decay<U>::type>(value);
  }
  template <typename U>
  U &set(U &&value) {
    return set_impl<typename std::decay<U>::type>(std::move(value));
  }

  template <typename U, typename... UArgs>
  U &emplace(UArgs &&...args) {
    return set_impl<typename std::decay<U>::type>(
      std::forward<UArgs>(args)...
    );
  }

  void clear() { unset_impl(control_.allocator()); }
  bool empty() const { return control_.storedType() == no_tag(); }

  void swap(variant &other) {
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
  using tag_for = std::integral_constant<type_tag, variant::tag<U>()>;

  template <typename UList>
  using tags_for = typename UList::template transform<tag_for>;

  template <typename... U>
  bool is_any_of() const {
    return tags_for<type_list<U...>>::template binary_search<>::exact(
      tag(), no_op_visitor()
    );
  }

  template <typename UList>
  bool is_any_from() const {
    return tags_for<UList>::template binary_search<>::exact(
      tag(), no_op_visitor()
    );
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

  allocator_type &allocator() const { return *control_.allocator(); }
  bool has_allocator() const { return control_.allocator() != nullptr; }

  void set_allocator(allocator_type &allocator) {
    control_.set_allocator(std::addressof(allocator));
  }

  variant &operator =(variant const &other) {
    static_assert(
      storage_policy::is_copyable(),
      "copy assignment disabled by the variant's policy"
    );

    if (this != std::addressof(other)) {
      unset_impl(control_.allocator());
      control_ = copy_impl(other);
    }

    return *this;
  }

  variant &operator =(variant &other) {
    return (*this = static_cast<variant const &>(other));
  }

  variant &operator =(variant &&other) {
    if (this != std::addressof(other)) {
      unset_impl(control_.allocator());
      control_ = move_impl(std::move(other));
    }

    return *this;
  }

  template <typename U>
  U const &operator =(U const &value) {
    return set<U>(value);
  }

  template <typename U, typename = safe_overload_t<variant, U>>
  U &operator =(U &&value) {
    return set<U>(std::forward<U>(value));
  }

  template <typename UStoragePolicy, typename... UArgs>
  variant &operator =(variant<UStoragePolicy, UArgs...> const &other) {
    if (!other.visit(copy_variant_visitor<false, true>(), *this)) {
      clear();
    }

    return *this;
  }

  template <typename UStoragePolicy, typename... UArgs>
  variant &operator =(variant<UStoragePolicy, UArgs...> &other) {
    if (!other.visit(copy_variant_visitor<false, true>(), *this)) {
      clear();
    }

    return *this;
  }

  template <typename... UArgs>
  variant &operator =(variant<storage_policy, UArgs...> &&other) {
    if (allocator() == other.allocator()) {
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
  variant &operator =(variant<UStoragePolicy, UArgs...> &&other) {
    if (!other.visit(copy_variant_visitor<false, true>(), *this)) {
      clear();
    }
    other.clear();

    return *this;
  }

  bool operator ==(variant const &other) const {
    auto const storedType = control_.storedType();

    if (storedType != other.control_.storedType()) {
      return false;
    }

    return traits::equals(storedType, union_, other.union_);
  }

  bool operator <(variant const &other) const {
    auto const storedType = control_.storedType();
    auto const otherStoredType = other.control_.storedType();

    if (storedType != otherStoredType) {
      return storedType < otherStoredType;
    }

    return traits::less_than(storedType, union_, other.union_);
  }

  bool operator !=(variant const &other) const { return !(*this == other); }
  bool operator >(variant const &other) const { return other < *this; }
  bool operator <=(variant const &other) const { return !(other < *this); }
  bool operator >=(variant const &other) const { return !(*this < other); }

private:
  union_type union_;
  control_block control_;

  template <typename U, typename... UArgs>
  U &set_impl(UArgs &&...args) {
    auto const storedType = control_.storedType();
    auto allocator = control_.allocator();

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

    unset_impl(allocator);

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

  void unset_impl(allocator_type *allocator) {
    auto const storedType = control_.storedType();

    if (storedType == no_tag()) {
      return;
    }

    traits::destroy(allocator, storedType, union_);
    traits::deallocate(allocator, storedType, union_);
    control_.setStoredType(no_tag());
  }

  // assumes already unset
  control_block copy_impl(variant const &other) {
    auto const otherStoredType = other.control_.storedType();
    auto otherAllocator = other.control_.allocator();

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
  control_block move_impl(variant &&other) {
    auto const otherStoredType = other.control_.storedType();

    if (otherStoredType != no_tag()) {
      traits::move_over(otherStoredType, other.union_, union_);
    }

    auto control = std::move(other.control_);
    other.control_.setStoredType(no_tag());
    return std::move(control);
  }

  template <bool ThrowIfUnsupported, bool ClearIfUnsupported>
  class copy_variant_visitor {
    template <typename U, bool>
    struct setter {
      static void set(variant &v, U const &value) { v.template set<U>(value); }
    };

    template <typename U>
    struct setter<U, false> {
      static void set(variant &v, U const &) {
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
    void operator ()(U const &value, variant &v) {
      static_assert(
        storage_policy::is_copyable(),
        "copy disabled by the variant's policy"
      );

      typedef typename std::decay<U>::type type;

      setter<type, variant::template is_supported<type>()>::set(v, value);
    }
  };
};

template <typename... Args>
using default_variant = variant<
  default_storage_policy<>,
  Args...
>;

template <typename... Args>
using auto_variant = variant<
  default_storage_policy<
    std::allocator<void>,
    automatic_allocation_policy
  >,
  Args...
>;

template <typename... Args>
using default_dynamic_variant = variant<
  default_storage_policy<
    std::allocator<void>,
    dynamic_allocation_policy
  >,
  Args...
>;

/**
 * Wraps non-derived visitors that return a value on operator()
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename TVisitor, typename TResult>
struct visitor_wrapper {
  typedef TVisitor visitor_type;
  typedef TResult result_type;

  explicit visitor_wrapper(visitor_type &visitor):
    visitor_(visitor),
    has_value_(false)
  {}

  template <typename... UArgs>
  void operator()(UArgs &&...args) {
    new (std::addressof(result_.value)) result_type(
      visitor_(std::forward<UArgs>(args)...)
    );

    has_value_ = true;
  }

  bool has_value() const { return has_value_; }

  fast_pass<result_type> value() const {
    assert(has_value_);
    return result_.value;
  }

  result_type &value() {
    assert(has_value_);
    return result_.value;
  }

private:
  visitor_type &visitor_;
  union placeholder {
    placeholder() {}
    placeholder(placeholder const &) {}
    placeholder(placeholder &&) {}
    ~placeholder() {}

    result_type value;
  } result_;
  bool has_value_;
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

} // namespace fatal {

namespace std {

template <typename TStoragePolicy, typename... Args>
struct hash<fatal::variant<TStoragePolicy, Args...>> {
  typedef std::size_t result_type;

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

#endif // FATAL_INCLUDE_fatal_container_variant_h
