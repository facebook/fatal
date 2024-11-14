#include <fatal/type/slice.h>

#include <fatal/type/constant.h>
#include <fatal/type/list.h>
#include <fatal/type/sequence.h>

#include <fatal/test/driver.h>

namespace fatal {

FATAL_TEST(pick_var, list) {
  FATAL_EXPECT_SAME<
    pick_var<
      list<int, float, char, double, long>,
      3, 1
    >,
    list<double, float>
  >();
}

FATAL_TEST(pick_var, seq) {
  FATAL_EXPECT_SAME<
    pick_var<
      char_sequence<'a', 'b', 'c', 'd', 'e'>,
      3, 1
    >,
    char_sequence<'d', 'b'>
  >();
}

FATAL_TEST(pick_seq, list) {
  FATAL_EXPECT_SAME<
    pick_seq<
      list<int, float, char, double, long>,
      index_sequence<3, 1>
    >,
    list<double, float>
  >();
}

FATAL_TEST(pick_seq, seq) {
  FATAL_EXPECT_SAME<
    pick_seq<
      char_sequence<'a', 'b', 'c', 'd', 'e'>,
      index_sequence<3, 1>
    >,
    char_sequence<'d', 'b'>
  >();
}

FATAL_TEST(at, single) {
  FATAL_EXPECT_SAME<at<get_constant<float>, 0>, float>();
  FATAL_EXPECT_SAME<at<pair<float, double>, 0>, float>();
  FATAL_EXPECT_SAME<at<pair<float, double>, 1>, double>();
}

FATAL_TEST(index_of, list) {
  using ls = list<int, double, bool, void, float, char, unsigned>;
  FATAL_EXPECT_EQ(0, index_of<ls, int>::value);
  FATAL_EXPECT_EQ(1, index_of<ls, double>::value);
  FATAL_EXPECT_EQ(2, index_of<ls, bool>::value);
  FATAL_EXPECT_EQ(3, index_of<ls, void>::value);
  FATAL_EXPECT_EQ(4, index_of<ls, float>::value);
  FATAL_EXPECT_EQ(5, index_of<ls, char>::value);
  FATAL_EXPECT_EQ(6, index_of<ls, unsigned>::value);
}

FATAL_TEST(index_apply, list) {
  using ls = list<int, double, bool, void, float, char, unsigned>;
  FATAL_EXPECT_EQ(0, index<ls>::apply<int>::value);
  FATAL_EXPECT_EQ(1, index<ls>::apply<double>::value);
  FATAL_EXPECT_EQ(2, index<ls>::apply<bool>::value);
  FATAL_EXPECT_EQ(3, index<ls>::apply<void>::value);
  FATAL_EXPECT_EQ(4, index<ls>::apply<float>::value);
  FATAL_EXPECT_EQ(5, index<ls>::apply<char>::value);
  FATAL_EXPECT_EQ(6, index<ls>::apply<unsigned>::value);
}

FATAL_TEST(index_apply, sequence) {
  using sq = index_sequence<0, 8, 9, 6, 7, 5, 4, 1, 3, 2>;
  FATAL_EXPECT_EQ(0, index<sq>::apply<0>::value);
  FATAL_EXPECT_EQ(1, index<sq>::apply<8>::value);
  FATAL_EXPECT_EQ(2, index<sq>::apply<9>::value);
  FATAL_EXPECT_EQ(3, index<sq>::apply<6>::value);
  FATAL_EXPECT_EQ(4, index<sq>::apply<7>::value);
  FATAL_EXPECT_EQ(5, index<sq>::apply<5>::value);
  FATAL_EXPECT_EQ(6, index<sq>::apply<4>::value);
  FATAL_EXPECT_EQ(7, index<sq>::apply<1>::value);
  FATAL_EXPECT_EQ(8, index<sq>::apply<3>::value);
  FATAL_EXPECT_EQ(9, index<sq>::apply<2>::value);
}

FATAL_TEST(try_index_of, empty_list) {
  using ls = list<>;
  FATAL_EXPECT_EQ(size<ls>::value, try_index_of<ls, int>::value);
  FATAL_EXPECT_EQ(size<ls>::value, try_index_of<ls, double>::value);
  FATAL_EXPECT_EQ(size<ls>::value, try_index_of<ls, bool>::value);
  FATAL_EXPECT_EQ(size<ls>::value, try_index_of<ls, void>::value);
  FATAL_EXPECT_EQ(size<ls>::value, try_index_of<ls, float>::value);
  FATAL_EXPECT_EQ(size<ls>::value, try_index_of<ls, char>::value);
  FATAL_EXPECT_EQ(size<ls>::value, try_index_of<ls, unsigned>::value);
  FATAL_EXPECT_EQ(size<ls>::value, try_index_of<ls, void *>::value);
}

FATAL_TEST(try_index_of, list) {
  using ls = list<int, double, bool, void, float, char, unsigned>;
  FATAL_EXPECT_EQ(0, try_index_of<ls, int>::value);
  FATAL_EXPECT_EQ(1, try_index_of<ls, double>::value);
  FATAL_EXPECT_EQ(2, try_index_of<ls, bool>::value);
  FATAL_EXPECT_EQ(3, try_index_of<ls, void>::value);
  FATAL_EXPECT_EQ(4, try_index_of<ls, float>::value);
  FATAL_EXPECT_EQ(5, try_index_of<ls, char>::value);
  FATAL_EXPECT_EQ(6, try_index_of<ls, unsigned>::value);
  FATAL_EXPECT_EQ(size<ls>::value, try_index_of<ls, void *>::value);
}

FATAL_TEST(try_index_apply, empty_list) {
  using ls = list<>;
  FATAL_EXPECT_EQ(size<ls>::value, try_index<ls>::apply<int>::value);
  FATAL_EXPECT_EQ(size<ls>::value, try_index<ls>::apply<double>::value);
  FATAL_EXPECT_EQ(size<ls>::value, try_index<ls>::apply<bool>::value);
  FATAL_EXPECT_EQ(size<ls>::value, try_index<ls>::apply<void>::value);
  FATAL_EXPECT_EQ(size<ls>::value, try_index<ls>::apply<float>::value);
  FATAL_EXPECT_EQ(size<ls>::value, try_index<ls>::apply<char>::value);
  FATAL_EXPECT_EQ(size<ls>::value, try_index<ls>::apply<unsigned>::value);
  FATAL_EXPECT_EQ(size<ls>::value, try_index<ls>::apply<void *>::value);
}

FATAL_TEST(try_index_apply, list) {
  using ls = list<int, double, bool, void, float, char, unsigned>;
  FATAL_EXPECT_EQ(0, try_index<ls>::apply<int>::value);
  FATAL_EXPECT_EQ(1, try_index<ls>::apply<double>::value);
  FATAL_EXPECT_EQ(2, try_index<ls>::apply<bool>::value);
  FATAL_EXPECT_EQ(3, try_index<ls>::apply<void>::value);
  FATAL_EXPECT_EQ(4, try_index<ls>::apply<float>::value);
  FATAL_EXPECT_EQ(5, try_index<ls>::apply<char>::value);
  FATAL_EXPECT_EQ(6, try_index<ls>::apply<unsigned>::value);
  FATAL_EXPECT_EQ(size<ls>::value, try_index<ls>::apply<void *>::value);
}

FATAL_TEST(try_index_apply, empty_sequence) {
  using sq = index_sequence<>;
  FATAL_EXPECT_EQ(size<sq>::value, try_index<sq>::apply<0>::value);
  FATAL_EXPECT_EQ(size<sq>::value, try_index<sq>::apply<8>::value);
  FATAL_EXPECT_EQ(size<sq>::value, try_index<sq>::apply<9>::value);
  FATAL_EXPECT_EQ(size<sq>::value, try_index<sq>::apply<6>::value);
  FATAL_EXPECT_EQ(size<sq>::value, try_index<sq>::apply<7>::value);
  FATAL_EXPECT_EQ(size<sq>::value, try_index<sq>::apply<5>::value);
  FATAL_EXPECT_EQ(size<sq>::value, try_index<sq>::apply<4>::value);
  FATAL_EXPECT_EQ(size<sq>::value, try_index<sq>::apply<1>::value);
  FATAL_EXPECT_EQ(size<sq>::value, try_index<sq>::apply<3>::value);
  FATAL_EXPECT_EQ(size<sq>::value, try_index<sq>::apply<2>::value);
  FATAL_EXPECT_EQ(size<sq>::value, try_index<sq>::apply<10>::value);
}

FATAL_TEST(try_index_apply, sequence) {
  using sq = index_sequence<0, 8, 9, 6, 7, 5, 4, 1, 3, 2>;
  FATAL_EXPECT_EQ(0, try_index<sq>::apply<0>::value);
  FATAL_EXPECT_EQ(1, try_index<sq>::apply<8>::value);
  FATAL_EXPECT_EQ(2, try_index<sq>::apply<9>::value);
  FATAL_EXPECT_EQ(3, try_index<sq>::apply<6>::value);
  FATAL_EXPECT_EQ(4, try_index<sq>::apply<7>::value);
  FATAL_EXPECT_EQ(5, try_index<sq>::apply<5>::value);
  FATAL_EXPECT_EQ(6, try_index<sq>::apply<4>::value);
  FATAL_EXPECT_EQ(7, try_index<sq>::apply<1>::value);
  FATAL_EXPECT_EQ(8, try_index<sq>::apply<3>::value);
  FATAL_EXPECT_EQ(9, try_index<sq>::apply<2>::value);
  FATAL_EXPECT_EQ(size<sq>::value, try_index<sq>::apply<10>::value);
}

} // namespace fatal {
