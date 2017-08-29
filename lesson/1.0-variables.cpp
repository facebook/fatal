/*
 *  Copyright (c) 2016, Facebook, Inc.
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
  "variables, part 1/2",
  "This lesson demonstrates how to achieve, in metaprogramming, something "
  "similar to the notion of variables in procedural programming."
) {
  COMMENT(
    "If you think about a duck typed language, we're used to declaring "
    "variables by assigning a value to a name."
  );
  CODE(
    auto x = 10;
  );

  COMMENT(
    "Whenever we want to retrieve the value stored in that variable, we "
    "reference it through its name:"
  )
  VALUE(x);

  COMMENT(
    "A similar idiom can be achieved for metaprogramming. Take the following "
    "line, for example:"
  );
  CODE(
    using y = std::integral_constant<int, 10>;
  );

  COMMENT(
    "We're also assigning a value `10`, of type `int`, to the name `y`."
    "\n\n"
    "The difference is that instead of variables, we're using an alias to a "
    "type."
    "\n\n"
    "Just like with variables, we can also retrieve what's stored in that "
    "type alias:"
  );

  TYPE(y);

  COMMENT(
    "Note that we must use a helper function called `type_str()` to properly "
    "convert types into their actual string representation."
    "\n\n"
    "The type `std::integral_constant`, in particular, can be used to "
    "represent a constant value. Keep in mind, though, that types are "
    "immutable, therefore we cannot change the value represented by the "
    "integral constant `y`."
    "\n\n"
    "We don't need to use `type_str()` here because the value itself is a "
    "constant, not a type:"
  );

  VALUE(y::value);

  ILLEGAL(
    "it's illegal to re-assign a value to a constant.",
    y::value = 99;
  );

  COMMENT(
    "That's the first thing you should notice about metaprogramming: we don't "
    "manipulate values. We manipulate types."
    "\n\n"
    "Types only exist during compilation, so one could say metaprograms run "
    "inside the compiler, at  compile time, as opposed to regular programs "
    "which run at runtime, after compilation is done."
    "\n\n"
    "There are some types that can represent values, like "
    "`std::integral_constant`, but that's not necessarily true for every type."
    "\n\n"
    "For instance, we could create an alias for the type `void`, or `int`, or "
    "even `std::string`:"
  );

  CODE(
    using z = void;
    using w = int;
    using k = std::string;
  );

  COMMENT(
    "A `int` or `std::string` runtime variable is able to store values, yes, "
    "but as for the types themselves, `int` and `std::string`, they don't "
    "really represent any values at compile time. Assigning values to them "
    "only make sense at runtime."
  );

  TYPE(z);
  TYPE(w);
  TYPE(k);

  COMMENT(
    "The last thing to notice is that types are immutable. Once we assign a "
    "type to an alias, that alias will always represent the same type."
    "\n\n"
    "That's a hint that metaprogramming in C++ works similarly to pure "
    "functional programming."
  );

  ILLEGAL(
    "it's illegal to re-assign a type to an alias.",
    w = long;
  );

  COMMENT(
    "SUMMARY: metaprogramming deals with types, not values. We can simulate "
    "values using types like `std::integral_constant` or `std::ratio`. But, "
    "ultimately, they're just types."
    "\n\n"
    "Types are immutable."
    "\n\n"
    "Metaprograms run at compile time, whereas regular programs run at runtime."
  );
}

/**
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
LESSON(
  "variables, part 2/2",
  "This lesson is a follow up to the variables lesson above."
  "\n\n"
  "Declarations of types below take place outside of the lesson block for "
  "reasons outside of the scope of this guide, but they should be considered "
  "part of it nevertheless.",

  struct m {
    int value;
  };

  class n {
    void method();
  };

  union u {
    int i;
    double fp;
  };

  enum e { field0, field1, field2 };

  enum class c { field3, field4, field5 };
) {
  COMMENT(
    "Not only type aliases, but classes, structures, enumerations and unions "
    "can also be considered analogous to procedural programming's variables:"
  );

  TYPE(m);
  TYPE(n);
  TYPE(u);
  TYPE(e);
  TYPE(c);

  COMMENT(
    "And they can be assigned to aliases as well."
  );

  CODE(
    using vm = m;
    using vn = n;
    using vu = u;
    using ve = e;
    using vc = c;
  );

  TYPE(vm);
  TYPE(vn);
  TYPE(vu);
  TYPE(ve);
  TYPE(vc);
}

} // namespace lesson {
