# Demonstration programs for Fatal
Demonstration programs can be found under this directory.


## Requirements
A compliant C++14 compiler. Currently tested under Clang {3.4, 3.5, 3.6, 3.7} and GCC {4.9, 5.1}.

There are no other external dependencies.


## Building
```sh
$ clang++ -Wall -Werror -O2 -std=c++1y -I path/to/fatal \
  -o path/to/output/binary path/to/demo.cpp
```
or
```sh
$ g++ -Wall -Werror -O2 -std=c++1y -I path/to/fatal \
  -o path/to/output/binary path/to/demo.cpp
```
