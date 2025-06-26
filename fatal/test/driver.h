/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/log/log.h>
#include <fatal/test/args.h>
#include <fatal/test/env.h>
#include <fatal/test/random_data.h>
#include <fatal/test/string.h>
#include <fatal/test/test.h>
#include <fatal/test/type.h>

#include <fstream>
#include <iostream>
#include <map>
#include <string>

////////////
// DRIVER //
////////////

int main(
  int const argc,
  char const *const *const argv,
  char const *const *const envp
) {
  auto const env_gtest_output = std::string("GTEST_OUTPUT");
  auto const arg_list = std::string("--list");
  auto const arg_filter = std::string("--filter");
  auto const arg_gtest = std::string("--gtest");
  auto const arg_gtest_list = std::string("--gtest_list_tests");
  auto const arg_gtest_filter = std::string("--gtest_filter");
  auto const arg_gtest_output = std::string("--gtest_output");

  auto const xml_ = std::string("xml:");
  auto const json_ = std::string("json:");

  if (argc == 0) {
    return 1; // protect parse_args below
  }

  using Opts = std::map<std::string, std::string>;
  auto const opts = fatal::test_impl::args::parse_args<Opts>(argc, argv);
  auto const envs = fatal::test_impl::env::parse_env<Opts>(envp);

  if (opts.empty()) {
    auto printer = fatal::test::default_printer{std::cout};
    return fatal::test::run_all(printer);
  }
  auto const iter_gtest = opts.find(arg_gtest);
  if (iter_gtest != opts.end()) {
    auto printer = fatal::test::gtest_printer{std::cout};
    return fatal::test::run_all(printer);
  }

  auto const iter_list = opts.find(arg_list);
  if (iter_list != opts.end()) {
    auto printer = fatal::test::default_printer{std::cout};
    return fatal::test::list(printer);
  }

  auto const iter_gtest_list = opts.find(arg_gtest_list);
  if (iter_gtest_list != opts.end()) {
    auto const iter_arg_output = opts.find(arg_gtest_output);
    auto const iter_env_output = envs.find(env_gtest_output);
    auto const &empty = std::string("");
    auto const &outspec =
      iter_arg_output != opts.end() ? iter_arg_output->second :
      iter_env_output != envs.end() ? iter_env_output->second :
      empty;

    if (!outspec.empty() && outspec.find(xml_) != 0 && outspec.find(json_) != 0) {
      std::cerr << "error: gtest-output value requires prefix xml: or json:" << std::endl;
      return 1;
    }

    if (outspec.empty()) {
      auto printer = fatal::test::gtest_printer{std::cout};
      return fatal::test::list(printer);
    }

    if (outspec.find(json_) == 0) {
      std::ofstream out{outspec.substr(json_.size())};
      auto printer = fatal::test::gtest_json_list_printer{out};
      return fatal::test::list(printer);
    } else if (outspec.find(xml_) == 0) {
      // XML listing not implemented yet, fall back to regular gtest printer
      auto printer = fatal::test::gtest_printer{std::cout};
      return fatal::test::list(printer);
    }
  }

  auto const iter_filter = opts.find(arg_filter);
  if (iter_filter != opts.end()) {
    auto printer = fatal::test::default_printer{std::cout};
    return fatal::test::run_one(printer, iter_filter->second);
  }
  auto const iter_gtest_filter = opts.find(arg_gtest_filter);
  auto const iter_arg_output = opts.find(arg_gtest_output);
  auto const iter_env_output = envs.find(env_gtest_output);
  auto const &empty = std::string("");
  auto const &outspec =
    iter_arg_output != opts.end() ? iter_arg_output->second :
    iter_env_output != envs.end() ? iter_env_output->second :
    empty;

  // If gtest_output is specified but no gtest_filter, default to "*" (run all)
  std::string filter_value = "*";
  if (iter_gtest_filter != opts.end()) {
    filter_value = iter_gtest_filter->second;
  }

  // Only proceed if we have a filter (explicit or implicit) or output specified
  if (iter_gtest_filter != opts.end() || !outspec.empty()) {
    auto printer = fatal::test::gtest_printer{std::cout};

    if (!outspec.empty() && outspec.find(xml_) != 0 && outspec.find(json_) != 0) {
      std::cerr << "error: gtest-output value requires prefix xml: or json:" << std::endl;
      return 1;
    }

    if (outspec.empty()) {
      if (filter_value == "*" || filter_value.empty()) {
        return fatal::test::run_all(printer);
      } else {
        return fatal::test::run_one(printer, filter_value);
      }
    }

    if (outspec.find(xml_) == 0) {
      std::ofstream out{outspec.substr(xml_.size())};
      auto printer2 = fatal::test::gtest_xml_printer{out};
      auto printer3 = combine_printers(printer, printer2);
      if (filter_value == "*" || filter_value.empty()) {
        return fatal::test::run_all(printer3);
      } else {
        return fatal::test::run_one(printer3, filter_value);
      }
    } else if (outspec.find(json_) == 0) {
      std::ofstream out{outspec.substr(json_.size())};
      auto printer2 = fatal::test::gtest_json_printer{out};
      auto printer3 = combine_printers(printer, printer2);
      if (filter_value == "*" || filter_value.empty()) {
        return fatal::test::run_all(printer3);
      } else {
        return fatal::test::run_one(printer3, filter_value);
      }
    }
  }

  return 1; // unrecognized input
}
