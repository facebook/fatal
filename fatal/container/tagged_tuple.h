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
#include <fatal/type/pair.h>

#include <tuple>
#include <type_traits>
#include <utility>

namespace fatal {

//////////////////
// tagged_tuple //
//////////////////

template <typename... Args>
struct tagged_tuple {
  using map = type_map<Args...>;
  using tags = tuple_tags<type_get_first<Args>...>;
  using values = type_list<type_get_second<Args>...>;
  using tuple_type = typename values::template apply<std::tuple>;

  template <
    typename... UArgs,
    typename = safe_ctor_overload_t<tagged_tuple, UArgs...>
  >
  explicit constexpr tagged_tuple(UArgs &&...args):
    data_(std::forward<UArgs>(args)...)
  {}

  template <typename TTag>
  constexpr fast_pass<
    typename tags::template type_of<TTag, tuple_type>
  > get() const {
    return tags::template get<TTag>(data_);
  }

  template <typename TTag>
  typename tags::template type_of<TTag, tuple_type> &get() {
    return tags::template get<TTag>(data_);
  }

  constexpr fast_pass<tuple_type> tuple() const { return data_; }
  tuple_type &tuple() { return data_; }

  // TODO: TEST
  template <typename... UArgs>
  bool operator ==(tagged_tuple<UArgs...> const &rhs) const {
    return data_ == rhs.tuple();
  }

  // TODO: TEST
  template <typename... UArgs>
  bool operator !=(tagged_tuple<UArgs...> const &rhs) const {
    return !(*this == rhs);
  }

private:
  tuple_type data_;
};

////////////////////////
// build_tagged_tuple //
////////////////////////

namespace detail {
namespace tagged_tuple_impl {

template <typename... Args>
class builder {
  typedef type_list<Args...> args;

  typedef typename args::template unzip<2, 0> tags;
  typedef typename args::template unzip<2, 1> types;

  static_assert(tags::size == types::size, "not all tags map to a type");

public:
  typedef typename tags::template combine<type_pair>::template list<types>
    ::template apply<tagged_tuple> type;
};

} // namespace tagged_tuple_impl {
} // namespace detail {

template <typename... Args>
using build_tagged_tuple = typename detail::tagged_tuple_impl::builder<
  Args...
>::type;

///////////////////////
// make_tagged_tuple //
///////////////////////

template <typename... TTags, typename... Args>
constexpr auto make_tagged_tuple(Args &&...args)
  -> tagged_tuple<type_pair<TTags, typename std::decay<Args>::type>...>
{
  return tagged_tuple<type_pair<TTags, typename std::decay<Args>::type>...>(
    std::forward<Args>(args)...
  );
}

template <typename... TTags, typename... Args>
constexpr auto make_tagged_tuple(std::tuple<Args...> tuple)
  -> tagged_tuple<type_pair<TTags, Args>...>
{
  return tagged_tuple<type_pair<TTags, Args>...>(std::move(tuple));
}

} // namespace fatal {
