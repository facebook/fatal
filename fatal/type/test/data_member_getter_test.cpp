/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/data_member_getter.h>

#include <fatal/test/driver.h>

#include <string>
#include <vector>
#include <utility>
#include <type_traits>
#include <memory>

// TODO: ADD TESTS FOR try_get AND try_getter

namespace fatal {

//////////////////////////////
// FATAL_DATA_MEMBER_GETTER //
//////////////////////////////

namespace data_member_getter_test {

struct data {
  data(
    int i_, std::string const &scl_, long &&lr_, double const dc_,
    bool &bl_, std::vector<short> v_, float const &&fcr_
  ):
    i(i_), scl(scl_), lr(std::move(lr_)), dc(dc_),
    bl(bl_), v(v_), fcr(std::move(fcr_))
  {}

  data(data const &rhs):
    i(rhs.i), scl(rhs.scl), lr(std::move(rhs.lr)), dc(rhs.dc),
    bl(rhs.bl), v(rhs.v), fcr(std::move(rhs.fcr))
  {}

  int i;
  std::string const &scl;
  long &&lr;
  double const dc;
  bool &bl;
  std::vector<short> v;
  float const &&fcr;

  bool operator ==(data const &rhs) const {
    return i == rhs.i
      && scl == rhs.scl
      && lr == rhs.lr
      && dc == rhs.dc
      && bl == rhs.bl
      && v == rhs.v
      && fcr == rhs.fcr;
  }
};

struct getter {
# define TEST_IMPL(...) \
  FATAL_DATA_MEMBER_GETTER(__VA_ARGS__, __VA_ARGS__)

  TEST_IMPL(bl);
  TEST_IMPL(cl);
  TEST_IMPL(cr);
  TEST_IMPL(d);
  TEST_IMPL(dc);
  TEST_IMPL(fcr);
  TEST_IMPL(i);
  TEST_IMPL(l);
  TEST_IMPL(lr);
  TEST_IMPL(r);
  TEST_IMPL(scl);
  TEST_IMPL(v);
  TEST_IMPL(non_existent1);
  TEST_IMPL(non_existent2);
  TEST_IMPL(non_existent3);

# undef TEST_IMPL
};

struct getter_name {
# define TEST_IMPL(...) \
  FATAL_S(__VA_ARGS__, FATAL_TO_STR(__VA_ARGS__))

