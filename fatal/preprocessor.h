/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/portability.h>

namespace fatal {

/////////////////
// source_info //
/////////////////

#define FATAL_SOURCE_INFO() \
  ::fatal::source_info(__FILE__, __LINE__)

struct source_info {

FATAL_DIAGNOSTIC_PUSH
FATAL_GCC_DIAGNOSTIC_IGNORED_SHADOW_IF_BROKEN

  source_info(char const *file, unsigned long line):
    file_(basename(file)),
    line_(line)
  {}

FATAL_DIAGNOSTIC_POP

  char const *file() const { return file_; }
  unsigned long line() const { return line_; }

  static char const *basename(char const *path) {
    for (auto i = path; *i; ++i) {
      if (*i == '/') {
        path = i + 1;
      }
    }
    return path;
  }

  template <typename Out>
  Out &print(Out &out) const {
    out << file_ << ':' << line_;
    return out;
  }

private:
  char const *const file_;
  unsigned long const line_;
};

template <typename Out>
Out &operator <<(Out &out, source_info source) {
  return source.print(out);
}

/////////////////
// FATAL_EMPTY //
/////////////////

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_EMPTY()

//////////////////
// FATAL_IGNORE //
//////////////////

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_IGNORE(...)

////////////////////
// FATAL_IDENTITY //
////////////////////

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_IDENTITY(...) \
  __VA_ARGS__

/////////////////
// FATAL_COMMA //
/////////////////

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_COMMA(...) \
  ,

/////////////////////
// FATAL_SEMICOLON //
/////////////////////

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_SEMICOLON(...) \
  ;

///////////////
// FATAL_CAT //
///////////////

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_CAT(LHS, ...) \
  FATAL_IMPL_CAT(LHS, __VA_ARGS__)

#define FATAL_IMPL_CAT(LHS, ...) \
  LHS ## __VA_ARGS__

///////////////
// FATAL_NOT //
///////////////

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_NOT(...) \
  FATAL_IMPL_NOT(FATAL_CAT(FATAL_IMPL_NOT_SWITCH_, __VA_ARGS__)())

#define FATAL_IMPL_NOT(...) \
  FATAL_ARG_2(__VA_ARGS__, 0)

#define FATAL_IMPL_NOT_SWITCH_() \
  FATAL_IMPL_NOT_WHEN_FALSE()

#define FATAL_IMPL_NOT_SWITCH_0(...) \
  FATAL_IMPL_NOT_WHEN_FALSE()

#define FATAL_IMPL_NOT_WHEN_FALSE() \
  ~, 1

////////////////
// FATAL_BOOL //
////////////////

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_BOOL(...) \
  FATAL_NOT(FATAL_NOT(__VA_ARGS__))

////////////////////////////
// FATAL_IS_PARENTHESIZED //
////////////////////////////

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_IS_PARENTHESIZED(...) \
  FATAL_IMPL_IS_PARENTHESIZED(FATAL_IMPL_IS_PARENTHESIZED_WHEN_TRUE __VA_ARGS__)

#define FATAL_IMPL_IS_PARENTHESIZED(...) \
  FATAL_NOT(FATAL_ARG_1(__VA_ARGS__))

#define FATAL_IMPL_IS_PARENTHESIZED_WHEN_TRUE(...) \
  0

//////////////////////////
// FATAL_UNPARENTHESIZE //
//////////////////////////

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_UNPARENTHESIZE(...) \
  FATAL_CONDITIONAL_INTERNAL_UNPARENTHESIZE( \
    FATAL_IS_PARENTHESIZED(__VA_ARGS__) \
  )( \
    FATAL_IDENTITY __VA_ARGS__ \
  )(__VA_ARGS__)

////////////////
// FATAL_CALL //
////////////////

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_CALL(Fn, ...) \
  FATAL_IDENTITY(Fn)(__VA_ARGS__)

///////////////////////
// FATAL_TUPLE_APPLY //
///////////////////////

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_TUPLE_APPLY(Fn, Arg, ...) \
  FATAL_IMPL_TUPLE_APPLY(Fn, Arg, FATAL_UNPARENTHESIZE(__VA_ARGS__))

#define FATAL_IMPL_TUPLE_APPLY(Fn, ...) \
  Fn(__VA_ARGS__)

///////////////////////
// FATAL_TUPLE_GET_1 //
///////////////////////

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_TUPLE_GET_1(...) \
  FATAL_IMPL_TUPLE_GET(FATAL_ARG_1, FATAL_UNPARENTHESIZE(__VA_ARGS__))

#define FATAL_TUPLE_GET_2(...) \
  FATAL_IMPL_TUPLE_GET(FATAL_ARG_2, FATAL_UNPARENTHESIZE(__VA_ARGS__))

#define FATAL_TUPLE_GET_3(...) \
  FATAL_IMPL_TUPLE_GET(FATAL_ARG_3, FATAL_UNPARENTHESIZE(__VA_ARGS__))

#define FATAL_TUPLE_GET_4(...) \
  FATAL_IMPL_TUPLE_GET(FATAL_ARG_4, FATAL_UNPARENTHESIZE(__VA_ARGS__))

#define FATAL_TUPLE_GET_5(...) \
  FATAL_IMPL_TUPLE_GET(FATAL_ARG_5, FATAL_UNPARENTHESIZE(__VA_ARGS__))

#define FATAL_TUPLE_GET_6(...) \
  FATAL_IMPL_TUPLE_GET(FATAL_ARG_6, FATAL_UNPARENTHESIZE(__VA_ARGS__))

#define FATAL_TUPLE_GET_7(...) \
  FATAL_IMPL_TUPLE_GET(FATAL_ARG_7, FATAL_UNPARENTHESIZE(__VA_ARGS__))

#define FATAL_TUPLE_GET_8(...) \
  FATAL_IMPL_TUPLE_GET(FATAL_ARG_8, FATAL_UNPARENTHESIZE(__VA_ARGS__))

#define FATAL_TUPLE_GET_9(...) \
  FATAL_IMPL_TUPLE_GET(FATAL_ARG_9, FATAL_UNPARENTHESIZE(__VA_ARGS__))

#define FATAL_IMPL_TUPLE_GET(Fn, ...) \
  Fn(__VA_ARGS__)

/////////////////
// FATAL_ARG_n //
/////////////////

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_ARG_1(x1, ...) x1

#define FATAL_ARG_2(x1, x2, ...) x2

#define FATAL_ARG_3(x1, x2, x3, ...) x3

#define FATAL_ARG_4(x1, x2, x3, x4, ...) x4

#define FATAL_ARG_5(x1, x2, x3, x4, x5, ...) x5

#define FATAL_ARG_6(x1, x2, x3, x4, x5, x6, ...) x6

#define FATAL_ARG_7(x1, x2, x3, x4, x5, x6, x7, ...) x7

#define FATAL_ARG_8(x1, x2, x3, x4, x5, x6, x7, x8, ...) x8

#define FATAL_ARG_9(x1, x2, x3, x4, x5, x6, x7, x8, x9, ...) x9

#define FATAL_ARG_10(x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, ...) x10

#define FATAL_ARG_11(x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, ...) x11

#define FATAL_ARG_12( \
  x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, ... \
) x12

