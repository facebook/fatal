/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_preprocessor_h
#define FATAL_INCLUDE_fatal_preprocessor_h

namespace fatal {

///////////////
// FATAL_CAT //
///////////////

#define FATAL_CAT(LHS, RHS) FATAL_CAT_IMPL(LHS, RHS)
#define FATAL_CAT_IMPL(LHS, RHS) LHS ## RHS

//////////////////
// FATAL_AS_STR //
//////////////////

#define FATAL_AS_STR(String) FATAL_AS_STR_IMPL(String)
#define FATAL_AS_STR_IMPL(String) #String

///////////////
// FATAL_UID //
///////////////

#define FATAL_UID(Prefix) FATAL_CAT(Prefix, FATAL_CAT(_, __LINE__))

//////////////////
// FATAL_IGNORE //
//////////////////

#define FATAL_IGNORE(...)

/////////////////////
// FATAL_ARG_1 ... //
/////////////////////

#define FATAL_ARG_1(x1, ...) x1
#define FATAL_ARG_2(x1, x2, ...) x2
#define FATAL_ARG_3(x1, x2, x3, ...) x3
#define FATAL_ARG_4(x1, x2, x3, x4, ...) x4
#define FATAL_ARG_5(x1, x2, x3, x4, x5, ...) x5

#define FATAL_IS_PROBE(...) FATAL_ARG_2(__VA_ARGS__, 0)
#define FATAL_PROBE() ~, 1

///////////////
// FATAL_NOT //
///////////////

#define FATAL_NOT(x) FATAL_IS_PROBE(FATAL_CAT(FATAL_NOT_IMPL_, x))
#define FATAL_NOT_IMPL_0 FATAL_PROBE()

////////////////
// FATAL_BOOL //
////////////////

#define FATAL_BOOL(x) FATAL_NOT(FATAL_NOT(x))

///////////////////
// FATAL_IF_ELSE //
///////////////////

#define FATAL_IF_ELSE(condition) FATAL_IF_ELSE_IMPL(BOOL(condition))
#define FATAL_IF_ELSE_IMPL(condition) FATAL_CAT(FATAL_IF_IMPL_, condition)

#define FATAL_IF_IMPL_1(...) __VA_ARGS__ FATAL_IF_IMPL_1_ELSE
#define FATAL_IF_IMPL_0(...) FATAL_IF_IMPL_0_ELSE

#define FATAL_IF_IMPL_1_ELSE(...)
#define FATAL_IF_IMPL_0_ELSE(...) __VA_ARGS__

////////////////
// FATAL_EVAL //
////////////////

#define FATAL_EVAL(...) FATAL_EVAL_IMPL_512(__VA_ARGS__)
#define FATAL_EVAL_IMPL_512(...) \
  FATAL_EVAL_IMPL_256(FATAL_EVAL_IMPL_256(__VA_ARGS__))
#define FATAL_EVAL_IMPL_256(...) \
  FATAL_EVAL_IMPL_128(FATAL_EVAL_IMPL_128(__VA_ARGS__))
#define FATAL_EVAL_IMPL_128(...) \
  FATAL_EVAL_IMPL_64(FATAL_EVAL_IMPL_64(__VA_ARGS__))
#define FATAL_EVAL_IMPL_64(...) \
  FATAL_EVAL_IMPL_32(FATAL_EVAL_IMPL_32(__VA_ARGS__))
#define FATAL_EVAL_IMPL_32(...) \
  FATAL_EVAL_IMPL_16(FATAL_EVAL_IMPL_16(__VA_ARGS__))
#define FATAL_EVAL_IMPL_16(...) \
  FATAL_EVAL_IMPL_8(FATAL_EVAL_IMPL_8(__VA_ARGS__))
#define FATAL_EVAL_IMPL_8(...) FATAL_EVAL_IMPL_4(FATAL_EVAL_IMPL_4(__VA_ARGS__))
#define FATAL_EVAL_IMPL_4(...) FATAL_EVAL_IMPL_2(FATAL_EVAL_IMPL_2(__VA_ARGS__))
#define FATAL_EVAL_IMPL_2(...) FATAL_EVAL_IMPL_1(FATAL_EVAL_IMPL_1(__VA_ARGS__))
#define FATAL_EVAL_IMPL_1(...) __VA_ARGS__

///////////////////
// FATAL_DEFER_* //
///////////////////

#define FATAL_DEFER_1(x) x FATAL_IGNORE()
#define FATAL_DEFER_2(x) x FATAL_IGNORE FATAL_IGNORE()()
#define FATAL_DEFER_3(x) x FATAL_IGNORE FATAL_IGNORE FATAL_IGNORE()()()
#define FATAL_DEFER_4(x) \
  x FATAL_IGNORE FATAL_IGNORE FATAL_IGNORE FATAL_IGNORE()()()()

///////////////
// FATAL_MAP //
///////////////

#define FATAL_MAP(Fn, x, ...) \
  Fn(x) \
  FATAL_IF_ELSE(HAS_ARGS(__VA_ARGS__))( \
    FATAL_DEFER_2(FATAL_MAP_IMPL)()(Fn, __VA_ARGS__) \
  )()

#define FATAL_MAP_IMPL() FATAL_MAP

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_preprocessor_h