  TEST_IMPL(i);
  TEST_IMPL(scl);
  TEST_IMPL(lr);
  TEST_IMPL(dc);
  TEST_IMPL(bl);
  TEST_IMPL(v);
  TEST_IMPL(fcr);

# undef TEST_IMPL
};

struct ns_tricky {
  double std;
  ::std::string desc;
};

struct ns_tricky_getter {
  FATAL_DATA_MEMBER_GETTER(std, std);
  FATAL_DATA_MEMBER_GETTER(desc, desc);
};

} // namespace data_member_getter_test {

FATAL_TEST(data_member_getter, name) {
# define TEST_IMPL(Field, ...) \
  do { \
    using getter = data_member_getter_test::getter::Field; \
    \
    FATAL_EXPECT_SAME< \
      data_member_getter_test::getter_name::Field, \
      getter::name \
    >(); \
  } while (false)

  TEST_IMPL(i);
  TEST_IMPL(scl);
  TEST_IMPL(lr);
  TEST_IMPL(dc);
  TEST_IMPL(bl);
  TEST_IMPL(v);
  TEST_IMPL(fcr);

# undef TEST_IMPL
}

FATAL_TEST(data_member_getter, type) {
# define TEST_IMPL(Owner, Field, ...) \
  do { \
    using getter = data_member_getter_test::getter::Field; \
    using data = data_member_getter_test::data; \
    using chained_getter = chained_data_member_getter< \
      data_member_getter_test::getter::Field \
    >; \
    \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::type<Owner>>(); \
    FATAL_EXPECT_SAME<__VA_ARGS__, chained_getter::type<Owner>>(); \
  } while (false)

  TEST_IMPL(data, i, int);
  TEST_IMPL(data, scl, std::string const &);
  TEST_IMPL(data, lr, long &&);
  TEST_IMPL(data, dc, double const);
  TEST_IMPL(data, bl, bool &);
  TEST_IMPL(data, v, std::vector<short>);
  TEST_IMPL(data, fcr, float const &&);

  TEST_IMPL(data &, i, int);
  TEST_IMPL(data &, scl, std::string const &);
  TEST_IMPL(data &, lr, long &&);
  TEST_IMPL(data &, dc, double const);
  TEST_IMPL(data &, bl, bool &);
  TEST_IMPL(data &, v, std::vector<short>);
  TEST_IMPL(data &, fcr, float const &&);

  TEST_IMPL(data &&, i, int);
  TEST_IMPL(data &&, scl, std::string const &);
  TEST_IMPL(data &&, lr, long &&);
  TEST_IMPL(data &&, dc, double const);
  TEST_IMPL(data &&, bl, bool &);
  TEST_IMPL(data &&, v, std::vector<short>);
  TEST_IMPL(data &&, fcr, float const &&);

  TEST_IMPL(data const, i, int);
  TEST_IMPL(data const, scl, std::string const &);
  TEST_IMPL(data const, lr, long &&);
  TEST_IMPL(data const, dc, double const);
  TEST_IMPL(data const, bl, bool &);
  TEST_IMPL(data const, v, std::vector<short>);
  TEST_IMPL(data const, fcr, float const &&);

  TEST_IMPL(data const &, i, int);
  TEST_IMPL(data const &, scl, std::string const &);
  TEST_IMPL(data const &, lr, long &&);
  TEST_IMPL(data const &, dc, double const);
  TEST_IMPL(data const &, bl, bool &);
  TEST_IMPL(data const &, v, std::vector<short>);
  TEST_IMPL(data const &, fcr, float const &&);

  TEST_IMPL(data const &&, i, int);
  TEST_IMPL(data const &&, scl, std::string const &);
  TEST_IMPL(data const &&, lr, long &&);
  TEST_IMPL(data const &&, dc, double const);
  TEST_IMPL(data const &&, bl, bool &);
  TEST_IMPL(data const &&, v, std::vector<short>);
  TEST_IMPL(data const &&, fcr, float const &&);

# undef TEST_IMPL
}

FATAL_TEST(data_member_getter, has) {
# define TEST_IMPL(Owner, Field, Expected) \
  do { \
    using getter = data_member_getter_test::getter::Field; \
    using data = data_member_getter_test::data; \
    \
    FATAL_EXPECT_EQ(Expected, getter::has<Owner>::value); \
  } while (false)
  // TODO: for when chained_data_member_getter::has is implemented
  // using chained_getter = chained_data_member_getter<
  //   data_member_getter_test::getter::Field
  // >;
  // FATAL_EXPECT_EQ(Expected, chained_getter::has<Owner>::value);

  TEST_IMPL(data, i, true);
  TEST_IMPL(data, scl, true);
  TEST_IMPL(data, lr, true);
  TEST_IMPL(data, dc, true);
  TEST_IMPL(data, bl, true);
  TEST_IMPL(data, v, true);
  TEST_IMPL(data, fcr, true);
  TEST_IMPL(data, non_existent2, false);
  TEST_IMPL(data, non_existent2, false);
  TEST_IMPL(data, non_existent3, false);

  TEST_IMPL(data &, i, true);
  TEST_IMPL(data &, scl, true);
  TEST_IMPL(data &, lr, true);
  TEST_IMPL(data &, dc, true);
  TEST_IMPL(data &, bl, true);
  TEST_IMPL(data &, v, true);
  TEST_IMPL(data &, fcr, true);
  TEST_IMPL(data &, non_existent1, false);
  TEST_IMPL(data &, non_existent2, false);
  TEST_IMPL(data &, non_existent3, false);

  TEST_IMPL(data &&, i, true);
  TEST_IMPL(data &&, scl, true);
  TEST_IMPL(data &&, lr, true);
  TEST_IMPL(data &&, dc, true);
  TEST_IMPL(data &&, bl, true);
  TEST_IMPL(data &&, v, true);
  TEST_IMPL(data &&, fcr, true);
  TEST_IMPL(data &&, non_existent1, false);
  TEST_IMPL(data &&, non_existent2, false);
  TEST_IMPL(data &&, non_existent3, false);

  TEST_IMPL(data const, i, true);
  TEST_IMPL(data const, scl, true);
  TEST_IMPL(data const, lr, true);
  TEST_IMPL(data const, dc, true);
  TEST_IMPL(data const, bl, true);
  TEST_IMPL(data const, v, true);
  TEST_IMPL(data const, fcr, true);
  TEST_IMPL(data const, non_existent1, false);
  TEST_IMPL(data const, non_existent2, false);
  TEST_IMPL(data const, non_existent3, false);

  TEST_IMPL(data const &, i, true);
  TEST_IMPL(data const &, scl, true);
  TEST_IMPL(data const &, lr, true);
  TEST_IMPL(data const &, dc, true);
  TEST_IMPL(data const &, bl, true);
  TEST_IMPL(data const &, v, true);
  TEST_IMPL(data const &, fcr, true);
  TEST_IMPL(data const &, non_existent1, false);
  TEST_IMPL(data const &, non_existent2, false);
  TEST_IMPL(data const &, non_existent3, false);

  TEST_IMPL(data const &&, i, true);
  TEST_IMPL(data const &&, scl, true);
  TEST_IMPL(data const &&, lr, true);
  TEST_IMPL(data const &&, dc, true);
  TEST_IMPL(data const &&, bl, true);
  TEST_IMPL(data const &&, v, true);
  TEST_IMPL(data const &&, fcr, true);
  TEST_IMPL(data const &&, non_existent1, false);
  TEST_IMPL(data const &&, non_existent2, false);
  TEST_IMPL(data const &&, non_existent3, false);

# undef TEST_IMPL
}

FATAL_TEST(data_member_getter, reference) {
# define TEST_IMPL(Owner, Field, ...) \
  do { \
    using getter = data_member_getter_test::getter::Field; \
    using data = data_member_getter_test::data; \
    using chained_getter = chained_data_member_getter< \
      data_member_getter_test::getter::Field \
    >; \
    \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::reference<Owner>>(); \
    FATAL_EXPECT_SAME<__VA_ARGS__, chained_getter::reference<Owner>>(); \
  } while (false)

  TEST_IMPL(data, i, int &&);
  TEST_IMPL(data, scl, std::string const &);
  TEST_IMPL(data, lr, long &&);
  TEST_IMPL(data, dc, double const &&);
  TEST_IMPL(data, bl, bool &);
  TEST_IMPL(data, v, std::vector<short> &&);
  TEST_IMPL(data, fcr, float const &&);

  TEST_IMPL(data &, i, int &);
  TEST_IMPL(data &, scl, std::string const &);
  TEST_IMPL(data &, lr, long &);
  TEST_IMPL(data &, dc, double const &);
  TEST_IMPL(data &, bl, bool &);
  TEST_IMPL(data &, v, std::vector<short> &);
  TEST_IMPL(data &, fcr, float const &);

  TEST_IMPL(data &&, i, int &&);
  TEST_IMPL(data &&, scl, std::string const &);
  TEST_IMPL(data &&, lr, long &&);
  TEST_IMPL(data &&, dc, double const &&);
  TEST_IMPL(data &&, bl, bool &);
  TEST_IMPL(data &&, v, std::vector<short> &&);
  TEST_IMPL(data &&, fcr, float const &&);

  TEST_IMPL(data const, i, int const &&);
  TEST_IMPL(data const, scl, std::string const &);
  TEST_IMPL(data const, lr, long const &&);
  TEST_IMPL(data const, dc, double const &&);
  TEST_IMPL(data const, bl, bool const &);
  TEST_IMPL(data const, v, std::vector<short> const &&);
  TEST_IMPL(data const, fcr, float const &&);

  TEST_IMPL(data const &, i, int const &);
  TEST_IMPL(data const &, scl, std::string const &);
  TEST_IMPL(data const &, lr, long const &);
  TEST_IMPL(data const &, dc, double const &);
  TEST_IMPL(data const &, bl, bool const &);
  TEST_IMPL(data const &, v, std::vector<short> const &);
  TEST_IMPL(data const &, fcr, float const &);

  TEST_IMPL(data const &&, i, int const &&);
  TEST_IMPL(data const &&, scl, std::string const &);
  TEST_IMPL(data const &&, lr, long const &&);
  TEST_IMPL(data const &&, dc, double const &&);
  TEST_IMPL(data const &&, bl, bool const &);
  TEST_IMPL(data const &&, v, std::vector<short> const &&);
  TEST_IMPL(data const &&, fcr, float const &&);

# undef TEST_IMPL
}

FATAL_TEST(data_member_getter, pointer) {
# define TEST_IMPL(Owner, Field, ...) \
  do { \
    using getter = data_member_getter_test::getter::Field; \
    using data = data_member_getter_test::data; \
    using chained_getter = chained_data_member_getter< \
      data_member_getter_test::getter::Field \
    >; \
    \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::pointer<Owner>>(); \
    FATAL_EXPECT_SAME<__VA_ARGS__, chained_getter::pointer<Owner>>(); \
  } while (false)

  TEST_IMPL(data, i, int *);
  TEST_IMPL(data, scl, std::string const *);
  TEST_IMPL(data, lr, long *);
  TEST_IMPL(data, dc, double const *);
  TEST_IMPL(data, bl, bool *);
  TEST_IMPL(data, v, std::vector<short> *);
  TEST_IMPL(data, fcr, float const *);

  TEST_IMPL(data &, i, int *);
  TEST_IMPL(data &, scl, std::string const *);
  TEST_IMPL(data &, lr, long *);
  TEST_IMPL(data &, dc, double const *);
  TEST_IMPL(data &, bl, bool *);
  TEST_IMPL(data &, v, std::vector<short> *);
  TEST_IMPL(data &, fcr, float const *);

  TEST_IMPL(data &&, i, int *);
  TEST_IMPL(data &&, scl, std::string const *);
  TEST_IMPL(data &&, lr, long *);
  TEST_IMPL(data &&, dc, double const *);
  TEST_IMPL(data &&, bl, bool *);
  TEST_IMPL(data &&, v, std::vector<short> *);
  TEST_IMPL(data &&, fcr, float const *);

  TEST_IMPL(data const, i, int const *);
  TEST_IMPL(data const, scl, std::string const *);
  TEST_IMPL(data const, lr, long const *);
  TEST_IMPL(data const, dc, double const *);
  TEST_IMPL(data const, bl, bool const *);
  TEST_IMPL(data const, v, std::vector<short> const *);
  TEST_IMPL(data const, fcr, float const *);

  TEST_IMPL(data const &, i, int const *);
  TEST_IMPL(data const &, scl, std::string const *);
  TEST_IMPL(data const &, lr, long const *);
  TEST_IMPL(data const &, dc, double const *);
  TEST_IMPL(data const &, bl, bool const *);
  TEST_IMPL(data const &, v, std::vector<short> const *);
  TEST_IMPL(data const &, fcr, float const *);

  TEST_IMPL(data const &&, i, int const *);
  TEST_IMPL(data const &&, scl, std::string const *);
  TEST_IMPL(data const &&, lr, long const *);
  TEST_IMPL(data const &&, dc, double const *);
  TEST_IMPL(data const &&, bl, bool const *);
  TEST_IMPL(data const &&, v, std::vector<short> const *);
  TEST_IMPL(data const &&, fcr, float const *);

# undef TEST_IMPL
}

FATAL_TEST(data_member_getter, getter) {
  int i = 99;
  std::string const scl("hello, world!");
  long lr = 27;
  double const dc = 5.6;
  bool bl = true;
  std::vector<short> v{1, 2, 3, 4, 5, 6};
  float const fcr = 7.2f;

  data_member_getter_test::data x(
    i, scl, std::move(lr), dc, bl, v, std::move(fcr)
  );

  FATAL_ASSERT_EQ(i, x.i);
  FATAL_ASSERT_EQ(scl, x.scl);
  FATAL_ASSERT_EQ(lr, x.lr);
  FATAL_ASSERT_EQ(dc, x.dc);
  FATAL_ASSERT_EQ(bl, x.bl);
  FATAL_ASSERT_EQ(v, x.v);
  FATAL_ASSERT_EQ(fcr, x.fcr);

  auto const &y = x;

  FATAL_ASSERT_EQ(i, y.i);
  FATAL_ASSERT_EQ(scl, y.scl);
  FATAL_ASSERT_EQ(lr, y.lr);
  FATAL_ASSERT_EQ(dc, y.dc);
  FATAL_ASSERT_EQ(bl, y.bl);
  FATAL_ASSERT_EQ(v, y.v);
  FATAL_ASSERT_EQ(fcr, y.fcr);

# define TEST_PREAMBLE_IMPL(Field, ...) \
    using getter = data_member_getter_test::getter::Field; \
    using chained_getter = chained_data_member_getter< \
      data_member_getter_test::getter::Field \
    >;

# define TEST_IMPL(Data, Field) \
  do { \
    TEST_PREAMBLE_IMPL(Field); \
    \
    FATAL_EXPECT_EQ(Field, getter::ref(Data)); \
    FATAL_EXPECT_EQ(Field, data_member_referencer<getter>()(Data)); \
    FATAL_EXPECT_EQ(Field, chained_getter::ref(Data)); \
    FATAL_EXPECT_EQ(Field, data_member_referencer<chained_getter>()(Data)); \
    \
    FATAL_EXPECT_EQ(Data.Field, getter::ref(Data)); \
    FATAL_EXPECT_EQ(Data.Field, data_member_referencer<getter>()(Data)); \
    FATAL_EXPECT_EQ(Data.Field, chained_getter::ref(Data)); \
    FATAL_EXPECT_EQ( \
      Data.Field, \
      data_member_referencer<chained_getter>()(Data) \
    ); \
    \
    FATAL_EXPECT_EQ( \
      std::addressof(Data.Field), \
      std::addressof(getter::ref(Data)) \
    ); \
    \
    FATAL_EXPECT_EQ( \
      std::addressof(Data.Field), \
      std::addressof(data_member_referencer<getter>()(Data)) \
    ); \
    \
    FATAL_EXPECT_EQ( \
      std::addressof(Data.Field), \
      std::addressof(chained_getter::ref(Data)) \
    ); \
    \
    FATAL_EXPECT_EQ( \
      std::addressof(Data.Field), \
      std::addressof(data_member_referencer<chained_getter>()(Data)) \
    ); \
    \
    FATAL_EXPECT_SAME< \
      std::add_lvalue_reference< \
        constify_from< \
          decltype(Data.Field), \
          std::remove_reference<decltype(Data)>::type \
        >::type \
      >::type, \
      decltype(getter::ref(Data)) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      std::add_lvalue_reference< \
        constify_from< \
          decltype(Data.Field), \
          std::remove_reference<decltype(Data)>::type \
        >::type \
      >::type, \
      decltype(data_member_referencer<getter>()(Data)) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      std::add_lvalue_reference< \
        constify_from< \
          decltype(Data.Field), \
          std::remove_reference<decltype(Data)>::type \
        >::type \
      >::type, \
      decltype(chained_getter::ref(Data)) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      std::add_lvalue_reference< \
        constify_from< \
          decltype(Data.Field), \
          std::remove_reference<decltype(Data)>::type \
        >::type \
      >::type, \
      decltype(data_member_referencer<chained_getter>()(Data)) \
    >(); \
  } while (false)

  TEST_IMPL(x, i);
  TEST_IMPL(x, scl);
  TEST_IMPL(x, lr);
  TEST_IMPL(x, dc);
  TEST_IMPL(x, bl);
  TEST_IMPL(x, v);
  TEST_IMPL(x, fcr);

  TEST_IMPL(y, i);
  TEST_IMPL(y, scl);
  TEST_IMPL(y, lr);
  TEST_IMPL(y, dc);
  TEST_IMPL(y, bl);
  TEST_IMPL(y, v);
  TEST_IMPL(y, fcr);

# undef TEST_IMPL
# define TEST_IMPL(Data, Field) \
  do { \
    TEST_PREAMBLE_IMPL(Field); \
    \
    FATAL_EXPECT_EQ(Field, getter::ref(std::move(Data))); \
    FATAL_EXPECT_EQ(Field, data_member_referencer<getter>()(std::move(Data))); \
    FATAL_EXPECT_EQ(Field, chained_getter::ref(std::move(Data))); \
    FATAL_EXPECT_EQ( \
      Field, \
      data_member_referencer<chained_getter>()(std::move(Data)) \
    ); \
    \
    FATAL_EXPECT_EQ(std::move(Data).Field, getter::ref(std::move(Data))); \
    FATAL_EXPECT_EQ( \
      std::move(Data).Field, \
      data_member_referencer<getter>()(std::move(Data)) \
    ); \
    \
    FATAL_EXPECT_EQ( \
      std::move(Data).Field, \
      chained_getter::ref(std::move(Data)) \
    ); \
    FATAL_EXPECT_EQ( \
      std::move(Data).Field, \
      data_member_referencer<chained_getter>()(std::move(Data)) \
    ); \
    \
    FATAL_EXPECT_SAME< \
      std::add_rvalue_reference< \
        constify_from< \
          decltype(std::move(Data).Field), \
          std::remove_reference<decltype(std::move(Data))>::type \
        >::type \
      >::type, \
      decltype(getter::ref(std::move(Data))) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      std::add_rvalue_reference< \
        constify_from< \
          decltype(std::move(Data).Field), \
          std::remove_reference<decltype(std::move(Data))>::type \
        >::type \
      >::type, \
      decltype(data_member_referencer<getter>()(std::move(Data))) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      std::add_rvalue_reference< \
        constify_from< \
          decltype(std::move(Data).Field), \
          std::remove_reference<decltype(std::move(Data))>::type \
        >::type \
      >::type, \
      decltype(chained_getter::ref(std::move(Data))) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      std::add_rvalue_reference< \
        constify_from< \
          decltype(std::move(Data).Field), \
          std::remove_reference<decltype(std::move(Data))>::type \
        >::type \
      >::type, \
      decltype(data_member_referencer<chained_getter>()(std::move(Data))) \
    >(); \
  } while (false)

  TEST_IMPL(x, i);
  TEST_IMPL(x, scl);
  TEST_IMPL(x, lr);
  TEST_IMPL(x, dc);
  TEST_IMPL(x, bl);
  TEST_IMPL(x, v);
  TEST_IMPL(x, fcr);

  TEST_IMPL(y, i);
  TEST_IMPL(y, scl);
  TEST_IMPL(y, lr);
  TEST_IMPL(y, dc);
  TEST_IMPL(y, bl);
  TEST_IMPL(y, v);
  TEST_IMPL(y, fcr);

# undef TEST_IMPL
# define TEST_IMPL(Data, Field) \
  do { \
    TEST_PREAMBLE_IMPL(Field); \
    \
    FATAL_EXPECT_EQ(std::addressof(Data.Field), getter::ptr(Data)); \
    FATAL_EXPECT_EQ( \
      std::addressof(Data.Field), \
      data_member_pointer<getter>()(Data) \
    ); \
    \
    FATAL_EXPECT_EQ(std::addressof(Data.Field), chained_getter::ptr(Data)); \
    FATAL_EXPECT_EQ( \
      std::addressof(Data.Field), \
      data_member_pointer<chained_getter>()(Data) \
    ); \
    \
    FATAL_EXPECT_SAME< \
      constify_from< \
        std::remove_pointer<decltype(std::addressof(Data.Field))>::type, \
        std::remove_reference<decltype(Data)>::type \
      >::type *, \
      decltype(getter::ptr(Data)) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      constify_from< \
        std::remove_pointer<decltype(std::addressof(Data.Field))>::type, \
        std::remove_reference<decltype(Data)>::type \
      >::type *, \
      decltype(data_member_pointer<getter>()(Data)) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      constify_from< \
        std::remove_pointer<decltype(std::addressof(Data.Field))>::type, \
        std::remove_reference<decltype(Data)>::type \
      >::type *, \
      decltype(chained_getter::ptr(Data)) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      constify_from< \
        std::remove_pointer<decltype(std::addressof(Data.Field))>::type, \
        std::remove_reference<decltype(Data)>::type \
      >::type *, \
      decltype(data_member_pointer<chained_getter>()(Data)) \
    >(); \
  } while (false)

  TEST_IMPL(x, i);
  TEST_IMPL(x, scl);
  TEST_IMPL(x, lr);
  TEST_IMPL(x, dc);
  TEST_IMPL(x, bl);
  TEST_IMPL(x, v);
  TEST_IMPL(x, fcr);

  TEST_IMPL(y, i);
  TEST_IMPL(y, scl);
  TEST_IMPL(y, lr);
  TEST_IMPL(y, dc);
  TEST_IMPL(y, bl);
  TEST_IMPL(y, v);
  TEST_IMPL(y, fcr);

# undef TEST_IMPL
# undef TEST_PREAMBLE_IMPL
}

FATAL_TEST(data_member_getter, namespace_std_collision) {
  using type = data_member_getter_test::ns_tricky;
  using getter = data_member_getter_test::ns_tricky_getter;
  type obj;
  getter::std::ref(obj) = 12.3;
  getter::desc::ref(obj) = "hi";
  FATAL_EXPECT_EQ(12.3, getter::std::ref(static_cast<type const&>(obj)));
  FATAL_EXPECT_EQ("hi", getter::desc::ref(static_cast<type const&>(obj)));
}

////////////////////////////////
// chained_data_member_getter //
////////////////////////////////

namespace data_member_getter_test {

struct chained_data {
  chained_data(data d_, data &l_, data &&r_, data const &cl_, data const &&cr_):
    d(d_), l(l_), r(std::move(r_)), cl(cl_), cr(std::move(cr_))
  {}

