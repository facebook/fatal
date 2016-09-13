/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_has_type_h
#define FATAL_INCLUDE_fatal_type_has_type_h

#include <type_traits>

namespace fatal {

/**
 * This macro creates a class named `Class` that can check whether some
 * type has a member type with name `Member`.
 *
 * The class created by this macro looks like this:
 *
 *  struct Class {
 *    template <typename T>
 *    using apply = <either std::true_type or std::false_type>;
 *  };
 *
 * Example:
 *
 *  FATAL_HAS_TYPE(has_xyz, xyz);
 *
 *  struct foo { using xyz = int; };
 *  struct bar { typedef int xyz; };
 *  struct baz {};
 *  struct gaz { struct xyz {}; };
 *
 *  // yields `std::true_type`
 *  using result1 = has_xyz::apply<foo>;
 *
 *  // yields `std::true_type`
 *  using result2 = has_xyz::apply<bar>;
 *
 *  // yields `std::false_type`
 *  using result3 = has_xyz::apply<baz>;
 *
 *  // yields `std::true_type`
 *  using result4 = has_xyz::apply<gaz>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define FATAL_HAS_TYPE(Class, ...) \
  struct Class { \
    template <typename T> \
    static std::true_type sfinae(typename T::__VA_ARGS__ *); \
    \
    template <typename> \
    static std::false_type sfinae(...); \
    \
    template <typename T> \
    using apply = decltype(sfinae<T>(nullptr)); \
  }

/**
 * A collection of instances of `FATAL_HAS_TYPE` for common member type names.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
namespace has_type {
# define FATAL_IMPL_HAS_TYPE(Name) FATAL_HAS_TYPE(Name, Name)

  FATAL_IMPL_HAS_TYPE(char_type);
  FATAL_IMPL_HAS_TYPE(int_type);
  FATAL_IMPL_HAS_TYPE(type);
  FATAL_IMPL_HAS_TYPE(types);

# define FATAL_IMPL_HAS_TYPE_TYPE(Name) \
  FATAL_IMPL_HAS_TYPE(Name); \
  FATAL_IMPL_HAS_TYPE(Name##_type)

  FATAL_IMPL_HAS_TYPE_TYPE(allocator);
  FATAL_IMPL_HAS_TYPE_TYPE(args);
  FATAL_IMPL_HAS_TYPE_TYPE(array);
  FATAL_IMPL_HAS_TYPE_TYPE(category);
  FATAL_IMPL_HAS_TYPE_TYPE(config);
  FATAL_IMPL_HAS_TYPE_TYPE(const_iterator);
  FATAL_IMPL_HAS_TYPE_TYPE(const_pointer);
  FATAL_IMPL_HAS_TYPE_TYPE(const_ptr);
  FATAL_IMPL_HAS_TYPE_TYPE(const_ref);
  FATAL_IMPL_HAS_TYPE_TYPE(const_reference);
  FATAL_IMPL_HAS_TYPE_TYPE(const_reverse_iterator);
  FATAL_IMPL_HAS_TYPE_TYPE(data);
  FATAL_IMPL_HAS_TYPE_TYPE(decode);
  FATAL_IMPL_HAS_TYPE_TYPE(decoder);
  FATAL_IMPL_HAS_TYPE_TYPE(difference);
  FATAL_IMPL_HAS_TYPE_TYPE(element);
  FATAL_IMPL_HAS_TYPE_TYPE(encode);
  FATAL_IMPL_HAS_TYPE_TYPE(encoder);
  FATAL_IMPL_HAS_TYPE_TYPE(extension);
  FATAL_IMPL_HAS_TYPE_TYPE(first);
  FATAL_IMPL_HAS_TYPE_TYPE(flag);
  FATAL_IMPL_HAS_TYPE_TYPE(hash);
  FATAL_IMPL_HAS_TYPE_TYPE(id);
  FATAL_IMPL_HAS_TYPE_TYPE(ids);
  FATAL_IMPL_HAS_TYPE_TYPE(index);
  FATAL_IMPL_HAS_TYPE_TYPE(info);
  FATAL_IMPL_HAS_TYPE_TYPE(information);
  FATAL_IMPL_HAS_TYPE_TYPE(instance);
  FATAL_IMPL_HAS_TYPE_TYPE(item);
  FATAL_IMPL_HAS_TYPE_TYPE(iterator);
  FATAL_IMPL_HAS_TYPE_TYPE(key);
  FATAL_IMPL_HAS_TYPE_TYPE(list);
  FATAL_IMPL_HAS_TYPE_TYPE(map);
  FATAL_IMPL_HAS_TYPE_TYPE(mapped);
  FATAL_IMPL_HAS_TYPE_TYPE(mapping);
  FATAL_IMPL_HAS_TYPE_TYPE(mappings);
  FATAL_IMPL_HAS_TYPE_TYPE(member);
  FATAL_IMPL_HAS_TYPE_TYPE(members);
  FATAL_IMPL_HAS_TYPE_TYPE(name);
  FATAL_IMPL_HAS_TYPE_TYPE(names);
  FATAL_IMPL_HAS_TYPE_TYPE(pair);
  FATAL_IMPL_HAS_TYPE_TYPE(pointer);
  FATAL_IMPL_HAS_TYPE_TYPE(predicate);
  FATAL_IMPL_HAS_TYPE_TYPE(ptr);
  FATAL_IMPL_HAS_TYPE_TYPE(reader);
  FATAL_IMPL_HAS_TYPE_TYPE(ref);
  FATAL_IMPL_HAS_TYPE_TYPE(reference);
  FATAL_IMPL_HAS_TYPE_TYPE(request);
  FATAL_IMPL_HAS_TYPE_TYPE(response);
  FATAL_IMPL_HAS_TYPE_TYPE(result);
  FATAL_IMPL_HAS_TYPE_TYPE(reverse);
  FATAL_IMPL_HAS_TYPE_TYPE(reverse_iterator);
  FATAL_IMPL_HAS_TYPE_TYPE(second);
  FATAL_IMPL_HAS_TYPE_TYPE(set);
  FATAL_IMPL_HAS_TYPE_TYPE(size);
  FATAL_IMPL_HAS_TYPE_TYPE(str);
  FATAL_IMPL_HAS_TYPE_TYPE(string);
  FATAL_IMPL_HAS_TYPE_TYPE(tag);
  FATAL_IMPL_HAS_TYPE_TYPE(traits);
  FATAL_IMPL_HAS_TYPE_TYPE(tuple);
  FATAL_IMPL_HAS_TYPE_TYPE(value);
  FATAL_IMPL_HAS_TYPE_TYPE(values);
  FATAL_IMPL_HAS_TYPE_TYPE(version);
  FATAL_IMPL_HAS_TYPE_TYPE(writer);

# undef FATAL_IMPL_HAS_TYPE_TYPE
# undef FATAL_IMPL_HAS_TYPE
};

} // namespace fatal

#endif // FATAL_INCLUDE_fatal_type_has_type_h
