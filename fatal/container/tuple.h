/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_container_tuple_h
#define FATAL_INCLUDE_fatal_container_tuple_h

#include <fatal/container/tuple_tags.h>
#include <fatal/type/list.h>
#include <fatal/type/pair.h>
#include <fatal/type/push.h>
#include <fatal/type/slice.h>
#include <fatal/type/tag.h>
#include <fatal/type/traits.h>
#include <fatal/type/transform.h>
#include <fatal/type/zip.h>

#include <tuple>
#include <type_traits>
#include <utility>

namespace fatal {

///////////
// tuple //
///////////

/**
 * A tagged tuple container that enhances `std::tuple`, allowing queries by
 * a type tag, in a similar way that `std::get` allows queries by index.
 *
 * See also `make_tuple` and `build_tuple` for convenient ways of creating
 * a tuple.
 *
 * Example:
 *
 *  struct tag_a {};
 *  struct tag_b {};
 *  struct tag_c {};
 *
 *  // declares a tuple where:
 *  // - element `0` is of type `int` and can be indexed with `tag_a`
 *  // - element `1` is of type `double` and can be indexed with `tag_b`
 *  // - element `2` is of type `std::string` and can be indexed with `tag_c`
 *  using type = tuple<
 *    pair<tag_a, int>,
 *    pair<tag_b, double>,
 *    pair<tag_c, std::string>
 *  >;
 *
 *  // instantiates the tuple above with given values for its fields
 *  type x(10, 5.6, "foo");
 *
 *  // yields `10`, the value of the element element associated with tag `tag_a`
 *  auto result1 = x.get<tag_a>();
 *
 *  // yields `"foo"`, the value of the third element
 *  auto result2 = x.at<2>();
 *
 *  // sets the element associated with tag `tag_c` to `"bar"`
 *  x.get<tag_c>() = "bar";
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename... Args>
struct tuple {
  /**
   * A type map of element tag -> element type.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using map = fatal::list<Args...>;

  /**
   * The `tuple_tags` type associated with this tuple.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using tags = tuple_tags<first<Args>...>;

  /**
   * A type list of the element types.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using values = list<second<Args>...>;

  /**
   * The non-tagged `std::tuple` wrapped by this tagged tuple.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using type = apply_to<values, std::tuple>;

  /**
   * Gets the type of the element associated with the given tag.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename Tag>
  using type_of = typename tags::template type_of<Tag, type>;

  /**
   * Gets the type of the element associated with the given index.
   *
   * This is equivalent to `std::tuple_element`.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <std::size_t Index>
  using type_at = typename std::tuple_element<Index, type>;

  /**
   * The constructor for this tagged tuple.
   *
   * The parameters given to this constructor will be perfect-forwarded
   * to the constructor of the non-tagged tuple wrapped by this tagged
   * tuple.
   *
   * Example:
   *
   *  struct tag_a {};
   *  struct tag_b {};
   *  struct tag_c {};
   *
   *  using type = tuple<
   *    pair<tag_a, int>,
   *    pair<tag_b, double>,
   *    pair<tag_c, std::string>
   *  >;
   *
   *  // default constructs the elements of the tuple
   *  type x;
   *
   *  // initializes the elements of this tuple with `10`, `5.6` and `"foo"`
   *  type y(10, 5.6, "foo");
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <
    typename... UArgs,
    typename = safe_overload<tuple, UArgs...>
  >
  explicit constexpr tuple(UArgs &&...args):
    data_(std::forward<UArgs>(args)...)
  {}

  /**
   * Gets the element associated with the given tag.
   *
   * Example:
   *
   *  struct tag_a {};
   *  struct tag_b {};
   *
   *  auto values = make_tuple<tag_a, tag_b>(10, 5.6);
   *
   *  // yields `10`
   *  auto result1 = values.get<tag_a>();
   *
   *  // yields `5.6`
   *  auto result1 = values.get<tag_b>();
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename Tag>
  constexpr fast_pass<type_of<Tag>> get() const noexcept {
    return tags::template get<Tag>(data_);
  }

  /**
   * Gets the element associated with the given tag.
   *
   * Example:
   *
   *  struct tag_a {};
   *  struct tag_b {};
   *
   *  auto values = make_tuple<tag_a, tag_b>(10, 5.6);
   *
   *  // yields `10`
   *  auto result1 = values.get<tag_a>();
   *
   *  // yields `5.6`
   *  auto result1 = values.get<tag_b>();
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename Tag>
  type_of<Tag> &get() noexcept { return tags::template get<Tag>(data_); }

  /**
   * Gets the element at the given index.
   *
   * Example:
   *
   *  struct tag_a {};
   *  struct tag_b {};
   *
   *  auto values = make_tuple<tag_a, tag_b>(10, 5.6);
   *
   *  // yields `10`
   *  auto result1 = values.at<0>();
   *
   *  // yields `5.6`
   *  auto result1 = values.at<1>();
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <std::size_t Index>
  constexpr fast_pass<type_at<Index>> at() const noexcept {
    return std::get<Index>(data_);
  }

  /**
   * Gets the element at the given index.
   *
   * Example:
   *
   *  struct tag_a {};
   *  struct tag_b {};
   *
   *  auto values = make_tuple<tag_a, tag_b>(10, 5.6);
   *
   *  // yields `10`
   *  auto result1 = values.at<0>();
   *
   *  // yields `5.6`
   *  auto result1 = values.at<1>();
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <std::size_t Index>
  type_at<Index> &at() noexcept { return std::get<Index>(data_); }

  /**
   * Gets the non-tagged `std::tuple` wrapped by this tagged tuple.
   *
   * Example:
   *
   *  struct tag_a {};
   *  struct tag_b {};
   *
   *  auto values = make_tuple<tag_a, tag_b>(10, 5.6);
   *
   *  // yields `std::tuple<int, double>(10, 5.6)`
   *  auto result1 = values.data();
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  constexpr fast_pass<type> data() const noexcept { return data_; }

  /**
   * Gets the non-tagged `std::tuple` wrapped by this tagged tuple.
   *
   * Example:
   *
   *  struct tag_a {};
   *  struct tag_b {};
   *
   *  auto values = make_tuple<tag_a, tag_b>(10, 5.6);
   *
   *  // yields `std::tuple<int, double>(10, 5.6)`
   *  auto result1 = values.data();
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  type &data() noexcept { return data_; }

  /**
   * TODO: DOCUMENT AND TEST
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <template <typename...> class Transform>
  using apply = fatal::apply<Transform, Args...>;

  /**
   * TODO: DOCUMENT
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... TPairs>
  using push_front = apply_to<
    push_front<map, TPairs...>,
    fatal::tuple
  >;

  /**
   * TODO: DOCUMENT
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... TPairs>
  using push_back = apply_to<
    push_back<map, TPairs...>,
    fatal::tuple
  >;

  /**
   * TODO: DOCUMENT
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <
    template <typename...> class TPredicate, typename V, typename... VArgs
  >
  constexpr std::size_t foreach_if(V &&visitor, VArgs &&...args) const {
    return tags::template foreach_if<TPredicate>(
      data_, std::forward<V>(visitor), std::forward<VArgs>(args)...
    );
  }

  /**
   * TODO: DOCUMENT AND TEST
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <
    template <typename...> class TPredicate, typename V, typename... VArgs
  >
  std::size_t foreach_if(V &&visitor, VArgs &&...args) {
    return tags::template foreach_if<TPredicate>(
      data_, std::forward<V>(visitor), std::forward<VArgs>(args)...
    );
  }


  /**
   * TODO: DOCUMENT AND TEST
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename V, typename... VArgs>
  constexpr bool foreach(V &&visitor, VArgs &&...args) const {
    return tags::foreach(
      data_, std::forward<V>(visitor), std::forward<VArgs>(args)...
    );
  }

  /**
   * TODO: DOCUMENT AND TEST
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename V, typename... VArgs>
  bool foreach(V &&visitor, VArgs &&...args) {
    return tags::foreach(
      data_, std::forward<V>(visitor), std::forward<VArgs>(args)...
    );
  }

  /**
   * Equality comparison operator.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  // TODO: TEST
  template <typename... UArgs>
  bool operator ==(tuple<UArgs...> const &rhs) const {
    return data_ == rhs.data();
  }

  /**
   * Inequality comparison operator.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  // TODO: TEST
  template <typename... UArgs>
  bool operator !=(tuple<UArgs...> const &rhs) const {
    return !(*this == rhs);
  }

private:
  type data_;
};

////////////////
// tuple_from //
////////////////

/**
 * A convenient way to declare tuples based on other types.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename... Args>
class tuple_from {
  template <
    template <typename...> class TagTransform,
    template <typename...> class TypeTransform
  >
  class impl {
    template <typename T>
    using pair = fatal::pair<
      fatal::apply<TagTransform, T>,
      fatal::apply<TypeTransform, T>
    >;

    template <typename T>
    using map_entry_transform = fatal::pair<
      TagTransform<first<T>>,
      TypeTransform<second<T>>
    >;

  public:
    template <typename... UArgs>
    using args = tuple<pair<UArgs>...>;

    template <typename List>
    using list = apply_to<transform<List, applier<pair>>, tuple>;

    template <typename Map>
    using map = apply_to<transform<Map, applier<map_entry_transform>>, tuple>;
  };

public:
  /**
   * Creates a tuple with elements coming from a variadic pack of types, by
   * applying transforms to them to derive both the tag and the value type.
   *
   * Example:
   *
   *  template <typename Tag, typename T>
   *  struct my_metadata {
   *    using tag = Tag;
   *    using type = Type;
   *  };
   *
   *  // yields `tuple<
   *  //   pair<int, double>,
   *  //   pair<float, bool>,
   *  //   pair<short, long>
   *  // >`
   *  using result = tuple_from<
   *    my_metadata<int, double>,
   *    my_metadata<float, bool>,
   *    my_metadata<short, long>
   *  >::args<
   *    fatal::get_member_type::tag,
   *    fatal::get_member_type::type
   *  >;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <
    template <typename...> class TypeTransform = identity,
    template <typename...> class TagTransform = identity
  >
  using args = typename impl<TagTransform, TypeTransform>
    ::template args<Args...>;

  /**
   * Creates a tuple with elements coming from a type list, by applying
   * transforms to them to derive both the tag and the value type.
   *
   * Example:
   *
   *  template <typename Tag, typename T>
   *  struct my_metadata {
   *    using tag = Tag;
   *    using type = Type;
   *  };
   *
   *  using my_list = list<
   *    my_metadata<int, double>,
   *    my_metadata<float, bool>,
   *    my_metadata<short, long>
   *  >;
   *
   *  // yields `tuple<
   *  //   pair<int, double>,
   *  //   pair<float, bool>,
   *  //   pair<short, long>
   *  // >`
   *  using result = tuple_from<my_list>::list<
   *    fatal::get_member_type::tag,
   *    fatal::get_member_type::type
   *  >;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <
    template <typename...> class TypeTransform = identity,
    template <typename...> class TagTransform = identity
  >
  using list = fatal::apply<
    impl<TagTransform, TypeTransform>::template list,
    Args...
  >;

  /**
   * Creates a tuple with elements coming from a type map, by applying
   * transforms to the keys to derive the tag, and to the values to derive
   * the value type.
   *
   * Example:
   *
   *  template <typename> struct my_tag {};
   *  template <typename> struct my_value {};
   *
   *  using my_map = build_type_map<
   *    int, double,
   *    float, bool,
   *    short, long
   *  >;
   *
   *  // yields `tuple<
   *  //   pair<int, double>,
   *  //   pair<float, bool>,
   *  //   pair<short, long>
   *  // >`
   *  using result1 = tuple_from<my_map>::map<>;
   *
   *  // yields `tuple<
   *  //   pair<my_tag<int>, my_value<double>>,
   *  //   pair<my_tag<float>, my_value<bool>>,
   *  //   pair<my_tag<short>, my_value<long>>
   *  // >`
   *  using result2 = tuple_from<my_map>::map<my_value, my_tag>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <
    template <typename...> class TypeTransform = identity,
    template <typename...> class TagTransform = identity
  >
  using map = fatal::apply<
    impl<TagTransform, TypeTransform>::template map,
    Args...
  >;
};

/////////////////
// build_tuple //
/////////////////

namespace detail {
namespace tuple_impl {
template <typename...> struct builder;
} // namespace tuple_impl {
} // namespace detail {

/**
 * A convenient way of declaring a tagged tuple out of a flat list of
 * tags and types. Tags and types are grouped pairwise.
 *
 * The template arguments should follow the format
 * `Tag0, Type0, Tag1, Type1, ..., TagN, TypeN`.
 *
 * Example:
 *
 *  struct tag_a {};
 *  struct tag_b {};
 *  struct tag_c {};
 *
 *  // declares a tagged tuple where:
 *  // - element `0` is of type `int` and can be indexed with `tag_a`
 *  // - element `1` is of type `double` and can be indexed with `tag_b`
 *  // - element `2` is of type `std::string` and can be indexed with `tag_c`
 *  using type = build_tuple<tag_a, int, tag_b, double, tag_c, std::string>;
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename... Args>
using build_tuple = typename detail::tuple_impl::builder<
  tuple<>,
  Args...
>::type;

////////////////
// make_tuple //
////////////////

/**
 * A convenient way to create a tagged tuple instance with initial values.
 *
 * The tags should be provided as template parameters to this function,
 * following the format `Tag0, Tag1, ..., TagN`. The respective values
 * should be provided as non-template parameters, following the format
 * `Value0, Value1, ..., ValueN`.
 *
 * Example:
 *
 *  struct tag_a {};
 *  struct tag_b {};
 *  struct tag_c {};
 *
 *  // creates a tagged tuple where:
 *  // - element `0` is of type `int` and can be indexed with `tag_a`
 *  // - element `1` is of type `double` and can be indexed with `tag_b`
 *  // - element `2` is of type `std::string` and can be indexed with `tag_c`
 *  // with, respectivelly, initial values `10`, `5.6` and `"foo"`
 *  auto values = make_tuple<tag_a, tag_b, tag_c>(10, 5.6, std::string("foo"));
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename... Tags, typename... Args>
constexpr auto make_tuple(Args &&...args)
  -> tuple<pair<Tags, typename std::decay<Args>::type>...>
{
  return tuple<pair<Tags, typename std::decay<Args>::type>...>(
    std::forward<Args>(args)...
  );
}

/**
 * A convenient way to create a tagged tuple instances
 * out of a regular `std::tuple`.
 *
 * The tags should be provided as template parameters to this function,
 * following the format `Tag0, Tag1, ..., TagN`.
 *
 * The amount of tags given should match the amount of elements
 * contained within the given std::tuple.
 *
 * Example:
 *
 *  struct tag_a {};
 *  struct tag_b {};
 *  struct tag_c {};
 *
 *  auto x = std::make_tuple(10, 5.6, std::string("foo"));
 *
 *  // creates a tagged tuple where:
 *  // - element `0` is of type `int` and can be indexed with `tag_a`
 *  // - element `1` is of type `double` and can be indexed with `tag_b`
 *  // - element `2` is of type `std::string` and can be indexed with `tag_c`
 *  // with, respectivelly, initial values `10`, `5.6` and `"foo"`
 *  auto values = make_tuple<tag_a, tag_b, tag_c>(x);
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename... Tags, typename... Args>
constexpr auto make_tuple(std::tuple<Args...> &&tuple)
  -> fatal::tuple<pair<Tags, Args>...>
{
  return fatal::tuple<pair<Tags, Args>...>(std::move(tuple));
}

template <typename... Tags, typename... Args>
constexpr auto make_tuple(std::tuple<Args...> const &tuple)
  -> fatal::tuple<pair<Tags, Args>...>
{
  return fatal::tuple<pair<Tags, Args>...>(tuple);
}

////////////////////////////
// IMPLEMENTATION DETAILS //
////////////////////////////

namespace detail {
namespace tuple_impl {

template <typename T>
struct builder<T> {
  using type = T;
};

template <typename... T, typename Tag, typename Type, typename... Args>
struct builder<tuple<T...>, Tag, Type, Args...>:
  builder<tuple<T..., pair<Tag, Type>>, Args...>
{
  static_assert(sizeof...(Args) % 2 == 0, "not all tags map to a type");
};

} // namespace tuple_impl {
} // namespace detail {
} // namespace fatal {

#endif // FATAL_INCLUDE_fatal_container_tuple_h
