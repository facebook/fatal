# Learn Fatal and Meta-programming by example
This directory contains a set of lessons aimed at teaching the basics of meta-programming in C++.

The lessons gradually introduce idioms to better use Fatal when writing software.

Lessons source code is self-contained, presenting a full textual explanation of the lesson itself.

The output of the lessons is also self-contained, presenting the full textual explanation and source code.

Lessons can be followed by reading the source code only, the lesson's output only, or both.


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
