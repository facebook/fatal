/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/variant_traits.h>

#include <fatal/type/call_traits.h>

#include <fatal/test/driver.h>

#include <memory>
#include <type_traits>
#include <utility>

namespace fatal {

class poor_mans_variant {
  union storage {
    int i;
    double d;
    bool b;
  };

public:
  enum class id { empty, i, d, b };
  id get_id() const { return id_; }

  int get_i() const & { return storage_.i; }
  double get_d() const & { return storage_.d; }
  bool get_b() const & { return storage_.b; }

  int &get_i() & { return storage_.i; }
  double &get_d() & { return storage_.d; }
  bool &get_b() & { return storage_.b; }

  int &&get_i() && { return std::move(storage_.i); }
  double &&get_d() && { return std::move(storage_.d); }
  bool &&get_b() && { return std::move(storage_.b); }

  void set_i(int i) {
    id_ = id::i;
    storage_.i = i;
  }

  void set_d(double d) {
    id_ = id::d;
    storage_.d = d;
  }

  void set_b(bool b) {
    id_ = id::b;
    storage_.b = b;
  }

  bool empty() const { return id_ == id::empty; }
  void clear() { id_ = id::empty; }

  storage const *operator ->() const { return std::addressof(storage_); }

private:
  id id_ = id::empty;
  storage storage_;
};

FATAL_S(poor_mans_variant_name, "poor_mans_variant");

class poor_mans_variant_traits {
  struct get {
    FATAL_CALL_TRAITS(i, get_i);
    FATAL_CALL_TRAITS(d, get_d);
    FATAL_CALL_TRAITS(b, get_b);
  };

  struct set {
    FATAL_CALL_TRAITS(i, set_i);
    FATAL_CALL_TRAITS(d, set_d);
    FATAL_CALL_TRAITS(b, set_b);
  };

public:
  using type = poor_mans_variant;
  using id = type::id;
  using name = poor_mans_variant_name;

  struct ids {
    using i = std::integral_constant<id, id::i>;
    using d = std::integral_constant<id, id::d>;
    using b = std::integral_constant<id, id::b>;
  };

  using descriptors = list<
    variant_member_descriptor<
      int,
      ids::i,
      get::i::member_function,
      set::i::member_function
    >,
    variant_member_descriptor<
      double,
      ids::d,
      get::d::member_function,
      set::d::member_function
    >,
    variant_member_descriptor<
      bool,
      ids::b,
      get::b::member_function,
      set::b::member_function
    >
  >;

