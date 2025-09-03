#include "algorithm_v1/trie.h"

void Trie::insert(std::string_view w) {
  Node *cur = root;
  for (const auto &ch : w) {
    if (cur->next[ch] == nullptr) {
      cur->next[ch] = new Node();
    }
    cur = cur->next[ch];
  }
  cur->is_word = true;
}

bool Trie::find(std::string_view w) const {
  Node *cur = root;
  for (const auto &ch : w) {
    if (cur->next[ch] == nullptr) {
      return false;
    }
    cur = cur->next[ch];
  }
  return cur->is_word;
}

bool Trie::starts_with(std::string_view w) const {
  Node *cur = root;
  for (const auto &ch : w) {
    if (cur->next[ch] == nullptr) {
      return false;
    }
    cur = cur->next[ch];
  }
  return true;
}
