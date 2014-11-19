/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_container_uninitialized_h
#define FATAL_INCLUDE_fatal_container_uninitialized_h

#include <fatal/container/unitary_union.h>

#include <memory>
#include <utility>

namespace fatal {

///////////////////
// uninitialized //
///////////////////

// TODO: DOCUMENT AND TEST
template <typename T, bool AutomaticallyDestroy = true>
struct uninitialized {
  using type = T;

  type const &reference() const noexcept { return container_.value; }

  type &reference() noexcept { return container_.value; }

  type const *pointer() const noexcept {
    return std::addressof(container_.value);
  }

  type *pointer() noexcept { return std::addressof(container_.value); }

  template <typename... Args>
  type &construct(Args &&...args)
    noexcept(noexcept(type(std::forward<Args>(args)...)))
  {
    return *new (pointer()) type(std::forward<Args>(args)...);
  }

  type *destroy() noexcept {
    reference().~type();
    return pointer();
  }

  type const &operator *() const noexcept { return container_.value; }

  type &operator *() noexcept { return container_.value; }

  type const *operator ->() const noexcept {
    return std::addressof(container_.value);
  }

  type *operator ->() noexcept { return std::addressof(container_.value); }


private:
  using union_type = unitary_union<type, AutomaticallyDestroy>;

  union_type container_;
};

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_container_uninitialized_h
