/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <cstring>

namespace fatal {
namespace test_impl {
namespace env {

// Not a real argument parser. Just enough to get started with.
template <typename Map>
void parse_env(char const *const *envp, Map &map) {
  for (auto e = envp; e != nullptr && *e != nullptr; ++e) {
    auto const p = std::strchr(*e, '=');
    if (p == nullptr) {
      map[*e] = "";
    } else {
      map[{*e, p}] = p + 1;
    }
  }
}

// Not a real argument parser. Just enough to get started with.
template <typename Map>
Map parse_env(char const *const *const envp) {
  Map result;
  parse_env(envp, result);
  return result;
}

}
}
}