#define FATAL_ARG_13( \
  x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, ... \
) x13

#define FATAL_ARG_14( \
  x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, ... \
) x14

#define FATAL_ARG_15( \
  x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, ... \
) x15

#define FATAL_ARG_16( \
  x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, ... \
) x16

///////////////////////
// FATAL_SKIP_n_ARGS //
///////////////////////

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_SKIP_1_ARG(x1, ...) __VA_ARGS__

#define FATAL_SKIP_2_ARGS(x1, x2, ...) __VA_ARGS__

#define FATAL_SKIP_3_ARGS(x1, x2, x3, ...) __VA_ARGS__

#define FATAL_SKIP_4_ARGS(x1, x2, x3, x4, ...) __VA_ARGS__

#define FATAL_SKIP_5_ARGS(x1, x2, x3, x4, x5, ...) __VA_ARGS__

#define FATAL_SKIP_6_ARGS(x1, x2, x3, x4, x5, x6, ...) __VA_ARGS__

#define FATAL_SKIP_7_ARGS(x1, x2, x3, x4, x5, x6, x7, ...) __VA_ARGS__

#define FATAL_SKIP_8_ARGS(x1, x2, x3, x4, x5, x6, x7, x8, ...) __VA_ARGS__

#define FATAL_SKIP_9_ARGS(x1, x2, x3, x4, x5, x6, x7, x8, x9, ...) __VA_ARGS__

