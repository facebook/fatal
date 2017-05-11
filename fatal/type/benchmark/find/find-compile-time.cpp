#include <fatal/type/apply.h>
#include <fatal/type/convert.h>
#include <fatal/type/find.h>
#include <fatal/type/scalar.h>
#include <fatal/type/sequence.h>
#include <fatal/type/transform.h>

using namespace fatal;

using count = size_constant<std::size_t(1) << 10>;
using numbers = as_list<make_index_sequence<count::value>>;

using find_number_fancy =
  curry_back<
    curry<
      applier<find>,
      numbers
    >,
    count
  >;

struct find_number_equiv {
  template <typename T>
  using apply = find<
    numbers,
    T,
    count
  >;
};

using roundtripped = transform<numbers, find_number_fancy>;
static_assert(std::is_same<roundtripped, numbers>::value, "broken");

int main() {}
