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

/////////////////
// FATAL_EMPTY //
/////////////////

#define FATAL_EMPTY()

//////////////////
// FATAL_IGNORE //
//////////////////

#define FATAL_IGNORE(...)

////////////////////
// FATAL_IDENTITY //
////////////////////

#define FATAL_IDENTITY(...) __VA_ARGS__

///////////////
// FATAL_CAT //
///////////////

#define FATAL_CAT(LHS, RHS) FATAL_CAT_IMPL(LHS, RHS)

///////////////
// FATAL_NOT //
///////////////

#define FATAL_NOT(x) \
  FATAL_NOT_IMPL(FATAL_CAT(FATAL_NOT_IMPL_, x))

////////////////
// FATAL_BOOL //
////////////////

#define FATAL_BOOL(x) FATAL_NOT(FATAL_NOT(x))

/////////////////
// FATAL_ARG_n //
/////////////////

#define FATAL_ARG_1(x1, ...) x1
#define FATAL_ARG_2(x1, x2, ...) x2
#define FATAL_ARG_3(x1, x2, x3, ...) x3
#define FATAL_ARG_4(x1, x2, x3, x4, ...) x4
#define FATAL_ARG_5(x1, x2, x3, x4, x5, ...) x5

////////////////////
// FATAL_HAS_ARGS //
////////////////////

#define FATAL_HAS_ARGS(...) \
  FATAL_BOOL(FATAL_ARG_1(FATAL_HAS_ARGS_IMPL __VA_ARGS__)())

///////////////////////
// FATAL_CONDITIONAL //
///////////////////////

#define FATAL_CONDITIONAL(condition) \
  FATAL_CONDITIONAL_IMPL(FATAL_BOOL(condition))

///////////////////
// FATAL_DEFER_n //
///////////////////

#define FATAL_DEFER_1(x) x FATAL_EMPTY()
#define FATAL_DEFER_2(x) x FATAL_EMPTY FATAL_EMPTY()()
#define FATAL_DEFER_3(x) x FATAL_EMPTY FATAL_EMPTY FATAL_EMPTY()()()
#define FATAL_DEFER_4(x) \
  x FATAL_EMPTY FATAL_EMPTY FATAL_EMPTY FATAL_EMPTY()()()()
#define FATAL_DEFER_5(x) \
  x FATAL_EMPTY FATAL_EMPTY FATAL_EMPTY FATAL_EMPTY FATAL_EMPTY()()()()()
#define FATAL_DEFER_6(x) \
  x FATAL_EMPTY FATAL_EMPTY FATAL_EMPTY FATAL_EMPTY FATAL_EMPTY \
    FATAL_EMPTY()()()()()()
#define FATAL_DEFER_7(x) \
  x FATAL_EMPTY FATAL_EMPTY FATAL_EMPTY FATAL_EMPTY FATAL_EMPTY \
    FATAL_EMPTY FATAL_EMPTY()()()()()()()
#define FATAL_DEFER_8(x) \
  x FATAL_EMPTY FATAL_EMPTY FATAL_EMPTY FATAL_EMPTY FATAL_EMPTY \
    FATAL_EMPTY FATAL_EMPTY FATAL_EMPTY()()()()()()()()
#define FATAL_DEFER_9(x) \
  x FATAL_EMPTY FATAL_EMPTY FATAL_EMPTY FATAL_EMPTY FATAL_EMPTY \
    FATAL_EMPTY FATAL_EMPTY FATAL_EMPTY FATAL_EMPTY()()()()()()()()()

////////////////
// FATAL_EVAL //
////////////////

#define FATAL_EVAL(...) FATAL_EVAL_IMPL_1024(__VA_ARGS__)

//////////////////////
// FATAL_SIMPLE_MAP //
//////////////////////

#define FATAL_SIMPLE_MAP(Fn, x, ...) \
  Fn(x) \
  FATAL_CONDITIONAL(FATAL_HAS_ARGS(__VA_ARGS__))( \
    FATAL_DEFER_2(FATAL_SIMPLE_MAP_IMPL)()(Fn, __VA_ARGS__) \
  )()

///////////////
// FATAL_MAP //
///////////////

#define FATAL_MAP(Fn, arg, ...) \
  FATAL_MAP_IMPL(Fn, arg, 1, 0, __VA_ARGS__)

//////////////////
// FATAL_TO_STR //
//////////////////

#define FATAL_TO_STR(...) FATAL_TO_STR_IMPL(__VA_ARGS__)

///////////////
// FATAL_UID //
///////////////

#define FATAL_UID(Prefix) FATAL_CAT(Prefix, FATAL_CAT(_, __LINE__))

////////////////////////////
// IMPLEMENTATION DETAILS //
////////////////////////////

//////////////////
// FATAL_TO_STR //
//////////////////

#define FATAL_TO_STR_IMPL(...) #__VA_ARGS__

///////////////
// FATAL_CAT //
///////////////

#define FATAL_CAT_IMPL(LHS, RHS) LHS ## RHS

///////////////
// FATAL_NOT //
///////////////

#define FATAL_NOT_IMPL(...) FATAL_ARG_2(__VA_ARGS__, 0)
#define FATAL_NOT_IMPL_0 FATAL_NOT_WHEN_FALSE_IMPL()
#define FATAL_NOT_WHEN_FALSE_IMPL() ~, 1

////////////////////
// FATAL_HAS_ARGS //
////////////////////

#define FATAL_HAS_ARGS_IMPL() 0

///////////////////////
// FATAL_CONDITIONAL //
///////////////////////

#define FATAL_CONDITIONAL_IMPL(condition) \
  FATAL_CAT(FATAL_CONDITIONAL_IMPL_, condition)
#define FATAL_CONDITIONAL_IMPL_1(...) __VA_ARGS__ FATAL_IGNORE
#define FATAL_CONDITIONAL_IMPL_0(...) FATAL_IDENTITY

////////////////
// FATAL_EVAL //
////////////////

#define FATAL_EVAL_IMPL_1024(...) \
  FATAL_EVAL_IMPL_512(FATAL_EVAL_IMPL_512(__VA_ARGS__))
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
#define FATAL_EVAL_IMPL_8(...) \
  FATAL_EVAL_IMPL_4(FATAL_EVAL_IMPL_4(__VA_ARGS__))
#define FATAL_EVAL_IMPL_4(...) \
  FATAL_EVAL_IMPL_2(FATAL_EVAL_IMPL_2(__VA_ARGS__))
#define FATAL_EVAL_IMPL_2(...) \
  FATAL_EVAL_IMPL_1(FATAL_EVAL_IMPL_1(__VA_ARGS__))
#define FATAL_EVAL_IMPL_1(...) \
  __VA_ARGS__

//////////////////////
// FATAL_SIMPLE_MAP //
//////////////////////

#define FATAL_SIMPLE_MAP_IMPL() FATAL_SIMPLE_MAP

///////////////
// FATAL_MAP //
///////////////

#define FATAL_MAP_IMPL(Fn, arg, is_first, index, x, ...) \
  Fn(arg, is_first, index, x) \
  FATAL_CONDITIONAL(FATAL_HAS_ARGS(__VA_ARGS__))( \
    FATAL_DEFER_2(FATAL_MAP_RECURSE_IMPL)()\
      (Fn, arg, 0, (index + 1), __VA_ARGS__) \
  )()

#define FATAL_MAP_RECURSE_IMPL() FATAL_MAP_IMPL

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_preprocessor_h