#define FATAL_SKIP_10_ARGS( \
  x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, ... \
) __VA_ARGS__

#define FATAL_SKIP_11_ARGS( \
  x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, ... \
) __VA_ARGS__

#define FATAL_SKIP_12_ARGS( \
  x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, ... \
) __VA_ARGS__

#define FATAL_SKIP_13_ARGS( \
  x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, ... \
) __VA_ARGS__

#define FATAL_SKIP_14_ARGS( \
  x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, ... \
) __VA_ARGS__

#define FATAL_SKIP_15_ARGS( \
  x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, ... \
) __VA_ARGS__

#define FATAL_SKIP_16_ARGS( \
  x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, ... \
) __VA_ARGS__

////////////////////
// FATAL_HAS_ARGS //
////////////////////

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_HAS_ARGS(...) \
  FATAL_BOOL( \
    FATAL_ARG_1( \
      FATAL_IMPL_HAS_ARGS_1( \
        FATAL_UNPARENTHESIZE( \
          FATAL_UNPARENTHESIZE( \
            FATAL_UNPARENTHESIZE( \
              FATAL_UNPARENTHESIZE( \
                FATAL_UNPARENTHESIZE( \
                  FATAL_UNPARENTHESIZE( \
                    FATAL_UNPARENTHESIZE( \
                      FATAL_UNPARENTHESIZE( \
                        FATAL_UNPARENTHESIZE( \
                          FATAL_UNPARENTHESIZE( \
                            FATAL_UNPARENTHESIZE( \
                              FATAL_UNPARENTHESIZE( \
                                FATAL_UNPARENTHESIZE( \
                                  FATAL_UNPARENTHESIZE( \
                                    FATAL_UNPARENTHESIZE( \
                                      FATAL_UNPARENTHESIZE( \
                                        FATAL_UNPARENTHESIZE( \
                                          FATAL_UNPARENTHESIZE( \
                                            FATAL_UNPARENTHESIZE(__VA_ARGS__) \
                                          ) \
                                        ) \
                                      ) \
                                    ) \
                                  ) \
                                ) \
                              ) \
                            ) \
                          ) \
                        ) \
                      ) \
                    ) \
                  ) \
                ) \
              ) \
            ) \
          ) \
        ) \
      ) \
    )() \
  )

#define FATAL_IMPL_HAS_ARGS_1(...) \
  FATAL_ARG_1(FATAL_IMPL_HAS_ARGS_2 __VA_ARGS__)()

#define FATAL_IMPL_HAS_ARGS_2(...) \
  FATAL_IMPL_HAS_ARGS_3 __VA_ARGS__

#define FATAL_IMPL_HAS_ARGS_3(...) \
  0

///////////////////////
// FATAL_CONDITIONAL //
///////////////////////

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_CONDITIONAL(condition) \
  FATAL_IMPL_CONDITIONAL(FATAL_BOOL(condition))

