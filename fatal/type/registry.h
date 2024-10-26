/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <type_traits>

namespace fatal {
namespace detail {
namespace registry_impl {
template <typename, typename> class lookup;
struct abort_on_not_found;
} // namespace registry_impl {
} // namespace detail {

/**
 * Associates a type pair `Tag`/`Key` with a type, so thay it can be looked up
 * from any scope.
 *
 * NOTE: this macro must be called from the same namespace of either the `Tag`
 * or `Key`, or some other namespace that respects the C++ rules for Argument
 * Dependent Lookup (http://en.cppreference.com/w/cpp/language/adl).
 *
 * See `registry_lookup` and `try_registry_lookup` below for more information
 * on how to query the registry.
 *
 * Example:
 *
 *  struct algo {
 *    FATAL_STR(quick_sort, "quicksort");
 *    FATAL_STR(merge_sort, "merge sort");
 *    FATAL_STR(insertion_sort, "insertion sort");
 *  };
 *
 *  struct prop {
 *    struct sorting {};
 *    struct adaptive {};
 *    struct stable {};
 *    struct in_place {};
 *  };
 *
 *  FATAL_REGISTER_TYPE(algo::quick_sort, prop::adaptive, std::false_type);
 *  FATAL_REGISTER_TYPE(algo::quick_sort, prop::stable, std::false_type);
 *  FATAL_REGISTER_TYPE(algo::quick_sort, prop::in_place, std::true_type);
 *
 *  FATAL_REGISTER_TYPE(algo::merge_sort, prop::adaptive, std::false_type);
 *  FATAL_REGISTER_TYPE(algo::merge_sort, prop::stable, std::true_type);
 *  FATAL_REGISTER_TYPE(algo::merge_sort, prop::in_place, std::false_type);
 *
 *  FATAL_REGISTER_TYPE(algo::insertion_sort, prop::adaptive, std::true_type);
 *  FATAL_REGISTER_TYPE(algo::insertion_sort, prop::stable, std::true_type);
 *  FATAL_REGISTER_TYPE(algo::insertion_sort, prop::in_place, std::true_type);
 *
 *  FATAL_REGISTER_TYPE(
 *    algo,
 *    prop::sorting,
 *    type_list<algo::quick_sort, algo::merge_sort, algo::insertion_sort>
 *  );
 *
 *  struct print_info_visitor {
 *    template <typename Algorithm, std::size_t Index>
 *    void operator ()(indexed_type_tag<Algorithm, Index>) const {
 *      std::cout << Algorithm::z_data() << '{'
 *        << " adaptive:" << registry_lookup<Algorithm, prop::adaptive>::value
 *        << " stable:" << registry_lookup<Algorithm, prop::stable>::value
 *        << " in-place:" << registry_lookup<Algorithm, prop::in_place>::value
 *        << " }" << std::endl;
 *    }
 *  };
 *
 *  // prints `
 *  //   quicksort { adaptive:0 stable:0 in-place:1 }
 *  //   merge sort { adaptive:0 stable:1 in-place:0 }
 *  //   insertion sort { adaptive:1 stable:1 in-place:1 }
 *  // `
 *  registry_lookup<algo, prop::sorting>::foreach(print_info_visitor());
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_REGISTER_TYPE(Tag, Key, ...) \
  __VA_ARGS__ operator <<(Tag, Key *)

/**
 * Looks up a type previously associated by `FATAL_REGISTER_TYPE` with the type
 * pair `Tag`/`Key`.
 *
 * If the type was not previosly associated then `Default` will be returned.
 *
 * Example:
 *
 *  struct my_tag {};
 *  struct my_key_1 {};
 *  struct my_key_2 {};
 *  struct my_metadata_1 {};
 *
 *  FATAL_REGISTER_TYPE(my_tag, my_key_1, my_metadata_1);
 *
 *  // yields `my_metadata_1`
 *  using result1 = try_registry_lookup<my_tag, my_key_1, void>;
 *
 *  // yields `void`
 *  using result2 = try_registry_lookup<my_tag, my_key_2, void>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename Tag, typename Key, typename Default>
using try_registry_lookup = typename detail::registry_impl::lookup<Tag, Default>
  ::template key<Key>;

/**
 * Looks up a type previously associated by `FATAL_REGISTER_TYPE` with the type
 * pair `Tag`/`Key`.
 *
 * If the type was not previosly associated then the compilation will fail.
 *
 * Example:
 *
 *  struct my_tag {};
 *  struct my_key_1 {};
 *  struct my_key_2 {};
 *  struct my_metadata_1 {};
 *
 *  FATAL_REGISTER_TYPE(my_tag, my_key_1, my_metadata_1);
 *
 *  // yields `my_metadata_1`
 *  using result1 = registry_lookup<my_tag, my_key_1>;
 *
 *  // fails compilation
 *  using result2 = registry_lookup<my_tag, my_key_2>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename Tag, typename Key>
using registry_lookup = try_registry_lookup<
  Tag, Key, detail::registry_impl::abort_on_not_found
>;

////////////////////////////
// IMPLEMENTATION DETAILS //
////////////////////////////

namespace detail {
namespace registry_impl {

template <typename Key>
struct registry_key {
  using key = Key;
};

template <typename Tag, typename Default>
class lookup {
  struct impl {
    template <
      typename Key,
      typename Metadata = decltype(
        std::declval<Tag>() << static_cast<Key *>(nullptr)
      )
    >
    static Metadata sfinae(registry_key<Key> *);

    template <typename... Args>
    static Default sfinae(...);
  };

public:
  template <typename Key>
  using key = decltype(
    impl::sfinae(static_cast<registry_key<Key> *>(nullptr))
  );
};

template <typename Tag>
class lookup<Tag, abort_on_not_found> {
public:
  template <typename Key>
  using key = decltype(std::declval<Tag>() << static_cast<Key *>(nullptr));
};

} // namespace registry_impl {
} // namespace detail {
} // namespace fatal {
