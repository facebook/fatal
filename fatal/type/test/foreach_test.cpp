#include <fatal/type/foreach.h>

#include <fatal/test/driver.h>

#include <string>
#include <vector>

namespace fatal {

struct example_visitor {
  template <typename Type, std::size_t Index>
  void operator()(
    indexed<Type, Index>,
    const std::string suffix,
    std::vector<std::string>& out
  ) {
    out.push_back(to_string(Type::get(), "_", Index, "_", suffix));
  }
};

FATAL_TEST(foreach_test, empty) {
  using types = list<>;

  auto actual = std::vector<std::string>{};
  foreach<types>(example_visitor(), "s", actual);

  const auto expected = std::vector<std::string>{};
  FATAL_EXPECT_TRUE(expected == actual);
}

FATAL_TEST(foreach_test, basic_example) {
  struct foo { static std::string get() { return "foo"; } };
  struct bar { static std::string get() { return "bar"; } };
  struct baz { static std::string get() { return "baz"; } };
  using types = list<foo, bar, baz>;

  auto actual = std::vector<std::string>{};
  foreach<types>(example_visitor(), "s", actual);

  const auto expected = std::vector<std::string>{
    "foo_0_s", "bar_1_s", "baz_2_s"
  };
  FATAL_EXPECT_TRUE(expected == actual);
}

// replicate<typename T, std::size_t N>
//
// An alias to a list<T...> with T replicated N times in the list.
template <typename T, typename>
struct replicate_impl;
template <typename T, std::size_t... Ns>
struct replicate_impl<T, index_sequence<Ns...>> {
  template <std::size_t N>
  using type_from_index = T;
  using type = list<type_from_index<Ns>...>;
};
template <typename T, std::size_t N>
using replicate = typename replicate_impl<T, make_index_sequence<N>>::type;

FATAL_TEST(foreach_test, very_long_type_list) {
  struct foo { static std::string get() { return "foo"; } };
  constexpr auto size = 1ULL << 8;
  using types = replicate<foo, size>;
  static_assert(fatal::size<types>::value == size, "size mismatch");

  auto actual = std::vector<std::string>{};
  foreach<types>(example_visitor(), "s", actual);

  const auto expected = [] {
    std::vector<std::string> ret{};
    for (std::size_t i = 0; i < size; ++i) {
      ret.push_back(to_string("foo_", i, "_s"));
    }
    return ret;
  }();
  FATAL_EXPECT_TRUE(expected == actual);
}

}
