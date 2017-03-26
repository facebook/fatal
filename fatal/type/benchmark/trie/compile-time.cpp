#include <fatal/test/words.h>
#include <fatal/type/trie.h>

#include <iostream>
#include <string>

int main() {
  using namespace fatal;

  for (std::string needle; std::cin >> needle; ) {
    std::cout << needle << ": " << std::boolalpha
      << trie_find<random_250_words<list, sequence>>(
        needle.begin(),
        needle.end()
      )
      << std::endl;
  }

  return 0;
}
