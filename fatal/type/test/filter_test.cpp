#include <fatal/type/list.h>
#include <fatal/type/push.h>
#include <fatal/type/sort.h>

#include <fatal/test/driver.h>


namespace fatal {

template <int x> struct T{
  static constexpr int value = x;
};

using L8 = list<T<1>, T<2>, T<3>, T<4>, T<5>, T<6>, T<7>, T<8>>;
using L15 = cat<L8, list<T<9>, T<10>, T<11>, T<12>, T<13>, T<14>, T<15>>>;
using L16 = push<L15>::back<T<16>>;
using L17 = push<L16>::back<T<17>>;

using L32 = cat<L16, L16>;
using L64 = cat<L32, L32>;
using L128 = cat<L64, L64>;
using L256 = cat<L128, L128>;
using L512 = cat<L256, L256>;
using L1024 = cat<L512, L512>;


template <typename T>
struct is_even{
  static constexpr bool value = T::value % 2 == 0;
};

FATAL_TEST(filter, len1024){
  FATAL_EXPECT_EQ(size<L1024>::value, 1024);
  FATAL_EXPECT_EQ((size<filter<L1024, applier<is_even>>>::value), 512);
}

FATAL_TEST(filter, len15){
  using expected = list<T<2>, T<4>, T<6>, T<8>, T<10>, T<12>, T<14>>;
  FATAL_EXPECT_SAME<filter<L15, applier<is_even>>, expected>();
}

FATAL_TEST(filter, len16){
  using expected = list<T<2>, T<4>, T<6>, T<8>, T<10>, T<12>, T<14>, T<16>>;
  FATAL_EXPECT_SAME<filter<L16, applier<is_even>>, expected>();
}

FATAL_TEST(filter, len17){
  using expected = list<T<2>, T<4>, T<6>, T<8>, T<10>, T<12>, T<14>, T<16>>;
  FATAL_EXPECT_SAME<filter<L17, applier<is_even>>, expected>();
}

}
