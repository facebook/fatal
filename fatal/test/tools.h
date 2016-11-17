/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_test_tools_h
#define FATAL_INCLUDE_fatal_test_tools_h

#include <sstream>
#include <string>

#include <cstdint>

namespace fatal {
namespace test {

inline char lshd(std::uint8_t c) {
  return "0123456789abcdef"[c & 0xf];
}

enum class literal_style { compact, expanded, readable };

template <
  literal_style Style = literal_style::expanded,
  typename Out,
  typename Buffer
>
void to_string_literal(Out &out, Buffer const &v) {
  switch (Style) {
    case literal_style::expanded: {
      out << '"';
      for (std::uint8_t c: v) {
        out << "\\x";
        out << lshd(c >> 4);
        out << lshd(c);
      }
      out << '"';
      break;
    }
    case literal_style::readable: {
      std::size_t size = 0;
      for (std::uint8_t c: v) {
        if (size) {
          out << ' ';
          if (size % 4 == 0) {
            out << "  ";
          }
        }
        out << lshd(c >> 4);
        out << lshd(c);
        ++size;
      }
      break;
    }
    case literal_style::compact: {
      out << '"';
      bool hex = false;
      for (std::uint8_t c: v) {
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
              out << '\\';
            default:
              out << c;
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
                out << lshd(c >> 4);
              }
              out << lshd(c);
              //out << std::hex << static_cast<std::uint8_t>(c);
              hex = true;
          }
        }
      }
      out << '"';
      break;
    }
  }
}

template <literal_style Style = literal_style::compact, typename Buffer>
std::string to_string_literal(Buffer const &v) {
  std::ostringstream ss;
  to_string_literal<Style>(ss, v);
  return ss.str();
}

template <literal_style Style = literal_style::compact>
std::string as_literal(std::string const &s) {
  return to_string_literal<Style>(s);
}

} // namespace test {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_test_tools_h
