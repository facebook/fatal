#include <fatal/type/builtin.h>

#include <fatal/test/driver.h>

namespace fatal {

template <typename T, T V> struct ival { static constexpr T value = V; };
template <typename T, T...> struct iseq {};

template <typename T, typename... S>
struct type_t_ { using type = T; };
template <typename Void, template <typename...> class, typename...>
struct detect_ : std::false_type {};
template <template <typename...> class T, typename... A>
struct detect_<typename type_t_<void, T<A...>>::type, T, A...>
  : std::true_type {};
template <template <typename...> class T, typename... A>
constexpr bool detect_v = detect_<void, T, A...>::value;

struct make_integer_seq_test {
  template <template <typename T, T...> class S, typename T, std::size_t N>
  using fallback = builtin::make_integer_seq::__make_integer_seq<S, T, N>;
  template <template <typename T, T...> class S, typename T, std::size_t N>
  using original = __make_integer_seq<S, T, N>;
};

FATAL_TEST(make_integer_seq, examples) {
  using test = make_integer_seq_test;

  FATAL_EXPECT_SAME<iseq<int>, test::fallback<iseq, int, 0>>();
  FATAL_EXPECT_SAME<iseq<int>, test::original<iseq, int, 0>>();

  FATAL_EXPECT_SAME<iseq<int, 0>, test::fallback<iseq, int, 1>>();
  FATAL_EXPECT_SAME<iseq<int, 0>, test::original<iseq, int, 1>>();

  using iseq9 = iseq<int, 0, 1, 2, 3, 4, 5, 6, 7, 8>;
  FATAL_EXPECT_SAME<iseq9, test::fallback<iseq, int, 9>>();
  FATAL_EXPECT_SAME<iseq9, test::original<iseq, int, 9>>();
}

struct type_pack_element_test {
  template <size_t I, typename... T>
  using fallback_rec = builtin::type_pack_element::impl::rec<I, T...>;
  template <size_t I, typename... T>
  using fallback_set = builtin::type_pack_element::impl::set<I, T...>;
  template <size_t I, typename... T>
  using fallback = builtin::type_pack_element::__type_pack_element<I, T...>;
  template <size_t I, typename... T>
  using original = __type_pack_element<I, T...>;

  template <typename IC, typename... T>
  using fallback_rec_ic = fallback_rec<IC::value, T...>;
  template <typename IC, typename... T>
  using fallback_set_ic = fallback_set<IC::value, T...>;
  template <typename IC, typename... T>
  using fallback_ic = fallback<IC::value, T...>;
  template <typename IC, typename... T>
  using original_ic = original<IC::value, T...>;
};

FATAL_TEST(type_pack_element, list) {
  using test = type_pack_element_test;
  using zero = ival<std::size_t, 0>;

  FATAL_EXPECT_SAME<int, test::fallback_rec<0, int>>();
  FATAL_EXPECT_SAME<int, test::fallback_set<0, int>>();
  FATAL_EXPECT_SAME<int, test::fallback<0, int>>();
  FATAL_EXPECT_SAME<int, test::original<0, int>>();

  FATAL_EXPECT_SAME<int, test::fallback_rec<0, int, char>>();
  FATAL_EXPECT_SAME<int, test::fallback_set<0, int, char>>();
  FATAL_EXPECT_SAME<int, test::fallback<0, int, char>>();
  FATAL_EXPECT_SAME<int, test::original<0, int, char>>();

  FATAL_EXPECT_SAME<char, test::fallback_rec<1, int, char>>();
  FATAL_EXPECT_SAME<char, test::fallback_set<1, int, char>>();
  FATAL_EXPECT_SAME<char, test::fallback<1, int, char>>();
  FATAL_EXPECT_SAME<char, test::original<1, int, char>>();

  FATAL_EXPECT_SAME<double, test::fallback_rec<2, int, int, double, int, int>>();
  FATAL_EXPECT_SAME<double, test::fallback_set<2, int, int, double, int, int>>();
  FATAL_EXPECT_SAME<double, test::fallback<2, int, int, double, int, int>>();
  FATAL_EXPECT_SAME<double, test::original<2, int, int, double, int, int>>();

  FATAL_EXPECT_TRUE((detect_v<test::fallback_rec_ic, zero, int>));
  FATAL_EXPECT_TRUE((detect_v<test::fallback_set_ic, zero, int>));
  FATAL_EXPECT_TRUE((detect_v<test::fallback_ic, zero, int>));
  FATAL_EXPECT_TRUE((detect_v<test::original_ic, zero, int>));

  FATAL_EXPECT_FALSE((detect_v<test::fallback_rec_ic, zero>));
  FATAL_EXPECT_FALSE((detect_v<test::fallback_set_ic, zero>));
  FATAL_EXPECT_FALSE((detect_v<test::fallback_ic, zero>));
  FATAL_EXPECT_FALSE((detect_v<test::original_ic, zero>));
}

} // namespace fatal {
