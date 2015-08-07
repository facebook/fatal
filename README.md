# Fatal: Facebook Template Library [![Build Status](https://secure.travis-ci.org/facebook/fatal.png?branch=dev "Build Status")](https://travis-ci.org/facebook/fatal)
Fatal is a library for fast prototyping software in C++11 and up.

It provides a broad range of template meta-programming tools for reflection and metadata manipulation.

The goal is to speed up prototyping of complex software, while abstracting the complexity of template meta-programming and making its benefits available to a wider audience other than hard-core library writers.


## Documentation and Examples
A learn-by-example guide is available under the [`lesson/`](lesson/) directory.

Demo programs are also available under the [`demo/`](demo/) directory.

Extensive documentation with examples can be found inline in the source header files.


## Building Fatal
Fatal is a header only library, therefore no building is required.


## Requirements
A compliant C++11 compiler. Currently tested under Clang {3.4, 3.5, 3.6, 3.7} and GCC {4.8, 4.9, 5.1}.

There are no other external dependencies.


## Links
[Bleeding edge](https://github.com/facebook/fatal/tree/dev/) with latest features

[Discussions group](https://www.facebook.com/groups/libfatal/) on [Facebook](https://www.facebook.com/Engineering)

[Try it online](http://abel.web.elte.hu/shell/metashell) using [Metashell](https://github.com/sabel83/metashell)


## Philosophy
Fatal moves fast, therefore it uses the latest and greatest in C++ standards. It aims to adopt new standard features as soon as they're officially out, as long as they provide benefits like performance, productivity and usability improvements.

The [`master`](https://github.com/facebook/fatal/) branch is considered the stable version of the library and should not change often.

The [`dev`](https://github.com/facebook/fatal/tree/dev/) branch is considered the development version of the library and experiences frequent updates. If you want the bleeding edge, that's the branch for you. Be advised, though, that it is under heavy development: breaking changes might be introduced without notice.


## Installation
There's no need to install Fatal, as long as you add its base directory to the include directories list.

For Clang and GCC, it suffices to either:
- use the `-I path/to/fatal` flag
- set the environment variable `CPLUS_INCLUDE_PATH=path/to/fatal`


## Building Benchmarks and Unit Tests
```sh
$ clang++ -Wall -Werror -O2 -std=c++11 -I path/to/fatal \
  -o path/to/output/binary path/to/test/or/benchmark.cpp \
```
or
```sh
$ g++ -Wall -Werror -O2 -std=c++11 -I path/to/fatal \
  -o path/to/output/binary path/to/test/or/benchmark.cpp \
```


## Join the Fatal community
- Website: https://github.com/facebook/fatal/
- Facebook Group: https://www.facebook.com/groups/libfatal/
- Talk at CppCon 2014: https://www.youtube.com/watch?v=0A9pYr8wevk

See the CONTRIBUTING file for how to help out.


## License
Fatal is BSD-licensed. We also provide an additional patent grant.
