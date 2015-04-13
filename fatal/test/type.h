/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_test_type_h
#define FATAL_INCLUDE_fatal_test_type_h

#include <string>
#include <type_traits>
#include <typeinfo>

// for test purposes only
#ifdef FATAL_USE_CXXABI
# include <cxxabi.h>
#endif // FATAL_USE_CXXABI

namespace fatal {

std::string type_str(std::string &out, std::type_info const &type) {
  // for test purposes only
# ifdef FATAL_USE_CXXABI
  int status;
  auto name = abi::__cxa_demangle(type.name(), 0, 0, &status);

  if (name) {
    out.append(name);
    std::free(name);
    return out;
  }
# endif // FATAL_USE_CXXABI

  out.append(type.name());
  return out;
}

template <typename T>
std::string &type_str(std::string &out) {
  type_str(out, typeid(T));

  using type = typename std::remove_reference<T>::type;

  if (std::is_const<type>::value) {
    out.append(" const");
  }

  if (std::is_volatile<type>::value) {
    out.append(" volatile");
  }

  if (std::is_lvalue_reference<T>::value) {
    out.append(" &");
  } else if (std::is_rvalue_reference<T>::value) {
    out.append(" &&");
  }

  return out;
}

template <typename T>
std::string type_str() {
  std::string result;
  type_str<T>(result);
  return result;
}

std::string type_str(std::type_info const &type) {
  std::string result;
  type_str(result, type);
  return result;
}

} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_test_type_h