#define FATAL_IMPL_CONDITIONAL(condition) \
  FATAL_CAT(FATAL_CONDITIONAL_IMPL_, condition)

#define FATAL_CONDITIONAL_IMPL_1(...) \
  __VA_ARGS__ FATAL_IGNORE

#define FATAL_CONDITIONAL_IMPL_0(...) \
  FATAL_IDENTITY

#define FATAL_CONDITIONAL_2(condition) \
  FATAL_IMPL_CONDITIONAL_2(FATAL_BOOL(condition))

#define FATAL_IMPL_CONDITIONAL_2(condition) \
  FATAL_CAT(FATAL_CONDITIONAL_2_IMPL_, condition)

#define FATAL_CONDITIONAL_2_IMPL_1(...) \
  __VA_ARGS__ FATAL_IGNORE

#define FATAL_CONDITIONAL_2_IMPL_0(...) \
  FATAL_IDENTITY

#define FATAL_CONDITIONAL_INTERNAL_UNPARENTHESIZE(condition) \
  FATAL_IMPL_CONDITIONAL_INTERNAL_UNPARENTHESIZE(FATAL_BOOL(condition))

#define FATAL_IMPL_CONDITIONAL_INTERNAL_UNPARENTHESIZE(condition) \
  FATAL_CAT(FATAL_CONDITIONAL_INTERNAL_UNPARENTHESIZE_IMPL_, condition)

#define FATAL_CONDITIONAL_INTERNAL_UNPARENTHESIZE_IMPL_1(...) \
  __VA_ARGS__ FATAL_IGNORE

#define FATAL_CONDITIONAL_INTERNAL_UNPARENTHESIZE_IMPL_0(...) \
  FATAL_IDENTITY

#define FATAL_CONDITIONAL_INTERNAL_MAP(condition) \
  FATAL_IMPL_CONDITIONAL_INTERNAL_MAP(FATAL_BOOL(condition))

#define FATAL_IMPL_CONDITIONAL_INTERNAL_MAP(condition) \
  FATAL_CAT(FATAL_CONDITIONAL_INTERNAL_MAP_IMPL_, condition)

#define FATAL_CONDITIONAL_INTERNAL_MAP_IMPL_1(...) \
  __VA_ARGS__ FATAL_IGNORE

#define FATAL_CONDITIONAL_INTERNAL_MAP_IMPL_0(...) \
  FATAL_IDENTITY

#define FATAL_CONDITIONAL_INTERNAL_MAP_2(condition) \
  FATAL_IMPL_CONDITIONAL_INTERNAL_MAP_2(FATAL_BOOL(condition))

#define FATAL_IMPL_CONDITIONAL_INTERNAL_MAP_2(condition) \
  FATAL_CAT(FATAL_CONDITIONAL_INTERNAL_MAP_2_IMPL_, condition)

#define FATAL_CONDITIONAL_INTERNAL_MAP_2_IMPL_1(...) \
  __VA_ARGS__ FATAL_IGNORE

#define FATAL_CONDITIONAL_INTERNAL_MAP_2_IMPL_0(...) \
  FATAL_IDENTITY

#define FATAL_CONDITIONAL_INTERNAL_MAP_3(condition) \
  FATAL_IMPL_CONDITIONAL_INTERNAL_MAP_3(FATAL_BOOL(condition))

#define FATAL_IMPL_CONDITIONAL_INTERNAL_MAP_3(condition) \
  FATAL_CAT(FATAL_CONDITIONAL_INTERNAL_MAP_3_IMPL_, condition)

#define FATAL_CONDITIONAL_INTERNAL_MAP_3_IMPL_1(...) \
  __VA_ARGS__ FATAL_IGNORE

#define FATAL_CONDITIONAL_INTERNAL_MAP_3_IMPL_0(...) \
  FATAL_IDENTITY

#define FATAL_CONDITIONAL_INTERNAL_MAP_4(condition) \
  FATAL_IMPL_CONDITIONAL_INTERNAL_MAP_4(FATAL_BOOL(condition))

