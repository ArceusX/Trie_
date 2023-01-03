#pragma once
#include <unordered_map>
#include <string>

class Trie {

	// Unneeded for node to store char when parent
	// already does in parent's children map
	class Node {
		bool isWord;
		Node* parent;
		std::unordered_map<char, Node*> children;

	public:

		Node(bool isWord = false, Node* parent = nullptr) :
			isWord(isWord), parent(parent) {}

		// Return node at end of longest present prefix of str
		Node* find(const std::string& str, size_t& depth) {
			std::unordered_map<char, Node*>::iterator it;

			Node* current = this;

			// While char still to find, traverse down child
			// matching next char to find if child is present
			for (size_t len = str.size(); depth < len; depth++) {

				it = current->children.find(str[depth]);
				if (it == current->children.end()) break;
				current = it->second;
			}

			return current;
		}

		bool add(const std::string& str) {
			size_t depth = 0;

			// Find longest present prefix and count 
			// remaining len needing to append to prefix
			Node* current = find(str, depth);

			// Case: Existing prefix adequately covers word
			//		 If word is already marked, no need to mark
			if (depth == str.size()) {
				if (current->isWord) return false;
				current->isWord = true;
			}
			// Case: Append to prefix until word is covered
			else {
				for (size_t len = str.size(); depth < len; depth++) {
					current = current->append(str[depth]);
				}
				current->isWord = true;
			}

			return true;
		}

		// Return child so can easily extend newly appended char
		Node* append(char c, bool isWord = false) {
			Node* child = new Node(isWord, this);
			children.emplace(c, child);
			return child;
		}

		// Upon remove word, remove nodes holding prefix of
		// str and are prefix of no other word and is needed 
		// no longer. Del highest such then rest recursively 
		bool remove(const std::string& str) {
			if (str.empty()) return true;
			size_t depth = 0;

			Node* current = find(str, depth);
			if (depth < str.size() || !current->isWord) {
				return false;
			}
			current->isWord = false;

			if (current->children.empty()) {

				// Move up if parent suffices to be deleted
				// depth > 0: To avoid deleting root (depth 0)
				while (depth > 0 && !current->parent->isWord &&
					   current->parent->children.size() < 2) {
					current = current->parent;
					depth -= 1;
				}

				// depth - 1: str[0] covered by node at depth 1
				current->parent->children.erase(str[depth - 1]);
				delete current;
			}

			return true;
		}

		// Find longest present prefix. If that prefix covers
		// word and word is marked, alert word is present
		bool has(const std::string& str) {
			size_t depth = 0;
			Node* target = find(str, depth);
			return (depth == str.size() && target->isWord);
		}

		// Depth-first traverse trie, putting words into vector
		void putWords(std::vector<std::string>& words, std::string& base) {

			for (auto& [c, child] : children) {

				base += c; // Traverse down 1
				child->putWords(words, base);

				base.pop_back(); // Backtrack to original base (parent) 
			}
			if (isWord) words.push_back(base);
		}

		// Destructor: Node deletes children, then itself
		~Node() {
			for (auto& [c, child] : children) {
				delete child;
			}
		}
	};
	Node* root;

public:

	Trie() : root(new Node(true, nullptr)) {}

	bool has(const std::string& str) {
		return root->has(str);
	}

	bool add(const std::string& str) {
		return root->add(str);
	}

	bool remove(const std::string& str) {
		return root->remove(str);
	}

	std::vector<std::string> getWords() {
		std::vector<std::string> words;
		std::string base = "";
		root->putWords(words, base);
		return words;
	}
};