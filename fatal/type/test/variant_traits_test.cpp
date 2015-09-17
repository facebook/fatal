/*
 *  Copyright (c) 2015, Facebook, Inc.
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
  enum class id { i, d, b, empty };

  using id_i = std::integral_constant<id, id::i>;
  using id_d = std::integral_constant<id, id::d>;
  using id_b = std::integral_constant<id, id::b>;

  int get_i() const & { return storage_.i; }
  double get_d() const & { return storage_.d; }
  bool get_b() const & { return storage_.b; }

  int &get_i() & { return storage_.i; }
  double &get_d() & { return storage_.d; }
  bool &get_b() & { return storage_.b; }

  int &&get_i() && { return std::move(storage_.i); }
  double &&get_d() && { return std::move(storage_.d); }
  bool &&get_b() && { return std::move(storage_.b); }

  void set_i(int i) { storage_.i = i; }
  void set_d(double d) { storage_.d = d; }
  void set_b(bool b) { storage_.b = b; }

  bool empty() const { return id_ == id::empty; }
  void clear() { id_ = id::empty; }

  storage const *operator ->() const { return std::addressof(storage_); }

private:
  id id_ = id::empty;
  storage storage_;
};

struct str {
  FATAL_STR(i, "i");
  FATAL_STR(d, "d");
  FATAL_STR(b, "b");
};

class poor_mans_variant_traits {
  struct names {
    using i = str::i;
    using d = str::d;
    using b = str::b;
  };

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


  using descriptors = fatal::type_list<
    variant_type_descriptor<
      int,
      std::integral_constant<id, id::i>,
      names::i,
      get::i::member_function,
      set::i::member_function
    >,
    variant_type_descriptor<
      double,
      std::integral_constant<id, id::d>,
      names::d,
      get::d::member_function,
      set::d::member_function
    >,
    variant_type_descriptor<
      bool,
      std::integral_constant<id, id::b>,
      names::b,
      get::b::member_function,
      set::b::member_function
    >
  >;
};

FATAL_REGISTER_VARIANT_TRAITS(poor_mans_variant_traits);

FATAL_TEST(poor_mans_variant, types) {
  using type = poor_mans_variant;
  using traits = variant_traits<type>;

  FATAL_EXPECT_SAME<type, traits::type>();
  FATAL_EXPECT_SAME<type::id, traits::id>();

  FATAL_EXPECT_SAME<
    poor_mans_variant_traits::descriptors,
    traits::descriptors
  >();
}

FATAL_TEST(poor_mans_variant, by_name) {
  using type = poor_mans_variant;
  using traits = variant_traits<type>::by_name;

  FATAL_EXPECT_SAME<type_list<str::i, str::d, str::b>, traits::tags>();

  FATAL_EXPECT_SAME<int, traits::type<str::i>>();
  FATAL_EXPECT_SAME<double, traits::type<str::d>>();
  FATAL_EXPECT_SAME<bool, traits::type<str::b>>();

  FATAL_EXPECT_SAME<type::id_i, traits::id<str::i>>();
  FATAL_EXPECT_SAME<type::id_d, traits::id<str::d>>();
  FATAL_EXPECT_SAME<type::id_b, traits::id<str::b>>();

  type v;
  type const &c = v;
  type &&r = std::move(v);

  v.set_i(10);
  FATAL_EXPECT_EQ(10, traits::get<str::i>(v));
  FATAL_EXPECT_EQ(10, traits::get<str::i>(c));
  FATAL_EXPECT_EQ(10, traits::get<str::i>(r));

  v.set_d(5.6);
  FATAL_EXPECT_EQ(5.6, traits::get<str::d>(v));
  FATAL_EXPECT_EQ(5.6, traits::get<str::d>(c));
  FATAL_EXPECT_EQ(5.6, traits::get<str::d>(r));

  v.set_b(true);
  FATAL_EXPECT_EQ(true, traits::get<str::b>(v));
  FATAL_EXPECT_EQ(true, traits::get<str::b>(c));
  FATAL_EXPECT_EQ(true, traits::get<str::b>(r));

  traits::set<str::i>(v, 97);
  FATAL_EXPECT_EQ(97, v->i);
  FATAL_EXPECT_EQ(97, c->i);
  FATAL_EXPECT_EQ(97, r->i);

  traits::set<str::d>(v, 7.2);
  FATAL_EXPECT_EQ(7.2, v->d);
  FATAL_EXPECT_EQ(7.2, c->d);
  FATAL_EXPECT_EQ(7.2, r->d);

  traits::set<str::b>(v, false);
  FATAL_EXPECT_EQ(false, v->b);
  FATAL_EXPECT_EQ(false, c->b);
  FATAL_EXPECT_EQ(false, r->b);
}

FATAL_TEST(poor_mans_variant, by_id) {
  using type = poor_mans_variant;
  using traits = variant_traits<type>::by_id;

  FATAL_EXPECT_SAME<
    type_list<type::id_i, type::id_d, type::id_b>,
    traits::tags
  >();

  FATAL_EXPECT_SAME<int, traits::type<type::id_i>>();
  FATAL_EXPECT_SAME<double, traits::type<type::id_d>>();
  FATAL_EXPECT_SAME<bool, traits::type<type::id_b>>();

  FATAL_EXPECT_SAME<type::id_i, traits::id<type::id_i>>();
  FATAL_EXPECT_SAME<type::id_d, traits::id<type::id_d>>();
  FATAL_EXPECT_SAME<type::id_b, traits::id<type::id_b>>();

  type v;
  type const &c = v;
  type &&r = std::move(v);

  v.set_i(10);
  FATAL_EXPECT_EQ(10, traits::get<type::id_i>(v));
  FATAL_EXPECT_EQ(10, traits::get<type::id_i>(c));
  FATAL_EXPECT_EQ(10, traits::get<type::id_i>(r));

  v.set_d(5.6);
  FATAL_EXPECT_EQ(5.6, traits::get<type::id_d>(v));
  FATAL_EXPECT_EQ(5.6, traits::get<type::id_d>(c));
  FATAL_EXPECT_EQ(5.6, traits::get<type::id_d>(r));

  v.set_b(true);
  FATAL_EXPECT_EQ(true, traits::get<type::id_b>(v));
  FATAL_EXPECT_EQ(true, traits::get<type::id_b>(c));
  FATAL_EXPECT_EQ(true, traits::get<type::id_b>(r));

  traits::set<type::id_i>(v, 97);
  FATAL_EXPECT_EQ(97, v->i);
  FATAL_EXPECT_EQ(97, c->i);
  FATAL_EXPECT_EQ(97, r->i);

  traits::set<type::id_d>(v, 7.2);
  FATAL_EXPECT_EQ(7.2, v->d);
  FATAL_EXPECT_EQ(7.2, c->d);
  FATAL_EXPECT_EQ(7.2, r->d);

  traits::set<type::id_b>(v, false);
  FATAL_EXPECT_EQ(false, v->b);
  FATAL_EXPECT_EQ(false, c->b);
  FATAL_EXPECT_EQ(false, r->b);
}

FATAL_TEST(poor_mans_variant, by_type) {
  using type = poor_mans_variant;
  using traits = variant_traits<type>::by_type;

  FATAL_EXPECT_SAME<type_list<int, double, bool>, traits::tags>();

  FATAL_EXPECT_SAME<int, traits::type<int>>();
  FATAL_EXPECT_SAME<double, traits::type<double>>();
  FATAL_EXPECT_SAME<bool, traits::type<bool>>();

  FATAL_EXPECT_SAME<type::id_i, traits::id<int>>();
  FATAL_EXPECT_SAME<type::id_d, traits::id<double>>();
  FATAL_EXPECT_SAME<type::id_b, traits::id<bool>>();

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
