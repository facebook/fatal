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

/**
 * This tutorial demonstrates how to achieve, in metaprogramming, something
 * similar to the notion of variables in procedural programming.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
FATAL_TUTORIAL(variables) {
  /* If you think about a duck typed language, we're used to declaring variables
   * by assigning a value to a name.
   */
  auto x = 10;

  /* Whenever we want to retrieve the value stored in that variable, we
   * reference it through its name:
   */
  PRINT << "x = " << x;

  /* A similar idiom can be achieved for metaprogramming. Take the following
   * line, for example:
   */
  using y = std::integral_constant<int, 10>;

  /* We're also assigning a value `10`, of type `int`, to the name `y`.
   *
   * The difference is that instead of variables, we're using an alias to a
   * type.
   */

  PRINT << "y = " << type_str<y>();

  /* Just like with variables, we can also retrieve what's stored in that
   * type alias:
   */

  PRINT << "y::value = " << y::value;

  /* That's the first thing you should notice about metaprogramming: we don't
   * manipulate values. We manipulate types.
   *
   * Types only exist during compilation, so one could say metaprograms run
   * inside the compiler, at  compile time, as opposed to regular programs which
   * run at runtime, after compilation is done.
   *
   * There are some types that can represent values, like
   * std::integral_constant, but that's not necessarily true for every type.
   *
   * For instance, we could create an alias for the type `void`, or `int`, or
   * even `std::string`:
   */

  using z = void;
  using w = int;
  using k = std::string;

  /* A `int` or `std::string` runtime variable is able to store values, yes, but
   * as for the types themselves, `int` and `std::string`, they don't really
   * represent any values at compile time. Assigning values to them only make
   * sense at runtime.
   */

  PRINT << "z = " << type_str<z>();
  PRINT << "w = " << type_str<w>();
  PRINT << "k = " << type_str<k>();

  /*
   * The last thing to notice is that types are immutable. Once we assign a type
   * to an alias, that alias will always represent the same type.
   *
   * That's a hint that metaprogramming in C++ works similarly to pure
   * functional programming.
   */

  // ERROR: it's illegal to re-assign a type to an alias. Uncomment the next
  // line to see a compilation error:
  //w = long;

  /* SUMMARY: metaprogramming deals with types, not values. We can simulate
   * values using types like `std::integral_constant` or `std::ratio`. But,
   * ultimately, they're just types.
   *
   * Types are immutable.
   *
   * Metaprograms run at compile time, whereas regular programs run at runtime.
   */
}
