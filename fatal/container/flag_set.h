/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/math/numerics.h>
#include <fatal/type/bitwise.h>
#include <fatal/type/conditional.h>
#include <fatal/type/find.h>
#include <fatal/type/foreach.h>
#include <fatal/type/list.h>
#include <fatal/type/logical.h>
#include <fatal/type/traits.h>

#include <type_traits>

#include <cassert>

namespace fatal {

/**
 * A type-safe way to represent a set of boolean flags.
 *
 * Assume, in the examples below, that these types are available:
 *
 *  struct my_flag_1 {};
 *  struct my_flag_2 {};
 *  struct my_flag_3 {};
 *  struct my_flag_4 {};
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename... Flags>
struct flag_set {
  static_assert(sizeof...(Flags) <= 64, "no matching integer type");

  /**
   * The list of supported flags.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using tag_list = list<Flags...>;

  /**
   * The integral representation of the flag_set.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  using flags_type = smallest_fast_unsigned_integral<size<tag_list>::value>;

  static_assert(
    size<tag_list>::value <= data_bits<flags_type>::value,
    "too many flags"
  );

  static_assert(
    logical_and<
      std::is_same<Flags, typename std::decay<Flags>::type>...
    >::value,
    "unsupported tag"
  );

private:
  using range_mask = mersenne_number<size<tag_list>::value>;

  template <bool IgnoreUnsupported, typename... UFlags>
  using mask_for = bitwise_or<
    std::integral_constant<flags_type, 0>,
    std::integral_constant<
      typename std::enable_if<
        IgnoreUnsupported || contains<tag_list, UFlags>::value,
        flags_type
      >::type,
      (try_index_of<tag_list, UFlags>::value < size<tag_list>::value
        ? flags_type(1ull << try_index_of<tag_list, UFlags>::value)
        : flags_type(0ull)
      )
    >...
  >;

  template <typename... UFlags>
  struct import_foreign {
    using foreign_set = flag_set<UFlags...>;

    struct visitor {
      template <typename UFlag, std::size_t Index>
      void operator ()(
        indexed<UFlag, Index>,
        foreign_set const &foreign,
        flags_type &out
      ) {
        if (foreign.template test<UFlag>()) {
          out |= mask_for<true, UFlag>::value;
        }
      }
    };

    static flags_type import(foreign_set const &foreign) {
      flags_type flags(0);
      foreach<typename foreign_set::tag_list>(visitor(), foreign, flags);
      assert((flags & range_mask::value) == flags);
      return flags;
    }
  };

public:
  /**
   * Default constructor that start with all flags unset.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  flag_set(): flags_(0) {}

  flag_set(flag_set const &) = default;
  flag_set(flag_set &&) = default;

  /**
   * Constructor that takes a list of flags to be initialized as set.
   *
   * Example:
   *
   *  // will initialize the set with `my_flag_1` and `my_flag_2`
   *  flag_set<my_flag_1, my_flag_2, my_flag_3> s{my_flag_1(), my_flag_2()};
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UFlags, typename = safe_overload<flag_set, UFlags...>>
  explicit flag_set(UFlags &&...):
    flags_(mask_for<false, typename std::decay<UFlags>::type...>::value)
  {}

  /**
   * Initializes this set setting all supported flags that are set in `other`,
   * ignoring the unsupported ones.
   *
   * Example:
   *
   *  flag_set<my_flag_1, my_flag_2, my_flag_3> s{my_flag_1(), my_flag_3()};
   *
   *  // only `my_flag_3` will be initially set since that's the only
   *  // supported flag that's set in `s`.
   *  flag_set<my_flag_2, my_flag_3, my_flag_4> r(s);
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UFlags>
  flag_set(flag_set<UFlags...> const &other):
    flags_(import_foreign<UFlags...>::import(other))
  {}

  //////////
  // test //
  //////////

  /**
   * Tells whether all given flags are set.
   *
   * Example:
   *
   *  flag_set<my_flag_1, my_flag_2, my_flag_3> s{my_flag_1(), my_flag_3()};
   *
   *  // yields `true`
   *  s.test(my_flag_1(), my_flag_3());
   *
   *  // yields `true`
   *  s.test(my_flag_1());
   *
   *  // yields `false`
   *  s.test(my_flag_1(), my_flag_2());
   *
   *  // yields `false`
   *  s.test(my_flag_2());
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UFlags>
  bool test(UFlags &&...) const {
    return test<typename std::decay<UFlags>::type...>();
  }

  /**
   * Tells whether all given flags are set.
   *
   * Example:
   *
   *  flag_set<my_flag_1, my_flag_2, my_flag_3> s{my_flag_1(), my_flag_3()};
   *
   *  // yields `true`
   *  s.test<my_flag_1, my_flag_3>();
   *
   *  // yields `true`
   *  s.test<my_flag_1>();
   *
   *  // yields `false`
   *  s.test<my_flag_1, my_flag_2>();
   *
   *  // yields `false`
   *  s.test<my_flag_2>();
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UFlags>
  bool test() const {
    using mask = mask_for<false, UFlags...>;
    return (flags_ & mask::value) == mask::value;
  }

  /////////
  // set //
  /////////

  /**
   * Sets the given flags to true.
   *
   * Example:
   *
   *  flag_set<my_flag_1, my_flag_2, my_flag_3> s;
   *
   *  // sets `my_flag_1` and `my_flag_2`
   *  s.set(my_flag_1(), my_flag_2());
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UFlags>
  flag_set &set(UFlags &&...) & {
    set<typename std::decay<UFlags>::type...>();
    return *this;
  }

  /**
   * Sets the given flags to true.
   *
   * Example:
   *
   *  // sets `my_flag_1` and `my_flag_2`
   *  auto s = flag_set<my_flag_1, my_flag_2, my_flag_3>()
   *    .set(my_flag_1(), my_flag_2());
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UFlags>
  flag_set &&set(UFlags &&...) && {
    set<typename std::decay<UFlags>::type...>();
    return std::move(*this);
  }

  /**
   * Sets the given flags to true.
   *
   * Example:
   *
   *  flag_set<my_flag_1, my_flag_2, my_flag_3> s;
   *
   *  // sets `my_flag_1` and `my_flag_2`
   *  s.set<my_flag_1, my_flag_2>();
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UFlags>
  flag_set &set() & {
    flags_ |= mask_for<false, UFlags...>::value;
    assert((flags_ & range_mask::value) == flags_);
    return *this;
  }

  /**
   * Sets the given flags to true.
   *
   * Example:
   *
   *  // sets `my_flag_1` and `my_flag_2`
   *  auto s = flag_set<my_flag_1, my_flag_2, my_flag_3>()
   *    .set<my_flag_1, my_flag_2>();
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UFlags>
  flag_set &&set() && {
    flags_ |= mask_for<false, UFlags...>::value;
    assert((flags_ & range_mask::value) == flags_);
    return std::move(*this);
  }

  ////////////
  // set_if //
  ////////////

  /**
   * Sets the `UFlag` flag to true if the `condition` is true,
   * otherwise leave it unchanged.
   *
   * Example:
   *
   *  flag_set<my_flag_1, my_flag_2, my_flag_3> s;
   *
   *  // sets `my_flag_1`
   *  s.set_if<my_flag_1>(true);
   *
   *  // leaves `my_flag_2` unchanged
   *  s.set_if<my_flag_2>(false);
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename UFlag>
  flag_set &set_if(bool condition) & {
    if (condition) {
      set<UFlag>();
    }

    return *this;
  }

  /**
   * Sets the `UFlag` flag  to true if the `condition` is true,
   * otherwise leave it unchanged.
   *
   * Example:
   *
   *  // sets `my_flag_1` but leaves `my_flag_2` unchanged
   *  auto s = flag_set<my_flag_1, my_flag_2, my_flag_3>()
   *    .set_if<my_flag_1>(true)
   *    .set_if<my_flag_2>(false);
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename UFlag>
  flag_set &&set_if(bool condition) && {
    if (condition) {
      set<UFlag>();
    }

    return std::move(*this);
  }

  ///////////
  // reset //
  ///////////

  /**
   * Resets the given flags to false.
   *
   * Example:
   *
   *  flag_set<my_flag_1, my_flag_2, my_flag_3> s;
   *
   *  // resets `my_flag_1` and `my_flag_2`
   *  s.reset(my_flag_1(), my_flag_2());
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UFlags>
  flag_set &reset(UFlags &&...) & {
    reset<typename std::decay<UFlags>::type...>();
    return *this;
  }

  /**
   * Resets the given flags to false.
   *
   * Example:
   *
   *  // resets `my_flag_1` and `my_flag_2`
   *  auto s = flag_set<my_flag_1, my_flag_2, my_flag_3>()
   *    .reset(my_flag_1(), my_flag_2());
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UFlags>
  flag_set &&reset(UFlags &&...) && {
    reset<typename std::decay<UFlags>::type...>();
    return std::move(*this);
  }

  /**
   * Resets the given flags to false.
   *
   * Example:
   *
   *  flag_set<my_flag_1, my_flag_2, my_flag_3> s;
   *
   *  // resets `my_flag_1` and `my_flag_2`
   *  s.reset<my_flag_1, my_flag_2>();
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UFlags>
  flag_set &reset() & {
    flags_ &= ~mask_for<false, UFlags...>::value;
    assert((flags_ & range_mask::value) == flags_);
    return *this;
  }

  /**
   * Resets the given flags to false.
   *
   * Example:
   *
   *  // resets `my_flag_1` and `my_flag_2`
   *  auto s = flag_set<my_flag_1, my_flag_2, my_flag_3>()
   *    .reset<my_flag_1, my_flag_2>();
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UFlags>
  flag_set &&reset() && {
    flags_ &= ~mask_for<false, UFlags...>::value;
    assert((flags_ & range_mask::value) == flags_);
    return std::move(*this);
  }

  //////////////
  // reset_if //
  //////////////

  /**
   * Resets the `UFlag` flag to false if the `condition` is true,
   * otherwise leave it unchanged.
   *
   * Example:
   *
   *  flag_set<my_flag_1, my_flag_2, my_flag_3> s;
   *
   *  // resets `my_flag_1`
   *  s.reset_if<my_flag_1>(true);
   *
   *  // leaves `my_flag_2` unchanged
   *  s.reset_if<my_flag_2>(false);
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename UFlag>
  flag_set &reset_if(bool condition) & {
    if (condition) {
      reset<UFlag>();
    }

    return *this;
  }

  /**
   * Resets the `UFlag` flag to false if the `condition` is true,
   * otherwise leave it unchanged.
   *
   * Example:
   *
   *  // resets `my_flag_1` but leaves `my_flag_2` unchanged
   *  auto s = flag_set<my_flag_1, my_flag_2, my_flag_3>()
   *    .reset_if<my_flag_1>(true)
   *    .reset_if<my_flag_2>(false);
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename UFlag>
  flag_set &&reset_if(bool condition) && {
    if (condition) {
      reset<UFlag>();
    }

    return std::move(*this);
  }

  //////////
  // flip //
  //////////

  /**
   * Flips the given flags to the opposite of its current value.
   *
   * Example:
   *
   *  flag_set<my_flag_1, my_flag_2, my_flag_3> s;
   *
   *  // flips `my_flag_1` and `my_flag_2`
   *  s.flip(my_flag_1(), my_flag_2());
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UFlags>
  flag_set &flip(UFlags &&...) & {
    flip<typename std::decay<UFlags>::type...>();
    return *this;
  }

  /**
   * Flips the given flags to the opposite of its current value.
   *
   * Example:
   *
   *  // flips `my_flag_1` and `my_flag_2`
   *  auto s = flag_set<my_flag_1, my_flag_2, my_flag_3>()
   *    .flip(my_flag_1(), my_flag_2());
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UFlags>
  flag_set &&flip(UFlags &&...) && {
    flip<typename std::decay<UFlags>::type...>();
    return std::move(*this);
  }

  /**
   * Flips the given flags to the opposite of its current value.
   *
   * Example:
   *
   *  flag_set<my_flag_1, my_flag_2, my_flag_3> s;
   *
   *  // flips `my_flag_1` and `my_flag_2`
   *  s.flip<my_flag_1, my_flag_2>();
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UFlags>
  flag_set &flip() & {
    flags_ ^= mask_for<false, UFlags...>::value;
    assert((flags_ & range_mask::value) == flags_);
    return *this;
  }

  /**
   * Flips the given flags to the opposite of its current value.
   *
   * Example:
   *
   *  // flips `my_flag_1` and `my_flag_2`
   *  auto s = flag_set<my_flag_1, my_flag_2, my_flag_3>()
   *    .flip<my_flag_1, my_flag_2>();
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UFlags>
  flag_set &&flip() && {
    flags_ ^= mask_for<false, UFlags...>::value;
    assert((flags_ & range_mask::value) == flags_);
    return std::move(*this);
  }

  /////////////
  // flip_if //
  /////////////

  /**
   * Flips the `UFlag` flag to the opposite of its current value if
   * the `condition` is true, otherwise leave it unchanged.
   *
   * Example:
   *
   *  flag_set<my_flag_1, my_flag_2, my_flag_3> s;
   *
   *  // flips `my_flag_1`
   *  s.flip_if<my_flag_1>(true);
   *
   *  // leaves `my_flag_2` unchanged
   *  s.flip_if<my_flag_2>(false);
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename UFlag>
  flag_set &flip_if(bool condition) & {
    if (condition) {
      flip<UFlag>();
    }

    return *this;
  }

  /**
   * Flips the `UFlag` flag to the opposite of its current value if
   * the `condition` is true, otherwise leave it unchanged.
   *
   * Example:
   *
   *  // flips `my_flag_1` but leaves `my_flag_2` unchanged
   *  auto s = flag_set<my_flag_1, my_flag_2, my_flag_3>()
   *    .flip_if<my_flag_1>(true)
   *    .flip_if<my_flag_2>(false);
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename UFlag>
  flag_set &&flip_if(bool condition) && {
    if (condition) {
      flip<UFlag>();
    }

    return std::move(*this);
  }

  ///////////
  // clear //
  ///////////

  /**
   * Unsets all the flags in this `flag_set`.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  void clear() { flags_ = 0; }

  ////////////
  // expand //
  ////////////

  /**
   * Adds `UFlag` to the list of supported flags (in which case it returns a
   * new type), if not supported already (in which case it returns the same
   * type).
   *
   * This is the type returned by `expand()`.
   *
   *  // yields `flag_set<my_flag_1>`
   *  using result0 = flag_set<>::expanded<my_flag_1>;
   *
   *  using my_set = flag_set<my_flag_2, my_flag_3>;
   *
   *  // yields `flag_set<my_flag_2, my_flag_3, my_flag_4>`
   *  using result1 = my_set::expanded<my_flag_4>;
   *
   *  // yields `flag_set<my_flag_2, my_flag_3, my_flag_1>`
   *  using result2 = my_set::expanded<my_flag_1>;
   *
   *  // yields `flag_set<my_flag_2, my_flag_3>`
   *  using result3 = my_set::expanded<my_flag_2>;
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename UFlag>
  using expanded = conditional<
    contains<tag_list, UFlag>::value,
    flag_set,
    apply_to<tag_list, fatal::flag_set, UFlag>
  >;

  /**
   * Expands the list of supported flags with `UFlag` and sets is in addition
   * to what's already set.
   *
   * Returns the result as a new set, leaving this set unchanged.
   *
   * Refer to `expanded` for more details.
   *
   * Example:
   *
   *  flag_set<> e;
   *
   *  // yields `flag_set<my_flag_1>` with `my_flag_1` set
   *  auto s0 = e.expand<my_flag_1>();
   *
   *  // yields `flag_set<my_flag_1>` with `my_flag_1` set
   *  auto s1 = flag_set<>().expand<my_flag_1>();
   *
   *  flag_set<my_flag_2, my_flag_3> s{my_flag_2()};
   *
   *  // yields `flag_set<my_flag_2, my_flag_3, my_flag_4>`
   *  // with `my_flag_2` and `my_flag_4` set
   *  using result0 = s.expand<my_flag_4>();
   *
   *  // yields `flag_set<my_flag_2, my_flag_3, my_flag_1>`
   *  // with `my_flag_2` and `my_flag_1` set
   *  using result1 = s.expand<my_flag_1>();
   *
   *  // yields `flag_set<my_flag_2, my_flag_3>` with `my_flag_2`
   *  using result2 = s.expand<my_flag_2>();
   *
   *  // yields `flag_set<my_flag_2, my_flag_3>`
   *  // with `my_flag_2` and `my_flag_3` set
   *  using result3 = s.expand<my_flag_3>();
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename UFlag>
  expanded<UFlag> expand() const {
    return expanded<UFlag>(*this).template set<UFlag>();
  }

  ///////////////
  // expand_if //
  ///////////////

  /**
   * Expands the list of supported flags with `UFlag` and leaves set whatever
   * was already set. If the condition is `true`, also sets `UFlag`.
   *
   * Returns the result as a new set, leaving this set unchanged.
   *
   * Refer to `expanded` and `expand` for more details.
   *
   * Example:
   *
   *  flag_set<> e;
   *
   *  // yields `flag_set<my_flag_1>` with `my_flag_1` set
   *  auto s0 = e.expand_if<my_flag_1>(true);
   *
   *  // yields `flag_set<my_flag_1>` with nothing set
   *  auto s1 = e.expand_if<my_flag_1>(false);
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename UFlag>
  expanded<UFlag> expand_if(bool condition) const {
    return expanded<UFlag>(*this).template set_if<UFlag>(condition);
  }

  /////////
  // get //
  /////////

  /**
   * Gets the integral representation of this `flag_set`.
   *
   * The supported flags are set from the lowest to the highest significant bit.
   *
   * Example:
   *
   *  // yields `0b111`
   *  flag_set<my_flag_1, my_flag_2, my_flag_3>{
   *    my_flag_1(), my_flag_3(), my_flag_3()
   *  }.get();
   *
   *  // yields `0b101`
   *  flag_set<my_flag_1, my_flag_2, my_flag_3>{my_flag_1(), my_flag_3()}.get();
   *
   *  // yields `0b001`
   *  flag_set<my_flag_1, my_flag_2, my_flag_3>{my_flag_1()}.get();
   *
   *  // yields `0b100`
   *  flag_set<my_flag_1, my_flag_2, my_flag_3>{ my_flag_3()}.get();
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  fast_pass<flags_type> get() const { return flags_; }

  ////////////
  // equals //
  ////////////
  
  /**
   * Checks if all given flags are set and none other.
   *
   * Example:
   *
   *  using flags_type = flag_set<my_flag_1, my_flag_2, my_flag_3>;
   *
   *  auto flags = flags_type().set<my_flag_1, my_flag_2>();
   *
   *  // yields `true`
   *  auto result1 = flags.equals<my_flag_1, my_flag_2>();
   *
   *  // yields `false`
   *  auto result2 = flags.equals<my_flag_1>();
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UFlags>
  bool equals() const {
    return flags_ == mask_for<false, UFlags...>::value;
  }

  ////////////////
  // operator = //
  ////////////////

  /**
   * Assignment operator. Sets this `flag_set` with exactly what's set in `rhs`.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  flag_set &operator =(fast_pass<flag_set> const &rhs) {
    flags_ = rhs.flags_;
    assert((flags_ & range_mask::value) == flags_);

    return *this;
  }

  /**
   * Assignment operator. Sets this `flag_set` with exactly what's set in `rhs`,
   * ignoring unsupported types.
   *
   * @author: Marcelo Juchem <marcelo@fb.com>
   */
  template <typename... UFlags>
  flag_set &operator =(flag_set<UFlags...> const &rhs) {
    flags_ = import_foreign<UFlags...>::import(rhs);
    return *this;
  }

private:
  flags_type flags_;
};

} // namespace fatal {
