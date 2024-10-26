/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

namespace fatal {
namespace test_impl {
namespace args {

// Not a real argument parser. Just enough to get started with.
template <typename Map>
void parse_args(int argc, char const *const *argv, Map &map) {
  using Key = typename Map::key_type;
  using Val = typename Map::mapped_type;

  --argc, ++argv;
  while (argc--) {
    auto key = Key(*argv++);
    auto val = Val();
    auto const eq = key.find('=');
    if (eq != Key::npos) {
      val.assign(key.data() + eq + 1, key.size() - eq - 1);
      key.erase(eq);
    }
    map[static_cast<Key&&>(key)] = static_cast<Val&&>(val);
  }
}

// Not a real argument parser. Just enough to get started with.
template <typename Map>
Map parse_args(int const argc, char const *const *const argv) {
  Map result;
  parse_args(argc, argv, result);
  return result;
}

}
}
}
