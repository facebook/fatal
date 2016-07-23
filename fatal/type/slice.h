/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_slice_h
#define FATAL_INCLUDE_fatal_type_slice_h

#include <fatal/type/size.h>

#include <cstdint>

#include <fatal/type/impl/slice.h>

namespace fatal {

template <typename T, std::size_t Index>
using at = typename impl_at::at<T, Index>::type;

template <typename T>
using first = at<T, 0>;

template <typename T>
using second = at<T, 1>;

template <typename T>
using third = at<T, 2>;

template <typename T, std::size_t... Indexes>
using pick = typename impl_at::pick<T, Indexes...>::type;

template <typename T, std::size_t Offset>
using tail = typename impl_at::tail<T, make_index_sequence<Offset>>::type;

template <typename T, std::size_t Offset>
using head = typename impl_at::head<T, Offset>
  ::template type<>;

template <typename T, std::size_t Begin, std::size_t End>
using slice = head<tail<T, Begin>, End - Begin>;

template <typename Container>
using index = typename impl_at::idx<
  Container,
  make_index_sequence<size<Container>::value>
>;

namespace bound {

template <std::size_t Index>
struct at {
  template <typename T>
  using apply = fatal::at<T, Index>;
};

template <std::size_t... Indexes>
struct pick {
  template <typename T>
  using apply = fatal::pick<T, Indexes...>;
};

template <std::size_t Offset>
struct tail {
  template <typename T>
  using apply = fatal::tail<T, Offset>;
};

template <std::size_t Offset>
struct head {
  template <typename T>
  using apply = fatal::head<T, Offset>;
};

template <std::size_t Begin, std::size_t End>
struct slice {
  template <typename T>
  using apply = fatal::slice<T, Begin, End>;
};

} // namespace bound {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_slice_h
