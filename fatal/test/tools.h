/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <sstream>
#include <string>

#include <cstdint>

namespace fatal {

inline char least_significant_hex_digit(std::uint8_t c) {
  return "0123456789abcdef"[c & 0xf];
}

enum class literal_style { compact, expanded, readable };

template <
  literal_style Style = literal_style::expanded,
  typename Out,
  typename Buffer
>
void data_as_literal(Out &out, Buffer const &v) {
  switch (Style) {
    case literal_style::expanded: {
      out << '"';
      for (auto c_: v) {
        auto const c = static_cast<std::uint8_t>(c_);
        out << "\\x";
        out << least_significant_hex_digit(c >> 4);
        out << least_significant_hex_digit(c);
      }
      out << '"';
      break;
    }
    case literal_style::readable: {
      std::size_t size = 0;
      for (auto c_: v) {
        auto const c = static_cast<std::uint8_t>(c_);
        if (size) {
          out << ' ';
          if (size % 4 == 0) {
            out << "  ";
          }
        }
        out << least_significant_hex_digit(c >> 4);
        out << least_significant_hex_digit(c);
        ++size;
      }
      break;
    }
    case literal_style::compact: {
      out << '"';
      bool hex = false;
      for (auto c_: v) {
        auto const c = static_cast<std::uint8_t>(c_);
        if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f')
          || (c >= 'A' && c <= 'F')
        ) {
          if (hex) {
            out << "\"\"";
            out << c;
            hex = false;
          } else {
            out << c;
          }
        } else if (c >= 32 && c < 127) {
          hex = false;
          switch (c) {
            case '\\': case '"':
              out << '\\' << c;
              break;
            default:
              out << c;
              break;
          }
        } else {
          hex = false;
          switch (c) {
            case '\0': out << "\\0"; break;
            case '\n': out << "\\n"; break;
            case '\r': out << "\\r"; break;
            case '\t': out << "\\t"; break;
            case '\v': out << "\\v"; break;
            case '\a': out << "\\a"; break;
            case '\b': out << "\\b"; break;
            case '\f': out << "\\f"; break;
            default:
              out << "\\x";
              if (c >> 4) {
                out << least_significant_hex_digit(c >> 4);
              }
              out << least_significant_hex_digit(c);
              hex = true;
          }
        }
      }
      out << '"';
      break;
    }
  }
}

template <literal_style Style = literal_style::expanded, typename Buffer>
std::string data_as_literal(Buffer const &v) {
  std::ostringstream ss;
  data_as_literal<Style>(ss, v);
  return ss.str();
}

template <literal_style Style = literal_style::expanded>
std::string string_as_literal(std::string const &s) {
  return data_as_literal<Style>(s);
}

} // namespace fatal {
