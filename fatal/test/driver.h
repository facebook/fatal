/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_test_driver_h
#define FATAL_INCLUDE_fatal_test_driver_h

#include <fatal/log/log.h>
#include <fatal/test/args.h>
#include <fatal/test/random_data.h>
#include <fatal/test/string.h>
#include <fatal/test/test.h>
#include <fatal/test/type.h>

#include <iostream>
#include <map>
#include <string>

////////////
// DRIVER //
////////////

int main(int const argc, char const *const *const argv) {
  auto const arg_list = std::string("--list");
  auto const arg_filter = std::string("--filter");
  auto const arg_gtest = std::string("--gtest");
  auto const arg_gtest_list = std::string("--gtest_list_tests");
  auto const arg_gtest_filter = std::string("--gtest_filter");

  if (argc == 0) {
    return 1; // protect parse_args below
  }

  using Opts = std::map<std::string, std::string>;
  auto const opts = fatal::test_impl::args::parse_args<Opts>(argc, argv);

  if (opts.empty()) {
    return fatal::test::run_all<fatal::test::default_printer>(std::cerr);
  }
  auto const iter_gtest = opts.find(arg_gtest);
  if (iter_gtest != opts.end()) {
    return fatal::test::run_all<fatal::test::gtest_printer>(std::cout);
  }

  auto const iter_list = opts.find(arg_list);
  if (iter_list != opts.end()) {
    return fatal::test::list<fatal::test::default_printer>(std::cout);
  }
  auto const iter_gtest_list = opts.find(arg_gtest_list);
  if (iter_gtest_list != opts.end()) {
    return fatal::test::list<fatal::test::gtest_printer>(std::cout);
  }

  auto const iter_filter = opts.find(arg_filter);
  if (iter_filter != opts.end()) {
    return fatal::test::run_one<fatal::test::default_printer>(
      std::cerr, iter_filter->second
    );
  }
  auto const iter_gtest_filter = opts.find(arg_gtest_filter);
  if (iter_gtest_filter != opts.end()) {
    return fatal::test::run_one<fatal::test::gtest_printer>(
      std::cout, iter_gtest_filter->second
    );
  }

  return 1; // unrecognized input
}

#endif // FATAL_INCLUDE_fatal_test_driver_h
