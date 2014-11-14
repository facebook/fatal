/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_type_tagged_tuple_h
#define FATAL_INCLUDE_fatal_type_tagged_tuple_h

#include <fatal/container/tuple_tags.h>
#include <fatal/type/list.h>
#include <fatal/type/pair.h>
#include <fatal/type/tag.h>

#include <tuple>
#include <type_traits>
#include <utility>

namespace fatal {
namespace detail {
namespace tagged_tuple_impl {

struct foreach_visitor {
  template <
    typename TTag, typename T, std::size_t Index,
    typename TTaggedTuple, typename V, typename... VArgs
  >
  void operator ()(
    indexed_type_tag<type_pair<TTag, T>, Index> tag,
    TTaggedTuple &&tuple,
    V &&visitor,
    VArgs &&...args
  ) const {
    visitor(tag, tuple.template get<TTag>(), std::forward<VArgs>(args)...);
  }
};

} // namespace tagged_tuple_impl {
} // namespace detail {

//////////////////
// tagged_tuple //
//////////////////

template <typename... Args>
struct tagged_tuple {
  using map = type_map<Args...>;
  using tags = tuple_tags<type_get_first<Args>...>;
  using values = type_list<type_get_second<Args>...>;
  using type = typename values::template apply<std::tuple>;

  template <typename TTag>
  using type_of = typename tags::template type_of<TTag, type>;

  template <
    typename... UArgs,
    typename = safe_overload_t<tagged_tuple, UArgs...>
  >
  explicit constexpr tagged_tuple(UArgs &&...args):
    data_(std::forward<UArgs>(args)...)
  {}

  template <typename TTag>
  constexpr fast_pass<
    typename tags::template type_of<TTag, type>
  > get() const {
    return tags::template get<TTag>(data_);
  }

  template <typename TTag>
  typename tags::template type_of<TTag, type> &get() {
    return tags::template get<TTag>(data_);
  }

  constexpr fast_pass<type> tuple() const { return data_; }
  type &tuple() { return data_; }

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

  // TODO: DOCUMENT AND TEST
  template <template <typename...> class TTransform>
  using apply = fatal::apply<TTransform, Args...>;

  // TODO: DOCUMENT AND TEST
  template <
    template <typename...> class TTypeTransform = identity_transform,
    template <typename...> class TTagTransform = identity_transform
  >
  using transform = typename map::template transform<
    TTypeTransform, TTagTransform
  >::contents::template apply<fatal::tagged_tuple>;

  // TODO: DOCUMENT AND TEST
  template <
    typename TTag,
    template <typename...> class TTypeTransform = identity_transform,
    template <typename...> class TTagTransform = identity_transform
  >
  using transform_at = typename map::template transform_at<
    TTag, TTypeTransform, TTagTransform
  >::template apply<fatal::tagged_tuple>;

  // TODO: DOCUMENT AND TEST
  template <typename... TPairs>
  using push_front = typename tags::list::template push_front<TPairs...>
    ::template apply<fatal::tagged_tuple>;

  // TODO: DOCUMENT AND TEST
  template <typename... TPairs>
  using push_back = typename tags::list::template push_back<TPairs...>
    ::template apply<fatal::tagged_tuple>;

  template <
    template <typename...> class TPredicate, typename V, typename... VArgs
  >
  std::size_t foreach_if(V &&visitor, VArgs &&...args) const {
    return map::contents::template foreach_if<TPredicate>(
      detail::tagged_tuple_impl::foreach_visitor(),
      *this,
      std::forward<V>(visitor),
      std::forward<VArgs>(args)...
    );
  };

  template <
    template <typename...> class TPredicate, typename V, typename... VArgs
  >
  std::size_t foreach_if(V &&visitor, VArgs &&...args) {
    return map::contents::template foreach_if<TPredicate>(
      detail::tagged_tuple_impl::foreach_visitor(),
      *this,
      std::forward<V>(visitor),
      std::forward<VArgs>(args)...
    );
  };

  template <typename V, typename... VArgs>
  bool foreach(V &&visitor, VArgs &&...args) const {
    return map::contents::foreach(
      detail::tagged_tuple_impl::foreach_visitor(),
      *this,
      std::forward<V>(visitor),
      std::forward<VArgs>(args)...
    );
  };

  template <typename V, typename... VArgs>
  bool foreach(V &&visitor, VArgs &&...args) {
    return map::contents::foreach(
      detail::tagged_tuple_impl::foreach_visitor(),
      *this,
      std::forward<V>(visitor),
      std::forward<VArgs>(args)...
    );
  };

private:
  type data_;
};

///////////////////////
// tagged_tuple_from //
///////////////////////

// TODO: DOCUMENT AND TEST
template <typename... Args>
class tagged_tuple_from {
  template <
    template <typename...> class TTagTransform,
    template <typename...> class TTypeTransform
  >
  class impl {
    template <typename T>
    using pair = type_pair<
      fatal::apply<TTagTransform, T>,
      fatal::apply<TTypeTransform, T>
    >;

  public:
    template <typename... UArgs>
    using args = tagged_tuple<pair<UArgs>...>;

    template <typename TList>
    using list = typename TList::template apply<tagged_tuple, pair>;

    template <typename TMap>
    using map = typename TMap::template apply<
      tagged_tuple, TTypeTransform, TTagTransform
    >;
  };

public:
  template <
    template <typename...> class TTypeTransform = identity_transform,
    template <typename...> class TTagTransform = get_member_type::tag
  >
  using args = typename impl<TTagTransform, TTypeTransform>
    ::template args<Args...>;

  template <
    template <typename...> class TTypeTransform = identity_transform,
    template <typename...> class TTagTransform = get_member_type::tag
  >
  using list = fatal::apply<
    impl<TTagTransform, TTypeTransform>::template list,
    Args...
  >;

  template <
    template <typename...> class TTypeTransform = identity_transform,
    template <typename...> class TTagTransform = identity_transform
  >
  using map = fatal::apply<
    impl<TTagTransform, TTypeTransform>::template map,
    Args...
  >;
};

////////////////////////
// build_tagged_tuple //
////////////////////////

namespace detail {
namespace tagged_tuple_impl {

template <typename... Args>
class builder {
  using args = type_list<Args...>;

  using tags = typename args::template unzip<2, 0>;
  using types = typename args::template unzip<2, 1>;

  static_assert(tags::size == types::size, "not all tags map to a type");

public:
  using type = typename tags::template combine<type_pair>::template list<types>
    ::template apply<tagged_tuple>;
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

#endif // FATAL_INCLUDE_fatal_type_tagged_tuple_h
