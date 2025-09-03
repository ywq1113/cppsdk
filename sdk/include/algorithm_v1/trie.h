#include <array>
#include <string_view>
#include <vector>

// (root)
// ├─ 't'
// │    └─ 'o' (word: "to")
// │    └─ 'e'
// │         ├─ 'a' (word: "tea")
// │         └─ 'n' (word: "ten")
// └─ 'i'
//      └─ 'n' (word: "in")
//           └─ 'n' (word: "inn")

struct Trie {
  struct Node {
    bool is_word = false;
    std::array<int, 128> next;
    Node() {
      next.fill(-1);
    }
  };
  Node *root;
  Trie() : root(new Node()) {}

  void insert(std::string_view w) {
    
  }

  bool find(std::string_view w) const {
    
  }

  bool starts_with(std::string_view w) const {
    
  }
};