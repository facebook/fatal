/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/string/string_view.h>
#include <fatal/time/time.h>
#include <fatal/type/tag.h>

#include <chrono>
#include <stdexcept>
#include <string>
#include <type_traits>

#include <cstring>

namespace fatal {

inline void append(std::string &out, bool from) {
  out.append(from ? "true" : "false");
}

template <typename T, typename = decltype(std::to_string(std::declval<T>()))>
void append(std::string &out, T from) {
  out.append(std::to_string(from));
}

template <typename R, typename P>
void append(std::string &out, std::chrono::duration<R, P> from) {
  append(out, from.count());
  out.append(time::suffix(from));
}

inline void append(std::string &out, std::string const &from) {
  out.append(from);
}

inline void append(std::string &out, string_view from) {
  out.append(from.data(), from.size());
}

inline void append(std::string &out, char from) {
  out.push_back(from);
}

inline void append(std::string &out, char const *from) {
  out.append(from);
}

namespace detail {
namespace string_impl {

inline bool parse(tag<bool>, std::string const &from) {
  // TODO: use a compile-time trie??
  if (from == "true") {
    return true;
  }

  if (from == "false") {
    return false;
  }

  throw std::invalid_argument("unrecognized boolean");
}

inline short parse(tag<short>, std::string const &from) {
  return static_cast<short>(std::stoi(from));
}

inline int parse(tag<int>, std::string const &from) {
  return std::stoi(from);
}

inline long parse(tag<long>, std::string const &from) {
  return std::stol(from);
}

inline long long parse(tag<long long>, std::string const &from) {
  return std::stoll(from);
}

inline unsigned long parse(tag<unsigned long>, std::string const &from) {
  return std::stoul(from);
}

inline unsigned long long parse(
  tag<unsigned long long>, std::string const &from
) {
  return std::stoull(from);
}

inline float parse(tag<float>, std::string const &from) {
  return std::stof(from);
}

inline double parse(tag<double>, std::string const &from) {
  return std::stod(from);
}

inline long double parse(tag<long double>, std::string const &from) {
  return std::stold(from);
}

inline std::string parse(tag<std::string>, std::string const &from) {
  return from;
}

void to_string_impl(std::string &) {}

template <typename T, typename... Args>
void to_string_impl(std::string &out, T &&value, Args &&...args) {
  append(out, std::forward<T>(value));

  to_string_impl(out, std::forward<Args>(args)...);
}

} // namespace string_impl {
} // namespace detail {

// for internal tests only - no guaranteed efficiency
// TODO: TEST
template <typename To>
To parse(std::string const &from) {
  return detail::string_impl::parse(tag<To>(), from);
}

// for internal tests only - no guaranteed efficiency
// TODO: TEST
template <typename... Args>
std::string &append_to_string(std::string &out, Args &&...args) {
  detail::string_impl::to_string_impl(out, std::forward<Args>(args)...);

  return out;
}

// TODO: TEST
template <typename... Args>
std::string to_string(Args &&...args) {
  std::string out;

  append_to_string(out, std::forward<Args>(args)...);

  return out;
}

std::string const &to_string(std::string const &s) { return s; }
std::string &to_string(std::string &s) { return s; }
std::string &&to_string(std::string &&s) { return std::move(s); }

} // namespace fatal {
