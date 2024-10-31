/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/test/test.h>

#include <fatal/test/string.h>
#include <fatal/test/type.h>

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <cstring>

namespace fatal {
namespace lesson {

// for internal use only

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
/* TODO: support multiple namespace scope definitions:

  LESSON(name)(
    (comment, definitions) {
      block
    }
  )(
    (comment, definitions) {
      block
    }
  )(
    ...
  )(
    (comment, definitions) {
      block
    }
  )
 */
#define LESSON(Name, Overview, ...) \
  __VA_ARGS__ \
  \
  FATAL_IMPL_LESSON( \
    FATAL_UID(lesson_class), \
    FATAL_UID(lesson_entry), \
    Name, \
    Overview, \
    FATAL_TO_STR(__VA_ARGS__) \
  )

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define COMMENT(...) \
  ::std::cout << __VA_ARGS__ \
    << ::std::endl \
    << ::std::endl;

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define CODE(...) \
  ::std::cout << ">> " << FATAL_SOURCE_INFO() << " ------" \
    << ::std::endl \
    << ::fatal::lesson::detail::format(FATAL_TO_STR(__VA_ARGS__)) \
    << ::std::endl; \
  \
  __VA_ARGS__ \
  \
  ::std::cout << "<< --------" \
    << ::fatal::lesson::detail::padding(FATAL_SOURCE_INFO()) \
    << ::std::endl \
    << ::std::endl;

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define PRINT(...) \
  ::std::cout << ">> " << FATAL_SOURCE_INFO() << " ------" \
    << ::std::endl \
    << "print(" << FATAL_TO_STR(__VA_ARGS__) << ");" \
    << ::std::endl \
    << "-- output -" << ::fatal::lesson::detail::padding(FATAL_SOURCE_INFO()) \
    << ::std::endl; \
  \
  ::fatal::lesson::print(__VA_ARGS__); \
  \
  ::std::cout << "<< --------" \
    << ::fatal::lesson::detail::padding(FATAL_SOURCE_INFO()) \
    << ::std::endl \
    << ::std::endl;

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define TYPE(...) \
  ::std::cout << ">> " << FATAL_SOURCE_INFO() << " ------" \
    << ::std::endl \
    << FATAL_TO_STR(__VA_ARGS__) << " = " << ::fatal::type_str<__VA_ARGS__>() \
    << ::std::endl \
    << "<< --------" << ::fatal::lesson::detail::padding(FATAL_SOURCE_INFO()) \
    << ::std::endl \
    << ::std::endl;

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define VALUE(...) \
  ::std::cout << ">> " << FATAL_SOURCE_INFO() << " ------" \
    << ::std::endl \
    << FATAL_TO_STR(__VA_ARGS__) << " = " << ::std::boolalpha << (__VA_ARGS__) \
    << std::endl \
    << "<< --------" << ::fatal::lesson::detail::padding(FATAL_SOURCE_INFO()) \
    << ::std::endl \
    << ::std::endl;

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define CONSTANT(...) \
  ::std::cout << ">> " << FATAL_SOURCE_INFO() << " ------" \
    << ::std::endl \
    << FATAL_TO_STR(__VA_ARGS__) << " = " << ::fatal::type_str<__VA_ARGS__>() \
    << ::std::endl \
    << FATAL_TO_STR(__VA_ARGS__) << "::value = " << ::std::boolalpha \
      << (__VA_ARGS__::value) \
    << ::std::endl \
    << "<< --------" << ::fatal::lesson::detail::padding(FATAL_SOURCE_INFO()) \
    << ::std::endl \
    << ::std::endl;

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define MEMBER_TYPE(Member, ...) \
  ::std::cout << ">> " << FATAL_SOURCE_INFO() << " ------" \
    << ::std::endl \
    << FATAL_TO_STR(__VA_ARGS__) << " = " << ::fatal::type_str<__VA_ARGS__>() \
    << ::std::endl \
    << FATAL_TO_STR(__VA_ARGS__) "::" FATAL_TO_STR(Member) " = " \
      << ::fatal::type_str<__VA_ARGS__::Member>() \
    << ::std::endl \
    << "<< --------" << ::fatal::lesson::detail::padding(FATAL_SOURCE_INFO()) \
    << ::std::endl \
    << ::std::endl;

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define ILLEGAL(Description, ...) \
  ::std::cout << ">> " << FATAL_SOURCE_INFO() << " -------------" \
    << ::std::endl \
    << FATAL_TO_STR(__VA_ARGS__) << ::std::endl \
    << "-- won't compile -" \
    << ::fatal::lesson::detail::padding(FATAL_SOURCE_INFO()) << ::std::endl \
    << Description << std::endl \
    << "<< ---------------" \
    << ::fatal::lesson::detail::padding(FATAL_SOURCE_INFO()) << ::std::endl \
    << ::std::endl;

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
void print() {
  std::cout << std::endl;
}

template <typename T, typename... Args>
void print(T &&value, Args &&...args) {
  std::cout << value;
  print(std::forward<Args>(args)...);
}

namespace detail {
template <typename> void run(bool);
} // namespace detail {

/**
 * TODO: DOCUMENT
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
class registry {
  registry() = default;

  struct base {
    virtual ~base() {}

    virtual void run(bool) const = 0;
  };

  template <typename T>
  struct entry: public base {
    void run(bool first) const override {
      detail::run<T>(first);
    }
  };

  static registry &get() {
    static registry instance;
    return instance;
  }

public:
  /**
   * TODO: DOCUMENT
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename T>
  static bool add() {
    get().list_.emplace_back(new entry<T>());
    return true;
  }

  /**
   * TODO: DOCUMENT
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  static void run() {
    bool first = true;
    for (auto const &i: get().list_) {
      i->run(first);

      if (first) {
        first = false;
      }
    }
  }

private:
  std::vector<std::unique_ptr<base>> list_;
};

////////////////////////////
// IMPLEMENTATION DETAILS //
////////////////////////////

namespace detail {

struct format {
  explicit format(char const *code): code_(code) {}

  template <typename Out>
  void operator ()(Out &out) const {
    // TODO: PROPERLY FORMAT CODE
    out << code_;
  }

private:
  char const *code_;
};

template <typename Out>
Out &operator <<(Out &out, format const &formatter) {
  formatter(out);
  return out;
}

template <typename T>
void run(bool first) {
  auto const &i = T::entry();

  if (!first) {
    std::cout << std::endl << std::endl << std::endl
      << std::endl << std::endl << std::endl;
  }

  std::cout << "== " << i.name << " == OVERVIEW ======"
    << std::endl << std::endl << i.overview << std::endl;

  if (i.declarations && *i.declarations) {
    std::cout << std::endl << "== " << i.name << " == DECLARATIONS =="
      << std::endl << std::endl << format(i.declarations)
      << std::endl;
  }

  std::cout << std::endl << "== " << i.name << " == START ========="
    << std::endl << std::endl;

  i();

  std::cout << "== " << i.name << " == END ==========="
    << std::endl;
}

struct padding {
  explicit padding(source_info source):
    // TODO: SMARTER CALCULATION OF PADDING SIZE
    size(std::strlen(source.file()) + std::to_string(source.line()).size())
  {}

  std::size_t size;
};

template <typename Out>
Out &operator <<(Out &out, padding pad) {
  while (pad.size--) {
    out.put('-');
  }
  return out;
}

#define FATAL_IMPL_LESSON(Class, Entry, Name, Overview, Declarations) \
  struct Class { \
    explicit Class(bool): \
      name(Name), \
      overview(Overview), \
      declarations(Declarations) \
    {} \
    \
    void operator ()() const; \
    static Class const &entry(); \
    \
    ::std::string const name; \
    char const *overview; \
    char const *declarations; \
  }; \
  \
  static Class const Entry(::fatal::lesson::registry::add<Class>()); \
  \
  Class const &Class::entry() { return Entry; } \
  \
  void Class::operator ()() const

} // namespace detail {
} // namespace lesson {
} // namespace fatal {
