# Facebook Template Library [![Build Status](https://secure.travis-ci.org/facebook/fatal.png?branch=master "Build Status")](https://travis-ci.org/facebook/fatal)
Fatal (Facebook Template Library) is a library for fast prototyping software in modern C++.

It provides facilities heavily based on template meta-programming, while keeping most of the complexity under-the-hood, to enhance the expressive power of C++.

Fatal also provides lessons on how to write meta-programs, as well as on how to make the best use of the library, starting at beginner levels.

The goal is make its benefits available to a wider audience other than just hard-core library writers.


## Documentation and Examples
A learn-by-example guide is available under the [`lesson/`](lesson/) directory.

Demo programs are also available under the [`demo/`](demo/) directory.

Extensive documentation with examples can be found inline in the source header files.


## Building Fatal
Fatal is a header only library, therefore no building is required.


## Requirements
A compliant C++14 compiler. Currently tested under Clang {[3.5](https://packages.debian.org/sid/clang-3.5), [3.6](https://packages.debian.org/sid/clang-3.6), [3.7](https://packages.debian.org/sid/clang-3.7), [3.8](https://packages.debian.org/sid/clang-3.8), [3.9](https://packages.debian.org/sid/clang-3.9), [5.0](https://packages.debian.org/sid/clang-5.0)} and GCC {[5](https://packages.debian.org/sid/g++-5), [6](https://packages.debian.org/sid/g++-6)}.

There are no other external dependencies.


## Links
[Discussions group](https://www.facebook.com/groups/libfatal/) on [Facebook](https://code.facebook.com/)

[Try it online](http://metashell.org/about/demo/index.html) using [Metashell](https://github.com/metashell/metashell).


## Philosophy
Fatal moves fast, therefore it uses the latest and greatest in C++ standards. It aims to adopt new standard features as soon as they're officially out, as long as they provide benefits like performance, productivity and usability improvements.

Stable versions are tagged and can be found under the [list of releases](https://github.com/facebook/fatal/releases). Look out for breaking changes whenever a new release is made (see below) - they should be listed on the release notes.

The [`master`](https://github.com/facebook/fatal/) branch is considered the development version of the library and experiences frequent updates. If you want the bleeding edge, that's the branch for you. Be advised, though, that it is under heavy development: breaking changes might be introduced without notice. They'll be tagged with the string `[break]` in the first line of the commit message.


## Installation
There's no need to install Fatal, as long as you add its base directory to the include directories list.

For Clang and GCC, it suffices to either:
- use the `-I path/to/fatal` flag
- set the environment variable `CPLUS_INCLUDE_PATH=path/to/fatal`


## Building Benchmarks and Unit Tests
```sh
$ clang++ -Wall -Werror -O2 -std=c++14 -I path/to/fatal \
  -o path/to/output/binary path/to/test/or/benchmark.cpp \
```
or
```sh
$ g++ -Wall -Werror -O2 -std=c++14 -I path/to/fatal \
  -o path/to/output/binary path/to/test/or/benchmark.cpp \
```


## Join the Fatal community
- Website: https://github.com/facebook/fatal/
- Facebook Group: https://www.facebook.com/groups/libfatal/
- Talk at CppCon 2014: https://www.youtube.com/watch?v=0A9pYr8wevk

See the CONTRIBUTING file for how to help out.


## License
Fatal is BSD-licensed. We also provide an additional patent grant.
