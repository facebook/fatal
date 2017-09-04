#include <fatal/type/type.h>

#include <fatal/type/sequence.h>
#include <fatal/type/tag.h>

#include <fatal/test/driver.h>
#include <fatal/log/log.h>

#include <type_traits>
#include <vector>

namespace fatal {

using seq = char_sequence<'a', 'b', 'c', 'd', 'e', 'f'>;

FATAL_TEST(type, type_of) {
  FATAL_EXPECT_SAME<std::true_type, type_of<std::true_type>>();
  FATAL_EXPECT_SAME<size_t, type_of<index_sequence<>>>();
  FATAL_EXPECT_SAME<void, type_of<indexed<void, 0>>>();
  FATAL_EXPECT_SAME<void, type_of<tag<void>>>();
  FATAL_EXPECT_SAME<char, type_of<seq>>();
}

FATAL_TEST(type, value_type_of) {
  FATAL_EXPECT_SAME<bool, value_type_of<std::true_type>>();
  FATAL_EXPECT_SAME<int, value_type_of<std::vector<int>>>();
  FATAL_EXPECT_SAME<size_t, value_type_of<index_sequence<>>>();
  FATAL_EXPECT_SAME<char, value_type_of<seq>>();
}

} // namespace fatal {
