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

LESSON(
  "decltype()",
  "Before we proceed, let's introduce a handy keyword: decltype."
  "\n\n"
  "This is some magic operator that returns the type of an expression."
  "\n\n"
  "decltype is very handy when trying to figure out what's the type of, say, a "
  "variable, a function or the result of a function call."
) {
  COMMENT(
    "We can use decltype to figure out what's the type of a literal. It works "
    "as if we had typed an alias or a type name directly."
  );
  CODE(
    using a = decltype(10);
    using b = decltype(true);
    using c = decltype("hello, world");
  );
  TYPE(a);
  TYPE(b);
  TYPE(c);

  COMMENT(
    "We can also use decltype to inspect the type of a variable."
  );
  CODE(
    int x = 20;
    bool y = false;
    auto z = "test";

    using d = decltype(x);
    using e = decltype(y);
    using f = decltype(z);
  );
  TYPE(d);
  TYPE(e);
  TYPE(f);

  COMMENT(
    "Here we use decltype to check what's the type returned by a function."
  );
  CODE(
    using j = decltype(std::atoi("10"));
  );
  TYPE(j);

  COMMENT(
    "But decltype can also be used to check what's the type of the function "
    "itself."
  );
  CODE(
    using k = decltype(std::atoi);
  );
  TYPE(k);
}

