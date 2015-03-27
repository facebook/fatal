/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_test_parse_sequence_input_h
#define FATAL_INCLUDE_fatal_type_test_parse_sequence_input_h

namespace fatal {

// TODO: ADD EDGE CASES
#define FATAL_IMPLT_PARSE_SEQUENCE_TEST_CALLS(Fn) \
  Fn(unsigned, 0, char, '0'); \
  Fn(short, 1, char, '1'); \
  Fn(std::size_t, 42, char, '4', '2'); \
  Fn(int, -56, char, '-', '5', '6'); \
  \
  Fn(unsigned, 0, wchar_t, L'0'); \
  Fn(short, 1, wchar_t, L'1'); \
  Fn(std::size_t, 42, wchar_t, L'4', L'2'); \
  Fn(int, -56, wchar_t, L'-', L'5', L'6'); \

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_type_test_parse_sequence_input_h