  static id get_id(type const &variant) { return variant.get_id(); }
  static bool empty(type const &variant) { return variant.empty(); }
  static void clear(type &variant) { variant.clear(); }
};

struct poor_mans_variant_metadata {};

FATAL_REGISTER_VARIANT_TRAITS(
  poor_mans_variant_traits,
  poor_mans_variant_metadata
);

FATAL_TEST(variant_traits, has_variant_traits) {
  FATAL_EXPECT_SAME<std::true_type, has_variant_traits<poor_mans_variant>>();
  FATAL_EXPECT_SAME<std::false_type, has_variant_traits<void>>();
}

FATAL_TEST(poor_mans_variant, types) {
  using type = poor_mans_variant;
  using ids = poor_mans_variant_traits::ids;
  using traits = variant_traits<type>;

  FATAL_EXPECT_SAME<type, traits::type>();
  FATAL_EXPECT_SAME<poor_mans_variant_name, traits::name>();
  FATAL_EXPECT_SAME<type::id, traits::id>();
  FATAL_EXPECT_SAME<poor_mans_variant_metadata, traits::metadata>();

  FATAL_EXPECT_SAME<ids::i, traits::ids::i>();
  FATAL_EXPECT_SAME<ids::d, traits::ids::d>();
  FATAL_EXPECT_SAME<ids::b, traits::ids::b>();

  FATAL_EXPECT_SAME<
    poor_mans_variant_traits::descriptors,
    traits::descriptors
  >();
}

FATAL_TEST(poor_mans_variant, functions) {
  using type = poor_mans_variant;
  using id = poor_mans_variant::id;
  using traits = variant_traits<type>;

  type v;
  type const &c = v;
  type &&r = std::move(v);

  FATAL_EXPECT_TRUE(traits::empty(v));
  FATAL_EXPECT_TRUE(traits::empty(c));
  FATAL_EXPECT_TRUE(traits::empty(r));
  FATAL_EXPECT_EQ(id::empty, traits::get_id(v));
  FATAL_EXPECT_EQ(id::empty, traits::get_id(c));
  FATAL_EXPECT_EQ(id::empty, traits::get_id(r));

  v.set_i(10);
  FATAL_EXPECT_FALSE(traits::empty(v));
  FATAL_EXPECT_FALSE(traits::empty(c));
  FATAL_EXPECT_FALSE(traits::empty(r));
  FATAL_EXPECT_EQ(id::i, traits::get_id(v));
  FATAL_EXPECT_EQ(id::i, traits::get_id(c));
  FATAL_EXPECT_EQ(id::i, traits::get_id(r));

  v.set_d(5.6);
  FATAL_EXPECT_FALSE(traits::empty(v));
  FATAL_EXPECT_FALSE(traits::empty(c));
  FATAL_EXPECT_FALSE(traits::empty(r));
  FATAL_EXPECT_EQ(id::d, traits::get_id(v));
  FATAL_EXPECT_EQ(id::d, traits::get_id(c));
  FATAL_EXPECT_EQ(id::d, traits::get_id(r));

  v.set_b(true);
  FATAL_EXPECT_FALSE(traits::empty(v));
  FATAL_EXPECT_FALSE(traits::empty(c));
  FATAL_EXPECT_FALSE(traits::empty(r));
  FATAL_EXPECT_EQ(id::b, traits::get_id(v));
  FATAL_EXPECT_EQ(id::b, traits::get_id(c));
  FATAL_EXPECT_EQ(id::b, traits::get_id(r));

  traits::clear(v);
  FATAL_EXPECT_TRUE(traits::empty(v));
  FATAL_EXPECT_TRUE(traits::empty(c));
  FATAL_EXPECT_TRUE(traits::empty(r));
  FATAL_EXPECT_EQ(id::empty, traits::get_id(v));
  FATAL_EXPECT_EQ(id::empty, traits::get_id(c));
  FATAL_EXPECT_EQ(id::empty, traits::get_id(r));
}

FATAL_TEST(poor_mans_variant, by_id) {
  using type = poor_mans_variant;
  using ids = poor_mans_variant_traits::ids;
  using traits = variant_traits<type>::by_id;

  FATAL_EXPECT_SAME<ids::i, traits::id<ids::i>>();
  FATAL_EXPECT_SAME<ids::d, traits::id<ids::d>>();
  FATAL_EXPECT_SAME<ids::b, traits::id<ids::b>>();

  FATAL_EXPECT_SAME<int, traits::type<ids::i>>();
  FATAL_EXPECT_SAME<double, traits::type<ids::d>>();
  FATAL_EXPECT_SAME<bool, traits::type<ids::b>>();

  type v;
  type const &c = v;
  type &&r = std::move(v);

  v.set_i(10);
  FATAL_EXPECT_EQ(10, traits::get<ids::i>(v));
  FATAL_EXPECT_EQ(10, traits::get<ids::i>(c));
  FATAL_EXPECT_EQ(10, traits::get<ids::i>(r));

  v.set_d(5.6);
  FATAL_EXPECT_EQ(5.6, traits::get<ids::d>(v));
  FATAL_EXPECT_EQ(5.6, traits::get<ids::d>(c));
  FATAL_EXPECT_EQ(5.6, traits::get<ids::d>(r));

  v.set_b(true);
  FATAL_EXPECT_EQ(true, traits::get<ids::b>(v));
  FATAL_EXPECT_EQ(true, traits::get<ids::b>(c));
  FATAL_EXPECT_EQ(true, traits::get<ids::b>(r));

  traits::set<ids::i>(v, 97);
  FATAL_EXPECT_EQ(97, v->i);
  FATAL_EXPECT_EQ(97, c->i);
  FATAL_EXPECT_EQ(97, r->i);

  traits::set<ids::d>(v, 7.2);
  FATAL_EXPECT_EQ(7.2, v->d);
  FATAL_EXPECT_EQ(7.2, c->d);
  FATAL_EXPECT_EQ(7.2, r->d);

  traits::set<ids::b>(v, false);
  FATAL_EXPECT_EQ(false, v->b);
  FATAL_EXPECT_EQ(false, c->b);
  FATAL_EXPECT_EQ(false, r->b);
}

FATAL_TEST(poor_mans_variant, by_type) {
  using type = poor_mans_variant;
  using ids = poor_mans_variant_traits::ids;
  using traits = variant_traits<type>::by_type;

  FATAL_EXPECT_SAME<ids::i, traits::id<int>>();
  FATAL_EXPECT_SAME<ids::d, traits::id<double>>();
  FATAL_EXPECT_SAME<ids::b, traits::id<bool>>();

  FATAL_EXPECT_SAME<int, traits::type<int>>();
  FATAL_EXPECT_SAME<double, traits::type<double>>();
  FATAL_EXPECT_SAME<bool, traits::type<bool>>();

  type v;
  type const &c = v;
  type &&r = std::move(v);

  v.set_i(10);
  FATAL_EXPECT_EQ(10, traits::get<int>(v));
  FATAL_EXPECT_EQ(10, traits::get<int>(c));
  FATAL_EXPECT_EQ(10, traits::get<int>(r));

  v.set_d(5.6);
  FATAL_EXPECT_EQ(5.6, traits::get<double>(v));
  FATAL_EXPECT_EQ(5.6, traits::get<double>(c));
  FATAL_EXPECT_EQ(5.6, traits::get<double>(r));

  v.set_b(true);
  FATAL_EXPECT_EQ(true, traits::get<bool>(v));
  FATAL_EXPECT_EQ(true, traits::get<bool>(c));
  FATAL_EXPECT_EQ(true, traits::get<bool>(r));

  traits::set<int>(v, 97);
  FATAL_EXPECT_EQ(97, v->i);
  FATAL_EXPECT_EQ(97, c->i);
  FATAL_EXPECT_EQ(97, r->i);

  traits::set<double>(v, 7.2);
  FATAL_EXPECT_EQ(7.2, v->d);
  FATAL_EXPECT_EQ(7.2, c->d);
  FATAL_EXPECT_EQ(7.2, r->d);

  traits::set<bool>(v, false);
  FATAL_EXPECT_EQ(false, v->b);
  FATAL_EXPECT_EQ(false, c->b);
  FATAL_EXPECT_EQ(false, r->b);
}

// TODO: empty
// TODO: clear

} // namespace fatal {
