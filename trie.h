#pragma once
#include <unordered_map>
#include <string>
#include <iostream>

class Trie {

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
			for (size_t len = str.size(); depth < len; depth++) {

				it = current->children.find(str[depth]);
				if (it == current->children.end()) break;
				current = it->second;
			}

			return current;
		}

		bool add(const std::string& str) {
			size_t depth = 0;
			Node* current = find(str, depth);

			if (depth == str.size()) {
				if (current->isWord) return false;
				current->isWord = true;
			}
			else {
				for (size_t len = str.size(); depth < len; depth++) {
					current = current->append(str[depth]);
				}
				current->isWord = true;
			}

			return true;
		}

		Node* append(char c, bool isWord = false) {
			Node* child = new Node(isWord, this);
			children.emplace(c, child);
			return child;
		}

		bool remove(const std::string& str) {
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

				if (current->isWord || current->children.size() > 1) {
					cand = nullptr;
				}
				else if (!cand) {
					cand = current;
					candChar = it->first;
				}
			}

			bool isPresent = false;
			if ((it = current->children.find(str[depth]))
				!= current->children.end()) {
				current = it->second;
				depth++;

				if (depth == len && current->isWord) {
					current->isWord = false;
					isPresent = true;

					if (!current->children.empty()) cand = nullptr;
					else if (!cand) {
						cand = current;
						candChar = it->first;
					}

					if (cand) {
						cand->parent->children.erase(candChar);
						delete cand;
					}
				}
			}

			return isPresent;
		}

		bool has(const std::string& str) {
			size_t depth = 0;
			Node* target = find(str, depth);
			return (depth == str.size() && target->isWord);
		}

		void putWords(std::vector<std::string>& words, std::string& base) {

			for (auto [c, child] : children) {
				base += c;
				child->putWords(words, base);
				base.pop_back();
			}
			if (isWord) words.push_back(base);
		}

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