/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/tag.h>

#include <fatal/test/driver.h>

namespace fatal {

#if __cplusplus > 201400
# define TEST_IMPL(...) \
  do { \
    TEST_CPP11_IMPL(__VA_ARGS__) \
  } while (false); \
  do { \
    TEST_CPP14_IMPL(__VA_ARGS__) \
  } while (false)
#else // C++14
# define TEST_IMPL(...) \
  do { \
    TEST_CPP11_IMPL(__VA_ARGS__) \
  } while (false);
#endif // C++14

FATAL_TEST(tag, tag_type) {
# define TEST_CPP11_IMPL(First, ...) \
  FATAL_EXPECT_SAME< \
    __VA_ARGS__, \
    decltype(tag_type(tag<__VA_ARGS__>())) \
  >(); \
  FATAL_EXPECT_SAME< \
    pair<First, __VA_ARGS__>, \
    decltype(tag_type(tag_pair<First, __VA_ARGS__>())) \
  >(); \
  FATAL_EXPECT_SAME< \
    __VA_ARGS__, \
    decltype(tag_type(indexed<__VA_ARGS__, __LINE__>())) \
  >(); \
  FATAL_EXPECT_SAME< \
    pair<First, __VA_ARGS__>, \
    decltype(tag_type(indexed_pair<First, __VA_ARGS__, __LINE__>())) \
  >();

# define TEST_CPP14_IMPL(First, ...) \
  auto const c = [](auto t) { \
    FATAL_EXPECT_SAME<__VA_ARGS__, decltype(tag_type(t))>(); \
  }; \
  auto const p = [](auto t) { \
    FATAL_EXPECT_SAME<pair<First, __VA_ARGS__>, decltype(tag_type(t))>(); \
  }; \
  \
  c(tag<__VA_ARGS__>()); \
  p(tag_pair<First, __VA_ARGS__>()); \
  c(indexed<__VA_ARGS__, __LINE__>()); \
  p(indexed_pair<First, __VA_ARGS__, __LINE__>());

  TEST_IMPL(int,          float);
  TEST_IMPL(int &,        float);
  TEST_IMPL(int const &,  float);
  TEST_IMPL(int &&,       float);
  TEST_IMPL(int const &&, float);

  TEST_IMPL(int,          float &);
  TEST_IMPL(int &,        float &);
  TEST_IMPL(int const &,  float &);
  TEST_IMPL(int &&,       float &);
  TEST_IMPL(int const &&, float &);

  TEST_IMPL(int,          float const &);
  TEST_IMPL(int &,        float const &);
  TEST_IMPL(int const &,  float const &);
  TEST_IMPL(int &&,       float const &);
  TEST_IMPL(int const &&, float const &);

  TEST_IMPL(int,          float &&);
  TEST_IMPL(int &,        float &&);
  TEST_IMPL(int const &,  float &&);
  TEST_IMPL(int &&,       float &&);
  TEST_IMPL(int const &&, float &&);

  TEST_IMPL(int,          float const &&);
  TEST_IMPL(int &,        float const &&);
  TEST_IMPL(int const &,  float const &&);
  TEST_IMPL(int &&,       float const &&);
  TEST_IMPL(int const &&, float const &&);

# undef TEST_CPP14_IMPL
# undef TEST_CPP11_IMPL
}

FATAL_TEST(tag, tag_index) {
# define TEST_CPP11_IMPL(First, ...) \
  FATAL_EXPECT_EQ( \
    __LINE__, \
    tag_index(indexed<pair<First, __VA_ARGS__>, __LINE__>()) \
  ); \
  FATAL_EXPECT_EQ( \
    __LINE__, \
    tag_index(indexed_pair<First, __VA_ARGS__, __LINE__>()) \
  );

# define TEST_CPP14_IMPL(First, ...) \
  auto const c = [](auto t) { \
    FATAL_EXPECT_EQ(__LINE__, tag_index(t)); \
  }; \
  \
  c(indexed<pair<First, __VA_ARGS__>, __LINE__>()); \
  c(indexed_pair<First, __VA_ARGS__, __LINE__>());

  TEST_IMPL(int,          float);
  TEST_IMPL(int &,        float);
  TEST_IMPL(int const &,  float);
  TEST_IMPL(int &&,       float);
  TEST_IMPL(int const &&, float);

  TEST_IMPL(int,          float &);
  TEST_IMPL(int &,        float &);
  TEST_IMPL(int const &,  float &);
  TEST_IMPL(int &&,       float &);
  TEST_IMPL(int const &&, float &);

  TEST_IMPL(int,          float const &);
  TEST_IMPL(int &,        float const &);
  TEST_IMPL(int const &,  float const &);
  TEST_IMPL(int &&,       float const &);
  TEST_IMPL(int const &&, float const &);

  TEST_IMPL(int,          float &&);
  TEST_IMPL(int &,        float &&);
  TEST_IMPL(int const &,  float &&);
  TEST_IMPL(int &&,       float &&);
  TEST_IMPL(int const &&, float &&);

  TEST_IMPL(int,          float const &&);
  TEST_IMPL(int &,        float const &&);
  TEST_IMPL(int const &,  float const &&);
  TEST_IMPL(int &&,       float const &&);
  TEST_IMPL(int const &&, float const &&);

# undef TEST_CPP14_IMPL
# undef TEST_CPP11_IMPL
}

FATAL_TEST(tag, tag_first) {
# define TEST_CPP11_IMPL(First, ...) \
  FATAL_EXPECT_SAME< \
    First, \
    decltype(tag_first(tag<pair<First, __VA_ARGS__>>())) \
  >(); \
  FATAL_EXPECT_SAME< \
    First, \
    decltype(tag_first(indexed<pair<First, __VA_ARGS__>, __LINE__>())) \
  >(); \
  FATAL_EXPECT_SAME< \
    First, \
    decltype(tag_first(indexed_pair<First, __VA_ARGS__, __LINE__>())) \
  >();

# define TEST_CPP14_IMPL(First, ...) \
  auto const c = [](auto t) { \
    FATAL_EXPECT_SAME<First, decltype(tag_first(t))>(); \
  }; \
  \
  c(tag<pair<First, __VA_ARGS__>>()); \
  c(tag_pair<First, __VA_ARGS__>()); \
  c(indexed<pair<First, __VA_ARGS__>, __LINE__>()); \
  c(indexed_pair<First, __VA_ARGS__, __LINE__>());

  TEST_IMPL(int,          float);
  TEST_IMPL(int &,        float);
  TEST_IMPL(int const &,  float);
  TEST_IMPL(int &&,       float);
  TEST_IMPL(int const &&, float);

  TEST_IMPL(int,          float &);
  TEST_IMPL(int &,        float &);
  TEST_IMPL(int const &,  float &);
  TEST_IMPL(int &&,       float &);
  TEST_IMPL(int const &&, float &);

  TEST_IMPL(int,          float const &);
  TEST_IMPL(int &,        float const &);
  TEST_IMPL(int const &,  float const &);
  TEST_IMPL(int &&,       float const &);
  TEST_IMPL(int const &&, float const &);

  TEST_IMPL(int,          float &&);
  TEST_IMPL(int &,        float &&);
  TEST_IMPL(int const &,  float &&);
  TEST_IMPL(int &&,       float &&);
  TEST_IMPL(int const &&, float &&);

  TEST_IMPL(int,          float const &&);
  TEST_IMPL(int &,        float const &&);
  TEST_IMPL(int const &,  float const &&);
  TEST_IMPL(int &&,       float const &&);
  TEST_IMPL(int const &&, float const &&);

# undef TEST_CPP14_IMPL
# undef TEST_CPP11_IMPL
}

FATAL_TEST(tag, tag_second) {
# define TEST_CPP11_IMPL(First, ...) \
  FATAL_EXPECT_SAME< \
    __VA_ARGS__, \
    decltype(tag_second(tag<pair<First, __VA_ARGS__>>())) \
  >(); \
  FATAL_EXPECT_SAME< \
    __VA_ARGS__, \
    decltype(tag_second(tag_pair<First, __VA_ARGS__>())) \
  >(); \
  FATAL_EXPECT_SAME< \
    __VA_ARGS__, \
    decltype(tag_second(indexed<pair<First, __VA_ARGS__>, __LINE__>())) \
  >(); \
  FATAL_EXPECT_SAME< \
    __VA_ARGS__, \
    decltype(tag_second(indexed_pair<First, __VA_ARGS__, __LINE__>())) \
  >();

# define TEST_CPP14_IMPL(First, ...) \
  auto const c = [](auto t) { \
    FATAL_EXPECT_SAME<__VA_ARGS__, decltype(tag_second(t))>(); \
  }; \
  \
  c(tag<pair<First, __VA_ARGS__>>()); \
  c(tag_pair<First, __VA_ARGS__>()); \
  c(indexed<pair<First, __VA_ARGS__>, __LINE__>()); \
  c(indexed_pair<First, __VA_ARGS__, __LINE__>());

  TEST_IMPL(int,          float);
  TEST_IMPL(int &,        float);
  TEST_IMPL(int const &,  float);
  TEST_IMPL(int &&,       float);
  TEST_IMPL(int const &&, float);

  TEST_IMPL(int,          float &);
  TEST_IMPL(int &,        float &);
  TEST_IMPL(int const &,  float &);
  TEST_IMPL(int &&,       float &);
  TEST_IMPL(int const &&, float &);

  TEST_IMPL(int,          float const &);
  TEST_IMPL(int &,        float const &);
  TEST_IMPL(int const &,  float const &);
  TEST_IMPL(int &&,       float const &);
  TEST_IMPL(int const &&, float const &);

  TEST_IMPL(int,          float &&);
  TEST_IMPL(int &,        float &&);
  TEST_IMPL(int const &,  float &&);
  TEST_IMPL(int &&,       float &&);
  TEST_IMPL(int const &&, float &&);

  TEST_IMPL(int,          float const &&);
  TEST_IMPL(int &,        float const &&);
  TEST_IMPL(int const &,  float const &&);
  TEST_IMPL(int &&,       float const &&);
  TEST_IMPL(int const &&, float const &&);

# undef TEST_CPP14_IMPL
# undef TEST_CPP11_IMPL
}

#undef TEST_IMPL

} // namespace fatal {
