/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/list.h>

#include "./driver.h"

namespace tutorial {

/**
 * TUTORIAL: an overview on how values are represented. Subsequent tutorials
 * will elaborate on proper ways of achieving such representation.
 *
 * The goal, for now, is to come up with the intuition behind it without drowing
 * in syntax and correctness.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */

template <int Value>
struct int_constant {
  static int value;
};

template <int Value>
int int_constant<Value>::value = Value;

FATAL_TUTORIAL(representing values, part 1/4) {
  /*
    A previous tutorial mentioned that values can be emulated using types to
    represent them. Here's an overview on the intuition of how this can be
    achieved.
  */
  using x = int_constant<15>;

  PRINT << "x = " << type_str<x>();
  PRINT << "x::value = " << x::value;
  NEW_LINE;

  /*
    Note, however, that `int_constant::value` is a regular runtime variable
    as opposed to a compile time constant. It is possible, for instance, to
    change the value associated with it:
  */
  x::value = 30;
  PRINT << "x::value = " << x::value;

  /*
    This makes it illegal to use such variable as an argument to a template.
    Template parameters must be immutable and available at compile time. This
    includes, for instance, type and integer constants.
  */

  // ERROR: `int_constant::value` is not a constant. Uncomment the next line for
  // a compilation error.
  //using y = int_constant<x::value>;
}

/**
 * TUTORIAL: demonstrates proper ways to represent values that can be used at
 * compile time.
 *
 * Let's modify the `int_constant` template to properly represent compile time
 * constants.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */

template <int Value>
struct int_constant_proper {
  static constexpr int const value = Value;
};

template <int Value>
constexpr int const int_constant_proper<Value>::value;

FATAL_TUTORIAL(representing values, part 2/4) {
  /*
    C++11 introduced the `constexpr` keyword which roughly allows us to tell the
    compiler that a given variable holds the result of a constant expression.

    Once we have such guarantee, the compiler can evaluate the contents of such
    variable at compile time, effectivelly making it a compile time constant.
  */
  using x = int_constant_proper<15>;

  PRINT << "x = " << type_str<x>();
  PRINT << "x::value = " << x::value;
  NEW_LINE;

  /*
    As noted before, constants can be used as template parameters.
  */
  using y = int_constant_proper<x::value>;

  PRINT << "y = " << type_str<y>();
  PRINT << "y::value = " << y::value;
  NEW_LINE;

  /*
    In fact, any expression that can be evaluated at compile time can be used as
    a compile time constant:
  */

  using z = int_constant_proper<x::value * 2>;

  PRINT << "z = " << type_str<z>();
  PRINT << "z::value = " << z::value;
  NEW_LINE;

  using w = int_constant_proper<x::value + z::value - 3>;

  PRINT << "w = " << type_str<w>();
  PRINT << "w::value = " << w::value;
}

/**
 * TUTORIAL: overview on the implementation of `std::integral_constant`.
 *
 * So far we've been limited to `int` constants. One could be interested in
 * employing other types for a constant, like `char` or `unsigned long`.
 *
 * Let's modify the `int_constant_proper` template to represent arbitrary
 * integral types.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */

template <typename T, T Value>
struct constant {
  static constexpr T const value = Value;
};

template <typename T, T Value>
constexpr T const constant<T, Value>::value;

FATAL_TUTORIAL(representing values, part 3/4) {
  /*
    Now we can specify the type of the constant, as well as its value.
  */
  using x = constant<int, -15>;

  PRINT << "x = " << type_str<x>();
  PRINT << "x::value = " << x::value;
  NEW_LINE;

  using y = constant<bool, true>;

  PRINT << "y = " << type_str<y>();
  PRINT << "y::value = " << std::boolalpha << y::value;
  NEW_LINE;

  /*
    Again, any expression that can be evaluated at compile time will do:
  */

  using z = constant<unsigned, (x::value > 0) ? x::value : -x::value>;

  PRINT << "z = " << type_str<z>();
  PRINT << "z::value = " << z::value;
}

/**
 * TUTORIAL: overview of what `std::integral_constant` has to offer.
 *
 * The implementation and library features built around `std::integral_constant`
 * are a bit more involved than what we've seen so far, but for the purposes of
 * a tutorial, we don't need to dig too deep.
 *
 * For now, let's look at a few more things that `std::integral_constant`offers.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
FATAL_TUTORIAL(representing values, part 4/4) {
  /*
    We already covered how to represent a compile time constant with a type, and
    how to access the constant's value.
  */
  using x = std::integral_constant<int, -15>;

  PRINT << "x = " << type_str<x>();
  PRINT << "x::value = " << x::value;

  /*
    For convenience purposes, `std::integral_constant` also provides an identity
    alias in the form of a member called `type`:
  */
  PRINT << "x::type = " << type_str<x::type>();

  /*
    It also exposes the type of the constant it represents:
  */
  PRINT << "x::value_type = " << type_str<x::value_type>();
  NEW_LINE;

  /*
    Shortcuts to boolean constants are also provided:
  */

  using t = std::true_type;
  PRINT << "t = " << type_str<t>();
  PRINT << "t::value = " << t::value;
  PRINT << "t::value_type = " << type_str<t::value_type>();
  NEW_LINE;

  using f = std::false_type;
  PRINT << "f = " << type_str<f>();
  PRINT << "f::value = " << f::value;
  PRINT << "f::value_type = " << type_str<f::value_type>();
}

} // namespace tutorial {