#define FATAL_IMPL_CONDITIONAL_INTERNAL_MAP_4(condition) \
  FATAL_CAT(FATAL_CONDITIONAL_INTERNAL_MAP_4_IMPL_, condition)

#define FATAL_CONDITIONAL_INTERNAL_MAP_4_IMPL_1(...) \
  __VA_ARGS__ FATAL_IGNORE

#define FATAL_CONDITIONAL_INTERNAL_MAP_4_IMPL_0(...) \
  FATAL_IDENTITY

#define FATAL_CONDITIONAL_INTERNAL_MAP_5(condition) \
  FATAL_IMPL_CONDITIONAL_INTERNAL_MAP_5(FATAL_BOOL(condition))

#define FATAL_IMPL_CONDITIONAL_INTERNAL_MAP_5(condition) \
  FATAL_CAT(FATAL_CONDITIONAL_INTERNAL_MAP_5_IMPL_, condition)

#define FATAL_CONDITIONAL_INTERNAL_MAP_5_IMPL_1(...) \
  __VA_ARGS__ FATAL_IGNORE

#define FATAL_CONDITIONAL_INTERNAL_MAP_5_IMPL_0(...) \
  FATAL_IDENTITY

///////////////////
// FATAL_DEFER_n //
///////////////////

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_DEFER_1(x) \
  x FATAL_EMPTY()

#define FATAL_DEFER_2(x) \
  x FATAL_EMPTY FATAL_EMPTY()()

#define FATAL_DEFER_3(x) \
  x FATAL_EMPTY FATAL_EMPTY FATAL_EMPTY()()()

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

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_EVAL(...) \
  FATAL_EVAL_512(FATAL_EVAL_512(__VA_ARGS__))

#define FATAL_EVAL_512(...) \
  FATAL_EVAL_256(FATAL_EVAL_256(__VA_ARGS__))

#define FATAL_EVAL_256(...) \
  FATAL_EVAL_128(FATAL_EVAL_128(__VA_ARGS__))

#define FATAL_EVAL_128(...) \
  FATAL_EVAL_64(FATAL_EVAL_64(__VA_ARGS__))

#define FATAL_EVAL_64(...) \
  FATAL_EVAL_32(FATAL_EVAL_32(__VA_ARGS__))

#define FATAL_EVAL_32(...) \
  FATAL_EVAL_16(FATAL_EVAL_16(__VA_ARGS__))

#define FATAL_EVAL_16(...) \
  FATAL_EVAL_8(FATAL_EVAL_8(__VA_ARGS__))

#define FATAL_EVAL_8(...) \
  FATAL_EVAL_4(FATAL_EVAL_4(__VA_ARGS__))

#define FATAL_EVAL_4(...) \
  FATAL_EVAL_2(FATAL_EVAL_2(__VA_ARGS__))

#define FATAL_EVAL_2(...) \
  FATAL_EVAL_1(FATAL_EVAL_1(__VA_ARGS__))

#define FATAL_EVAL_1(...) \
  __VA_ARGS__

//////////////////////
// FATAL_SIMPLE_MAP //
//////////////////////

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_SIMPLE_MAP(Fn, ...) \
  FATAL_MAP(FATAL_IMPL_SIMPLE_MAP, Fn, __VA_ARGS__)

#define FATAL_IMPL_SIMPLE_MAP(Fn, is_first, index, ...) \
  Fn(__VA_ARGS__)

///////////////
// FATAL_MAP //
///////////////

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_MAP(Fn, arg, ...) \
  FATAL_EVAL(FATAL_IMPL_MAP(Fn, arg, 1, 0, __VA_ARGS__))

