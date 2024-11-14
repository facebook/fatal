#include <fatal/type/search.h>

#include <fatal/type/list.h>

#include <fatal/test/driver.h>

namespace fatal {

template <typename Expected, std::size_t ExpectedIndex>
struct search_visitor {
  template <typename Actual, std::size_t Index>
  void operator ()(indexed<Actual, Index>) const {
    FATAL_EXPECT_SAME<Expected, Actual>();
    FATAL_EXPECT_EQ(ExpectedIndex, Index);
  }
};

template <std::size_t Value, std::size_t Index>
using value_search_visitor = search_visitor<size_constant<Value>, Index>;

FATAL_TEST(sorted_search, empty) {
  using h = index_list<>;
  FATAL_EXPECT_FALSE(sorted_search<h>(10, value_search_visitor<10, 0>()));
  FATAL_EXPECT_FALSE(sorted_search<h>(20, value_search_visitor<20, 1>()));
  FATAL_EXPECT_FALSE(sorted_search<h>(30, value_search_visitor<30, 2>()));
  FATAL_EXPECT_FALSE(sorted_search<h>(40, value_search_visitor<40, 3>()));
  FATAL_EXPECT_FALSE(sorted_search<h>(50, value_search_visitor<50, 4>()));
  FATAL_EXPECT_FALSE(sorted_search<h>(60, value_search_visitor<60, 5>()));
}

FATAL_TEST(sorted_search, list) {
  using h = index_list<10, 20, 30, 40, 50>;
  FATAL_EXPECT_TRUE(sorted_search<h>(10, value_search_visitor<10, 0>()));
  FATAL_EXPECT_TRUE(sorted_search<h>(20, value_search_visitor<20, 1>()));
  FATAL_EXPECT_TRUE(sorted_search<h>(30, value_search_visitor<30, 2>()));
  FATAL_EXPECT_TRUE(sorted_search<h>(40, value_search_visitor<40, 3>()));
  FATAL_EXPECT_TRUE(sorted_search<h>(50, value_search_visitor<50, 4>()));
  FATAL_EXPECT_FALSE(sorted_search<h>(60, value_search_visitor<60, 5>()));
}

FATAL_TEST(scalar_search, empty) {
  using h = index_list<>;
  FATAL_EXPECT_FALSE(scalar_search<h>(10, value_search_visitor<10, 0>()));
  FATAL_EXPECT_FALSE(scalar_search<h>(20, value_search_visitor<20, 1>()));
  FATAL_EXPECT_FALSE(scalar_search<h>(30, value_search_visitor<30, 2>()));
  FATAL_EXPECT_FALSE(scalar_search<h>(40, value_search_visitor<40, 3>()));
  FATAL_EXPECT_FALSE(scalar_search<h>(50, value_search_visitor<50, 4>()));
  FATAL_EXPECT_FALSE(scalar_search<h>(60, value_search_visitor<60, 5>()));
}

FATAL_TEST(scalar_search, list) {
  using h = index_list<50, 20, 10, 40, 30>;
  FATAL_EXPECT_TRUE(scalar_search<h>(10, value_search_visitor<10, 0>()));
  FATAL_EXPECT_TRUE(scalar_search<h>(20, value_search_visitor<20, 1>()));
  FATAL_EXPECT_TRUE(scalar_search<h>(30, value_search_visitor<30, 2>()));
  FATAL_EXPECT_TRUE(scalar_search<h>(40, value_search_visitor<40, 3>()));
  FATAL_EXPECT_TRUE(scalar_search<h>(50, value_search_visitor<50, 4>()));
  FATAL_EXPECT_FALSE(scalar_search<h>(60, value_search_visitor<60, 5>()));
}

FATAL_TEST(index_search, empty) {
  using h = list<>;
  FATAL_EXPECT_FALSE(index_search<h>(0, search_visitor<void, 0>()));
  FATAL_EXPECT_FALSE(index_search<h>(1, search_visitor<bool, 1>()));
  FATAL_EXPECT_FALSE(index_search<h>(2, search_visitor<double, 2>()));
  FATAL_EXPECT_FALSE(index_search<h>(3, search_visitor<int, 3>()));
  FATAL_EXPECT_FALSE(index_search<h>(4, search_visitor<unsigned, 4>()));
  FATAL_EXPECT_FALSE(index_search<h>(5, search_visitor<void *, 5>()));
}

FATAL_TEST(index_search, list) {
  using h = list<void, bool, double, int, unsigned>;
  FATAL_EXPECT_TRUE(index_search<h>(0, search_visitor<void, 0>()));
  FATAL_EXPECT_TRUE(index_search<h>(1, search_visitor<bool, 1>()));
  FATAL_EXPECT_TRUE(index_search<h>(2, search_visitor<double, 2>()));
  FATAL_EXPECT_TRUE(index_search<h>(3, search_visitor<int, 3>()));
  FATAL_EXPECT_TRUE(index_search<h>(4, search_visitor<unsigned, 4>()));
  FATAL_EXPECT_FALSE(index_search<h>(5, search_visitor<void *, 5>()));
}

} // namespace fatal {
