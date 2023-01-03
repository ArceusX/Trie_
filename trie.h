#pragma once
#include <unordered_map>
#include <string>

class Trie {

	// Unneeded for node to store char it represents when
	// parent already does in children map
	class Node {
		bool isWord;
		Node* parent;
		std::unordered_map<char, Node*> children;

	public:

		Node(bool isWord = false, Node* parent = nullptr) :
			isWord(isWord), parent(parent) {}

		Node* find(const std::string& str, size_t& depth) {
			std::unordered_map<char, Node*>::iterator it;

			Node* current = this;

			// While char still to find, traverse down child
			// matching next char to find if such child is present
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
			//		 If word is already marked, none is added
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

		// Return child so can append to newly appended char
		Node* append(char c, bool isWord = false) {
			Node* child = new Node(isWord, this);
			children.emplace(c, child);
			return child;
		}

		// When removing word, remove those nodes that  
		// were needed previously because they held 
		// prefix of removed word and are no longer needed
		bool remove(const std::string& str) {

			// Resolve null case distinctly for simplicity
			if (str.empty()) return true;

			Node* cand = nullptr;
			char candChar;

			Node* current = this;
			size_t depth = 0, len = str.size();
			std::unordered_map<char, Node*>::iterator it;
  
			for (; depth < len - 1; depth++) {
				it = current->children.find(str[depth]);
				if (it == current->children.end()) break;
				current = it->second;

				// Given depth < len - 1: Know this node doesn't
				// hold str.back(). If it isWord or holds prefix
				// for multiple words, even if 1 such word being
				// str is removed, at least 1 other word will
				// remain for which it is prefix
				if (current->isWord || current->children.size() > 1) {
					cand = nullptr;
				}

				// !current->isWord && current->children.size() <= 1
				// is known. Mark this as candidate unless previous
				// cand is already present, for which then keep prev
				// cand that would lead to longer erasable chain
				else if (!cand) {
					cand = current;
					candChar = it->first;
				}
			}

			// isPresent: True if str found and is not marked
			bool isPresent = false;

			// If (depth < len - 1) loop finished early, this
			// will fail like it had before break -> return false
			if ((it = current->children.find(str[depth]))
				!= current->children.end()) {
				current = it->second;
				depth++;

				if (depth == len && current->isWord) {
					current->isWord = false;
					isPresent = true;

					// if (depth == len && current->isWord)
					// Must remove word for cand to lead to node del

					// current that holds removed word must have 
					// no child and thus not be prefix 
					// in order for node deletion to happen
					// Notice prev candidate can have 1 child as
					// current could be child to be removed
					if (!current->children.empty()) cand = nullptr;

					// If any other cand higher up was invalidated
					// know current to not be prefix and !isWord
					else if (!cand) {
						cand = current;
						candChar = it->first;
					}

					// Erase ptr to cand from its parent, then del
					if (cand) {
						cand->parent->children.erase(candChar);
						delete cand;
					}
				}
			}

			return isPresent;
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

			for (auto [c, child] : children) {

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