/**
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
LESSON(
  "template instantiations as metafunctions",
  "Just like in procedural programming where we can apply functions on values "
  "and obtain another value as a result, in metaprogramming we can apply "
  "metafunctions on types and obtain another type as a result. The concept is "
  "not too different. Only now, instead of manipulating values, we're "
  "manipulating types."
  "\n\n"
  "C++ also allows metafunctions to operate on constants and other "
  "metafunctions (higher order metafunctions). Roughly speaking, metafunctions "
  "in C++ operate on anything that can be passed as a template parameter."
  "\n\n"
  "For the purpose of this lesson, we'll not differentiate types and constants "
  "as metafunction parameters."
  "\n\n"
  "Throughout this lesson and the Fatal library we also use the terms "
  "\"operation\" and \"transform\" to refer to metafunctions. For the sake of "
  "the exercise, let's assume they are all the same thing."
  "\n\n"
  "The simplest metafunction we can think of is a template instantiation. That "
  "is, given that we have a class template, if you will, we can create an "
  "actual type by instantiating it after passing proper template arguments."
  "\n\n"
  "Why is a type template considered a metafunction? Because it takes types as "
  "parameters (template arguments) and outputs a type (template instantiation) "
  "as the result. This may not make much sense now but, as we will see in a "
  "later lesson, making no distinction greatly simplifies things when using "
  "higher-order metafunctions.",

  template <typename T, typename U>
  class foo {
    T data1;
    U data2;
  };
) {
  COMMENT(
    "`foo` template is a class template, not a class. In other words, it's a "
    "type template, not an actual type. In order to stay away from the formal "
    "lingo, let's just say that it's impossible to instantiate an object of "
    "type `foo`."
  );
  ILLEGAL(
    "can't use an uninstantiated type template as a type",
    foo f1;
  );

  COMMENT(
    "In order to obtain an actual type from a type template, we must "
    "instantiate it by passing the appropriate template parameters required by "
    "the template."
    "\n\n"
    "Since we're talking about metafunctions, the code below passes two "
    "parameters, `int` and `double`, to `foo` and obtains the type "
    "`foo<int, double>` as a result."
  );
  CODE(
    using template_instantiation = foo<int, double>;
  );

  COMMENT(
    "Now that we finally have a type, we can instantiate an object with it:"
  );
  CODE(
    template_instantiation f2;
  );

  COMMENT(
    "Or we can skip the alias altogether and instantiate the template and the "
    "object in the same expression:"
  );
  CODE(
    foo<int, double> f3;
  );

  COMMENT(
    "Both `f2` and `f3` are variables of the same type: `foo<int, double>`."
    "\n\n"
    "The code below will be further explained in a later lesson. For now, it "
    "suffices to know that it will prevent the program from compiling if both "
    "arguments passed to `std::is_same` do not represent the same type."
    "\n\n"
    "In other words, if this code compiles, then both arguments passed to "
    "`std::is_same` refer to exactly the same type."
  );
  CODE(
    static_assert(std::is_same<decltype(f2), decltype(f3)>::value, "mismatch");
  );

  COMMENT(
    "For a more familiar example, let's use the `std::vector` template instead:"
  );
  ILLEGAL(
    "again, can't use an uninstantiated type template as a type",
    std::vector v1;
  );
  COMMENT(
    "But it works when we instantiate the template in order to obtain a type. "
    "In this case, `std::vector<int>`:"
  );
  CODE(
    using my_list = std::vector<int>;

    my_list v2;

    std::vector<int> v3;

    static_assert(std::is_same<decltype(v2), decltype(v3)>::value, "mismatch");
  );
}

/**
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
LESSON(
  "custom metafunctions",
  "In this lesson we will write some simple metafunctions. Some of them will "
  "be pretty useless while some will be used by a later lesson to illustrate "
  "another concept."
  "\n\n"
  "The point of this lesson is to demonstrate how to declare and use custom "
  "metafunctions by passing parameters and getting results.",

  template <typename T>
  struct unary {};

  template <typename T, typename U>
  struct binary {};

  template <typename T, typename U, typename V>
  struct ternary {};

  template <typename... Args>
  struct variadic {};

  template <typename T, typename U, typename... Args>
  struct another_variadic {};

  template <typename T>
  struct expose_member_named_yyz {
    using yyz = T;
  };

  template <typename First, typename Second>
  struct simple_pair {
    using first = First;
    using second = Second;
  };
) {
  COMMENT(
    "Let's start with a simple template that takes a single parameter and does "
    "nothing. It doesn't really return anything so, just for the sake of the "
    "exercise, we'll consider the template instantiation itself as being the "
    "result of the metafunction."
  );
  CODE(
    using a = unary<int>;
    using b = unary<bool>;
    using c = unary<int>;
  );
  TYPE(a);
  TYPE(b);
  TYPE(c);

  COMMENT(
    "Note that `a` and `c` pass the same parameters to the metafunction. "
    "Template metaprogramming is considered purely functional. Therefore, "
    "it is not possible to have side-effects from a call to a metafunction."
    "\n\n"
    "This means that both `a` and `c` above represent exactly the same type "
    "since there's no internal state in the metafunctions that could change "
    "between the calls."
    "\n\n"
    "The static assertion below makes sure of it."
  );
  CODE(
    static_assert(std::is_same<a, c>::value, "mismatch");
  );
  COMMENT(
    "The assertions below are similar, but they will compile if and only if "
    "both arguments passed to `std::is_same` do NOT represent the same type."
  );
  CODE(
    static_assert(!std::is_same<a, b>::value, "mismatch");
    static_assert(!std::is_same<b, c>::value, "mismatch");
  );

  COMMENT(
    "This is a dummy example, similar to the one above, but demonstrating a "
    "metafunction taking two parameters instead of one."
  );
  CODE(
    using d = binary<int, bool>;
    using e = binary<bool, int>;
    using f = binary<int, bool>;
  );
  TYPE(d);
  TYPE(e);
  TYPE(f);

  COMMENT(
    "Again, subsequent calls will have the same results."
  );
  CODE(
    static_assert(std::is_same<d, f>::value, "mismatch");
    static_assert(!std::is_same<d, e>::value, "mismatch");
    static_assert(!std::is_same<e, f>::value, "mismatch");
  );

  COMMENT(
    "Below is a third example, just like the ones above. This time it "
    "demonstrates a metafunction taking three parameters."
  );
  CODE(
    using g = ternary<int, bool, double>;
    using h = ternary<bool, double, int>;
    using i = ternary<int, bool, double>;
  );
  TYPE(g);
  TYPE(h);
  TYPE(i);

  COMMENT(
    "Once more, subsequent calls have the same results."
  );
  CODE(
    static_assert(std::is_same<g, i>::value, "mismatch");
    static_assert(!std::is_same<g, h>::value, "mismatch");
    static_assert(!std::is_same<h, i>::value, "mismatch");
  );

  COMMENT(
    "Below is an example of a variadic metafunction. That means it can take "
    "any number of parameters."
    "\n\n"
    "We'll take a closer look at variadics later. Right now we're only "
    "interested in knowing they exist, how to declare and how to use them."
  );
  CODE(
    using j = variadic<>;
    using k = variadic<bool>;
    using l = variadic<int, double>;
    using m = variadic<int, bool, double>;
    using n = variadic<bool, void, short, long, double, float, long, bool>;
    using o = variadic<int, bool, double>;
  );
  TYPE(j);
  TYPE(k);
  TYPE(l);
  TYPE(m);
  TYPE(n);
  TYPE(o);

  COMMENT(
    "It is still true that subsequent calls have the same results."
  );
  CODE(
    static_assert(std::is_same<m, o>::value, "mismatch");
    static_assert(!std::is_same<j, m>::value, "mismatch");
  );

  COMMENT(
    "We can also require a minimum number of parameters by declaring "
    "non-variadic parameters for the metafunction:"
  );
  ILLEGAL(
    "`another_variadic` requires at least two arguments to be passed",
    using p = another_variadic<>;
    using q = another_variadic<bool>;
  );
  CODE(
    using r = another_variadic<int, double>;
    using s = another_variadic<int, bool, double>;
    using t = another_variadic<bool, void, short, long, double, long, bool>;
    using u = another_variadic<int, bool, double>;
  );
  TYPE(r);
  TYPE(s);
  TYPE(t);
  TYPE(u);

  COMMENT(
    "Once more, the same parameters yield the same results."
  );
  CODE(
    static_assert(std::is_same<s, u>::value, "mismatch");
    static_assert(!std::is_same<s, t>::value, "mismatch");
  );

  COMMENT(
    "A classic example of a variadic template is `std::tuple`."
  );
  CODE(
    using v = std::tuple<>;
    using w = std::tuple<bool>;
    using x = std::tuple<int, bool, double>;
    using y = std::tuple<bool, short, long, double, float, long, bool>;
  );
  TYPE(v);
  TYPE(w);
  TYPE(x);
  TYPE(y);

  COMMENT(
    "Sometimes it's useful to return something from the metafunction, other "
    "than the template instantiation itself. The easiest way to do that is "
    "with a member type alias."
    "\n\n"
    "The metafunction `expose_member_named_yyz`, as its name suggests, exposes "
    "a member named `yyz`. For the sake of this exercise, we will consider "
    "this member to represent its result."
    "\n\n"
    "There's no rule telling how to properly return results from metafunctions "
    "so the best bet is to resort to some arbitrary convention. Using member "
    "alias templates is one such convention. As long as the intention is made "
    "clear and it doesn't hurt API usability, all's good."
    "\n\n"
    "Granted, `yyz` is not a very good member name, but it's definitely a damn "
    "great song, so let's stick with it for now."
    "\n\n"
    "Since the aim of this lesson is to illustrate how to return results let's "
    "not focus on what is returned, just on how to set the result and how to "
    "access it from the caller's standpoint."
  );
  CODE(
    using z = expose_member_named_yyz<void>;
  );
  TYPE(z);
  TYPE(z::yyz);

  COMMENT(
    "There's also the possibility of returning more than one result from a "
    "metafunction. This is actually not uncommon in template metaprogramming."
    "\n\n"
    "The easiest way to accomplish that is still to provide a member type "
    "alias for each of the results we want to return."
    "\n\n"
    "The metafunction `simple_pair` exposes two member type aliases called "
    "`first` and `second`. Again, let's not worry about what is returned. For "
    "the purpose of this exercise we'll assume `first` and `second` to "
    "represent the two results returned by this metafunction."
  );
  CODE(
    using A = simple_pair<short, long>;
  );
  TYPE(A);
  TYPE(A::first);
  TYPE(A::second);
}

/**
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
LESSON(
  "operations on values (1/2)",
  "In this lesson we will write metafunctions that operate on numbers. For "
  "instance, we can define the four basic arithmetic operations as "
  "metafunctions."
  "\n\n"
  "It may not seem very useful, but this allows us to, as we will see, compose "
  "these operations in any way and pass this composition around as new "
  "metafunctions. The usefulness of this code will become much clearer when we "
  "cover higher-order metafunctions.",

  // convenience alias for the arithmetic examples
  template <int Value>
  using int_val = std::integral_constant<int, Value>;

  template <typename LHS, typename RHS>
  using add = int_val<LHS::value + RHS::value>;

  template <typename LHS, typename RHS>
  using subtract = int_val<LHS::value - RHS::value>;

  template <typename LHS, typename RHS>
  using multiply = int_val<LHS::value * RHS::value>;

  template <typename LHS, typename RHS>
  using divide = int_val<LHS::value / RHS::value>;

  template <typename A, typename B, typename C, typename D>
  using composite = multiply<subtract<A, B>, add<C, D>>;
) {
  COMMENT(
    "Let's start by declaring a few constants, just so our examples don't get "
    "too verbose."
  );
  CODE(
    using i3 = int_val<3>;
    using i5 = int_val<5>;
    using i7 = int_val<7>;
    using i10 = int_val<10>;
    using i20 = int_val<20>;
  );
  CONSTANT(i3);
  CONSTANT(i5);
  CONSTANT(i7);
  CONSTANT(i10);
  CONSTANT(i20);

  COMMENT(
    "Similarly to a regular function call, we pass the constants above as "
    "parameters to the metafunctions, which in turn return the result as a "
    "type."
  );
  CODE(
    using a = add<i5, i10>;
    using b = subtract<i3, i5>;
    using c = multiply<i3, i7>;
    using d = divide<i20, i7>;
  );
  CONSTANT(a);
  CONSTANT(b);
  CONSTANT(c);
  CONSTANT(d);

  COMMENT(
    "Metafunctions can also be composed:"
  );
  CONSTANT(add<add<i5, i10>, i20>);
  CONSTANT(subtract<multiply<i20, i3>, add<i5, i10>>);
  CONSTANT(multiply<subtract<i20, i10>, add<i3, i3>>);
  CONSTANT(divide<i20, add<i3, i3>>);
  CONSTANT(
    divide<
      multiply<
        add<i5, i10>,
        subtract<i20, i7>
      >,
      i3
    >
  );

  COMMENT(
    "And the composition can be exposed as yet another metafunction, as in "
    "`composite`:"
  );
  CONSTANT(composite<i20, i10, i3, i5>);
}

/**
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
LESSON(
  "operations on values (2/2)",
  "In this lesson we will show an example of how operations on values can be "
  "useful for a more practical problem."
  "\n\n"
  "Specifically, we will determine, at compile time, the size of a buffer as a "
  "function of the maximum amount of bytes we want it to take up."
  "\n\n"
  "We will reuse the metafunctions defined by the previous 'operations on "
  "values' lesson.",

  template <typename T>
  using size_of = int_val<sizeof(T)>;

  template <typename LHS, typename RHS>
  using maximum = int_val<LHS::value < RHS::value ? RHS::value : LHS::value>;

  template <typename T, typename MaxByteSize>
  using buffer_size = maximum<int_val<1>, divide<MaxByteSize, size_of<T>>>;

  template <typename T, int MaxByteSize>
  using buffer = std::array<T, buffer_size<T, int_val<MaxByteSize>>::value>;

  struct my_element {
    std::int32_t field1;
    std::int16_t field2[4];
    std::uint64_t field3;
    char field4[80];
  };
) {
  COMMENT(
    "We start by writing a metafunction called `size_of`, that calculates the "
    "size, in bytes, of a given type."
  );
  CONSTANT(size_of<char>);
  CONSTANT(size_of<double>);
  CONSTANT(size_of<std::int16_t>);
  CONSTANT(size_of<std::int64_t>);
  CONSTANT(size_of<my_element>);

  COMMENT(
    "We then write a metafunction called `buffer_size` to calculate the "
    "maximum number of elements of a given type that will not exceed a given "
    "number of bytes."
    "\n\n"
    "Since we're not interested in a buffer with zero elements, we make an "
    "exception for when a single element exceeds the byte threshold."
  );
  CODE(
    using max_byte_size = int_val<90>;
  );
  CONSTANT(buffer_size<char, max_byte_size>);
  CONSTANT(buffer_size<double, max_byte_size>);
  CONSTANT(buffer_size<std::int16_t, max_byte_size>);
  CONSTANT(buffer_size<std::int64_t, max_byte_size>);
  CONSTANT(buffer_size<my_element, max_byte_size>);

  COMMENT(
    "Finally, we write a metafunction called `buffer` that returns the type of "
    "the buffer as a `std::array`, using `buffer_size` to determine the "
    "appropriate size."
  );
  TYPE(buffer<char, 820>);
  TYPE(buffer<double, 514>);
  TYPE(buffer<std::int16_t, 5150>);
  TYPE(buffer<std::int64_t, 2112>);
  TYPE(buffer<my_element, 600>);
}

/**
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
LESSON(
  "standard operations on types",
  "Not all metafunctions need to be type templates or operate on constants. In "
  "fact, it's very common to have alias templates that operate on types, "
  "regardless of whether these represent actual values or not."
  "\n\n"
  "It will become clearer on later lessons how useful it is to manipulate "
  "types that do not represent actual values."
  "\n\n"
  "For now, it suffices to understand how such operations on types work, in "
  "order not to become biased by the examples that use "
  "`std::integral_constant`."
  "\n\n"
  "The standard library already provides a lot of ready-to-use metafunctions. "
  "We'll start with these existing operations. They can mostly be found in the "
  "`<type_traits>` header."
) {
  COMMENT(
    "One such metafunction provided by the standard library is called "
    "`is_reference`. It receives a type as a parameter and, as its name "
    "suggests, returns a `true` or `false` constant telling whether the "
    "type is a reference or not."
  );
  CODE(
    using ir1 = std::is_reference<int>;
    using ir2 = std::is_reference<int &>;
  );
  CONSTANT(ir1);
  CONSTANT(ir2);

  COMMENT(
    "Another metafunction, called `add_lvalue_reference`, takes a type as a "
    "parameter and returns a l-value reference to this type. If the input type "
    "is already an l-value reference, it returns the type itself."
  );
  CODE(
    using r1 = std::add_lvalue_reference<int>;
    using r2 = std::add_lvalue_reference<int &>;
    using r3 = std::add_lvalue_reference<int const>;
    using r4 = std::add_lvalue_reference<int const &>;
  );
  TYPE(r1);
  TYPE(r1::type);
  CONSTANT(std::is_reference<r1::type>);
  TYPE(r2);
  TYPE(r2::type);
  CONSTANT(std::is_reference<r2::type>);
  TYPE(r3);
  TYPE(r3::type);
  CONSTANT(std::is_reference<r3::type>);
  TYPE(r4);
  TYPE(r4::type);
  CONSTANT(std::is_reference<r4::type>);

  COMMENT(
    "There's also a metafunction called `is_signed` which tells whether a type "
    "a signed arithmetic type or not."
  );
  CODE(
    using is1 = std::is_signed<int>;
    using is2 = std::is_signed<std::uint32_t>;
    using is3 = std::is_signed<double>;
    using is4 = std::is_signed<std::string>;
  );
  CONSTANT(is1);
  CONSTANT(is2);
  CONSTANT(is3);
  CONSTANT(is4);

  COMMENT(
    "It's also widely known that C++ templates are Turing complete. Here's an "
    "example of how to write conditional statements that choose one of two "
    "types depending on a condition expression."
  );
  CODE(
    using t = std::conditional<true, float, double>;
    using f = std::conditional<false, short, long>;
  );
  TYPE(t);
  TYPE(t::type);
  TYPE(f);
  TYPE(f::type);

  COMMENT(
    "Finally, we can compose these metafunctions in countless ways."
  );
  CODE(
    using c = std::conditional<
      std::is_signed<int>::value,
      std::add_lvalue_reference<bool>::type,
      void
    >;
  );
  TYPE(c);
  TYPE(c::type);
}

/**
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
LESSON(
  "custom operations on types 1/2",
  "We'll now show how to write a few custom metafunctions that operate on "
  "types."
  "\n\n"
  "We will use some of the metafunctions introduced by the previous 'custom "
  "metafunctions' lesson.",

  template <typename T>
  struct get_member_named_yyz {
    using result = typename T::yyz;
  };

  template <typename T>
  struct get_type_and_member_named_yyz {
    using type = T;
    using result = typename T::yyz;
  };

  template <typename T>
  struct another_type_and_yyz {
    using type = T;
    using result = typename get_member_named_yyz<type>::result;
  };

  template <typename T>
  using simpler_get_yyz = typename T::yyz;

  template <typename T>
  struct fancier_type_and_yyz {
    using type = T;
    using result = simpler_get_yyz<type>;
  };
) {
  COMMENT(
    "The metafunction called `get_member_named_yyz`, as its name suggests, "
    "looks for a member type alias named `yyz` in the type received as a "
    "parameter. It then returns the type represented by that alias as its "
    "result."
  );
  CODE(
    using a = expose_member_named_yyz<void>;
    using b = get_member_named_yyz<a>;
  );
  TYPE(b);
  TYPE(b::result);

  COMMENT(
    "The metafunction called `get_type_and_member_named_yyz` returns two "
    "results. It exposes the argument it received as a member type alias "
    "called `type`. It also exposes its second result as a member type alias "
    "called `result`, representing `type::yyz`."
  );
  CODE(
    using c = get_type_and_member_named_yyz<a>;
  );
  TYPE(c);
  TYPE(c::type);
  TYPE(c::result);

  COMMENT(
    "The metafunction called `another_type_and_yyz` does the same thing as "
    "`get_type_and_member_named_yyz`, albeit in a different way. It employs "
    "another metafunction, `get_member_named_yyz`, to obtain `type::yyz`."
  );
  CODE(
    using d = another_type_and_yyz<a>;
  );
  TYPE(d);
  TYPE(d::type);
  TYPE(d::result);

  COMMENT(
    "Metafunctions don't have to be written as classes or structs. They can "
    "also be written as alias templates. Instead of returning results as "
    "members, their actual template instantiation represents their results."
    "\n\n"
    "This allows for a friendlier syntax, much closer to that of a function "
    "call in prodcedural languages. We've seen a similar approach in the "
    "lesson 'operations on values'."
    "\n\n"
    "Below we demonstrate `simpler_get_yyz`, which does exactly what "
    "`get_member_named_yyz` does, but in the form of an alias template."
  );
  TYPE(simpler_get_yyz<a>);

  COMMENT(
    "In the same fashion as `another_type_and_yyz`, `fancier_type_and_yyz` "
    "uses `simpler_get_yyz` to obtain `type::yyz`."
  );
  CODE(
    using e = fancier_type_and_yyz<a>;
  );
  TYPE(e);
  TYPE(e::type);
  TYPE(e::result);
}

/**
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
LESSON(
  "custom operations on types 2/2",
  "This lesson introduces some metafunctions that perform type manipulation on "
  "their parameters.",

  template <typename T>
  struct make_it_a_pointer {
    using pointer = T *;
  };

  template <typename T>
  struct make_it_a_pointer_if_not_already {
    using type = typename std::conditional<
      std::is_pointer<T>::value,
      T,
      T *
    >::type;
  };

  template <typename T>
  using simpler_make_it_a_pointer_if_not_already = typename std::conditional<
    std::is_pointer<T>::value,
    T,
    T *
  >::type;

  template <typename T>
  struct cleanup_type {
    using type = typename std::remove_const<
      typename std::remove_reference<T>::type
    >::type;
  };
) {
  COMMENT(
    "The metafunction called `make_it_a_pointer`, as its name suggests, "
    "converts the input type into a pointer to it."
  );
  CODE(
    using a = make_it_a_pointer<double>;
    using b = make_it_a_pointer<short *>;
  );
  TYPE(a);
  TYPE(a::pointer);
  TYPE(b);
  TYPE(b::pointer);

  COMMENT(
    "The metafunction called `make_it_a_pointer_if_not_already` is a little "
    "smarter and only performs the conversion if the input type is not yet a "
    "pointer."
  );
  CODE(
    using c = make_it_a_pointer_if_not_already<double>;
    using d = make_it_a_pointer_if_not_already<short *>;
  );
  TYPE(c);
  TYPE(c::type);
  TYPE(d);
  TYPE(d::type);

  COMMENT(
    "`simpler_make_it_a_pointer_if_not_already` does the same thing as "
    "`make_it_a_pointer_if_not_already`, but in the form of an alias template."
  );
  TYPE(simpler_make_it_a_pointer_if_not_already<double>);
  TYPE(simpler_make_it_a_pointer_if_not_already<short *>);

  COMMENT(
    "`cleanup_type` aims to remove any references and const-qualifiers from "
    "the input type."
  );
  CODE(
    using e = cleanup_type<double>;
    using f = cleanup_type<short *>;
    using g = cleanup_type<int const>;
    using h = cleanup_type<double &>;
    using i = cleanup_type<bool &&>;
    using j = cleanup_type<float const &>;
    using k = cleanup_type<unsigned const &&>;
    using l = cleanup_type<long const *const &>;
  );
  TYPE(e);
  TYPE(e::type);
  TYPE(f);
  TYPE(f::type);
  TYPE(g);
  TYPE(g::type);
  TYPE(h);
  TYPE(h::type);
  TYPE(i);
  TYPE(i::type);
  TYPE(j);
  TYPE(j::type);
  TYPE(k);
  TYPE(k::type);
  TYPE(l);
  TYPE(l::type);

  COMMENT(
    "`cleanup_type` resembles a much more useful metafunction present in the "
    "standard library called `std::decay`. This is a very important "
    "metafunction, widely used in metaprogramming."
    "\n\n"
    "It is worth to take some time to get more familiar with it since we'll "
    "need this metafunction it in later lessons: "
    "http://en.cppreference.com/w/cpp/types/decay"
  );
  CODE(
    using m = std::decay<double>;
    using n = std::decay<short *>;
    using o = std::decay<int const>;
    using p = std::decay<double &>;
    using q = std::decay<bool &&>;
    using r = std::decay<float const &>;
    using s = std::decay<unsigned const &&>;
    using t = std::decay<long const *const &>;
  );
  TYPE(m);
  TYPE(m::type);
  TYPE(n);
  TYPE(n::type);
  TYPE(o);
  TYPE(o::type);
  TYPE(p);
  TYPE(p::type);
  TYPE(q);
  TYPE(q::type);
  TYPE(r);
  TYPE(r::type);
  TYPE(s);
  TYPE(s::type);
  TYPE(t);
  TYPE(t::type);
}

/**
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
LESSON(
  "nested metafunctions",
  "Sometimes it's desirable to have metafunctions taking several arguments. "
  "This can easily hinder usability of the API, therefore a nice solution is "
  "needed."
  "\n\n"
  "There can also be a need to group several related metafunctions together, "
  "with a possible intersection on the set of parameters they accept."
  "\n\n"
  "One way to tackle this problem is to use nested metafunctions. That is, a "
  "metafunction that is not limited to exposing results, but also other "
  "inner metafunctions that depend on the parameters of the outer one."
  "\n\n"
  "Nested metafunctions will make more sense once we cover higher-order "
  "metafunctions on a later lesson. For now, it suffices to know they are "
  "possible and how they are declared."
  "\n\n"
  "C++ template syntax can be quite daunting, but the examples presented in "
  "this lesson are not that complicated. Try to identify what does each member "
  "represent and the patterns used to implement them. Several of these "
  "patterns are quite recurring.",

  template <typename T>
  struct nested {
    using type = unary<T>;

    template <typename U>
    using inner = std::pair<T, U>;
  };

  template <typename T>
  struct nested_2 {
    using type = unary<T>;

    template <typename U>
    struct inner {
      using type = binary<T, U>;
    };
  };

  template <typename T>
  struct nested_3 {
    using type = unary<T>;

    template <typename U>
    struct inner {
      using type = binary<T, U>;

      template <typename V>
      using innermost = ternary<T, U, V>;
    };
  };

  template <typename T, typename U>
  struct nested_4_inner {
    using type = binary<T, U>;

    template <typename V>
    using innermost = ternary<T, U, V>;
  };

  template <typename T>
  struct nested_4 {
    using type = unary<T>;

    template <typename U>
    using inner = nested_4_inner<T, U>;
  };
) {
  COMMENT(
    "Let's start by calling the metafunction `nested`. It exposes two things:\n"
    "- a member alias called `type`, which we'll consider its result\n"
    "- a member metafunction called `inner`"
    "\n\n"
    "As far as the member `type` is concerned, nothing new here:"
  );
  CODE(
    using a = nested<int>;
  );
  TYPE(a);
  TYPE(a::type);
  COMMENT(
    "Now, let's take a look at the inner metafunction. It is implemented as an "
    "alias template, therefore its result will come directly from its "
    "instantiation:"
  );
  CODE(
    using b = a::inner<double>;
  );
  TYPE(b);

  COMMENT(
    "Let's also look at `nested_2`, which is a slight variation of `nested`."
    "\n\n"
    "The only difference between them is that `nested`'s inner metafunction is "
    "implemented with an alias template, whereas `nested_2`'s inner "
    "metafunction is implemented as a regular class template which exposes its "
    "result as a member."
  );
  CODE(
    using c = nested_2<int>;
  );
  TYPE(c);
  TYPE(c::type);
  COMMENT(
    "So let's look at the application of the inner metafunction and also at "
    "its result:"
  );
  CODE(
    using d = c::inner<double>;
  );
  TYPE(d);
  TYPE(d::type);

  COMMENT(
    "There are pros and cons for each approach. For instance, the first one "
    "requires less typing and looks simpler, while the second one allows for "
    "more than one result to be returned."
    "\n\n"
    "The latter also allows exposing yet another level of one or more inner "
    "metafunctions, as illustrated by `nested_3`:"
  );
  CODE(
    using e = nested_3<int>;
  );
  TYPE(e);
  TYPE(e::type);
  COMMENT(
    "Let's inspect `nested_3`'s inner metafunction and its result:"
  );
  CODE(
    using f = e::inner<double>;
  );
  TYPE(f);
  TYPE(f::type);
  COMMENT(
    "And now let's call the innermost metafunction:"
  );
  CODE(
    using g = f::innermost<bool>;
  );
  TYPE(g);

  COMMENT(
    "It may seem too complicated having several nested levels of "
    "metafunctions, and indeed it's usually best to avoid it for the same "
    "reason one should avoid too many nested levels of if statements in "
    "procedural programming: it's best to break the code down into smaller "
    "components than to have a large mammoth that does everything in one place."
    "\n\n"
    "But as far as the technique goes, not much has changed for those familiar "
    "with recursion (which one should be when entering the world of template "
    "metaprogramming). If we consider `f`, the application of `nested_3`'s "
    "inner metafunction, as if it were a separate outer metafunction, it "
    "becomes easier to understand. "
    "\n\n"
    "That's exactly what `nested_4` illustrates below. Note that the usage of "
    "`nested_4` is exactly the same as `nested_3`."
  );
  CODE(
    using h = nested_4<int>;
    using i = h::inner<double>;
    using j = i::innermost<bool>;
  );
  TYPE(h);
  TYPE(h::type);
  TYPE(i);
  TYPE(i::type);
  TYPE(j);
}

} // namespace lesson {
