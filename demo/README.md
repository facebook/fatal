# Demonstration programs for Fatal
Demonstration programs can be found under this directory.


## Requirements for demos
A compliant C++14 compiler. Currently tested under Clang {[3.4](https://packages.debian.org/sid/clang-3.4), [3.5](https://packages.debian.org/sid/clang-3.5), [3.6](https://packages.debian.org/sid/clang-3.6), [3.7](https://packages.debian.org/sid/clang-3.7), [3.8](https://packages.debian.org/sid/clang-3.8)} and GCC {[4.9](https://packages.debian.org/sid/g++-4.9), [5](https://packages.debian.org/sid/g++-5)}.

There are no other external dependencies.


## Building demos
```sh
$ clang++ -Wall -Werror -O2 -std=c++1y -I path/to/fatal \
  -o path/to/output/binary demo/demo_filename.cpp
```
or
```sh
$ g++ -Wall -Werror -O2 -std=c++1y -I path/to/fatal \
  -o path/to/output/binary demo/demo_filename.cpp
```
