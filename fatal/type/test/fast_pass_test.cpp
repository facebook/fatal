/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/fast_pass.h>

#include <fatal/test/driver.h>

namespace fatal {

//////////////////
// is_fast_pass //
//////////////////

FATAL_TEST(traits, is_fast_pass) {
  FATAL_EXPECT_TRUE(is_fast_pass<bool>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<bool &>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<bool &&>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<bool const>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<bool const &>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<bool const &&>::value);

  FATAL_EXPECT_TRUE(is_fast_pass<bool *>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<bool *&>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<bool *&&>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<bool *const &>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<bool *const &&>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<bool const *>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<bool const *&>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<bool const *&&>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<bool const *const &>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<bool const *const &&>::value);

  FATAL_EXPECT_TRUE(is_fast_pass<int>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<int &>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<int &&>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<int const>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<int const &>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<int const &&>::value);

  FATAL_EXPECT_TRUE(is_fast_pass<int *>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<int *&>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<int *&&>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<int *const &>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<int *const &&>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<int const *>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<int const *&>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<int const *&&>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<int const *const &>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<int const *const &&>::value);

  FATAL_EXPECT_FALSE(is_fast_pass<std::string>::value);
  FATAL_EXPECT_FALSE(is_fast_pass<std::string &>::value);
  FATAL_EXPECT_FALSE(is_fast_pass<std::string &&>::value);
  FATAL_EXPECT_FALSE(is_fast_pass<std::string const>::value);
  FATAL_EXPECT_FALSE(is_fast_pass<std::string const &>::value);
  FATAL_EXPECT_FALSE(is_fast_pass<std::string const &&>::value);

  FATAL_EXPECT_TRUE(is_fast_pass<std::string *>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<std::string *&>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<std::string *&&>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<std::string *const &>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<std::string *const &&>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<std::string const *>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<std::string const *&>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<std::string const *&&>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<std::string const *const &>::value);
  FATAL_EXPECT_TRUE(is_fast_pass<std::string const *const &&>::value);
}

///////////////
// fast_pass //
///////////////

FATAL_TEST(traits, fast_pass) {
  FATAL_EXPECT_SAME<bool const, fast_pass<bool>>();
  FATAL_EXPECT_SAME<bool const, fast_pass<bool &>>();
  FATAL_EXPECT_SAME<bool const, fast_pass<bool &&>>();
  FATAL_EXPECT_SAME<bool const, fast_pass<bool const>>();
  FATAL_EXPECT_SAME<bool const, fast_pass<bool const &>>();
  FATAL_EXPECT_SAME<bool const, fast_pass<bool const &&>>();

  FATAL_EXPECT_SAME<bool *const, fast_pass<bool *>>();
  FATAL_EXPECT_SAME<bool *const, fast_pass<bool *&>>();
  FATAL_EXPECT_SAME<bool *const, fast_pass<bool *&&>>();
  FATAL_EXPECT_SAME<bool *const, fast_pass<bool * const &>>();
  FATAL_EXPECT_SAME<bool *const, fast_pass<bool * const &&>>();
  FATAL_EXPECT_SAME<bool const *const, fast_pass<bool const *>>();
  FATAL_EXPECT_SAME<bool const *const, fast_pass<bool const *&>>();
  FATAL_EXPECT_SAME<bool const *const, fast_pass<bool const *&&>>();
  FATAL_EXPECT_SAME<bool const *const, fast_pass<bool const * const &>>();
  FATAL_EXPECT_SAME<bool const *const, fast_pass<bool const * const &&>>();

  FATAL_EXPECT_SAME<int const, fast_pass<int>>();
  FATAL_EXPECT_SAME<int const, fast_pass<int &>>();
  FATAL_EXPECT_SAME<int const, fast_pass<int &&>>();
  FATAL_EXPECT_SAME<int const, fast_pass<int const>>();
  FATAL_EXPECT_SAME<int const, fast_pass<int const &>>();
  FATAL_EXPECT_SAME<int const, fast_pass<int const &&>>();

  FATAL_EXPECT_SAME<int *const, fast_pass<int *>>();
  FATAL_EXPECT_SAME<int *const, fast_pass<int *&>>();
  FATAL_EXPECT_SAME<int *const, fast_pass<int *&&>>();
  FATAL_EXPECT_SAME<int *const, fast_pass<int * const &>>();
  FATAL_EXPECT_SAME<int *const, fast_pass<int * const &&>>();
  FATAL_EXPECT_SAME<int const *const, fast_pass<int const *>>();
  FATAL_EXPECT_SAME<int const *const, fast_pass<int const *&>>();
  FATAL_EXPECT_SAME<int const *const, fast_pass<int const *&&>>();
  FATAL_EXPECT_SAME<int const *const, fast_pass<int const * const &>>();
  FATAL_EXPECT_SAME<int const *const, fast_pass<int const * const &&>>();

  FATAL_EXPECT_SAME<std::string const &, fast_pass<std::string>>();
  FATAL_EXPECT_SAME<std::string const &, fast_pass<std::string &>>();
  FATAL_EXPECT_SAME<std::string const &, fast_pass<std::string &&>>();
  FATAL_EXPECT_SAME<std::string const &, fast_pass<std::string const>>();
  FATAL_EXPECT_SAME<std::string const &, fast_pass<std::string const &>>();
  FATAL_EXPECT_SAME<std::string const &, fast_pass<std::string const &&>>();

  FATAL_EXPECT_SAME<std::string *const, fast_pass<std::string *>>();
  FATAL_EXPECT_SAME<std::string *const, fast_pass<std::string *&>>();
  FATAL_EXPECT_SAME<std::string *const, fast_pass<std::string *&&>>();
  FATAL_EXPECT_SAME<std::string *const, fast_pass<std::string * const &>>();
  FATAL_EXPECT_SAME<std::string *const, fast_pass<std::string * const &&>>();
  FATAL_EXPECT_SAME<std::string const *const, fast_pass<std::string const *>>();
  FATAL_EXPECT_SAME<
    std::string const *const,
    fast_pass<std::string const *&>
  >();
  FATAL_EXPECT_SAME<
    std::string const *const,
    fast_pass<std::string const *&&>
  >();
  FATAL_EXPECT_SAME<
    std::string const *const,
    fast_pass<std::string const * const &>
  >();
  FATAL_EXPECT_SAME<
    std::string const *const,
    fast_pass<std::string const * const &&>
  >();
}

} // namespace fatal {
