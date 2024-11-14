#include <fatal/functional/no_op.h>
#include <fatal/type/apply.h>
#include <fatal/type/array.h>
#include <fatal/type/cat.h>
#include <fatal/type/compare.h>
#include <fatal/type/convert.h>
#include <fatal/type/find.h>
#include <fatal/type/foreach.h>
#include <fatal/type/get.h>
#include <fatal/type/group_by.h>
#include <fatal/type/list.h>
#include <fatal/type/logical.h>
#include <fatal/type/longest_common_prefix.h>
#include <fatal/type/pair.h>
#include <fatal/type/replace.h>
#include <fatal/type/search.h>
#include <fatal/type/select.h>
#include <fatal/type/sequence.h>
#include <fatal/type/size.h>
#include <fatal/type/slice.h>
#include <fatal/type/sort.h>
#include <fatal/type/split.h>
#include <fatal/type/tag.h>
#include <fatal/type/transform.h>
#include <fatal/type/type.h>
#include <fatal/type/unique.h>
#include <fatal/type/zip.h>

#include <fatal/test/driver.h>
#include <fatal/test/type.h>

#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <cstdint>

namespace fatal {

// helpers /////////////////////////////////////////////////////////////////////

namespace impl_detail {

template <typename T, typename U>
struct same_impl {
  static_assert(std::is_same<T, U>::value );
};

#define SAME(...) impl_detail::same_impl<__VA_ARGS__>()
#define IST(...) SAME(std::true_type, __VA_ARGS__)
#define ISF(...) SAME(std::false_type, __VA_ARGS__)

template <typename T, decltype(T::value) U>
struct equal_impl {
  static_assert(T::value == U );
};

#define EQUAL(Expected, ...) impl_detail::equal_impl<__VA_ARGS__, Expected>()

#define EQ(...) FATAL_EXPECT_EQ(__VA_ARGS__)

#define EQT(...) EQ(true, __VA_ARGS__)
#define EQF(...) EQ(false, __VA_ARGS__)

} // namespace impl_detail {

// test types //////////////////////////////////////////////////////////////////

template <typename...> struct t0 {};
template <typename...> struct t1 {};
template <typename...> struct t2 {};
template <typename...> struct t3 {};
template <typename...> struct t4 {};
template <typename...> struct t5 {};
template <typename...> struct t6 {};
template <typename...> struct t7 {};
template <typename...> struct t8 {};
template <typename...> struct t9 {};
template <typename...> struct t10 {};

struct t_0 { template <typename... Args> using apply = t0<Args...>; };
struct t_1 { template <typename... Args> using apply = t1<Args...>; };
struct t_2 { template <typename... Args> using apply = t2<Args...>; };
struct t_3 { template <typename... Args> using apply = t3<Args...>; };
struct t_4 { template <typename... Args> using apply = t4<Args...>; };
struct t_5 { template <typename... Args> using apply = t5<Args...>; };
struct t_6 { template <typename... Args> using apply = t6<Args...>; };
struct t_7 { template <typename... Args> using apply = t7<Args...>; };
struct t_8 { template <typename... Args> using apply = t8<Args...>; };
struct t_9 { template <typename... Args> using apply = t9<Args...>; };
struct t_10 { template <typename... Args> using apply = t10<Args...>; };

template <char Value>
using ch = std::integral_constant<char, Value>;

template <std::size_t Value>
using sz = std::integral_constant<std::size_t, Value>;

template <std::size_t> struct sz_val {};

template <typename T>
using to_sz_val = sz_val<T::value>;

template <typename>
using all = std::true_type;

struct none {
  template <typename>
  using apply = std::false_type;
};

template <std::size_t Size>
struct size_eq {
  template <typename T>
  using apply = std::integral_constant<bool, size<T>::value == Size>;
};

template <typename T>
using is_odd = std::integral_constant<bool, (T::value & 1) != 1>;

template <typename T>
using is_even = std::integral_constant<bool, !(T::value & 1)>;

template <std::size_t... Values>
using sz_list = list<std::integral_constant<std::size_t, Values>...>;

template <typename T>
struct dummy {
  using type = T;
};

template <typename...> struct test_list {};
template <typename T, T...> struct test_seq {};

template <std::size_t V>
using mp_entry = pair<sz<V>, dummy<sz<V>>>;

template <typename T>
using to_map_entry = pair<T, dummy<T>>;

template <typename... Expected>
struct test_search_visitor {
  void operator ()(Expected...) const {}

