/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

// production build
#ifdef NDEBUG

# include <type_traits>

namespace fatal {

using debug_mode = std::false_type;

# define FATAL_DEBUG_ONLY(...)
# define FATAL_DEBUG_STATEMENT(...) ::fatal::fn::no_op()
# define FATAL_DEBUG_BLOCK while (false)
# define FATAL_DEBUG_MEMBER_BLOCK while (false)

# define FATAL_TODO(Message) while (false)

} // namespace fatal {

// debug build
#else // NDEBUG

# include <cstdlib>
# include <iostream>
# include <type_traits>

namespace fatal {

using debug_mode = std::true_type;

# define FATAL_DEBUG_ONLY(...) __VA_ARGS__
# define FATAL_DEBUG_STATEMENT(...) __VA_ARGS__
# define FATAL_DEBUG_BLOCK ::fatal::detail::debug_impl::debug_block() + [&]()
# define FATAL_DEBUG_MEMBER_BLOCK ::fatal::detail::debug_impl::debug_block() \
  + [this, &]()

# define FATAL_TODO(Message) \
  do { \
    ::std::cerr << "TODO (aborting): " << Message << ::std::endl; \
    ::std::abort(); \
  } while (false)

namespace detail {
namespace debug_impl {

struct debug_block {
  template <typename Block>
  debug_block operator +(Block &&block) const {
    block();
    return debug_block();
  }
};

} // namespace debug_impl {
} // namespace detail {
} // namespace fatal {

#endif // NDEBUG
