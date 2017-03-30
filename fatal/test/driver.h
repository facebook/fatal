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
  auto const arg_filter = std::string("--filter");

  if (argc == 0) {
    return 1; // protect parse_args below
  }

  using Opts = std::map<std::string, std::string>;
  auto const opts = fatal::test_impl::args::parse_args<Opts>(argc, argv);

  if (opts.empty()) {
    return fatal::test::run_all(std::cerr);
  }

  auto const iter_filter = opts.find(arg_filter);
  if (iter_filter != opts.end()) {
    return fatal::test::run_one(std::cerr, iter_filter->second);
  }

  return 1; // unrecognized input
}

#endif // FATAL_INCLUDE_fatal_test_driver_h