#define FATAL_IMPL_MAP(Fn, arg, is_first, index, x, ...) \
  Fn(arg, is_first, index, x) \
  FATAL_CONDITIONAL_INTERNAL_MAP(FATAL_HAS_ARGS(__VA_ARGS__))( \
    FATAL_DEFER_2(FATAL_IMPL_MAP_RECURSE)()\
      (Fn, arg, 0, (index + 1), __VA_ARGS__) \
  )()

#define FATAL_IMPL_MAP_RECURSE() \
  FATAL_IMPL_MAP

/////////////////
// FATAL_MAP_n //
/////////////////

/**
 * TODO: DOCUMENT AND TEST
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_MAP_2(Fn, arg, ...) \
  FATAL_IMPL_MAP_2(Fn, arg, 1, 0, __VA_ARGS__)

#define FATAL_IMPL_MAP_2(Fn, arg, is_first, index, x, ...) \
  Fn(arg, is_first, index, x) \
  FATAL_CONDITIONAL_INTERNAL_MAP_2(FATAL_HAS_ARGS(__VA_ARGS__))( \
    FATAL_DEFER_2(FATAL_IMPL_MAP_2_RECURSE)()\
      (Fn, arg, 0, (index + 1), __VA_ARGS__) \
  )()

#define FATAL_IMPL_MAP_2_RECURSE() \
  FATAL_IMPL_MAP_2

#define FATAL_MAP_3(Fn, arg, ...) \
  FATAL_IMPL_MAP_3(Fn, arg, 1, 0, __VA_ARGS__)

#define FATAL_IMPL_MAP_3(Fn, arg, is_first, index, x, ...) \
  Fn(arg, is_first, index, x) \
  FATAL_CONDITIONAL_INTERNAL_MAP_3(FATAL_HAS_ARGS(__VA_ARGS__))( \
    FATAL_DEFER_3(FATAL_IMPL_MAP_3_RECURSE)()\
      (Fn, arg, 0, (index + 1), __VA_ARGS__) \
  )()

#define FATAL_IMPL_MAP_3_RECURSE() \
  FATAL_IMPL_MAP_3

#define FATAL_MAP_4(Fn, arg, ...) \
  FATAL_IMPL_MAP_4(Fn, arg, 1, 0, __VA_ARGS__)

#define FATAL_IMPL_MAP_4(Fn, arg, is_first, index, x, ...) \
  Fn(arg, is_first, index, x) \
  FATAL_CONDITIONAL_INTERNAL_MAP_4(FATAL_HAS_ARGS(__VA_ARGS__))( \
    FATAL_DEFER_4(FATAL_IMPL_MAP_4_RECURSE)()\
      (Fn, arg, 0, (index + 1), __VA_ARGS__) \
  )()

#define FATAL_IMPL_MAP_4_RECURSE() \
  FATAL_IMPL_MAP_4

#define FATAL_MAP_5(Fn, arg, ...) \
  FATAL_IMPL_MAP_5(Fn, arg, 1, 0, __VA_ARGS__)

#define FATAL_IMPL_MAP_5(Fn, arg, is_first, index, x, ...) \
  Fn(arg, is_first, index, x) \
  FATAL_CONDITIONAL_INTERNAL_MAP_5(FATAL_HAS_ARGS(__VA_ARGS__))( \
    FATAL_DEFER_5(FATAL_IMPL_MAP_5_RECURSE)()\
      (Fn, arg, 0, (index + 1), __VA_ARGS__) \
  )()

#define FATAL_IMPL_MAP_5_RECURSE() \
  FATAL_IMPL_MAP_5

//////////////////
// FATAL_TO_STR //
//////////////////

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_TO_STR(...) \
  FATAL_IMPL_TO_STR(__VA_ARGS__)

#define FATAL_IMPL_TO_STR(...) \
  # __VA_ARGS__

///////////////
// FATAL_UID //
///////////////

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_UID(Prefix) \
  FATAL_CAT(Prefix, FATAL_CAT(_, __LINE__))

} // namespace fatal {
