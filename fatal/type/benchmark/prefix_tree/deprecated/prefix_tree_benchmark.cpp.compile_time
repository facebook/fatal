#include <fatal/test/words.h>
#include <fatal/type/prefix_tree.h>

#include <iostream>
#include <string>

int main() {
  using namespace fatal;

  using trie = random_words<type_list, constant_sequence>
    ::apply<build_type_prefix_tree<>::from>;

  for (std::string needle; std::cin >> needle; ) {
    std::cout << needle << ": " << std::boolalpha
      << trie::match<>::exact(needle.begin(), needle.end(), fn::no_op())
      << std::endl;
  }

  return 0;
}
