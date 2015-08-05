/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/lesson/driver.h>

namespace lesson {

/**
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
LESSON(
  "representing values, part 1/4",
  "This lesson gives an overview on how values are represented. Subsequent "
  "tutorials will elaborate on proper ways of achieving such representation."
  "\n\n"
  "The goal, for now, is to come up with the intuition behind it without "
  "drowing in syntax and correctness.",

  template <int Value>
  struct int_constant {
    static int value;
  };

  template <int Value>
  int int_constant<Value>::value = Value;
) {
  COMMENT(
    "A previous lesson mentioned that values can be emulated using types to "
    "represent them. Here's an overview on the intuition of how this can be "
    "achieved."
  );

  CODE(
    using x = int_constant<15>;
  );

  PRINT("x = ", type_str<x>());
  PRINT("x::value = ", x::value);

  COMMENT(
    "Note, however, that `int_constant::value` is a regular runtime variable "
    "as opposed to a compile time constant. It is possible, for instance, to "
    "change the value associated with it:"
  );

  CODE(
    x::value = 30;
  );

  PRINT("x::value = ", x::value);

  COMMENT(
    "This makes it illegal to use such variable as an argument to a template. "
    "Template parameters must be immutable and available at compile time. This "
    "includes, for instance, type and integer constants."
  );

  ILLEGAL(
    "`int_constant::value` is not a constant",
    using y = int_constant<x::value>;
  );
}

/**
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
LESSON(
  "representing values, part 2/4",
  "This lesson demonstrates proper ways to represent values that can be used "
  "at compile time."
  "\n\n"
  "Let's modify the `int_constant` template to properly represent compile time "
  "constants.",

  template <int Value>
  struct int_constant_proper {
    static constexpr int const value = Value;
  };

  template <int Value>
  constexpr int const int_constant_proper<Value>::value;
) {
  COMMENT(
    "C++11 introduced the `constexpr` keyword which roughly allows us to tell "
    "the compiler that a given variable holds the result of a constant "
    "expression."
    "\n\n"
    "Once we have such guarantee, the compiler can evaluate the contents of "
    "such variable at compile time, effectivelly making it a compile time "
    "constant."
  );

  CODE(
    using x = int_constant_proper<15>;
  );

  PRINT("x = ", type_str<x>());
  PRINT("x::value = ", x::value);

  COMMENT(
    "As noted before, constants can be used as template parameters."
  );

  CODE(
    using y = int_constant_proper<x::value>;
  );

  PRINT("y = ", type_str<y>());
  PRINT("y::value = ", y::value);

  COMMENT(
    "In fact, any expression that can be evaluated at compile time can be used "
    "as a compile time constant:"
  );

  CODE(
    using z = int_constant_proper<x::value * 2>;
  );

  PRINT("z = ", type_str<z>());
  PRINT("z::value = ", z::value);

  CODE(
    using w = int_constant_proper<x::value + z::value - 3>;
  );

  PRINT("w = ", type_str<w>());
  PRINT("w::value = ", w::value);
}

/**
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
LESSON(
  "representing values, part 3/4",
  "This lesson gives an overview on the implementation of "
  "`std::integral_constant`."
  "\n\n"
  "So far we've been limited to `int` constants. One could be interested in "
  "employing other types for a constant, like `char` or `unsigned long`."
  "\n\n"
  "Let's modify the `int_constant_proper` template to represent arbitrary "
  "integral types.",

  template <typename T, T Value>
  struct constant {
    static constexpr T const value = Value;
  };

  template <typename T, T Value>
  constexpr T const constant<T, Value>::value;
) {
  COMMENT(
    "Now we can specify the type of the constant, as well as its value."
  );
  using x = constant<int, -15>;

  PRINT("x = ", type_str<x>());
  PRINT("x::value = ", x::value);

  using y = constant<bool, true>;

  PRINT("y = ", type_str<y>());
  PRINT("y::value = ", std::boolalpha, y::value);

  COMMENT(
    "Again, any expression that can be evaluated at compile time will do:"
  );

  using z = constant<unsigned, (x::value > 0) ? x::value : -x::value>;

  PRINT("z = ", type_str<z>());
  PRINT("z::value = ", z::value);
}

/**
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
LESSON(
  "representing values, part 4/4",
  "This lesson gives an overview of some basic features that "
  "`std::integral_constant` offers."
  "\n\n"
  "The implementation and library features built around "
  "`std::integral_constant` are a bit more involved than what we've seen so "
  "far, but for the purposes of a lesson, we don't need to dig too deep."
  "\n\n"
  "For now, let's look at a few more things that `std::integral_constant` "
  "offers."
) {
  COMMENT(
    "We already covered how to represent a compile time constant with a type, "
    "and how to access the constant's value."
  );
  using x = std::integral_constant<int, -15>;

  PRINT("x = ", type_str<x>());
  PRINT("x::value = ", x::value);

  COMMENT(
    "For convenience purposes, `std::integral_constant` also provides an "
    "identity alias in the form of a member called `type`:"
  );
  PRINT("x::type = ", type_str<x::type>());

  COMMENT(
    "It also exposes the type of the constant it represents:"
  );
  PRINT("x::value_type = ", type_str<x::value_type>());

  COMMENT(
    "Shortcuts to boolean constants are also provided:"
  );

  using t = std::true_type;
  PRINT("t = ", type_str<t>());
  PRINT("t::value = ", std::boolalpha, t::value);
  PRINT("t::value_type = ", type_str<t::value_type>());

  using f = std::false_type;
  PRINT("f = ", type_str<f>());
  PRINT("f::value = ", std::boolalpha, f::value);
  PRINT("f::value_type = ", type_str<f::value_type>());
}

/**
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
LESSON(
  "convenience aliases",
  "This lesson gives an overview on how to reduce verbosity through the use of "
  "convenience aliases."
  "\n\n"
  "Some types will be extensively used throughout the examples in this lesson. "
  "For instance, `std::integral_constant` for `int` values."
  "\n\n"
  "For this reason, let's see how we can shorten the code we write when "
  "declaring an integral constant through the use of aliases.",

  template <int Value>
  using int_value = std::integral_constant<int, Value>;
) {
  COMMENT(
    "Let's start by going the verbose route and fully specifying `x` as an "
    "`std::integral_constant`."
  );

  using x = std::integral_constant<int, 10>;

  PRINT("x = ", type_str<x>());
  PRINT("x::value = ", x::value);

  COMMENT(
    "Now let's use the convenient alias `int_value` to declare the same thing."
  );

  using y = int_value<10>;

  PRINT("y = ", type_str<y>());
  PRINT("y::value = ", y::value);

  COMMENT(
    "The beauty of aliases is that they don't create new types. Instead, "
    "they're just shortcuts to existing types. For instance, by checking the "
    "output of this lesson, it's easy to see that both `x` and `y` reference "
    "exactly the same type: `std::integral_constant<int, 10>`."
    "\n\n"
    "The code below will be further explained in a later lesson. For now, it "
    "suffices to know that it will prevent the program from compiling "
    "correctly if both `x` and `y` do not represent the same type."
    "\n\n"
    "This means that, if the line below doesn't result in a compilation error, "
    "then both `x` and `y` are guaranteed to reference the same type."
  );

  static_assert(std::is_same<x, y>::value, "type mismatch");
}

} // namespace lesson {