  data d;
  data &l;
  data &&r;
  data const &cl;
  data const &&cr;
};

struct chained_getter {
# define TEST_IMPL(Outer, Inner) \
  using Outer##_##Inner = chained_data_member_getter< \
    data_member_getter_test::getter::Outer, \
    data_member_getter_test::getter::Inner \
  >

  TEST_IMPL(d, i);
  TEST_IMPL(d, scl);
  TEST_IMPL(d, lr);
  TEST_IMPL(d, dc);
  TEST_IMPL(d, bl);
  TEST_IMPL(d, v);
  TEST_IMPL(d, fcr);

  TEST_IMPL(l, i);
  TEST_IMPL(l, scl);
  TEST_IMPL(l, lr);
  TEST_IMPL(l, dc);
  TEST_IMPL(l, bl);
  TEST_IMPL(l, v);
  TEST_IMPL(l, fcr);

  TEST_IMPL(r, i);
  TEST_IMPL(r, scl);
  TEST_IMPL(r, lr);
  TEST_IMPL(r, dc);
  TEST_IMPL(r, bl);
  TEST_IMPL(r, v);
  TEST_IMPL(r, fcr);

  TEST_IMPL(cl, i);
  TEST_IMPL(cl, scl);
  TEST_IMPL(cl, lr);
  TEST_IMPL(cl, dc);
  TEST_IMPL(cl, bl);
  TEST_IMPL(cl, v);
  TEST_IMPL(cl, fcr);

