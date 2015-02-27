# Fatal: Facebook Template Library
Fatal is a library for fast prototyping of software in C++11 and up.

It provides a broad range of template meta-programming tools for reflection and metadata manipulation.

The goal is to speed up prototyping of complex software, while abstracting the complexity of template meta-programming and making its benefits available to a wider audience other than hard-core library writers.

Bleeding edge: https://github.com/facebook/fatal/tree/dev

Discussions group: https://www.facebook.com/groups/libfatal/

## Examples
Demonstration applications can be found under the `demo/` directory.

## Philosophy
Fatal moves fast, therefore it uses the latest and greatest in C++ standards. It aims to adopt new standard features as soon as they're officially out, as long as they provide benefits like performance, productivity and usability improvements.

The `master` branch is considered the stable version of the library and should not change often.

The `dev` branch is considered the development version of the library and experiences frequent updates. If you want the bleeding edge, that's the branch for you. Be advised, though, that it is under heavy development: breaking changes might be introduced without notice.


## Requirements
There are no external dependencies in order to use Fatal as a library.

In order to build and run Fatal's benchmarks and unit tests, you'll need:

- Folly: Facebook Open-source LibrarY (https://github.com/facebook/folly/)
- GTest: Google C++ Testing Framework (https://code.google.com/p/googletest/)
- GLog: Google Logging Library for C++ (https://code.google.com/p/google-glog/)
- a compliant C++11 compiler. Currently tested under GCC 4.8, GCC 4.9 and Clang 3.4.

In order to build and run Fatal's demos, you'll need:

- Folly: Facebook Open-source LibrarY (https://github.com/facebook/folly/)
- a compliant C++14 compiler. Currently tested under GCC 4.9 and Clang 3.4.

## Building Fatal
Fatal is a header only library, therefore no building is required.

## Building Benchmarks and Unit Tests
Provided that the dependencies are properly installed:

```sh
$ clang++ -Wall -std=c++11 -I path/to/fatal \
  -o path/to/output/binary path/to/test/or/benchmark.cpp \
  -lfolly -lfollybenchmark -ldouble-conversion -lglog
```
or
```sh
$ g++ -Wall -std=c++11 -I path/to/fatal \
  -o path/to/output/binary path/to/test/or/benchmark.cpp \
  -lfolly -lfollybenchmark -ldouble-conversion -lglog
```

## Building Demos
Provided that the dependencies are properly installed:

```sh
$ clang++ -Wall -std=c++1y -I path/to/fatal \
  -o path/to/output/binary path/to/demo.cpp \
  -lfolly -ldouble-conversion
```
or
```sh
$ g++ -Wall -std=c++1y -I path/to/fatal \
  -o path/to/output/binary path/to/demo.cpp \
  -lfolly -ldouble-conversion
```

## Installation
There's no need to install Fatal, as long as you add its base directory to the include directories list.

For GCC and Clang, it suffices to either:
- use the `-I path/to/fatal` flag
- set the environment variable `CPLUS_INCLUDE_PATH=path/to/fatal`

## Documentation
Extensive documentation with examples can be found inline in the source header files.

## Join the Fatal community
- Website: https://github.com/facebook/fatal/
- Facebook Page: https://www.facebook.com/libfatal
- Talk at CppCon 2014: https://www.youtube.com/watch?v=0A9pYr8wevk

See the CONTRIBUTING file for how to help out.

## License
Fatal is BSD-licensed. We also provide an additional patent grant.
