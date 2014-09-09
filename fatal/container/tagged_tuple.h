/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#pragma once

#include <fatal/container/tuple_tags.h>
#include <fatal/type/list.h>

#include <tuple>
#include <type_traits>
#include <utility>

namespace ftl {

/////////////////
// tagged_type //
/////////////////

template <typename TTag, typename T>
struct tagged_type {
  typedef TTag tag;
  typedef T type;
};

//////////////////
// tagged_tuple //
//////////////////

template <typename... Args>
class tagged_tuple {
  static_assert(
    logical_and_constants<
      std::true_type,
      is_template<tagged_type>::template instantiation<Args>...
    >::value,
    "tagged_tuple parameters must be instantiations of tagged_type"
  );

public:
  typedef tuple_tags<typename Args::tag...> tags;
  typedef std::tuple<typename Args::type...> tuple_type;

  template <
    typename... UArgs,
    typename X = safe_ctor_overload_t<tagged_tuple, UArgs...>
  >
  explicit constexpr tagged_tuple(UArgs &&...args):
    data_(std::forward<UArgs>(args)...)
  {}

  template <typename TTag>
  constexpr fast_pass<typename tags::template type_of<TTag, tuple_type>> get() const {
    return tags::template get<TTag>(data_);
  }

  template <typename TTag>
  typename tags::template type_of<TTag, tuple_type> &get() {
    return tags::template get<TTag>(data_);
  }

  constexpr fast_pass<tuple_type> tuple() const { return data_; }
  tuple_type &tuple() { return data_; }

private:
  tuple_type data_;
};

/////////////////////////
// paired_tagged_tuple //
/////////////////////////

namespace detail {
namespace tagged_tuple_impl {

template <typename... Args>
class builder {
  typedef type_list<Args...> args;

  typedef typename args::template unzip<1, 0> tags;
  typedef typename args::template unzip<1, 1> types;

  static_assert(tags::size == types::size, "not all tags map to a type");

public:
  typedef typename tags::template combine<types, tagged_type>::template apply<
    tagged_tuple
  > type;
};

} // namespace tagged_tuple_impl {
} // namespace detail {

template <typename... Args>
using paired_tagged_tuple = typename detail::tagged_tuple_impl::builder<
  Args...
>::type;

///////////////////////
// make_tagged_tuple //
///////////////////////

template <typename... TTags, typename... Args>
constexpr auto make_tagged_tuple(Args &&...args)
  -> tagged_tuple<tagged_type<TTags, typename std::decay<Args>::type>...>
{
  return tagged_tuple<tagged_type<TTags, typename std::decay<Args>::type>...>(
    std::forward<Args>(args)...
  );
}

template <typename... TTags, typename... Args>
constexpr auto make_tagged_tuple(std::tuple<Args...> tuple)
  -> tagged_tuple<tagged_type<TTags, Args>...>
{
  return tagged_tuple<tagged_type<TTags, Args>...>(std::move(tuple));
}

} // namespace ftl {
