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

/**
 * A convenient macro that creates a transform which
 * evaluates to the given member type.
 *
 * Provides a template alias named `Name` which extracts
 * the member type with the same name.
 *
 * Example:
 *
 *  FATAL_GET_TYPE(get_type, type);
 *
 *  // evaluates to `int const`
 *  using result = get_type<std::add_const<int>>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
# define FATAL_GET_TYPE(Name, ...) \
  struct Name { \
    template <typename Name##T> \
    using apply = typename Name##T::__VA_ARGS__; \
  }

/**
 * Provides transforms that evaluate to a member type of a given type.
 *
 * Example:
 *
 *  // yields `int`
 *  using result1 = get_type::type<std::add_const<int>>;
 *
 *  using map = std::map<double, std::string>;
 *
 *  // yields `std::pair<double, std::string>`
 *  using result2 = get_type::value_type<map>;
 *
 *  // yields `double`
 *  using result3 = get_type::key_type<map>;
 *
 *  // yields `std::string`
 *  using result4 = get_type::mapped_type<map>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
namespace get_type {
# define FATAL_IMPL_GET_TYPE(Name) \
  FATAL_GET_TYPE(Name, Name)

  FATAL_IMPL_GET_TYPE(char_type);
  FATAL_IMPL_GET_TYPE(return_type);
  FATAL_IMPL_GET_TYPE(int_type);

# define FATAL_IMPL_GET_TYPE_TYPE(Name) \
  FATAL_GET_TYPE(Name, Name); \
  FATAL_GET_TYPE(Name##_type, Name##_type)

  FATAL_IMPL_GET_TYPE_TYPE(allocator);
  FATAL_IMPL_GET_TYPE_TYPE(args);
  FATAL_IMPL_GET_TYPE_TYPE(array);
  FATAL_IMPL_GET_TYPE_TYPE(category);
  FATAL_IMPL_GET_TYPE_TYPE(client);
  FATAL_IMPL_GET_TYPE_TYPE(clients);
  FATAL_IMPL_GET_TYPE_TYPE(config);
  FATAL_IMPL_GET_TYPE_TYPE(const_iterator);
  FATAL_IMPL_GET_TYPE_TYPE(const_pointer);
  FATAL_IMPL_GET_TYPE_TYPE(const_ptr);
  FATAL_IMPL_GET_TYPE_TYPE(const_ref);
  FATAL_IMPL_GET_TYPE_TYPE(const_reference);
  FATAL_IMPL_GET_TYPE_TYPE(const_reverse_iterator);
  FATAL_IMPL_GET_TYPE_TYPE(constant);
  FATAL_IMPL_GET_TYPE_TYPE(constants);
  FATAL_IMPL_GET_TYPE_TYPE(data);
  FATAL_IMPL_GET_TYPE_TYPE(decode);
  FATAL_IMPL_GET_TYPE_TYPE(decoder);
  FATAL_IMPL_GET_TYPE_TYPE(difference);
  FATAL_IMPL_GET_TYPE_TYPE(element);
  FATAL_IMPL_GET_TYPE_TYPE(encode);
  FATAL_IMPL_GET_TYPE_TYPE(encoder);
  FATAL_IMPL_GET_TYPE_TYPE(enumeration);
  FATAL_IMPL_GET_TYPE_TYPE(enumerations);
  FATAL_IMPL_GET_TYPE_TYPE(enums);
  FATAL_IMPL_GET_TYPE_TYPE(exception);
  FATAL_IMPL_GET_TYPE_TYPE(exceptions);
  FATAL_IMPL_GET_TYPE_TYPE(extension);
  FATAL_IMPL_GET_TYPE_TYPE(extensions);
  FATAL_IMPL_GET_TYPE_TYPE(fault);
  FATAL_IMPL_GET_TYPE_TYPE(faults);
  FATAL_IMPL_GET_TYPE_TYPE(field);
  FATAL_IMPL_GET_TYPE_TYPE(fields);
  FATAL_IMPL_GET_TYPE_TYPE(first);
  FATAL_IMPL_GET_TYPE_TYPE(flag);
  FATAL_IMPL_GET_TYPE_TYPE(flags);
  FATAL_IMPL_GET_TYPE_TYPE(get);
  FATAL_IMPL_GET_TYPE_TYPE(getter);
  FATAL_IMPL_GET_TYPE_TYPE(getters);
  FATAL_IMPL_GET_TYPE_TYPE(hash);
  FATAL_IMPL_GET_TYPE_TYPE(hasher);
  FATAL_IMPL_GET_TYPE_TYPE(id);
  FATAL_IMPL_GET_TYPE_TYPE(identifier);
  FATAL_IMPL_GET_TYPE_TYPE(ids);
  FATAL_IMPL_GET_TYPE_TYPE(index);
  FATAL_IMPL_GET_TYPE_TYPE(info);
  FATAL_IMPL_GET_TYPE_TYPE(information);
  FATAL_IMPL_GET_TYPE_TYPE(instance);
  FATAL_IMPL_GET_TYPE_TYPE(item);
  FATAL_IMPL_GET_TYPE_TYPE(iterator);
  FATAL_IMPL_GET_TYPE_TYPE(key);
  FATAL_IMPL_GET_TYPE_TYPE(keys);
  FATAL_IMPL_GET_TYPE_TYPE(keyword);
  FATAL_IMPL_GET_TYPE_TYPE(last);
  FATAL_IMPL_GET_TYPE_TYPE(legacy);
  FATAL_IMPL_GET_TYPE_TYPE(legacy_geter);
  FATAL_IMPL_GET_TYPE_TYPE(legacy_id);
  FATAL_IMPL_GET_TYPE_TYPE(legacy_name);
  FATAL_IMPL_GET_TYPE_TYPE(legacy_property);
  FATAL_IMPL_GET_TYPE_TYPE(legacy_setter);
  FATAL_IMPL_GET_TYPE_TYPE(legacy_value);
  FATAL_IMPL_GET_TYPE_TYPE(lhs);
  FATAL_IMPL_GET_TYPE_TYPE(list);
  FATAL_IMPL_GET_TYPE_TYPE(map);
  FATAL_IMPL_GET_TYPE_TYPE(mapped);
  FATAL_IMPL_GET_TYPE_TYPE(mapping);
  FATAL_IMPL_GET_TYPE_TYPE(mappings);
  FATAL_IMPL_GET_TYPE_TYPE(member);
  FATAL_IMPL_GET_TYPE_TYPE(members);
  FATAL_IMPL_GET_TYPE_TYPE(metadata);
  FATAL_IMPL_GET_TYPE_TYPE(name);
  FATAL_IMPL_GET_TYPE_TYPE(names);
  FATAL_IMPL_GET_TYPE_TYPE(pair);
  FATAL_IMPL_GET_TYPE_TYPE(pod);
  FATAL_IMPL_GET_TYPE_TYPE(pointer);
  FATAL_IMPL_GET_TYPE_TYPE(predicate);
  FATAL_IMPL_GET_TYPE_TYPE(prefix);
  FATAL_IMPL_GET_TYPE_TYPE(prefixes);
  FATAL_IMPL_GET_TYPE_TYPE(ptr);
  FATAL_IMPL_GET_TYPE_TYPE(ratio);
  FATAL_IMPL_GET_TYPE_TYPE(reader);
  FATAL_IMPL_GET_TYPE_TYPE(readers);
  FATAL_IMPL_GET_TYPE_TYPE(ref);
  FATAL_IMPL_GET_TYPE_TYPE(reference);
  FATAL_IMPL_GET_TYPE_TYPE(refs);
  FATAL_IMPL_GET_TYPE_TYPE(request);
  FATAL_IMPL_GET_TYPE_TYPE(response);
  FATAL_IMPL_GET_TYPE_TYPE(result);
  FATAL_IMPL_GET_TYPE_TYPE(reverse);
  FATAL_IMPL_GET_TYPE_TYPE(reverse_iterator);
  FATAL_IMPL_GET_TYPE_TYPE(rhs);
  FATAL_IMPL_GET_TYPE_TYPE(s);
  FATAL_IMPL_GET_TYPE_TYPE(second);
  FATAL_IMPL_GET_TYPE_TYPE(service);
  FATAL_IMPL_GET_TYPE_TYPE(services);
  FATAL_IMPL_GET_TYPE_TYPE(set);
  FATAL_IMPL_GET_TYPE_TYPE(setter);
  FATAL_IMPL_GET_TYPE_TYPE(setters);
  FATAL_IMPL_GET_TYPE_TYPE(size);
  FATAL_IMPL_GET_TYPE_TYPE(str);
  FATAL_IMPL_GET_TYPE_TYPE(string);
  FATAL_IMPL_GET_TYPE_TYPE(structs);
  FATAL_IMPL_GET_TYPE_TYPE(structure);
  FATAL_IMPL_GET_TYPE_TYPE(suffix);
  FATAL_IMPL_GET_TYPE_TYPE(suffixes);
  FATAL_IMPL_GET_TYPE_TYPE(tag);
  FATAL_IMPL_GET_TYPE_TYPE(third);
  FATAL_IMPL_GET_TYPE_TYPE(traits);
  FATAL_IMPL_GET_TYPE_TYPE(tuple);
  FATAL_IMPL_GET_TYPE_TYPE(type);
  FATAL_IMPL_GET_TYPE_TYPE(type_class);
  FATAL_IMPL_GET_TYPE_TYPE(types);
  FATAL_IMPL_GET_TYPE_TYPE(unions);
  FATAL_IMPL_GET_TYPE_TYPE(value);
  FATAL_IMPL_GET_TYPE_TYPE(values);
  FATAL_IMPL_GET_TYPE_TYPE(var);
  FATAL_IMPL_GET_TYPE_TYPE(variable);
  FATAL_IMPL_GET_TYPE_TYPE(variant);
  FATAL_IMPL_GET_TYPE_TYPE(verb);
  FATAL_IMPL_GET_TYPE_TYPE(version);
  FATAL_IMPL_GET_TYPE_TYPE(writer);
  FATAL_IMPL_GET_TYPE_TYPE(writers);

# undef FATAL_IMPL_GET_TYPE_TYPE
# undef FATAL_IMPL_GET_TYPE
} // namespace get_type {

} // namespace fatal {
