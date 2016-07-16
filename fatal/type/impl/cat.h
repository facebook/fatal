/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_impl_cat_h
#define FATAL_INCLUDE_fatal_type_impl_cat_h

namespace fatal {
namespace impl_cat {

template <typename, typename> struct cat;

template <
  template <typename...> class L, typename... LHS,
  template <typename...> class R, typename... RHS
>
struct cat<L<LHS...>, R<RHS...>> {
  using type = L<LHS..., RHS...>;
};

template <
  typename T,
  template <typename, T...> class L, T... LHS,
  template <typename, T...> class R, T... RHS
>
struct cat<L<T, LHS...>, R<T, RHS...>> {
  using type = L<T, LHS..., RHS...>;
};

} // namespace impl_cat {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_impl_cat_h
