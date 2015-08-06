# Learn Fatal and Meta-programming by example
This directory contains a set of lessons aimed at teaching the basics of meta-programming in C++.

The lessons gradually introduce idioms to better use Fatal when writing software.

Lessons source code is self-contained, presenting a full textual explanation of the lesson itself.

The output of the lessons is also self-contained, presenting the full textual explanation and source code.

Lessons can be followed by reading the source code only, the lesson's output only, or both.


## Try it online

These lessons can be [tried online](http://abel.web.elte.hu/shell/metashell) using [Metashell](https://github.com/sabel83/metashell): http://abel.web.elte.hu/shell/metashell.

Metashell allows you to easily inspect the type of an alias. Whenever you encounter the `type_str<T>()` function in a lesson, you can simply type `T` instead to get the type represented by it:

```
> using T = std::integral_constant<std::size_t, 42>;
> T
std::integral_constant<std::size_t, 42>
```

Here's an example of how to follow a lesson in Metashell:

```
> #include <fatal/lesson/lesson.h>
> template <typename T, T... Values> \
...> struct sequence_1 {};
> using mp = sequence_1<std::uint32_t, 3, 7, 31, 127, 8191, 131071, 524287>;
> mp
sequence_1<std::uint32_t, 3, 7, 31, 127, 8191, 131071, 524287>
```

Due to its REPL nature it's a much more dynamic environment, highly recommended for learning.


## Requirements
A compliant C++11 compiler. Currently tested under Clang {3.4, 3.5, 3.6, 3.7} and GCC {4.8, 4.9, 5.2}.

There are no other external dependencies.


## Building
```sh
$ clang++ -Wall -Werror -O2 -std=c++11 -I path/to/fatal \
  -o path/to/output/binary path/to/lesson.cpp
```
or
```sh
$ g++ -Wall -Werror -O2 -std=c++11 -I path/to/fatal \
  -o path/to/output/binary path/to/lesson.cpp
```