  template <typename... Args>
  void operator ()(Args &&...) const {
    std::cout << "assertion failed:"
      << std::endl << "  expected " << type_str<list<Expected...>>()
      << std::endl << "  actual " << type_str<list<Args...>>()
      << std::endl;
  }
};

// experiments /////////////////////////////////////////////////////////////////

// main ////////////////////////////////////////////////////////////////////////

template <std::size_t... Values>
void test_cat() {
  SAME(index_list<Values...>, cat<index_list<Values>...>);
  SAME(index_sequence<Values...>, cat<index_sequence<Values>...>);
}

template <std::size_t... Head>
struct test_head {
  template <std::size_t... Tail>
  static void go() {
    SAME(
      index_list<Head...>,
      head<index_list<Head..., Tail...>, sizeof...(Head)>
    );
    SAME(
      index_sequence<Head...>,
      head<index_sequence<Head..., Tail...>, sizeof...(Head)>
    );
  }
};

template <typename... Args>
using tst_longest_common_prefix = longest_common_prefix<
  at,
  0,
  vmin<less, size<Args>...>::value,
  Args...
>;

#define TEST_AS_ARRAY(Which, ...) \
  do { \
    using array = as_array<str::Which::__VA_ARGS__>; \
    SAME(size<str::Which::__VA_ARGS__>, array::size); \
    std::string const expected(#__VA_ARGS__); \
    std::string const actual(array::data, array::size::value); \
    if (expected != actual) { \
      EQ(expected, actual); \
      std::cout << "  expected='" << expected << "' actual='" << actual << '\''\
        << std::endl; \
    } \
  } while (false)

struct test_as_array_from_factory {
  template <typename T>
  static constexpr decltype(T::value) get() { return T::value; }
};

#define TEST_AS_ARRAY_FROM(Which, ...) \
  do { \
    using array = as_array_from< \
      str::Which::__VA_ARGS__, \
      test_as_array_from_factory \
    >; \
    SAME(size<str::Which::__VA_ARGS__>, array::size); \
    std::string const expected(#__VA_ARGS__); \
    std::string const actual(array::data, array::size::value); \
    if (expected != actual) { \
      EQ(expected, actual); \
      std::cout << "  expected='" << expected << "' actual='" << actual << '\''\
        << std::endl; \
    } \
  } while (false)

#define TEST_Z_DATA(Which, ...) \
  do { \
    std::string const expected(#__VA_ARGS__); \
    std::string const actual(z_data<str::Which::__VA_ARGS__>()); \
    if (expected != actual) { \
      EQ(expected, actual); \
      std::cout << "  expected='" << expected << "' actual='" << actual << '\''\
        << std::endl; \
    } \
  } while (false)

struct str {
  struct misc {
    FATAL_S(foo, "foo");
    FATAL_S(foobar, "foobar");
    FATAL_S(foobaz, "foobaz");
    FATAL_S(foobAr, "foobAr");
  };

  struct fn {
    template <typename T>
    using group_by = pair<
      first<T>,
      transform<second<T>, bound::as_list<list>>
    >;
  };

  struct seq {
    using gooey = sequence<char, 'g', 'o', 'o', 'e', 'y'>;
    using fast = sequence<char, 'f', 'a', 's', 't'>;
    using granite = sequence<char, 'g', 'r', 'a', 'n', 'i', 't', 'e'>;
    using fastest = sequence<char, 'f', 'a', 's', 't', 'e', 's', 't'>;
    using fart = sequence<char, 'f', 'a', 'r', 't'>;
    using far = sequence<char, 'f', 'a', 'r'>;
    using good = sequence<char, 'g', 'o', 'o', 'd'>;
    using great = sequence<char, 'g', 'r', 'e', 'a', 't'>;
    using grok = sequence<char, 'g', 'r', 'o', 'k'>;
    using faster = sequence<char, 'f', 'a', 's', 't', 'e', 'r'>;
    using green = sequence<char, 'g', 'r', 'e', 'e', 'n'>;
    using gold = sequence<char, 'g', 'o', 'l', 'd'>;
    using farther = sequence<char, 'f', 'a', 'r', 't', 'h', 'e', 'r'>;
    using groove = sequence<char, 'g', 'r', 'o', 'o', 'v', 'e'>;
    using fat = sequence<char, 'f', 'a', 't'>;
    using fist = sequence<char, 'f', 'i', 's', 't'>;

    using empty = sequence<char>;
    using x = sequence<char, 'x'>;
    using notfound = sequence<char, 'n', 'o', 't', 'f', 'o', 'u', 'n', 'd'>;

    using shuffled = list<
      gooey, fast, granite, fastest, fart, far, good, great,
      grok, faster, green, gold, farther, groove, fat, fist
    >;

    using sorted = list<
      far, fart, farther, fast, faster, fastest, fat, fist,
      gold, good, gooey, granite, great, green, grok, groove
    >;

    using group_by = list<
      list<far, fart, farther, fast, faster, fastest, fat, fist>,
      list<gold, good, gooey, granite, great, green, grok, groove>
    >;

    using filtered_group_by_3 = pair<
      list<far, fat>,
      list<
        list<fart, farther, fast, faster, fastest, fist>,
        list<gold, good, gooey, granite, great, green, grok, groove>
      >
    >;

    using filtered_group_by_7 = pair<
      list<farther, fastest, granite>,
      list<
        list<far, fart, fast, faster, fat, fist>,
        list<gold, good, gooey, great, green, grok, groove>
      >
    >;
  };

  struct lst {
    using gooey = as_list<seq::gooey>;
    using fast = as_list<seq::fast>;
    using granite = as_list<seq::granite>;
    using fastest = as_list<seq::fastest>;
    using fart = as_list<seq::fart>;
    using far = as_list<seq::far>;
    using good = as_list<seq::good>;
    using great = as_list<seq::great>;
    using grok = as_list<seq::grok>;
    using faster = as_list<seq::faster>;
    using green = as_list<seq::green>;
    using gold = as_list<seq::gold>;
    using farther = as_list<seq::farther>;
    using groove = as_list<seq::groove>;
    using fat = as_list<seq::fat>;
    using fist = as_list<seq::fist>;

    using empty = as_list<seq::empty>;
    using x = as_list<seq::x>;
    using notfound = as_list<seq::notfound>;


    using shuffled = list<
      gooey, fast, granite, fastest, fart, far, good, great,
      grok, faster, green, gold, farther, groove, fat, fist
    >;

    using sorted = list<
      far, fart, farther, fast, faster, fastest, fat, fist,
      gold, good, gooey, granite, great, green, grok, groove
    >;

    using group_by = list<
      list<far, fart, farther, fast, faster, fastest, fat, fist>,
      list<gold, good, gooey, granite, great, green, grok, groove>
    >;

    using filtered_group_by_3 = pair<
      list<far, fat>,
      list<
        list<fart, farther, fast, faster, fastest, fist>,
        list<gold, good, gooey, granite, great, green, grok, groove>
      >
    >;

    using filtered_group_by_7 = pair<
      list<farther, fastest, granite>,
      list<
        list<far, fart, fast, faster, fat, fist>,
        list<gold, good, gooey, great, green, grok, groove>
      >
    >;
  };

  struct mp {
    using shuffled = list<
      to_map_entry<seq::gooey>,
      to_map_entry<seq::fast>,
      to_map_entry<seq::granite>,
      to_map_entry<seq::fastest>,
      to_map_entry<seq::fart>,
      to_map_entry<seq::far>,
      to_map_entry<seq::good>,
      to_map_entry<seq::great>,
      to_map_entry<seq::grok>,
      to_map_entry<seq::faster>,
      to_map_entry<seq::green>,
      to_map_entry<seq::gold>,
      to_map_entry<seq::farther>,
      to_map_entry<seq::groove>,
      to_map_entry<seq::fat>,
      to_map_entry<seq::fist>
    >;

    using sorted = list<
      to_map_entry<seq::far>,
      to_map_entry<seq::fart>,
      to_map_entry<seq::farther>,
      to_map_entry<seq::fast>,
      to_map_entry<seq::faster>,
      to_map_entry<seq::fastest>,
      to_map_entry<seq::fat>,
      to_map_entry<seq::fist>,
      to_map_entry<seq::gold>,
      to_map_entry<seq::good>,
      to_map_entry<seq::gooey>,
      to_map_entry<seq::granite>,
      to_map_entry<seq::great>,
      to_map_entry<seq::green>,
      to_map_entry<seq::grok>,
      to_map_entry<seq::groove>
    >;
  };
};

void run_sanity_check() {
  using sq1 = make_index_sequence<3>;
  using sq2 = make_index_sequence<4>;
  using seq = make_index_sequence<size<sq1>::value + size<sq2>::value>;
  using shuf_sq = index_sequence<0, 8, 9, 6, 7, 5, 4, 1, 3, 2>;
  using srt_sq = index_sequence<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>;

  using ls1 = list<int, double, bool>;
  using ls2 = list<void, float, char, unsigned>;
  using lst = list<int, double, bool, void, float, char, unsigned>;
  using shuf_ls = sz_list<0, 8, 9, 6, 7, 5, 4, 1, 3, 2>;
  using srt_ls = sz_list<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>;
  using dmy_ls = list<
    dummy<sz<0>>,
    dummy<sz<1>>,
    dummy<sz<2>>,
    dummy<sz<3>>,
    dummy<sz<4>>,
    dummy<sz<5>>,
    dummy<sz<6>>,
    dummy<sz<7>>,
    dummy<sz<8>>,
    dummy<sz<9>>
  >;

  using mp = list<
    pair<int, double>,
    pair<bool, void>,
    pair<float, char>,
    pair<unsigned, short *>
  >;

  using shuf_mp = list<
    mp_entry<0>,
    mp_entry<8>,
    mp_entry<9>,
    mp_entry<6>,
    mp_entry<7>,
    mp_entry<5>,
    mp_entry<4>,
    mp_entry<1>,
    mp_entry<3>,
    mp_entry<2>
  >;
  using srt_mp = list<
    mp_entry<0>,
    mp_entry<1>,
    mp_entry<2>,
    mp_entry<3>,
    mp_entry<4>,
    mp_entry<5>,
    mp_entry<6>,
    mp_entry<7>,
    mp_entry<8>,
    mp_entry<9>
  >;

  SAME(int, at<lst, 0>);
  SAME(double, at<lst, 1>);
  SAME(bool, at<lst, 2>);
  SAME(void, at<lst, 3>);
  SAME(float, at<lst, 4>);
  SAME(char, at<lst, 5>);
  SAME(unsigned, at<lst, 6>);

  SAME(int, try_at<lst, 0>);
  SAME(double, try_at<lst, 1>);
  SAME(bool, try_at<lst, 2>);
  SAME(void, try_at<lst, 3>);
  SAME(float, try_at<lst, 4>);
  SAME(char, try_at<lst, 5>);
  SAME(unsigned, try_at<lst, 6>);
  SAME(not_found, try_at<lst, 7>);

  EQUAL(0, at<seq, 0>);
  EQUAL(1, at<seq, 1>);
  EQUAL(2, at<seq, 2>);
  EQUAL(3, at<seq, 3>);
  EQUAL(4, at<seq, 4>);
  EQUAL(5, at<seq, 5>);
  EQUAL(6, at<seq, 6>);

  EQUAL(0, try_at<seq, 0>);
  EQUAL(1, try_at<seq, 1>);
  EQUAL(2, try_at<seq, 2>);
  EQUAL(3, try_at<seq, 3>);
  EQUAL(4, try_at<seq, 4>);
  EQUAL(5, try_at<seq, 5>);
  EQUAL(6, try_at<seq, 6>);
  SAME(not_found, try_at<seq, 7>);

  EQUAL(0, at<cat<sq1, sq2>, 0>);
  EQUAL(1, at<cat<sq1, sq2>, 1>);
  EQUAL(2, at<cat<sq1, sq2>, 2>);
  EQUAL(0, at<cat<sq1, sq2>, 3>);
  EQUAL(1, at<cat<sq1, sq2>, 4>);
  EQUAL(2, at<cat<sq1, sq2>, 5>);
  EQUAL(3, at<cat<sq1, sq2>, 6>);

  EQUAL(3, size<ls1>);
  EQUAL(4, size<ls2>);
  EQUAL(7, size<lst>);

  EQUAL(3, size<sq1>);
  EQUAL(4, size<sq2>);
  EQUAL(7, size<seq>);

  EQUAL(true, empty<list<>>);
  EQUAL(false, empty<ls1>);
  EQUAL(false, empty<ls2>);
  EQUAL(false, empty<lst>);

  EQUAL(true, empty<index_sequence<>>);
  EQUAL(false, empty<sq1>);
  EQUAL(false, empty<sq2>);
  EQUAL(false, empty<seq>);

  SAME(ls1, cat<ls1>);
  SAME(lst, cat<ls1, ls2>);
  SAME(sq1, cat<sq1>);
  SAME(index_sequence<0, 1, 2, 0, 1, 2, 3>, cat<sq1, sq2>);
  test_cat<0, 1, 2>();
  test_cat<0, 1, 2, 3>();
  test_cat<0, 1, 2, 3, 4>();
  test_cat<0, 1, 2, 3, 4, 5>();
  test_cat<0, 1, 2, 3, 4, 5, 6>();
  test_cat<0, 1, 2, 3, 4, 5, 6, 7>();
  test_cat<0, 1, 2, 3, 4, 5, 6, 7, 8>();
  test_cat<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>();
  test_cat<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10>();
  test_cat<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11>();
  test_cat<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12>();
  test_cat<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13>();
  test_cat<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14>();
  test_cat<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15>();
  test_cat<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16>();
  test_cat<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17>();

  test_head<>
    ::go<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17>();
  test_head<0>::go<1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17>();
  test_head<0, 1>::go<2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17>();
  test_head<0, 1, 2>::go<3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17>();
  test_head<0, 1, 2, 3>::go<4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17>();
  test_head<0, 1, 2, 3, 4>::go<5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17>();
  test_head<0, 1, 2, 3, 4, 5>::go<6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17>();
  test_head<0, 1, 2, 3, 4, 5, 6>::go<7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17>();
  test_head<0, 1, 2, 3, 4, 5, 6, 7>::go<8, 9, 10, 11, 12, 13, 14, 15, 16, 17>();
  test_head<0, 1, 2, 3, 4, 5, 6, 7, 8>::go<9, 10, 11, 12, 13, 14, 15, 16, 17>();
  test_head<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>::go<10, 11, 12, 13, 14, 15, 16, 17>();
  test_head<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10>::go<11, 12, 13, 14, 15, 16, 17>();
  test_head<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11>::go<12, 13, 14, 15, 16, 17>();
  test_head<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12>::go<13, 14, 15, 16, 17>();
  test_head<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13>::go<14, 15, 16, 17>();
  test_head<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14>::go<15, 16, 17>();
  test_head<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15>::go<16, 17>();
  test_head<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16>::go<17>();
  test_head<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17>
    ::go<>();

  SAME(pair<int, double>, get<mp, int>);
  SAME(pair<bool, void>, get<mp, bool>);
  SAME(pair<float, char>, get<mp, float>);
  SAME(pair<unsigned, short *>, get<mp, unsigned>);

  SAME(pair<int, double>, get<mp, int, get_first>);
  SAME(pair<bool, void>, get<mp, bool, get_first>);
  SAME(pair<float, char>, get<mp, float, get_first>);
  SAME(pair<unsigned, short *>, get<mp, unsigned, get_first>);

  SAME(double, get<mp, int, get_first, get_second>);
  SAME(void, get<mp, bool, get_first, get_second>);
  SAME(char, get<mp, float, get_first, get_second>);
  SAME(short *, get<mp, unsigned, get_first, get_second>);

  SAME(int, find<lst, int, not_found>);
  SAME(bool, find<lst, bool, not_found>);
  SAME(float, find<lst, float, not_found>);
  SAME(unsigned, find<lst, unsigned, not_found>);
  SAME(not_found, find<lst, void *, not_found>);

  SAME(pair<int, double>, find<mp, int, not_found, get_first>);
  SAME(pair<bool, void>, find<mp, bool, not_found, get_first>);
  SAME(pair<float, char>, find<mp, float, not_found, get_first>);
  SAME(pair<unsigned, short *>, find<mp, unsigned, not_found, get_first>);
  SAME(not_found, find<mp, void *, not_found, get_first>);

  SAME(double, find<mp, int, not_found, get_first, get_second>);
  SAME(void, find<mp, bool, not_found, get_first, get_second>);
  SAME(char, find<mp, float, not_found, get_first, get_second>);
  SAME(short *, find<mp, unsigned, not_found, get_first, get_second>);
  SAME(not_found, find<mp, double, not_found, get_first, get_second>);

  EQUAL(true, contains<lst, int>);
  EQUAL(true, contains<lst, bool>);
  EQUAL(true, contains<lst, float>);
  EQUAL(true, contains<lst, unsigned>);
  EQUAL(false, contains<lst, void *>);

  EQUAL(true, contains<mp, int, get_first>);
  EQUAL(true, contains<mp, bool, get_first>);
  EQUAL(true, contains<mp, float, get_first>);
  EQUAL(true, contains<mp, unsigned, get_first>);
  EQUAL(false, contains<mp, void *, get_first>);

  SAME(shuf_mp, apply_to<transform<shuf_ls, applier<to_map_entry>>, list>);
  SAME(srt_mp, apply_to<transform<srt_ls, applier<to_map_entry>>, list>);

  SAME(dmy_ls, transform<srt_ls, applier<dummy>>);

  SAME(lst, tail<lst, 0>);
  SAME(list<double, bool, void, float, char, unsigned>, tail<lst, 1>);
  SAME(list<void, float, char, unsigned>, tail<lst, 3>);
  SAME(list<unsigned>, tail<lst, size<lst>::value - 1>);
  SAME(list<>, tail<lst, size<lst>::value>);
  SAME(list<>, tail<list<>, 0>);

  SAME(list<>, head<lst, 0>);
  SAME(list<int>, head<lst, 1>);
  SAME(list<int, double, bool>, head<lst, 3>);
  SAME(
    list<int, double, bool, void, float, char>,
    head<lst, size<lst>::value - 1>
  );
  SAME(lst, head<lst, size<lst>::value>);
  SAME(list<>, head<list<>, 0>);

  SAME(lst, slice<lst, 0, size<lst>::value>);
  SAME(list<float, char, unsigned>, slice<lst, 4, size<lst>::value>);
  SAME(list<int, double, bool, void>, slice<lst, 0, 4>);
  SAME(list<void, float, char>, slice<lst, 3, 6>);
  SAME(list<>, slice<lst, 4, 4>);
  SAME(list<float>, slice<lst, 4, 5>);
  SAME(list<>, slice<lst, size<lst>::value, size<lst>::value>);

  SAME(pair<list<>, lst>, split<lst, 0>);
  SAME(
    pair<list<int, double, bool, void>, list<float, char, unsigned>>,
    split<lst, 4>
  );
  SAME(pair<lst, list<>>, split<lst, size<lst>::value>);

  SAME(seq, slice<seq, 0, size<seq>::value>);
  SAME(index_sequence<4, 5, 6>, slice<seq, 4, size<seq>::value>);
  SAME(index_sequence<0, 1, 2, 3>, slice<seq, 0, 4>);
  SAME(index_sequence<3, 4, 5>, slice<seq, 3, 6>);
  SAME(index_sequence<>, slice<seq, 4, 4>);
  SAME(index_sequence<4>, slice<seq, 4, 5>);
  SAME(index_sequence<>, slice<seq, size<seq>::value, size<seq>::value>);

  SAME(std::true_type, type_of<std::true_type>);
  SAME(size_t, type_of<index_sequence<>>);
  SAME(size_t, type_of<seq>);

  SAME(bool, value_type_of<std::true_type>);
  SAME(int, value_type_of<std::vector<int>>);
  SAME(size_t, value_type_of<index_sequence<>>);
  SAME(size_t, value_type_of<seq>);

  SAME(srt_ls, as_list<srt_ls>);
  SAME(srt_ls, as_list<srt_sq>);
  SAME(list<>, as_list<index_sequence<>>);
  SAME(srt_sq, as_sequence<srt_sq>);
  SAME(srt_sq, as_sequence<srt_ls>);
  SAME(index_sequence<>, as_sequence<list<>, sequence, type_of<srt_sq>>);

  SAME(pair<index_sequence<>, seq>, split<seq, 0>);
  SAME(
    pair<index_sequence<0, 1, 2, 3>, index_sequence<4, 5, 6>>,
    split<seq, 4>
  );
  SAME(pair<seq, index_sequence<>>, split<seq, size<seq>::value>);

  SAME(
    list<void *, double, bool, void, float, char, unsigned>,
    replace<lst>::at<0, void *>
  );
  SAME(
    list<int, void *, bool, void, float, char, unsigned>,
    replace<lst>::at<1, void *>
  );
  SAME(
    list<int, double, void *, void, float, char, unsigned>,
    replace<lst>::at<2, void *>
  );
  SAME(
    list<int, double, bool, void *, float, char, unsigned>,
    replace<lst>::at<3, void *>
  );
  SAME(
    list<int, double, bool, void, void *, char, unsigned>,
    replace<lst>::at<4, void *>
  );
  SAME(
    list<int, double, bool, void, float, void *, unsigned>,
    replace<lst>::at<5, void *>
  );
  SAME(
    list<int, double, bool, void, float, char, void *>,
    replace<lst>::at<6, void *>
  );

  SAME(index_sequence<100, 1, 2, 3, 4, 5, 6>, replace<seq>::at<0, 100>);
  SAME(index_sequence<0, 100, 2, 3, 4, 5, 6>, replace<seq>::at<1, 100>);
  SAME(index_sequence<0, 1, 100, 3, 4, 5, 6>, replace<seq>::at<2, 100>);
  SAME(index_sequence<0, 1, 2, 100, 4, 5, 6>, replace<seq>::at<3, 100>);
  SAME(index_sequence<0, 1, 2, 3, 100, 5, 6>, replace<seq>::at<4, 100>);
  SAME(index_sequence<0, 1, 2, 3, 4, 100, 6>, replace<seq>::at<5, 100>);
  SAME(index_sequence<0, 1, 2, 3, 4, 5, 100>, replace<seq>::at<6, 100>);

  SAME(
    test_list<int, double, bool, void, float, char, unsigned>,
    apply_to<lst, test_list>
  );
  SAME(
    test_list<int, double, bool, void, float, char, unsigned>,
    apply_to_front<lst, test_list>
  );
  SAME(
    test_list<int, double, bool, void, float, char, unsigned, void *>,
    apply_to<lst, test_list, void *>
  );
  SAME(
    test_list<void *, int, double, bool, void, float, char, unsigned>,
    apply_to_front<lst, test_list, void *>
  );
  SAME(
    test_list<
      int, double, bool, void, float, char, unsigned, void *, int *, char *
    >,
    apply_to<lst, test_list, void *, int *, char *>
  );
  SAME(
    test_list<
      void *, int *, char *, int, double, bool, void, float, char, unsigned
    >,
    apply_to_front<lst, test_list, void *, int *, char *>
  );

  SAME(
    test_seq<std::size_t, 0, 1, 2, 3, 4, 5, 6>,
    sequence_apply<seq>::apply<test_seq>
  );
  SAME(
    test_seq<std::size_t, 0, 1, 2, 3, 4, 5, 6>,
    sequence_apply<seq>::front<test_seq>
  );
  SAME(
    test_seq<std::size_t, 0, 1, 2, 3, 4, 5, 6, 100>,
    sequence_apply<seq>::apply<test_seq, 100>
  );
  SAME(
    test_seq<std::size_t, 100, 0, 1, 2, 3, 4, 5, 6>,
    sequence_apply<seq>::front<test_seq, 100>
  );
  SAME(
    test_seq<std::size_t, 0, 1, 2, 3, 4, 5, 6, 100, 200, 300>,
    sequence_apply<seq>::apply<test_seq, 100, 200, 300>
  );
  SAME(
    test_seq<std::size_t, 100, 200, 300, 0, 1, 2, 3, 4, 5, 6>,
    sequence_apply<seq>::front<test_seq, 100, 200, 300>
  );

  SAME(
    pair<list<int, bool, char, unsigned>, list<double, void, float>>,
    partition<lst, applier<std::is_integral>>
  );
  SAME(
    pair<list<double, float>, list<int, bool, void, char, unsigned>>,
    partition<lst, applier<std::is_floating_point>>
  );

  SAME(list<int, bool, char, unsigned>, filter<lst, applier<std::is_integral>>);
  SAME(list<double, float>, filter<lst, applier<std::is_floating_point>>);

  SAME(list<double, void, float>, reject<lst, applier<std::is_integral>>);
  SAME(
    list<int, bool, void, char, unsigned>,
    reject<lst, applier<std::is_floating_point>>
  );

  SAME(sz<0>, min<sz<0>, sz<0>>);
  SAME(sz<0>, min<sz<0>, sz<1>>);
  SAME(sz<0>, min<sz<1>, sz<0>>);

  SAME(sz<1>, max<sz<0>, sz<1>>);
  SAME(sz<1>, max<sz<1>, sz<0>>);
  SAME(sz<1>, max<sz<1>, sz<1>>);

  SAME(sz<1>, vmin<less, sz<1>>);
  SAME(sz<1>, vmin<less, sz<1>, sz<4>>);
  SAME(sz<1>, vmin<less, sz<1>, sz<4>, sz<7>>);
  SAME(sz<1>, vmin<less, sz<1>, sz<4>, sz<7>, sz<10>>);
  SAME(sz<1>, vmin<less, sz<1>, sz<4>, sz<7>, sz<10>, sz<13>>);
  SAME(sz<1>, vmin<less, sz<1>, sz<4>, sz<7>, sz<10>, sz<13>, sz<16>>);
  SAME(sz<1>, vmin<less, sz<1>, sz<4>, sz<7>, sz<10>, sz<13>, sz<16>, sz<19>>);
  SAME(
    sz<1>,
    vmin<less, sz<1>, sz<4>, sz<7>, sz<10>, sz<13>, sz<16>, sz<19>, sz<22>>
  );
  SAME(
    sz<1>,
    vmin<
      less,
      sz<1>, sz<4>, sz<7>, sz<10>, sz<13>, sz<16>, sz<19>, sz<22>, sz<25>
    >
  );
  SAME(
    sz<1>,
    vmin<
      less,
      sz<1>, sz<4>, sz<7>, sz<10>, sz<13>, sz<16>, sz<19>, sz<22>, sz<25>,
      sz<28>, sz<31>, sz<34>, sz<37>, sz<40>, sz<43>
    >
  );
  SAME(
    sz<1>,
    vmin<
      less,
      sz<1>, sz<4>, sz<7>, sz<10>, sz<13>, sz<16>, sz<19>, sz<22>, sz<25>,
      sz<28>, sz<31>, sz<34>, sz<37>, sz<40>, sz<43>, sz<46>
    >
  );
  SAME(
    sz<1>,
    vmin<
      less,
      sz<1>, sz<4>, sz<7>, sz<10>, sz<13>, sz<16>, sz<19>, sz<22>, sz<25>,
      sz<28>, sz<31>, sz<34>, sz<37>, sz<40>, sz<43>, sz<46>, sz<49>
    >
  );
  SAME(
    sz<1>,
    vmin<
      less,
      sz<1>, sz<4>, sz<7>, sz<10>, sz<13>, sz<16>, sz<19>, sz<22>, sz<25>,
      sz<28>, sz<31>, sz<34>, sz<37>, sz<40>, sz<43>, sz<46>, sz<49>, sz<52>,
      sz<55>, sz<58>, sz<61>, sz<64>, sz<67>, sz<70>, sz<73>, sz<76>, sz<79>,
      sz<82>, sz<85>, sz<88>, sz<91>
    >
  );
  SAME(
    sz<1>,
    vmin<
      less,
      sz<1>, sz<4>, sz<7>, sz<10>, sz<13>, sz<16>, sz<19>, sz<22>, sz<25>,
      sz<28>, sz<31>, sz<34>, sz<37>, sz<40>, sz<43>, sz<46>, sz<49>, sz<52>,
      sz<55>, sz<58>, sz<61>, sz<64>, sz<67>, sz<70>, sz<73>, sz<76>, sz<79>,
      sz<82>, sz<85>, sz<88>, sz<91>, sz<94>
    >
  );
  SAME(
    sz<1>,
    vmin<
      less,
      sz<1>, sz<4>, sz<7>, sz<10>, sz<13>, sz<16>, sz<19>, sz<22>, sz<25>,
      sz<28>, sz<31>, sz<34>, sz<37>, sz<40>, sz<43>, sz<46>, sz<49>, sz<52>,
      sz<55>, sz<58>, sz<61>, sz<64>, sz<67>, sz<70>, sz<73>, sz<76>, sz<79>,
      sz<82>, sz<85>, sz<88>, sz<91>, sz<94>, sz<97>
    >
  );
  SAME(
    sz<1>,
    vmin<
      less,
      sz<1>, sz<4>, sz<7>, sz<10>, sz<13>, sz<16>, sz<19>, sz<22>, sz<25>,
      sz<28>, sz<31>, sz<34>, sz<37>, sz<40>, sz<43>, sz<46>, sz<49>, sz<52>,
      sz<55>, sz<58>, sz<61>, sz<64>, sz<67>, sz<70>, sz<73>, sz<76>, sz<79>,
      sz<82>, sz<85>, sz<88>, sz<91>, sz<94>, sz<97>, sz<100>, sz<103>, sz<106>,
      sz<109>, sz<112>, sz<115>, sz<118>, sz<121>, sz<124>, sz<127>, sz<130>,
      sz<133>, sz<136>, sz<139>, sz<142>, sz<145>, sz<148>, sz<151>, sz<154>,
      sz<157>, sz<160>, sz<163>, sz<166>, sz<169>, sz<172>, sz<175>, sz<178>,
      sz<181>, sz<184>, sz<187>, sz<190>, sz<193>, sz<196>, sz<199>
    >
  );

  SAME(sz<1>, vmax<less, sz<1>>);
  SAME(sz<4>, vmax<less, sz<1>, sz<4>>);
  SAME(sz<7>, vmax<less, sz<1>, sz<4>, sz<7>>);
  SAME(sz<10>, vmax<less, sz<1>, sz<4>, sz<7>, sz<10>>);
  SAME(sz<13>, vmax<less, sz<1>, sz<4>, sz<7>, sz<10>, sz<13>>);
  SAME(sz<16>, vmax<less, sz<1>, sz<4>, sz<7>, sz<10>, sz<13>, sz<16>>);
  SAME(sz<19>, vmax<less, sz<1>, sz<4>, sz<7>, sz<10>, sz<13>, sz<16>, sz<19>>);
  SAME(
    sz<22>,
    vmax<less, sz<1>, sz<4>, sz<7>, sz<10>, sz<13>, sz<16>, sz<19>, sz<22>>
  );
  SAME(
    sz<25>,
    vmax<
      less,
      sz<1>, sz<4>, sz<7>, sz<10>, sz<13>, sz<16>, sz<19>, sz<22>, sz<25>
    >
  );
  SAME(
    sz<43>,
    vmax<
      less,
      sz<1>, sz<4>, sz<7>, sz<10>, sz<13>, sz<16>, sz<19>, sz<22>, sz<25>,
      sz<28>, sz<31>, sz<34>, sz<37>, sz<40>, sz<43>
    >
  );
  SAME(
    sz<46>,
    vmax<
      less,
      sz<1>, sz<4>, sz<7>, sz<10>, sz<13>, sz<16>, sz<19>, sz<22>, sz<25>,
      sz<28>, sz<31>, sz<34>, sz<37>, sz<40>, sz<43>, sz<46>
    >
  );
  SAME(
    sz<49>,
    vmax<
      less,
      sz<1>, sz<4>, sz<7>, sz<10>, sz<13>, sz<16>, sz<19>, sz<22>, sz<25>,
      sz<28>, sz<31>, sz<34>, sz<37>, sz<40>, sz<43>, sz<46>, sz<49>
    >
  );
  SAME(
    sz<91>,
    vmax<
      less,
      sz<1>, sz<4>, sz<7>, sz<10>, sz<13>, sz<16>, sz<19>, sz<22>, sz<25>,
      sz<28>, sz<31>, sz<34>, sz<37>, sz<40>, sz<43>, sz<46>, sz<49>, sz<52>,
      sz<55>, sz<58>, sz<61>, sz<64>, sz<67>, sz<70>, sz<73>, sz<76>, sz<79>,
      sz<82>, sz<85>, sz<88>, sz<91>
    >
  );
  SAME(
    sz<94>,
    vmax<
      less,
      sz<1>, sz<4>, sz<7>, sz<10>, sz<13>, sz<16>, sz<19>, sz<22>, sz<25>,
      sz<28>, sz<31>, sz<34>, sz<37>, sz<40>, sz<43>, sz<46>, sz<49>, sz<52>,
      sz<55>, sz<58>, sz<61>, sz<64>, sz<67>, sz<70>, sz<73>, sz<76>, sz<79>,
      sz<82>, sz<85>, sz<88>, sz<91>, sz<94>
    >
  );
  SAME(
    sz<97>,
    vmax<
      less,
      sz<1>, sz<4>, sz<7>, sz<10>, sz<13>, sz<16>, sz<19>, sz<22>, sz<25>,
      sz<28>, sz<31>, sz<34>, sz<37>, sz<40>, sz<43>, sz<46>, sz<49>, sz<52>,
      sz<55>, sz<58>, sz<61>, sz<64>, sz<67>, sz<70>, sz<73>, sz<76>, sz<79>,
      sz<82>, sz<85>, sz<88>, sz<91>, sz<94>, sz<97>
    >
  );
  SAME(
    sz<199>,
    vmax<
      less,
      sz<1>, sz<4>, sz<7>, sz<10>, sz<13>, sz<16>, sz<19>, sz<22>, sz<25>,
      sz<28>, sz<31>, sz<34>, sz<37>, sz<40>, sz<43>, sz<46>, sz<49>, sz<52>,
      sz<55>, sz<58>, sz<61>, sz<64>, sz<67>, sz<70>, sz<73>, sz<76>, sz<79>,
      sz<82>, sz<85>, sz<88>, sz<91>, sz<94>, sz<97>, sz<100>, sz<103>, sz<106>,
      sz<109>, sz<112>, sz<115>, sz<118>, sz<121>, sz<124>, sz<127>, sz<130>,
      sz<133>, sz<136>, sz<139>, sz<142>, sz<145>, sz<148>, sz<151>, sz<154>,
      sz<157>, sz<160>, sz<163>, sz<166>, sz<169>, sz<172>, sz<175>, sz<178>,
      sz<181>, sz<184>, sz<187>, sz<190>, sz<193>, sz<196>, sz<199>
    >
  );

  SAME(
    make_index_sequence<10>,
    merge<index_sequence<0, 2, 4, 6, 8>, index_sequence<1, 3, 5, 7, 9>>
  );
  SAME(
    make_index_sequence<10>,
    merge<make_index_sequence<5>, make_index_interval<5, 10>>
  );

  SAME(make_index_sequence<size<shuf_sq>::value>, sort<shuf_sq>);
  SAME(
    as_list<make_index_sequence<size<shuf_ls>::value>>,
    sort<shuf_ls>
  );

  SAME(str::seq::sorted, sort<str::seq::shuffled, sequence_compare<less>>);
  SAME(str::lst::sorted, sort<str::lst::shuffled, sequence_compare<less>>);

  SAME(list<>, adjacent_unique<list<>>);
  SAME(list<int>, adjacent_unique<list<int>>);
  SAME(
    list<int, void, double, bool>,
    adjacent_unique<list<int, void, double, bool>>
  );
  SAME(
    list<int, void, double, bool, int, double>,
    adjacent_unique<list<int, void, double, bool, int, double>>
  );
  SAME(
    list<int, void, double, bool>,
    adjacent_unique<
      list<int, int, void, double, double, double, double, bool, bool>
    >
  );
  SAME(
    list<int, void, double, bool, float>,
    adjacent_unique<
      list<int, int, void, double, double, double, double, bool, bool, float>
    >
  );
  SAME(
    list<int, void, double, bool, int>,
    adjacent_unique<
      list<int, int, void, double, double, double, double, bool, bool, int>
    >
  );

  SAME(str::seq::group_by, group_by<str::seq::sorted, get_first>);
  SAME(str::lst::group_by, group_by<str::lst::sorted, get_first>);

  SAME(
    pair<list<>, str::seq::group_by>,
    filtered_group_by<str::seq::sorted, get_first, none>
  );
  SAME(
    pair<list<>, str::lst::group_by>,
    filtered_group_by<str::lst::sorted, get_first, none>
  );

  SAME(
    str::seq::filtered_group_by_3,
    filtered_group_by<str::seq::sorted, get_first, size_eq<3>>
  );
  SAME(
    str::lst::filtered_group_by_3,
    filtered_group_by<str::lst::sorted, get_first, size_eq<3>>
  );

  SAME(
    str::seq::filtered_group_by_7,
    filtered_group_by<str::seq::sorted, get_first, size_eq<7>>
  );
  SAME(
    str::lst::filtered_group_by_7,
    filtered_group_by<str::lst::sorted, get_first, size_eq<7>>
  );

  SAME(str::seq::group_by, group_by<str::seq::sorted, get_first>);
  SAME(str::lst::group_by, group_by<str::lst::sorted, get_first>);

  foreach<lst>(fn::no_op());

  EQT(sorted_search<srt_ls>(0));
  EQT(sorted_search<srt_ls>(1));
  EQT(sorted_search<srt_ls>(2));
  EQT(sorted_search<srt_ls>(3));
  EQT(sorted_search<srt_ls>(4));
  EQT(sorted_search<srt_ls>(5));
  EQT(sorted_search<srt_ls>(6));
  EQT(sorted_search<srt_ls>(7));
  EQT(sorted_search<srt_ls>(8));
  EQT(sorted_search<srt_ls>(9));
  EQF(sorted_search<srt_ls>(10));

  EQT(sorted_search<seq>(0));
  EQT(sorted_search<seq>(1));
  EQT(sorted_search<seq>(2));
  EQT(sorted_search<seq>(3));
  EQT(sorted_search<seq>(4));
  EQT(sorted_search<seq>(5));
  EQT(sorted_search<seq>(6));
  EQF(sorted_search<seq>(7));

  EQT(sorted_search<srt_ls>(0, test_search_visitor<indexed<sz<0>, 0>>()));
  EQT(sorted_search<srt_ls>(1, test_search_visitor<indexed<sz<1>, 1>>()));
  EQT(sorted_search<srt_ls>(2, test_search_visitor<indexed<sz<2>, 2>>()));
  EQT(sorted_search<srt_ls>(3, test_search_visitor<indexed<sz<3>, 3>>()));
  EQT(sorted_search<srt_ls>(4, test_search_visitor<indexed<sz<4>, 4>>()));
  EQT(sorted_search<srt_ls>(5, test_search_visitor<indexed<sz<5>, 5>>()));
  EQT(sorted_search<srt_ls>(6, test_search_visitor<indexed<sz<6>, 6>>()));
  EQT(sorted_search<srt_ls>(7, test_search_visitor<indexed<sz<7>, 7>>()));
  EQT(sorted_search<srt_ls>(8, test_search_visitor<indexed<sz<8>, 8>>()));
  EQT(sorted_search<srt_ls>(9, test_search_visitor<indexed<sz<9>, 9>>()));
  EQF(sorted_search<srt_ls>(10, test_search_visitor<indexed<sz<10>, 10>>()));

  EQT(sorted_search<seq>(0, test_search_visitor<indexed<sz<0>, 0>>()));
  EQT(sorted_search<seq>(1, test_search_visitor<indexed<sz<1>, 1>>()));
  EQT(sorted_search<seq>(2, test_search_visitor<indexed<sz<2>, 2>>()));
  EQT(sorted_search<seq>(3, test_search_visitor<indexed<sz<3>, 3>>()));
  EQT(sorted_search<seq>(4, test_search_visitor<indexed<sz<4>, 4>>()));
  EQT(sorted_search<seq>(5, test_search_visitor<indexed<sz<5>, 5>>()));
  EQT(sorted_search<seq>(6, test_search_visitor<indexed<sz<6>, 6>>()));
  EQF(sorted_search<seq>(7, test_search_visitor<indexed<sz<7>, 7>>()));

  EQF(sorted_search<make_index_sequence<0>>(0));

  EQT(sorted_search<make_index_sequence<1>>(0));
  EQF(sorted_search<make_index_sequence<1>>(1));

  EQT(sorted_search<make_index_sequence<2>>(0));
  EQT(sorted_search<make_index_sequence<2>>(1));
  EQF(sorted_search<make_index_sequence<2>>(2));

  EQT(sorted_search<make_index_sequence<3>>(0));
  EQT(sorted_search<make_index_sequence<3>>(1));
  EQT(sorted_search<make_index_sequence<3>>(2));
  EQF(sorted_search<make_index_sequence<3>>(3));

  EQT(sorted_search<make_index_sequence<4>>(0));
  EQT(sorted_search<make_index_sequence<4>>(1));
  EQT(sorted_search<make_index_sequence<4>>(2));
  EQT(sorted_search<make_index_sequence<4>>(3));
  EQF(sorted_search<make_index_sequence<4>>(4));

  EQT(sorted_search<make_index_sequence<5>>(0));
  EQT(sorted_search<make_index_sequence<5>>(1));
  EQT(sorted_search<make_index_sequence<5>>(2));
  EQT(sorted_search<make_index_sequence<5>>(3));
  EQT(sorted_search<make_index_sequence<5>>(4));
  EQF(sorted_search<make_index_sequence<5>>(5));

  EQT(sorted_search<make_index_sequence<6>>(0));
  EQT(sorted_search<make_index_sequence<6>>(1));
  EQT(sorted_search<make_index_sequence<6>>(2));
  EQT(sorted_search<make_index_sequence<6>>(3));
  EQT(sorted_search<make_index_sequence<6>>(4));
  EQT(sorted_search<make_index_sequence<6>>(5));
  EQF(sorted_search<make_index_sequence<6>>(6));

  EQT(sorted_search<make_index_sequence<7>>(0));
  EQT(sorted_search<make_index_sequence<7>>(1));
  EQT(sorted_search<make_index_sequence<7>>(2));
  EQT(sorted_search<make_index_sequence<7>>(3));
  EQT(sorted_search<make_index_sequence<7>>(4));
  EQT(sorted_search<make_index_sequence<7>>(5));
  EQT(sorted_search<make_index_sequence<7>>(6));
  EQF(sorted_search<make_index_sequence<7>>(7));

  EQT(sorted_search<make_index_sequence<8>>(0));
  EQT(sorted_search<make_index_sequence<8>>(1));
  EQT(sorted_search<make_index_sequence<8>>(2));
  EQT(sorted_search<make_index_sequence<8>>(3));
  EQT(sorted_search<make_index_sequence<8>>(4));
  EQT(sorted_search<make_index_sequence<8>>(5));
  EQT(sorted_search<make_index_sequence<8>>(6));
  EQT(sorted_search<make_index_sequence<8>>(7));
  EQF(sorted_search<make_index_sequence<8>>(8));

  EQT(sorted_search<make_index_sequence<9>>(0));
  EQT(sorted_search<make_index_sequence<9>>(1));
  EQT(sorted_search<make_index_sequence<9>>(2));
  EQT(sorted_search<make_index_sequence<9>>(3));
  EQT(sorted_search<make_index_sequence<9>>(4));
  EQT(sorted_search<make_index_sequence<9>>(5));
  EQT(sorted_search<make_index_sequence<9>>(6));
  EQT(sorted_search<make_index_sequence<9>>(7));
  EQT(sorted_search<make_index_sequence<9>>(8));
  EQF(sorted_search<make_index_sequence<9>>(9));

  EQT(sorted_search<make_index_sequence<10>>(0));
  EQT(sorted_search<make_index_sequence<10>>(1));
  EQT(sorted_search<make_index_sequence<10>>(2));
  EQT(sorted_search<make_index_sequence<10>>(3));
  EQT(sorted_search<make_index_sequence<10>>(4));
  EQT(sorted_search<make_index_sequence<10>>(5));
  EQT(sorted_search<make_index_sequence<10>>(6));
  EQT(sorted_search<make_index_sequence<10>>(7));
  EQT(sorted_search<make_index_sequence<10>>(8));
  EQT(sorted_search<make_index_sequence<10>>(9));
  EQF(sorted_search<make_index_sequence<10>>(10));

  TEST_AS_ARRAY(lst, gooey);
  TEST_AS_ARRAY(lst, fast);
  TEST_AS_ARRAY(lst, granite);
  TEST_AS_ARRAY(lst, fastest);
  TEST_AS_ARRAY(lst, fart);
  TEST_AS_ARRAY(lst, far);
  TEST_AS_ARRAY(lst, good);
  TEST_AS_ARRAY(lst, great);
  TEST_AS_ARRAY(lst, grok);
  TEST_AS_ARRAY(lst, faster);
  TEST_AS_ARRAY(lst, green);
  TEST_AS_ARRAY(lst, gold);
  TEST_AS_ARRAY(lst, farther);
  TEST_AS_ARRAY(lst, groove);
  TEST_AS_ARRAY(lst, fat);
  TEST_AS_ARRAY(lst, fist);

  TEST_AS_ARRAY(seq, gooey);
  TEST_AS_ARRAY(seq, fast);
  TEST_AS_ARRAY(seq, granite);
  TEST_AS_ARRAY(seq, fastest);
  TEST_AS_ARRAY(seq, fart);
  TEST_AS_ARRAY(seq, far);
  TEST_AS_ARRAY(seq, good);
  TEST_AS_ARRAY(seq, great);
  TEST_AS_ARRAY(seq, grok);
  TEST_AS_ARRAY(seq, faster);
  TEST_AS_ARRAY(seq, green);
  TEST_AS_ARRAY(seq, gold);
  TEST_AS_ARRAY(seq, farther);
  TEST_AS_ARRAY(seq, groove);
  TEST_AS_ARRAY(seq, fat);
  TEST_AS_ARRAY(seq, fist);

  TEST_Z_DATA(lst, gooey);
  TEST_Z_DATA(lst, fast);
  TEST_Z_DATA(lst, granite);
  TEST_Z_DATA(lst, fastest);
  TEST_Z_DATA(lst, fart);
  TEST_Z_DATA(lst, far);
  TEST_Z_DATA(lst, good);
  TEST_Z_DATA(lst, great);
  TEST_Z_DATA(lst, grok);
  TEST_Z_DATA(lst, faster);
  TEST_Z_DATA(lst, green);
  TEST_Z_DATA(lst, gold);
  TEST_Z_DATA(lst, farther);
  TEST_Z_DATA(lst, groove);
  TEST_Z_DATA(lst, fat);
  TEST_Z_DATA(lst, fist);

  TEST_Z_DATA(seq, gooey);
  TEST_Z_DATA(seq, fast);
  TEST_Z_DATA(seq, granite);
  TEST_Z_DATA(seq, fastest);
  TEST_Z_DATA(seq, fart);
  TEST_Z_DATA(seq, far);
  TEST_Z_DATA(seq, good);
  TEST_Z_DATA(seq, great);
  TEST_Z_DATA(seq, grok);
  TEST_Z_DATA(seq, faster);
  TEST_Z_DATA(seq, green);
  TEST_Z_DATA(seq, gold);
  TEST_Z_DATA(seq, farther);
  TEST_Z_DATA(seq, groove);
  TEST_Z_DATA(seq, fat);
  TEST_Z_DATA(seq, fist);

  TEST_AS_ARRAY_FROM(lst, gooey);
  TEST_AS_ARRAY_FROM(lst, fast);
  TEST_AS_ARRAY_FROM(lst, granite);
  TEST_AS_ARRAY_FROM(lst, fastest);
  TEST_AS_ARRAY_FROM(lst, fart);
  TEST_AS_ARRAY_FROM(lst, far);
  TEST_AS_ARRAY_FROM(lst, good);
  TEST_AS_ARRAY_FROM(lst, great);
  TEST_AS_ARRAY_FROM(lst, grok);
  TEST_AS_ARRAY_FROM(lst, faster);
  TEST_AS_ARRAY_FROM(lst, green);
  TEST_AS_ARRAY_FROM(lst, gold);
  TEST_AS_ARRAY_FROM(lst, farther);
  TEST_AS_ARRAY_FROM(lst, groove);
  TEST_AS_ARRAY_FROM(lst, fat);
  TEST_AS_ARRAY_FROM(lst, fist);

  SAME(
    test_list<list<sz<10>>, list<sz<11>>, list<sz<12>>, list<sz<13>>>,
    zip<test_list, list, list<sz<10>, sz<11>, sz<12>, sz<13>>>
  );

  SAME(
    test_list<
      list<sz<10>, sz<20>>,
      list<sz<11>, sz<21>>,
      list<sz<12>, sz<22>>,
      list<sz<13>, sz<23>>
    >,
    zip<
      test_list, list,
      list<sz<10>, sz<11>, sz<12>, sz<13>>,
      list<sz<20>, sz<21>, sz<22>, sz<23>>
    >
  );

  SAME(
    test_list<
      list<sz<10>, sz<20>, sz<30>>,
      list<sz<11>, sz<21>, sz<31>>,
      list<sz<12>, sz<22>, sz<32>>,
      list<sz<13>, sz<23>, sz<33>>
    >,
    zip<
      test_list, list,
      list<sz<10>, sz<11>, sz<12>, sz<13>>,
      list<sz<20>, sz<21>, sz<22>, sz<23>>,
      list<sz<30>, sz<31>, sz<32>, sz<33>>
    >
  );

  SAME(
    test_list<
      list<sz<10>, sz<20>, sz<30>, sz<40>>,
      list<sz<11>, sz<21>, sz<31>, sz<41>>,
      list<sz<12>, sz<22>, sz<32>, sz<42>>,
      list<sz<13>, sz<23>, sz<33>, sz<43>>
    >,
    zip<
      test_list, list,
      list<sz<10>, sz<11>, sz<12>, sz<13>>,
      list<sz<20>, sz<21>, sz<22>, sz<23>>,
      list<sz<30>, sz<31>, sz<32>, sz<33>>,
      list<sz<40>, sz<41>, sz<42>, sz<43>>
    >
  );

  EQUAL(3, tst_longest_common_prefix<str::misc::foo>);
  EQUAL(6, tst_longest_common_prefix<str::misc::foobar>);
  EQUAL(3, tst_longest_common_prefix<str::misc::foo, str::misc::foobar>);
  EQUAL(5, tst_longest_common_prefix<str::misc::foobar, str::misc::foobaz>);

  EQUAL(
    4,
    tst_longest_common_prefix<
      str::misc::foobar,
      str::misc::foobaz,
      str::misc::foobAr
    >
  );

  EQUAL(
    3,
    tst_longest_common_prefix<
      str::misc::foo,
      str::misc::foobar,
      str::misc::foobaz,
      str::misc::foobAr
    >
  );

  EQ(std::string("fast"), z_data<str::lst::fast>());
  EQ(std::string("fast"), z_data<str::seq::fast>());
  EQ("fast", to_instance<std::string, str::lst::fast>());
  EQ("fast", to_instance<std::string, str::seq::fast>());
  EQ("faster", to_instance<std::string, str::lst::fast>('e', 'r'));
  EQ("faster", to_instance<std::string, str::seq::fast>('e', 'r'));

  IST(tautology::apply<>);
  IST(tautology::apply<int>);
  IST(tautology::apply<std::true_type>);
  IST(tautology::apply<std::false_type>);
  IST(tautology::apply<int, double, bool>);

  ISF(contradiction::apply<>);
  ISF(contradiction::apply<int>);
  ISF(contradiction::apply<std::true_type>);
  ISF(contradiction::apply<std::false_type>);
  ISF(contradiction::apply<int, double, bool>);

  {
    using f = std::false_type;
    using t = std::true_type;

    IST(negate<f>);
    ISF(negate<t>);

    ISF(logical_or<f>);
    IST(logical_or<t>);
    ISF(logical_or<f, f>);
    IST(logical_or<f, t>);
    IST(logical_or<t, f>);
    IST(logical_or<t, t>);
    ISF(logical_or<f, f, f>);
    IST(logical_or<f, f, t>);
    IST(logical_or<f, t, f>);
    IST(logical_or<f, t, t>);
    IST(logical_or<t, f, f>);
    IST(logical_or<t, f, t>);
    IST(logical_or<t, t, f>);
    IST(logical_or<t, t, t>);

    IST(logical_nor<f>);
    ISF(logical_nor<t>);
    IST(logical_nor<f, f>);
    ISF(logical_nor<f, t>);
    ISF(logical_nor<t, f>);
    ISF(logical_nor<t, t>);
    IST(logical_nor<f, f, f>);
    ISF(logical_nor<f, f, t>);
    ISF(logical_nor<f, t, f>);
    ISF(logical_nor<f, t, t>);
    ISF(logical_nor<t, f, f>);
    ISF(logical_nor<t, f, t>);
    ISF(logical_nor<t, t, f>);
    ISF(logical_nor<t, t, t>);

    ISF(logical_and<f>);
    IST(logical_and<t>);
    ISF(logical_and<f, f>);
    ISF(logical_and<f, t>);
    ISF(logical_and<t, f>);
    IST(logical_and<t, t>);
    ISF(logical_and<f, f, f>);
    ISF(logical_and<f, f, t>);
    ISF(logical_and<f, t, f>);
    ISF(logical_and<f, t, t>);
    ISF(logical_and<t, f, f>);
    ISF(logical_and<t, f, t>);
    ISF(logical_and<t, t, f>);
    IST(logical_and<t, t, t>);

    IST(logical_nand<f>);
    ISF(logical_nand<t>);
    IST(logical_nand<f, f>);
    IST(logical_nand<f, t>);
    IST(logical_nand<t, f>);
    ISF(logical_nand<t, t>);
    IST(logical_nand<f, f, f>);
    IST(logical_nand<f, f, t>);
    IST(logical_nand<f, t, f>);
    IST(logical_nand<f, t, t>);
    IST(logical_nand<t, f, f>);
    IST(logical_nand<t, f, t>);
    IST(logical_nand<t, t, f>);
    ISF(logical_nand<t, t, t>);

    ISF(logical_xor<f>);
    IST(logical_xor<t>);
    ISF(logical_xor<f, f>);
    IST(logical_xor<f, t>);
    IST(logical_xor<t, f>);
    ISF(logical_xor<t, t>);
    ISF(logical_xor<f, f, f>);
    IST(logical_xor<f, f, t>);
    IST(logical_xor<f, t, f>);
    ISF(logical_xor<f, t, t>);
    IST(logical_xor<t, f, f>);
    ISF(logical_xor<t, f, t>);
    ISF(logical_xor<t, t, f>);
    IST(logical_xor<t, t, t>);

    IST(logical_xnor<f>);
    ISF(logical_xnor<t>);
    IST(logical_xnor<f, f>);
    ISF(logical_xnor<f, t>);
    ISF(logical_xnor<t, f>);
    IST(logical_xnor<t, t>);
    IST(logical_xnor<f, f, f>);
    ISF(logical_xnor<f, f, t>);
    ISF(logical_xnor<f, t, f>);
    IST(logical_xnor<f, t, t>);
    ISF(logical_xnor<t, f, f>);
    IST(logical_xnor<t, f, t>);
    IST(logical_xnor<t, t, f>);
    ISF(logical_xnor<t, t, t>);
  }
}

}

FATAL_TEST(type, sanity_check) {
  fatal::run_sanity_check();
}
