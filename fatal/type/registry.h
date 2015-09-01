/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_registry_h
#define FATAL_INCLUDE_fatal_type_registry_h

#include <type_traits>

namespace fatal {
namespace detail {
namespace registry_impl {

template <typename, typename> class lookup;

struct abort_on_not_found;

} // namespace registry_impl {
} // namespace detail {

// TODO: DOCUMENT AND TEST
#define FATAL_REGISTER_TYPE(Tag, Key, Metadata) \
  Metadata operator <<(Tag, Key *)

// TODO: DOCUMENT AND TEST
template <
  typename Tag,
  typename Key,
  typename Default = detail::registry_impl::abort_on_not_found
>
using registry_lookup = typename detail::registry_impl::lookup<Tag, Default>
  ::template key<Key>;

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

struct abort_on_not_found;
struct not_found;

template <typename Tag>
class lookup<Tag, abort_on_not_found> {
public:
  template <typename Key>
  using key = decltype(std::declval<Tag>() << static_cast<Key *>(nullptr));
};

} // namespace registry_impl {
} // namespace detail {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_registry_h
