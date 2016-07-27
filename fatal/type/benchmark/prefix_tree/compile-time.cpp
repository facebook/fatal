#include <fatal/test/words.h>
#include <fatal/type/prefix_tree.h>

#include <iostream>
#include <string>

int main() {
  using namespace fatal;

  using trie = prefix_tree<random_words<list, sequence>>;

  for (std::string needle; std::cin >> needle; ) {
    std::cout << needle << ": " << std::boolalpha
      << trie::find(needle.begin(), needle.end())
      << std::endl;
  }

  return 0;
}
