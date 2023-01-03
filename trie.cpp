#include <iostream>
#include "trie.h"

int main() {
	Trie trie;
	trie.add("he"); trie.add("tat"); trie.add("tute"); trie.add("tut");

	auto words = trie.getWords();
	for (auto& word : words) std::cout << word << "\n";

	trie.remove("tut");
	trie.remove("tute");

	std::cout << "\nAfter removing tut && tutea\n";
	for (auto& word : trie.getWords()) {
		std::cout << word << "\n";
	}
}