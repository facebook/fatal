# Demonstration programs for Fatal
Demonstration programs can be found under this directory.


## Requirements for demos
A compliant C++14 compiler.

There are no other external dependencies.


## Building demos
```sh
$ clang++ -Wall -Werror -O2 -std=c++17 -I path/to/fatal \
  -o path/to/output/binary demo/demo_filename.cpp
```
or
```sh
$ g++ -Wall -Werror -O2 -std=c++17 -I path/to/fatal \
  -o path/to/output/binary demo/demo_filename.cpp
```