  TEST_IMPL(cr, i);
  TEST_IMPL(cr, scl);
  TEST_IMPL(cr, lr);
  TEST_IMPL(cr, dc);
  TEST_IMPL(cr, bl);
  TEST_IMPL(cr, v);
  TEST_IMPL(cr, fcr);

# undef TEST_IMPL
};

} // namespace data_member_getter_test {

FATAL_TEST(chained_data_member_getter, type) {
# define TEST_IMPL(T) \
  do { \
    FATAL_EXPECT_SAME<T, chained_data_member_getter<>::type<T>>(); \
  } while (false)

  TEST_IMPL(int);
  TEST_IMPL(int &);
  TEST_IMPL(int &&);
  TEST_IMPL(int const &);
  TEST_IMPL(int const &&);

# undef TEST_IMPL

# define TEST_IMPL(Owner, Outer, Inner, ...) \
  do { \
    using data = data_member_getter_test::chained_data; \
    using getter = chained_data_member_getter< \
      data_member_getter_test::getter::Outer, \
      data_member_getter_test::getter::Inner \
    >; \
    \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::type<Owner>>(); \
  } while (false)

  TEST_IMPL(data, d, i, int);
  TEST_IMPL(data, d, scl, std::string const &);
  TEST_IMPL(data, d, lr, long &&);
  TEST_IMPL(data, d, dc, double const);
  TEST_IMPL(data, d, bl, bool &);
  TEST_IMPL(data, d, v, std::vector<short>);
  TEST_IMPL(data, d, fcr, float const &&);

  TEST_IMPL(data, l, i, int);
  TEST_IMPL(data, l, scl, std::string const &);
  TEST_IMPL(data, l, lr, long &&);
  TEST_IMPL(data, l, dc, double const);
  TEST_IMPL(data, l, bl, bool &);
  TEST_IMPL(data, l, v, std::vector<short>);
  TEST_IMPL(data, l, fcr, float const &&);

  TEST_IMPL(data, r, i, int);
  TEST_IMPL(data, r, scl, std::string const &);
  TEST_IMPL(data, r, lr, long &&);
  TEST_IMPL(data, r, dc, double const);
  TEST_IMPL(data, r, bl, bool &);
  TEST_IMPL(data, r, v, std::vector<short>);
  TEST_IMPL(data, r, fcr, float const &&);

  TEST_IMPL(data, cl, i, int);
  TEST_IMPL(data, cl, scl, std::string const &);
  TEST_IMPL(data, cl, lr, long &&);
  TEST_IMPL(data, cl, dc, double const);
  TEST_IMPL(data, cl, bl, bool &);
  TEST_IMPL(data, cl, v, std::vector<short>);
  TEST_IMPL(data, cl, fcr, float const &&);

  TEST_IMPL(data, cr, i, int);
  TEST_IMPL(data, cr, scl, std::string const &);
  TEST_IMPL(data, cr, lr, long &&);
  TEST_IMPL(data, cr, dc, double const);
  TEST_IMPL(data, cr, bl, bool &);
  TEST_IMPL(data, cr, v, std::vector<short>);
  TEST_IMPL(data, cr, fcr, float const &&);

  TEST_IMPL(data &, d, i, int);
  TEST_IMPL(data &, d, scl, std::string const &);
  TEST_IMPL(data &, d, lr, long &&);
  TEST_IMPL(data &, d, dc, double const);
  TEST_IMPL(data &, d, bl, bool &);
  TEST_IMPL(data &, d, v, std::vector<short>);
  TEST_IMPL(data &, d, fcr, float const &&);

  TEST_IMPL(data &, l, i, int);
  TEST_IMPL(data &, l, scl, std::string const &);
  TEST_IMPL(data &, l, lr, long &&);
  TEST_IMPL(data &, l, dc, double const);
  TEST_IMPL(data &, l, bl, bool &);
  TEST_IMPL(data &, l, v, std::vector<short>);
  TEST_IMPL(data &, l, fcr, float const &&);

  TEST_IMPL(data &, r, i, int);
  TEST_IMPL(data &, r, scl, std::string const &);
  TEST_IMPL(data &, r, lr, long &&);
  TEST_IMPL(data &, r, dc, double const);
  TEST_IMPL(data &, r, bl, bool &);
  TEST_IMPL(data &, r, v, std::vector<short>);
  TEST_IMPL(data &, r, fcr, float const &&);

  TEST_IMPL(data &, cl, i, int);
  TEST_IMPL(data &, cl, scl, std::string const &);
  TEST_IMPL(data &, cl, lr, long &&);
  TEST_IMPL(data &, cl, dc, double const);
  TEST_IMPL(data &, cl, bl, bool &);
  TEST_IMPL(data &, cl, v, std::vector<short>);
  TEST_IMPL(data &, cl, fcr, float const &&);

  TEST_IMPL(data &, cr, i, int);
  TEST_IMPL(data &, cr, scl, std::string const &);
  TEST_IMPL(data &, cr, lr, long &&);
  TEST_IMPL(data &, cr, dc, double const);
  TEST_IMPL(data &, cr, bl, bool &);
  TEST_IMPL(data &, cr, v, std::vector<short>);
  TEST_IMPL(data &, cr, fcr, float const &&);

  TEST_IMPL(data &&, d, i, int);
  TEST_IMPL(data &&, d, scl, std::string const &);
  TEST_IMPL(data &&, d, lr, long &&);
  TEST_IMPL(data &&, d, dc, double const);
  TEST_IMPL(data &&, d, bl, bool &);
  TEST_IMPL(data &&, d, v, std::vector<short>);
  TEST_IMPL(data &&, d, fcr, float const &&);

  TEST_IMPL(data &&, l, i, int);
  TEST_IMPL(data &&, l, scl, std::string const &);
  TEST_IMPL(data &&, l, lr, long &&);
  TEST_IMPL(data &&, l, dc, double const);
  TEST_IMPL(data &&, l, bl, bool &);
  TEST_IMPL(data &&, l, v, std::vector<short>);
  TEST_IMPL(data &&, l, fcr, float const &&);

  TEST_IMPL(data &&, r, i, int);
  TEST_IMPL(data &&, r, scl, std::string const &);
  TEST_IMPL(data &&, r, lr, long &&);
  TEST_IMPL(data &&, r, dc, double const);
  TEST_IMPL(data &&, r, bl, bool &);
  TEST_IMPL(data &&, r, v, std::vector<short>);
  TEST_IMPL(data &&, r, fcr, float const &&);

  TEST_IMPL(data &&, cl, i, int);
  TEST_IMPL(data &&, cl, scl, std::string const &);
  TEST_IMPL(data &&, cl, lr, long &&);
  TEST_IMPL(data &&, cl, dc, double const);
  TEST_IMPL(data &&, cl, bl, bool &);
  TEST_IMPL(data &&, cl, v, std::vector<short>);
  TEST_IMPL(data &&, cl, fcr, float const &&);

  TEST_IMPL(data &&, cr, i, int);
  TEST_IMPL(data &&, cr, scl, std::string const &);
  TEST_IMPL(data &&, cr, lr, long &&);
  TEST_IMPL(data &&, cr, dc, double const);
  TEST_IMPL(data &&, cr, bl, bool &);
  TEST_IMPL(data &&, cr, v, std::vector<short>);
  TEST_IMPL(data &&, cr, fcr, float const &&);

  TEST_IMPL(data const, d, i, int);
  TEST_IMPL(data const, d, scl, std::string const &);
  TEST_IMPL(data const, d, lr, long &&);
  TEST_IMPL(data const, d, dc, double const);
  TEST_IMPL(data const, d, bl, bool &);
  TEST_IMPL(data const, d, v, std::vector<short>);
  TEST_IMPL(data const, d, fcr, float const &&);

  TEST_IMPL(data const, l, i, int);
  TEST_IMPL(data const, l, scl, std::string const &);
  TEST_IMPL(data const, l, lr, long &&);
  TEST_IMPL(data const, l, dc, double const);
  TEST_IMPL(data const, l, bl, bool &);
  TEST_IMPL(data const, l, v, std::vector<short>);
  TEST_IMPL(data const, l, fcr, float const &&);

  TEST_IMPL(data const, r, i, int);
  TEST_IMPL(data const, r, scl, std::string const &);
  TEST_IMPL(data const, r, lr, long &&);
  TEST_IMPL(data const, r, dc, double const);
  TEST_IMPL(data const, r, bl, bool &);
  TEST_IMPL(data const, r, v, std::vector<short>);
  TEST_IMPL(data const, r, fcr, float const &&);

  TEST_IMPL(data const, cl, i, int);
  TEST_IMPL(data const, cl, scl, std::string const &);
  TEST_IMPL(data const, cl, lr, long &&);
  TEST_IMPL(data const, cl, dc, double const);
  TEST_IMPL(data const, cl, bl, bool &);
  TEST_IMPL(data const, cl, v, std::vector<short>);
  TEST_IMPL(data const, cl, fcr, float const &&);

  TEST_IMPL(data const, cr, i, int);
  TEST_IMPL(data const, cr, scl, std::string const &);
  TEST_IMPL(data const, cr, lr, long &&);
  TEST_IMPL(data const, cr, dc, double const);
  TEST_IMPL(data const, cr, bl, bool &);
  TEST_IMPL(data const, cr, v, std::vector<short>);
  TEST_IMPL(data const, cr, fcr, float const &&);

  TEST_IMPL(data const &, d, i, int);
  TEST_IMPL(data const &, d, scl, std::string const &);
  TEST_IMPL(data const &, d, lr, long &&);
  TEST_IMPL(data const &, d, dc, double const);
  TEST_IMPL(data const &, d, bl, bool &);
  TEST_IMPL(data const &, d, v, std::vector<short>);
  TEST_IMPL(data const &, d, fcr, float const &&);

  TEST_IMPL(data const &, l, i, int);
  TEST_IMPL(data const &, l, scl, std::string const &);
  TEST_IMPL(data const &, l, lr, long &&);
  TEST_IMPL(data const &, l, dc, double const);
  TEST_IMPL(data const &, l, bl, bool &);
  TEST_IMPL(data const &, l, v, std::vector<short>);
  TEST_IMPL(data const &, l, fcr, float const &&);

  TEST_IMPL(data const &, r, i, int);
  TEST_IMPL(data const &, r, scl, std::string const &);
  TEST_IMPL(data const &, r, lr, long &&);
  TEST_IMPL(data const &, r, dc, double const);
  TEST_IMPL(data const &, r, bl, bool &);
  TEST_IMPL(data const &, r, v, std::vector<short>);
  TEST_IMPL(data const &, r, fcr, float const &&);

  TEST_IMPL(data const &, cl, i, int);
  TEST_IMPL(data const &, cl, scl, std::string const &);
  TEST_IMPL(data const &, cl, lr, long &&);
  TEST_IMPL(data const &, cl, dc, double const);
  TEST_IMPL(data const &, cl, bl, bool &);
  TEST_IMPL(data const &, cl, v, std::vector<short>);
  TEST_IMPL(data const &, cl, fcr, float const &&);

  TEST_IMPL(data const &, cr, i, int);
  TEST_IMPL(data const &, cr, scl, std::string const &);
  TEST_IMPL(data const &, cr, lr, long &&);
  TEST_IMPL(data const &, cr, dc, double const);
  TEST_IMPL(data const &, cr, bl, bool &);
  TEST_IMPL(data const &, cr, v, std::vector<short>);
  TEST_IMPL(data const &, cr, fcr, float const &&);

  TEST_IMPL(data const &&, d, i, int);
  TEST_IMPL(data const &&, d, scl, std::string const &);
  TEST_IMPL(data const &&, d, lr, long &&);
  TEST_IMPL(data const &&, d, dc, double const);
  TEST_IMPL(data const &&, d, bl, bool &);
  TEST_IMPL(data const &&, d, v, std::vector<short>);
  TEST_IMPL(data const &&, d, fcr, float const &&);

  TEST_IMPL(data const &&, l, i, int);
  TEST_IMPL(data const &&, l, scl, std::string const &);
  TEST_IMPL(data const &&, l, lr, long &&);
  TEST_IMPL(data const &&, l, dc, double const);
  TEST_IMPL(data const &&, l, bl, bool &);
  TEST_IMPL(data const &&, l, v, std::vector<short>);
  TEST_IMPL(data const &&, l, fcr, float const &&);

  TEST_IMPL(data const &&, r, i, int);
  TEST_IMPL(data const &&, r, scl, std::string const &);
  TEST_IMPL(data const &&, r, lr, long &&);
  TEST_IMPL(data const &&, r, dc, double const);
  TEST_IMPL(data const &&, r, bl, bool &);
  TEST_IMPL(data const &&, r, v, std::vector<short>);
  TEST_IMPL(data const &&, r, fcr, float const &&);

  TEST_IMPL(data const &&, cl, i, int);
  TEST_IMPL(data const &&, cl, scl, std::string const &);
  TEST_IMPL(data const &&, cl, lr, long &&);
  TEST_IMPL(data const &&, cl, dc, double const);
  TEST_IMPL(data const &&, cl, bl, bool &);
  TEST_IMPL(data const &&, cl, v, std::vector<short>);
  TEST_IMPL(data const &&, cl, fcr, float const &&);

  TEST_IMPL(data const &&, cr, i, int);
  TEST_IMPL(data const &&, cr, scl, std::string const &);
  TEST_IMPL(data const &&, cr, lr, long &&);
  TEST_IMPL(data const &&, cr, dc, double const);
  TEST_IMPL(data const &&, cr, bl, bool &);
  TEST_IMPL(data const &&, cr, v, std::vector<short>);
  TEST_IMPL(data const &&, cr, fcr, float const &&);

# undef TEST_IMPL
}

FATAL_TEST(chained_data_member_getter, reference) {
# define TEST_IMPL(T) \
  do { \
    FATAL_EXPECT_SAME< \
      std::add_rvalue_reference<T>::type, \
      chained_data_member_getter<>::reference<T> \
    >(); \
  } while (false)

  TEST_IMPL(int);
  TEST_IMPL(int &);
  TEST_IMPL(int &&);
  TEST_IMPL(int const &);
  TEST_IMPL(int const &&);

# undef TEST_IMPL

# define TEST_IMPL(Owner, Outer, Inner, ...) \
  do { \
    using data = data_member_getter_test::chained_data; \
    using getter = chained_data_member_getter< \
      data_member_getter_test::getter::Outer, \
      data_member_getter_test::getter::Inner \
    >; \
    \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::reference<Owner>>(); \
  } while (false)

  TEST_IMPL(data, d, i, int &&);
  TEST_IMPL(data, d, scl, std::string const &);
  TEST_IMPL(data, d, lr, long &&);
  TEST_IMPL(data, d, dc, double const &&);
  TEST_IMPL(data, d, bl, bool &);
  TEST_IMPL(data, d, v, std::vector<short> &&);
  TEST_IMPL(data, d, fcr, float const &&);

  TEST_IMPL(data, l, i, int &);
  TEST_IMPL(data, l, scl, std::string const &);
  TEST_IMPL(data, l, lr, long &);
  TEST_IMPL(data, l, dc, double const &);
  TEST_IMPL(data, l, bl, bool &);
  TEST_IMPL(data, l, v, std::vector<short> &);
  TEST_IMPL(data, l, fcr, float const &);

  TEST_IMPL(data, r, i, int &&);
  TEST_IMPL(data, r, scl, std::string const &);
  TEST_IMPL(data, r, lr, long &&);
  TEST_IMPL(data, r, dc, double const &&);
  TEST_IMPL(data, r, bl, bool &);
  TEST_IMPL(data, r, v, std::vector<short> &&);
  TEST_IMPL(data, r, fcr, float const &&);

  TEST_IMPL(data, cl, i, int const &);
  TEST_IMPL(data, cl, scl, std::string const &);
  TEST_IMPL(data, cl, lr, long const &);
  TEST_IMPL(data, cl, dc, double const &);
  TEST_IMPL(data, cl, bl, bool const &);
  TEST_IMPL(data, cl, v, std::vector<short> const &);
  TEST_IMPL(data, cl, fcr, float const &);

  TEST_IMPL(data, cr, i, int const &&);
  TEST_IMPL(data, cr, scl, std::string const &);
  TEST_IMPL(data, cr, lr, long const &&);
  TEST_IMPL(data, cr, dc, double const &&);
  TEST_IMPL(data, cr, bl, bool const &);
  TEST_IMPL(data, cr, v, std::vector<short> const &&);
  TEST_IMPL(data, cr, fcr, float const &&);

  TEST_IMPL(data &, d, i, int &);
  TEST_IMPL(data &, d, scl, std::string const &);
  TEST_IMPL(data &, d, lr, long &);
  TEST_IMPL(data &, d, dc, double const &);
  TEST_IMPL(data &, d, bl, bool &);
  TEST_IMPL(data &, d, v, std::vector<short> &);
  TEST_IMPL(data &, d, fcr, float const &);

  TEST_IMPL(data &, l, i, int &);
  TEST_IMPL(data &, l, scl, std::string const &);
  TEST_IMPL(data &, l, lr, long &);
  TEST_IMPL(data &, l, dc, double const &);
  TEST_IMPL(data &, l, bl, bool &);
  TEST_IMPL(data &, l, v, std::vector<short> &);
  TEST_IMPL(data &, l, fcr, float const &);

  TEST_IMPL(data &, r, i, int &);
  TEST_IMPL(data &, r, scl, std::string const &);
  TEST_IMPL(data &, r, lr, long &);
  TEST_IMPL(data &, r, dc, double const &);
  TEST_IMPL(data &, r, bl, bool &);
  TEST_IMPL(data &, r, v, std::vector<short> &);
  TEST_IMPL(data &, r, fcr, float const &);

  TEST_IMPL(data &, cl, i, int const &);
  TEST_IMPL(data &, cl, scl, std::string const &);
  TEST_IMPL(data &, cl, lr, long const &);
  TEST_IMPL(data &, cl, dc, double const &);
  TEST_IMPL(data &, cl, bl, bool const &);
  TEST_IMPL(data &, cl, v, std::vector<short> const &);
  TEST_IMPL(data &, cl, fcr, float const &);

  TEST_IMPL(data &, cr, i, int const &);
  TEST_IMPL(data &, cr, scl, std::string const &);
  TEST_IMPL(data &, cr, lr, long const &);
  TEST_IMPL(data &, cr, dc, double const &);
  TEST_IMPL(data &, cr, bl, bool const &);
  TEST_IMPL(data &, cr, v, std::vector<short> const &);
  TEST_IMPL(data &, cr, fcr, float const &);

  TEST_IMPL(data &&, d, i, int &&);
  TEST_IMPL(data &&, d, scl, std::string const &);
  TEST_IMPL(data &&, d, lr, long &&);
  TEST_IMPL(data &&, d, dc, double const &&);
  TEST_IMPL(data &&, d, bl, bool &);
  TEST_IMPL(data &&, d, v, std::vector<short> &&);
  TEST_IMPL(data &&, d, fcr, float const &&);

  TEST_IMPL(data &&, l, i, int &);
  TEST_IMPL(data &&, l, scl, std::string const &);
  TEST_IMPL(data &&, l, lr, long &);
  TEST_IMPL(data &&, l, dc, double const &);
  TEST_IMPL(data &&, l, bl, bool &);
  TEST_IMPL(data &&, l, v, std::vector<short> &);
  TEST_IMPL(data &&, l, fcr, float const &);

  TEST_IMPL(data &&, r, i, int &&);
  TEST_IMPL(data &&, r, scl, std::string const &);
  TEST_IMPL(data &&, r, lr, long &&);
  TEST_IMPL(data &&, r, dc, double const &&);
  TEST_IMPL(data &&, r, bl, bool &);
  TEST_IMPL(data &&, r, v, std::vector<short> &&);
  TEST_IMPL(data &&, r, fcr, float const &&);

  TEST_IMPL(data &&, cl, i, int const &);
  TEST_IMPL(data &&, cl, scl, std::string const &);
  TEST_IMPL(data &&, cl, lr, long const &);
  TEST_IMPL(data &&, cl, dc, double const &);
  TEST_IMPL(data &&, cl, bl, bool const &);
  TEST_IMPL(data &&, cl, v, std::vector<short> const &);
  TEST_IMPL(data &&, cl, fcr, float const &);

  TEST_IMPL(data &&, cr, i, int const &&);
  TEST_IMPL(data &&, cr, scl, std::string const &);
  TEST_IMPL(data &&, cr, lr, long const &&);
  TEST_IMPL(data &&, cr, dc, double const &&);
  TEST_IMPL(data &&, cr, bl, bool const &);
  TEST_IMPL(data &&, cr, v, std::vector<short> const &&);
  TEST_IMPL(data &&, cr, fcr, float const &&);

  TEST_IMPL(data const, d, i, int const  &&);
  TEST_IMPL(data const, d, scl, std::string const  &);
  TEST_IMPL(data const, d, lr, long const  &&);
  TEST_IMPL(data const, d, dc, double const  &&);
  TEST_IMPL(data const, d, bl, bool const  &);
  TEST_IMPL(data const, d, v, std::vector<short> const  &&);
  TEST_IMPL(data const, d, fcr, float const  &&);

  TEST_IMPL(data const, l, i, int const  &);
  TEST_IMPL(data const, l, scl, std::string const  &);
  TEST_IMPL(data const, l, lr, long const  &);
  TEST_IMPL(data const, l, dc, double const  &);
  TEST_IMPL(data const, l, bl, bool const  &);
  TEST_IMPL(data const, l, v, std::vector<short> const  &);
  TEST_IMPL(data const, l, fcr, float const  &);

  TEST_IMPL(data const, r, i, int const  &&);
  TEST_IMPL(data const, r, scl, std::string const  &);
  TEST_IMPL(data const, r, lr, long const  &&);
  TEST_IMPL(data const, r, dc, double const  &&);
  TEST_IMPL(data const, r, bl, bool const  &);
  TEST_IMPL(data const, r, v, std::vector<short> const  &&);
  TEST_IMPL(data const, r, fcr, float const  &&);

  TEST_IMPL(data const, cl, i, int const  &);
  TEST_IMPL(data const, cl, scl, std::string const  &);
  TEST_IMPL(data const, cl, lr, long const  &);
  TEST_IMPL(data const, cl, dc, double const  &);
  TEST_IMPL(data const, cl, bl, bool const  &);
  TEST_IMPL(data const, cl, v, std::vector<short> const  &);
  TEST_IMPL(data const, cl, fcr, float const  &);

  TEST_IMPL(data const, cr, i, int const  &&);
  TEST_IMPL(data const, cr, scl, std::string const  &);
  TEST_IMPL(data const, cr, lr, long const  &&);
  TEST_IMPL(data const, cr, dc, double const  &&);
  TEST_IMPL(data const, cr, bl, bool const  &);
  TEST_IMPL(data const, cr, v, std::vector<short> const  &&);
  TEST_IMPL(data const, cr, fcr, float const  &&);

  TEST_IMPL(data const &, d, i, int const  &);
  TEST_IMPL(data const &, d, scl, std::string const  &);
  TEST_IMPL(data const &, d, lr, long const  &);
  TEST_IMPL(data const &, d, dc, double const  &);
  TEST_IMPL(data const &, d, bl, bool const  &);
  TEST_IMPL(data const &, d, v, std::vector<short> const  &);
  TEST_IMPL(data const &, d, fcr, float const  &);

  TEST_IMPL(data const &, l, i, int const  &);
  TEST_IMPL(data const &, l, scl, std::string const  &);
  TEST_IMPL(data const &, l, lr, long const  &);
  TEST_IMPL(data const &, l, dc, double const  &);
  TEST_IMPL(data const &, l, bl, bool const  &);
  TEST_IMPL(data const &, l, v, std::vector<short> const  &);
  TEST_IMPL(data const &, l, fcr, float const  &);

  TEST_IMPL(data const &, r, i, int const  &);
  TEST_IMPL(data const &, r, scl, std::string const  &);
  TEST_IMPL(data const &, r, lr, long const  &);
  TEST_IMPL(data const &, r, dc, double const  &);
  TEST_IMPL(data const &, r, bl, bool const  &);
  TEST_IMPL(data const &, r, v, std::vector<short> const  &);
  TEST_IMPL(data const &, r, fcr, float const  &);

  TEST_IMPL(data const &, cl, i, int const  &);
  TEST_IMPL(data const &, cl, scl, std::string const  &);
  TEST_IMPL(data const &, cl, lr, long const  &);
  TEST_IMPL(data const &, cl, dc, double const  &);
  TEST_IMPL(data const &, cl, bl, bool const  &);
  TEST_IMPL(data const &, cl, v, std::vector<short> const  &);
  TEST_IMPL(data const &, cl, fcr, float const  &);

  TEST_IMPL(data const &, cr, i, int const  &);
  TEST_IMPL(data const &, cr, scl, std::string const  &);
  TEST_IMPL(data const &, cr, lr, long const  &);
  TEST_IMPL(data const &, cr, dc, double const  &);
  TEST_IMPL(data const &, cr, bl, bool const  &);
  TEST_IMPL(data const &, cr, v, std::vector<short> const  &);
  TEST_IMPL(data const &, cr, fcr, float const  &);

  TEST_IMPL(data const &&, d, i, int const  &&);
  TEST_IMPL(data const &&, d, scl, std::string const  &);
  TEST_IMPL(data const &&, d, lr, long const  &&);
  TEST_IMPL(data const &&, d, dc, double const  &&);
  TEST_IMPL(data const &&, d, bl, bool const  &);
  TEST_IMPL(data const &&, d, v, std::vector<short> const  &&);
  TEST_IMPL(data const &&, d, fcr, float const  &&);

  TEST_IMPL(data const &&, l, i, int const  &);
  TEST_IMPL(data const &&, l, scl, std::string const  &);
  TEST_IMPL(data const &&, l, lr, long const  &);
  TEST_IMPL(data const &&, l, dc, double const  &);
  TEST_IMPL(data const &&, l, bl, bool const  &);
  TEST_IMPL(data const &&, l, v, std::vector<short> const  &);
  TEST_IMPL(data const &&, l, fcr, float const  &);

  TEST_IMPL(data const &&, r, i, int const  &&);
  TEST_IMPL(data const &&, r, scl, std::string const  &);
  TEST_IMPL(data const &&, r, lr, long const  &&);
  TEST_IMPL(data const &&, r, dc, double const  &&);
  TEST_IMPL(data const &&, r, bl, bool const  &);
  TEST_IMPL(data const &&, r, v, std::vector<short> const  &&);
  TEST_IMPL(data const &&, r, fcr, float const  &&);

  TEST_IMPL(data const &&, cl, i, int const  &);
  TEST_IMPL(data const &&, cl, scl, std::string const  &);
  TEST_IMPL(data const &&, cl, lr, long const  &);
  TEST_IMPL(data const &&, cl, dc, double const  &);
  TEST_IMPL(data const &&, cl, bl, bool const  &);
  TEST_IMPL(data const &&, cl, v, std::vector<short> const  &);
  TEST_IMPL(data const &&, cl, fcr, float const  &);

  TEST_IMPL(data const &&, cr, i, int const  &&);
  TEST_IMPL(data const &&, cr, scl, std::string const  &);
  TEST_IMPL(data const &&, cr, lr, long const  &&);
  TEST_IMPL(data const &&, cr, dc, double const  &&);
  TEST_IMPL(data const &&, cr, bl, bool const  &);
  TEST_IMPL(data const &&, cr, v, std::vector<short> const  &&);
  TEST_IMPL(data const &&, cr, fcr, float const  &&);

# undef TEST_IMPL
}

FATAL_TEST(chained_data_member_getter, pointer) {
# define TEST_IMPL(Owner, Expected) \
  do { \
    FATAL_EXPECT_SAME< \
      Expected, \
      chained_data_member_getter<>::pointer<Owner> \
    >(); \
  } while (false)

  TEST_IMPL(int, int *);
  TEST_IMPL(int &, int *);
  TEST_IMPL(int &&, int *);
  TEST_IMPL(int const &, int const *);
  TEST_IMPL(int const &&, int const *);

# undef TEST_IMPL

# define TEST_IMPL(Owner, Outer, Inner, ...) \
  do { \
    using data = data_member_getter_test::chained_data; \
    using getter = chained_data_member_getter< \
      data_member_getter_test::getter::Outer, \
      data_member_getter_test::getter::Inner \
    >; \
    \
    FATAL_EXPECT_SAME<__VA_ARGS__, getter::pointer<Owner>>(); \
  } while (false)

  TEST_IMPL(data, d, i, int *);
  TEST_IMPL(data, d, scl, std::string const *);
  TEST_IMPL(data, d, lr, long *);
  TEST_IMPL(data, d, dc, double const *);
  TEST_IMPL(data, d, bl, bool *);
  TEST_IMPL(data, d, v, std::vector<short> *);
  TEST_IMPL(data, d, fcr, float const *);

  TEST_IMPL(data, l, i, int *);
  TEST_IMPL(data, l, scl, std::string const *);
  TEST_IMPL(data, l, lr, long *);
  TEST_IMPL(data, l, dc, double const *);
  TEST_IMPL(data, l, bl, bool *);
  TEST_IMPL(data, l, v, std::vector<short> *);
  TEST_IMPL(data, l, fcr, float const *);

  TEST_IMPL(data, r, i, int *);
  TEST_IMPL(data, r, scl, std::string const *);
  TEST_IMPL(data, r, lr, long *);
  TEST_IMPL(data, r, dc, double const *);
  TEST_IMPL(data, r, bl, bool *);
  TEST_IMPL(data, r, v, std::vector<short> *);
  TEST_IMPL(data, r, fcr, float const *);

  TEST_IMPL(data, cl, i, int const *);
  TEST_IMPL(data, cl, scl, std::string const *);
  TEST_IMPL(data, cl, lr, long const *);
  TEST_IMPL(data, cl, dc, double const *);
  TEST_IMPL(data, cl, bl, bool const *);
  TEST_IMPL(data, cl, v, std::vector<short> const *);
  TEST_IMPL(data, cl, fcr, float const *);

  TEST_IMPL(data, cr, i, int const *);
  TEST_IMPL(data, cr, scl, std::string const *);
  TEST_IMPL(data, cr, lr, long const *);
  TEST_IMPL(data, cr, dc, double const *);
  TEST_IMPL(data, cr, bl, bool const *);
  TEST_IMPL(data, cr, v, std::vector<short> const *);
  TEST_IMPL(data, cr, fcr, float const *);

  TEST_IMPL(data &, d, i, int *);
  TEST_IMPL(data &, d, scl, std::string const *);
  TEST_IMPL(data &, d, lr, long *);
  TEST_IMPL(data &, d, dc, double const *);
  TEST_IMPL(data &, d, bl, bool *);
  TEST_IMPL(data &, d, v, std::vector<short> *);
  TEST_IMPL(data &, d, fcr, float const *);

  TEST_IMPL(data &, l, i, int *);
  TEST_IMPL(data &, l, scl, std::string const *);
  TEST_IMPL(data &, l, lr, long *);
  TEST_IMPL(data &, l, dc, double const *);
  TEST_IMPL(data &, l, bl, bool *);
  TEST_IMPL(data &, l, v, std::vector<short> *);
  TEST_IMPL(data &, l, fcr, float const *);

  TEST_IMPL(data &, r, i, int *);
  TEST_IMPL(data &, r, scl, std::string const *);
  TEST_IMPL(data &, r, lr, long *);
  TEST_IMPL(data &, r, dc, double const *);
  TEST_IMPL(data &, r, bl, bool *);
  TEST_IMPL(data &, r, v, std::vector<short> *);
  TEST_IMPL(data &, r, fcr, float const *);

  TEST_IMPL(data &, cl, i, int const *);
  TEST_IMPL(data &, cl, scl, std::string const *);
  TEST_IMPL(data &, cl, lr, long const *);
  TEST_IMPL(data &, cl, dc, double const *);
  TEST_IMPL(data &, cl, bl, bool const *);
  TEST_IMPL(data &, cl, v, std::vector<short> const *);
  TEST_IMPL(data &, cl, fcr, float const *);

  TEST_IMPL(data &, cr, i, int const *);
  TEST_IMPL(data &, cr, scl, std::string const *);
  TEST_IMPL(data &, cr, lr, long const *);
  TEST_IMPL(data &, cr, dc, double const *);
  TEST_IMPL(data &, cr, bl, bool const *);
  TEST_IMPL(data &, cr, v, std::vector<short> const *);
  TEST_IMPL(data &, cr, fcr, float const *);

  TEST_IMPL(data &&, d, i, int *);
  TEST_IMPL(data &&, d, scl, std::string const *);
  TEST_IMPL(data &&, d, lr, long *);
  TEST_IMPL(data &&, d, dc, double const *);
  TEST_IMPL(data &&, d, bl, bool *);
  TEST_IMPL(data &&, d, v, std::vector<short> *);
  TEST_IMPL(data &&, d, fcr, float const *);

  TEST_IMPL(data &&, l, i, int *);
  TEST_IMPL(data &&, l, scl, std::string const *);
  TEST_IMPL(data &&, l, lr, long *);
  TEST_IMPL(data &&, l, dc, double const *);
  TEST_IMPL(data &&, l, bl, bool *);
  TEST_IMPL(data &&, l, v, std::vector<short> *);
  TEST_IMPL(data &&, l, fcr, float const *);

  TEST_IMPL(data &&, r, i, int *);
  TEST_IMPL(data &&, r, scl, std::string const *);
  TEST_IMPL(data &&, r, lr, long *);
  TEST_IMPL(data &&, r, dc, double const *);
  TEST_IMPL(data &&, r, bl, bool *);
  TEST_IMPL(data &&, r, v, std::vector<short> *);
  TEST_IMPL(data &&, r, fcr, float const *);

  TEST_IMPL(data &&, cl, i, int const *);
  TEST_IMPL(data &&, cl, scl, std::string const *);
  TEST_IMPL(data &&, cl, lr, long const *);
  TEST_IMPL(data &&, cl, dc, double const *);
  TEST_IMPL(data &&, cl, bl, bool const *);
  TEST_IMPL(data &&, cl, v, std::vector<short> const *);
  TEST_IMPL(data &&, cl, fcr, float const *);

  TEST_IMPL(data &&, cr, i, int const *);
  TEST_IMPL(data &&, cr, scl, std::string const *);
  TEST_IMPL(data &&, cr, lr, long const *);
  TEST_IMPL(data &&, cr, dc, double const *);
  TEST_IMPL(data &&, cr, bl, bool const *);
  TEST_IMPL(data &&, cr, v, std::vector<short> const *);
  TEST_IMPL(data &&, cr, fcr, float const *);

  TEST_IMPL(data const, d, i, int const  *);
  TEST_IMPL(data const, d, scl, std::string const  *);
  TEST_IMPL(data const, d, lr, long const  *);
  TEST_IMPL(data const, d, dc, double const  *);
  TEST_IMPL(data const, d, bl, bool const  *);
  TEST_IMPL(data const, d, v, std::vector<short> const  *);
  TEST_IMPL(data const, d, fcr, float const  *);

  TEST_IMPL(data const, l, i, int const  *);
  TEST_IMPL(data const, l, scl, std::string const  *);
  TEST_IMPL(data const, l, lr, long const  *);
  TEST_IMPL(data const, l, dc, double const  *);
  TEST_IMPL(data const, l, bl, bool const  *);
  TEST_IMPL(data const, l, v, std::vector<short> const  *);
  TEST_IMPL(data const, l, fcr, float const  *);

  TEST_IMPL(data const, r, i, int const  *);
  TEST_IMPL(data const, r, scl, std::string const  *);
  TEST_IMPL(data const, r, lr, long const  *);
  TEST_IMPL(data const, r, dc, double const  *);
  TEST_IMPL(data const, r, bl, bool const  *);
  TEST_IMPL(data const, r, v, std::vector<short> const  *);
  TEST_IMPL(data const, r, fcr, float const  *);

  TEST_IMPL(data const, cl, i, int const  *);
  TEST_IMPL(data const, cl, scl, std::string const  *);
  TEST_IMPL(data const, cl, lr, long const  *);
  TEST_IMPL(data const, cl, dc, double const  *);
  TEST_IMPL(data const, cl, bl, bool const  *);
  TEST_IMPL(data const, cl, v, std::vector<short> const  *);
  TEST_IMPL(data const, cl, fcr, float const  *);

  TEST_IMPL(data const, cr, i, int const  *);
  TEST_IMPL(data const, cr, scl, std::string const  *);
  TEST_IMPL(data const, cr, lr, long const  *);
  TEST_IMPL(data const, cr, dc, double const  *);
  TEST_IMPL(data const, cr, bl, bool const  *);
  TEST_IMPL(data const, cr, v, std::vector<short> const  *);
  TEST_IMPL(data const, cr, fcr, float const  *);

  TEST_IMPL(data const &, d, i, int const  *);
  TEST_IMPL(data const &, d, scl, std::string const  *);
  TEST_IMPL(data const &, d, lr, long const  *);
  TEST_IMPL(data const &, d, dc, double const  *);
  TEST_IMPL(data const &, d, bl, bool const  *);
  TEST_IMPL(data const &, d, v, std::vector<short> const  *);
  TEST_IMPL(data const &, d, fcr, float const  *);

  TEST_IMPL(data const &, l, i, int const  *);
  TEST_IMPL(data const &, l, scl, std::string const  *);
  TEST_IMPL(data const &, l, lr, long const  *);
  TEST_IMPL(data const &, l, dc, double const  *);
  TEST_IMPL(data const &, l, bl, bool const  *);
  TEST_IMPL(data const &, l, v, std::vector<short> const  *);
  TEST_IMPL(data const &, l, fcr, float const  *);

  TEST_IMPL(data const &, r, i, int const  *);
  TEST_IMPL(data const &, r, scl, std::string const  *);
  TEST_IMPL(data const &, r, lr, long const  *);
  TEST_IMPL(data const &, r, dc, double const  *);
  TEST_IMPL(data const &, r, bl, bool const  *);
  TEST_IMPL(data const &, r, v, std::vector<short> const  *);
  TEST_IMPL(data const &, r, fcr, float const  *);

  TEST_IMPL(data const &, cl, i, int const  *);
  TEST_IMPL(data const &, cl, scl, std::string const  *);
  TEST_IMPL(data const &, cl, lr, long const  *);
  TEST_IMPL(data const &, cl, dc, double const  *);
  TEST_IMPL(data const &, cl, bl, bool const  *);
  TEST_IMPL(data const &, cl, v, std::vector<short> const  *);
  TEST_IMPL(data const &, cl, fcr, float const  *);

  TEST_IMPL(data const &, cr, i, int const  *);
  TEST_IMPL(data const &, cr, scl, std::string const  *);
  TEST_IMPL(data const &, cr, lr, long const  *);
  TEST_IMPL(data const &, cr, dc, double const  *);
  TEST_IMPL(data const &, cr, bl, bool const  *);
  TEST_IMPL(data const &, cr, v, std::vector<short> const  *);
  TEST_IMPL(data const &, cr, fcr, float const  *);

  TEST_IMPL(data const &&, d, i, int const  *);
  TEST_IMPL(data const &&, d, scl, std::string const  *);
  TEST_IMPL(data const &&, d, lr, long const  *);
  TEST_IMPL(data const &&, d, dc, double const  *);
  TEST_IMPL(data const &&, d, bl, bool const  *);
  TEST_IMPL(data const &&, d, v, std::vector<short> const  *);
  TEST_IMPL(data const &&, d, fcr, float const  *);

  TEST_IMPL(data const &&, l, i, int const  *);
  TEST_IMPL(data const &&, l, scl, std::string const  *);
  TEST_IMPL(data const &&, l, lr, long const  *);
  TEST_IMPL(data const &&, l, dc, double const  *);
  TEST_IMPL(data const &&, l, bl, bool const  *);
  TEST_IMPL(data const &&, l, v, std::vector<short> const  *);
  TEST_IMPL(data const &&, l, fcr, float const  *);

  TEST_IMPL(data const &&, r, i, int const  *);
  TEST_IMPL(data const &&, r, scl, std::string const  *);
  TEST_IMPL(data const &&, r, lr, long const  *);
  TEST_IMPL(data const &&, r, dc, double const  *);
  TEST_IMPL(data const &&, r, bl, bool const  *);
  TEST_IMPL(data const &&, r, v, std::vector<short> const  *);
  TEST_IMPL(data const &&, r, fcr, float const  *);

  TEST_IMPL(data const &&, cl, i, int const  *);
  TEST_IMPL(data const &&, cl, scl, std::string const  *);
  TEST_IMPL(data const &&, cl, lr, long const  *);
  TEST_IMPL(data const &&, cl, dc, double const  *);
  TEST_IMPL(data const &&, cl, bl, bool const  *);
  TEST_IMPL(data const &&, cl, v, std::vector<short> const  *);
  TEST_IMPL(data const &&, cl, fcr, float const  *);

  TEST_IMPL(data const &&, cr, i, int const  *);
  TEST_IMPL(data const &&, cr, scl, std::string const  *);
  TEST_IMPL(data const &&, cr, lr, long const  *);
  TEST_IMPL(data const &&, cr, dc, double const  *);
  TEST_IMPL(data const &&, cr, bl, bool const  *);
  TEST_IMPL(data const &&, cr, v, std::vector<short> const  *);
  TEST_IMPL(data const &&, cr, fcr, float const  *);

# undef TEST_IMPL
}

FATAL_TEST(chained_data_member_getter, getter) {
  int i = 99;
  std::string const scl("hello, world!");
  long lr = 27;
  double const dc = 5.6;
  bool bl = true;
  std::vector<short> v{1, 2, 3, 4, 5, 6};
  float const fcr = 7.2f;

# define TEST_IMPL(Data) \
  do { \
    using getter = chained_data_member_getter<>; \
    \
    FATAL_EXPECT_EQ(Data, getter::ref(Data)); \
    FATAL_EXPECT_EQ(Data, data_member_referencer<getter>()(Data)); \
    \
    FATAL_EXPECT_EQ(Data, getter::ref(std::move(Data))); \
    FATAL_EXPECT_EQ(Data, data_member_referencer<getter>()(std::move(Data))); \
    \
    FATAL_EXPECT_EQ( \
      std::addressof(Data), \
      std::addressof(getter::ref(Data)) \
    ); \
    \
    FATAL_EXPECT_EQ( \
      std::addressof(Data), \
      std::addressof(data_member_referencer<getter>()(Data)) \
    ); \
    \
    FATAL_EXPECT_SAME<decltype(Data) &, decltype(getter::ref(Data))>(); \
    \
    FATAL_EXPECT_SAME< \
      decltype(Data) &, \
      decltype(data_member_referencer<getter>()(Data)) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      decltype(Data) &&, \
      decltype(getter::ref(std::move(Data))) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      decltype(Data) &&, \
      decltype(data_member_referencer<getter>()(std::move(Data))) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      std::remove_reference<decltype(Data)>::type *, \
      decltype(getter::ptr(Data)) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      std::remove_reference<decltype(Data)>::type *, \
      decltype(data_member_pointer<getter>()(Data)) \
    >(); \
  } while (false)

  TEST_IMPL(i);
  TEST_IMPL(scl);
  TEST_IMPL(lr);
  TEST_IMPL(dc);
  TEST_IMPL(bl);
  TEST_IMPL(v);
  TEST_IMPL(fcr);

# undef TEST_IMPL

  int d_i = 99;
  std::string const d_scl("hello, world!");
  long d_lr = 27;
  double const d_dc = 5.6;
  bool d_bl = true;
  std::vector<short> d_v{1, 2, 3, 4, 5, 6};
  float const d_fcr = 7.2f;
  data_member_getter_test::data d(
    d_i, d_scl, std::move(d_lr), d_dc, d_bl, d_v, std::move(d_fcr)
  );

  FATAL_ASSERT_EQ(d_i, d.i);
  FATAL_ASSERT_EQ(d_scl, d.scl);
  FATAL_ASSERT_EQ(d_lr, d.lr);
  FATAL_ASSERT_EQ(d_dc, d.dc);
  FATAL_ASSERT_EQ(d_bl, d.bl);
  FATAL_ASSERT_EQ(d_v, d.v);
  FATAL_ASSERT_EQ(d_fcr, d.fcr);

  int l_i = 99;
  std::string const l_scl("hello, world!");
  long l_lr = 27;
  double const l_dc = 5.6;
  bool l_bl = true;
  std::vector<short> l_v{1, 2, 3, 4, 5, 6};
  float const l_fcr = 7.2f;
  data_member_getter_test::data l(
    l_i, l_scl, std::move(l_lr), l_dc, l_bl, l_v, std::move(l_fcr)
  );

  FATAL_ASSERT_EQ(l_i, l.i);
  FATAL_ASSERT_EQ(l_scl, l.scl);
  FATAL_ASSERT_EQ(l_lr, l.lr);
  FATAL_ASSERT_EQ(l_dc, l.dc);
  FATAL_ASSERT_EQ(l_bl, l.bl);
  FATAL_ASSERT_EQ(l_v, l.v);
  FATAL_ASSERT_EQ(l_fcr, l.fcr);

  int r_i = 99;
  std::string const r_scl("hello, world!");
  long r_lr = 27;
  double const r_dc = 5.6;
  bool r_bl = true;
  std::vector<short> r_v{1, 2, 3, 4, 5, 6};
  float const r_fcr = 7.2f;
  data_member_getter_test::data r(
    r_i, r_scl, std::move(r_lr), r_dc, r_bl, r_v, std::move(r_fcr)
  );

  FATAL_ASSERT_EQ(r_i, r.i);
  FATAL_ASSERT_EQ(r_scl, r.scl);
  FATAL_ASSERT_EQ(r_lr, r.lr);
  FATAL_ASSERT_EQ(r_dc, r.dc);
  FATAL_ASSERT_EQ(r_bl, r.bl);
  FATAL_ASSERT_EQ(r_v, r.v);
  FATAL_ASSERT_EQ(r_fcr, r.fcr);

  int cl_i = 99;
  std::string const cl_scl("hello, world!");
  long cl_lr = 27;
  double const cl_dc = 5.6;
  bool cl_bl = true;
  std::vector<short> cl_v{1, 2, 3, 4, 5, 6};
  float const cl_fcr = 7.2f;
  data_member_getter_test::data cl(
    cl_i, cl_scl, std::move(cl_lr), cl_dc, cl_bl, cl_v, std::move(cl_fcr)
  );

  FATAL_ASSERT_EQ(cl_i, cl.i);
  FATAL_ASSERT_EQ(cl_scl, cl.scl);
  FATAL_ASSERT_EQ(cl_lr, cl.lr);
  FATAL_ASSERT_EQ(cl_dc, cl.dc);
  FATAL_ASSERT_EQ(cl_bl, cl.bl);
  FATAL_ASSERT_EQ(cl_v, cl.v);
  FATAL_ASSERT_EQ(cl_fcr, cl.fcr);

  int cr_i = 99;
  std::string const cr_scl("hello, world!");
  long cr_lr = 27;
  double const cr_dc = 5.6;
  bool cr_bl = true;
  std::vector<short> cr_v{1, 2, 3, 4, 5, 6};
  float const cr_fcr = 7.2f;
  data_member_getter_test::data cr(
    cr_i, cr_scl, std::move(cr_lr), cr_dc, cr_bl, cr_v, std::move(cr_fcr)
  );

  FATAL_ASSERT_EQ(cr_i, cr.i);
  FATAL_ASSERT_EQ(cr_scl, cr.scl);
  FATAL_ASSERT_EQ(cr_lr, cr.lr);
  FATAL_ASSERT_EQ(cr_dc, cr.dc);
  FATAL_ASSERT_EQ(cr_bl, cr.bl);
  FATAL_ASSERT_EQ(cr_v, cr.v);
  FATAL_ASSERT_EQ(cr_fcr, cr.fcr);

  data_member_getter_test::chained_data x(
    d, l, std::move(r), cl, std::move(cr)
  );

  FATAL_ASSERT_EQ(d, x.d);
  FATAL_ASSERT_EQ(l, x.l);
  FATAL_ASSERT_EQ(r, x.r);
  FATAL_ASSERT_EQ(cl, x.cl);
  FATAL_ASSERT_EQ(cr, x.cr);

  auto const &y = x;

  FATAL_ASSERT_EQ(d, y.d);
  FATAL_ASSERT_EQ(l, y.l);
  FATAL_ASSERT_EQ(r, y.r);
  FATAL_ASSERT_EQ(cl, y.cl);
  FATAL_ASSERT_EQ(cr, y.cr);

# define TEST_PREAMBLE_IMPL(Outer, Inner) \
    using getter = chained_data_member_getter< \
      data_member_getter_test::getter::Outer, \
      data_member_getter_test::getter::Inner \
    >;

# define TEST_IMPL(Data, Outer, Inner) \
  do { \
    TEST_PREAMBLE_IMPL(Outer, Inner); \
    \
    FATAL_EXPECT_EQ(Outer##_##Inner, getter::ref(Data)); \
    FATAL_EXPECT_EQ(Outer##_##Inner, data_member_referencer<getter>()(Data)); \
    \
    FATAL_EXPECT_EQ(Data.Outer.Inner, getter::ref(Data)); \
    FATAL_EXPECT_EQ(Data.Outer.Inner, data_member_referencer<getter>()(Data)); \
    \
    FATAL_EXPECT_EQ( \
      std::addressof(Data.Outer.Inner), \
      std::addressof(getter::ref(Data)) \
    ); \
    \
    FATAL_EXPECT_EQ( \
      std::addressof(Data.Outer.Inner), \
      std::addressof(data_member_referencer<getter>()(Data)) \
    ); \
    \
    FATAL_EXPECT_SAME< \
      std::add_lvalue_reference< \
        constify_from< \
          decltype(Data.Outer.Inner), \
          constify_from< \
            std::remove_reference<decltype(Data)>::type, \
            std::remove_reference<decltype(Data.Outer)>::type \
          >::type \
        >::type \
      >::type, \
      decltype(getter::ref(Data)) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      std::add_lvalue_reference< \
        constify_from< \
          decltype(Data.Outer.Inner), \
          constify_from< \
            std::remove_reference<decltype(Data)>::type, \
            std::remove_reference<decltype(Data.Outer)>::type \
          >::type \
        >::type \
      >::type, \
      decltype(data_member_referencer<getter>()(Data)) \
    >(); \
  } while (false)

  TEST_IMPL(x, d, i);
  TEST_IMPL(x, d, scl);
  TEST_IMPL(x, d, lr);
  TEST_IMPL(x, d, dc);
  TEST_IMPL(x, d, bl);
  TEST_IMPL(x, d, v);
  TEST_IMPL(x, d, fcr);

  TEST_IMPL(y, d, i);
  TEST_IMPL(y, d, scl);
  TEST_IMPL(y, d, lr);
  TEST_IMPL(y, d, dc);
  TEST_IMPL(y, d, bl);
  TEST_IMPL(y, d, v);
  TEST_IMPL(y, d, fcr);

  TEST_IMPL(x, l, i);
  TEST_IMPL(x, l, scl);
  TEST_IMPL(x, l, lr);
  TEST_IMPL(x, l, dc);
  TEST_IMPL(x, l, bl);
  TEST_IMPL(x, l, v);
  TEST_IMPL(x, l, fcr);

  TEST_IMPL(y, l, i);
  TEST_IMPL(y, l, scl);
  TEST_IMPL(y, l, lr);
  TEST_IMPL(y, l, dc);
  TEST_IMPL(y, l, bl);
  TEST_IMPL(y, l, v);
  TEST_IMPL(y, l, fcr);

  TEST_IMPL(x, r, i);
  TEST_IMPL(x, r, scl);
  TEST_IMPL(x, r, lr);
  TEST_IMPL(x, r, dc);
  TEST_IMPL(x, r, bl);
  TEST_IMPL(x, r, v);
  TEST_IMPL(x, r, fcr);

  TEST_IMPL(y, r, i);
  TEST_IMPL(y, r, scl);
  TEST_IMPL(y, r, lr);
  TEST_IMPL(y, r, dc);
  TEST_IMPL(y, r, bl);
  TEST_IMPL(y, r, v);
  TEST_IMPL(y, r, fcr);

  TEST_IMPL(x, cl, i);
  TEST_IMPL(x, cl, scl);
  TEST_IMPL(x, cl, lr);
  TEST_IMPL(x, cl, dc);
  TEST_IMPL(x, cl, bl);
  TEST_IMPL(x, cl, v);
  TEST_IMPL(x, cl, fcr);

  TEST_IMPL(y, cl, i);
  TEST_IMPL(y, cl, scl);
  TEST_IMPL(y, cl, lr);
  TEST_IMPL(y, cl, dc);
  TEST_IMPL(y, cl, bl);
  TEST_IMPL(y, cl, v);
  TEST_IMPL(y, cl, fcr);

  TEST_IMPL(x, cr, i);
  TEST_IMPL(x, cr, scl);
  TEST_IMPL(x, cr, lr);
  TEST_IMPL(x, cr, dc);
  TEST_IMPL(x, cr, bl);
  TEST_IMPL(x, cr, v);
  TEST_IMPL(x, cr, fcr);

  TEST_IMPL(y, cr, i);
  TEST_IMPL(y, cr, scl);
  TEST_IMPL(y, cr, lr);
  TEST_IMPL(y, cr, dc);
  TEST_IMPL(y, cr, bl);
  TEST_IMPL(y, cr, v);
  TEST_IMPL(y, cr, fcr);

# undef TEST_IMPL
# define TEST_IMPL(Data, Outer, Inner) \
  do { \
    TEST_PREAMBLE_IMPL(Outer, Inner); \
    \
    FATAL_EXPECT_EQ(Outer##_##Inner, getter::ref(std::move(Data))); \
    FATAL_EXPECT_EQ( \
      Outer##_##Inner, \
      data_member_referencer<getter>()(std::move(Data)) \
    ); \
    \
    FATAL_EXPECT_EQ( \
      std::move(Data).Outer.Inner, \
      getter::ref(std::move(Data)) \
    ); \
    FATAL_EXPECT_EQ( \
      std::move(Data).Outer.Inner, \
      data_member_referencer<getter>()(std::move(Data)) \
    ); \
    \
    FATAL_EXPECT_SAME< \
      add_reference_from_t< \
        constify_from< \
          decltype(std::move(Data.Outer).Inner), \
          constify_from< \
            std::remove_reference<decltype(std::move(Data))>::type, \
            std::remove_reference<decltype(Data.Outer)>::type \
          >::type \
        >::type &&, \
        decltype(std::move(Data).Outer) \
      >, \
      decltype(getter::ref(std::move(Data))) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      add_reference_from_t< \
        constify_from< \
          decltype(std::move(Data).Outer.Inner), \
          constify_from< \
            std::remove_reference<decltype(std::move(Data))>::type, \
            std::remove_reference<decltype(Data.Outer)>::type \
          >::type \
        >::type &&, \
        decltype(std::move(Data).Outer) \
      >, \
      decltype(data_member_referencer<getter>()(std::move(Data))) \
    >(); \
  } while (false)

  TEST_IMPL(x, d, i);
  TEST_IMPL(x, d, scl);
  TEST_IMPL(x, d, lr);
  TEST_IMPL(x, d, dc);
  TEST_IMPL(x, d, bl);
  TEST_IMPL(x, d, v);
  TEST_IMPL(x, d, fcr);

  TEST_IMPL(y, d, i);
  TEST_IMPL(y, d, scl);
  TEST_IMPL(y, d, lr);
  TEST_IMPL(y, d, dc);
  TEST_IMPL(y, d, bl);
  TEST_IMPL(y, d, v);
  TEST_IMPL(y, d, fcr);

  TEST_IMPL(x, l, i);
  TEST_IMPL(x, l, scl);
  TEST_IMPL(x, l, lr);
  TEST_IMPL(x, l, dc);
  TEST_IMPL(x, l, bl);
  TEST_IMPL(x, l, v);
  TEST_IMPL(x, l, fcr);

  TEST_IMPL(y, l, i);
  TEST_IMPL(y, l, scl);
  TEST_IMPL(y, l, lr);
  TEST_IMPL(y, l, dc);
  TEST_IMPL(y, l, bl);
  TEST_IMPL(y, l, v);
  TEST_IMPL(y, l, fcr);

  TEST_IMPL(x, r, i);
  TEST_IMPL(x, r, scl);
  TEST_IMPL(x, r, lr);
  TEST_IMPL(x, r, dc);
  TEST_IMPL(x, r, bl);
  TEST_IMPL(x, r, v);
  TEST_IMPL(x, r, fcr);

  TEST_IMPL(y, r, i);
  TEST_IMPL(y, r, scl);
  TEST_IMPL(y, r, lr);
  TEST_IMPL(y, r, dc);
  TEST_IMPL(y, r, bl);
  TEST_IMPL(y, r, v);
  TEST_IMPL(y, r, fcr);

  TEST_IMPL(x, cl, i);
  TEST_IMPL(x, cl, scl);
  TEST_IMPL(x, cl, lr);
  TEST_IMPL(x, cl, dc);
  TEST_IMPL(x, cl, bl);
  TEST_IMPL(x, cl, v);
  TEST_IMPL(x, cl, fcr);

  TEST_IMPL(y, cl, i);
  TEST_IMPL(y, cl, scl);
  TEST_IMPL(y, cl, lr);
  TEST_IMPL(y, cl, dc);
  TEST_IMPL(y, cl, bl);
  TEST_IMPL(y, cl, v);
  TEST_IMPL(y, cl, fcr);

  TEST_IMPL(x, cr, i);
  TEST_IMPL(x, cr, scl);
  TEST_IMPL(x, cr, lr);
  TEST_IMPL(x, cr, dc);
  TEST_IMPL(x, cr, bl);
  TEST_IMPL(x, cr, v);
  TEST_IMPL(x, cr, fcr);

  TEST_IMPL(y, cr, i);
  TEST_IMPL(y, cr, scl);
  TEST_IMPL(y, cr, lr);
  TEST_IMPL(y, cr, dc);
  TEST_IMPL(y, cr, bl);
  TEST_IMPL(y, cr, v);
  TEST_IMPL(y, cr, fcr);

# undef TEST_IMPL
# define TEST_IMPL(Data, Outer, Inner) \
  do { \
    TEST_PREAMBLE_IMPL(Outer, Inner); \
    \
    FATAL_EXPECT_EQ(std::addressof(Data.Outer.Inner), getter::ptr(Data)); \
    FATAL_EXPECT_EQ( \
      std::addressof(Data.Outer.Inner), \
      data_member_pointer<getter>()(Data) \
    ); \
    \
    FATAL_EXPECT_SAME< \
      constify_from< \
        std::remove_pointer<decltype(std::addressof(Data.Outer.Inner))>::type, \
        constify_from< \
          std::remove_reference<decltype(Data)>::type, \
          std::remove_reference<decltype(Data.Outer)>::type \
        >::type \
      >::type *, \
      decltype(getter::ptr(Data)) \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      constify_from< \
        std::remove_pointer<decltype(std::addressof(Data.Outer.Inner))>::type, \
        constify_from< \
          std::remove_reference<decltype(Data)>::type, \
          std::remove_reference<decltype(Data.Outer)>::type \
        >::type \
      >::type *, \
      decltype(data_member_pointer<getter>()(Data)) \
    >(); \
  } while (false)

  TEST_IMPL(x, d, i);
  TEST_IMPL(x, d, scl);
  TEST_IMPL(x, d, lr);
  TEST_IMPL(x, d, dc);
  TEST_IMPL(x, d, bl);
  TEST_IMPL(x, d, v);
  TEST_IMPL(x, d, fcr);

  TEST_IMPL(y, d, i);
  TEST_IMPL(y, d, scl);
  TEST_IMPL(y, d, lr);
  TEST_IMPL(y, d, dc);
  TEST_IMPL(y, d, bl);
  TEST_IMPL(y, d, v);
  TEST_IMPL(y, d, fcr);

  TEST_IMPL(x, l, i);
  TEST_IMPL(x, l, scl);
  TEST_IMPL(x, l, lr);
  TEST_IMPL(x, l, dc);
  TEST_IMPL(x, l, bl);
  TEST_IMPL(x, l, v);
  TEST_IMPL(x, l, fcr);

  TEST_IMPL(y, l, i);
  TEST_IMPL(y, l, scl);
  TEST_IMPL(y, l, lr);
  TEST_IMPL(y, l, dc);
  TEST_IMPL(y, l, bl);
  TEST_IMPL(y, l, v);
  TEST_IMPL(y, l, fcr);

  TEST_IMPL(x, r, i);
  TEST_IMPL(x, r, scl);
  TEST_IMPL(x, r, lr);
  TEST_IMPL(x, r, dc);
  TEST_IMPL(x, r, bl);
  TEST_IMPL(x, r, v);
  TEST_IMPL(x, r, fcr);

  TEST_IMPL(y, r, i);
  TEST_IMPL(y, r, scl);
  TEST_IMPL(y, r, lr);
  TEST_IMPL(y, r, dc);
  TEST_IMPL(y, r, bl);
  TEST_IMPL(y, r, v);
  TEST_IMPL(y, r, fcr);

  TEST_IMPL(x, cl, i);
  TEST_IMPL(x, cl, scl);
  TEST_IMPL(x, cl, lr);
  TEST_IMPL(x, cl, dc);
  TEST_IMPL(x, cl, bl);
  TEST_IMPL(x, cl, v);
  TEST_IMPL(x, cl, fcr);

  TEST_IMPL(y, cl, i);
  TEST_IMPL(y, cl, scl);
  TEST_IMPL(y, cl, lr);
  TEST_IMPL(y, cl, dc);
  TEST_IMPL(y, cl, bl);
  TEST_IMPL(y, cl, v);
  TEST_IMPL(y, cl, fcr);

  TEST_IMPL(x, cr, i);
  TEST_IMPL(x, cr, scl);
  TEST_IMPL(x, cr, lr);
  TEST_IMPL(x, cr, dc);
  TEST_IMPL(x, cr, bl);
  TEST_IMPL(x, cr, v);
  TEST_IMPL(x, cr, fcr);

  TEST_IMPL(y, cr, i);
  TEST_IMPL(y, cr, scl);
  TEST_IMPL(y, cr, lr);
  TEST_IMPL(y, cr, dc);
  TEST_IMPL(y, cr, bl);
  TEST_IMPL(y, cr, v);
  TEST_IMPL(y, cr, fcr);

# undef TEST_IMPL
# undef TEST_PREAMBLE_IMPL
}

} // namespace fatal {
