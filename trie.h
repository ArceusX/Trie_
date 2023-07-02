#pragma once
#include <unordered_map>
#include <unordered_set> // As getWords()'s return
#include <string>
#include <stack>		 // To recurse for addWords() and write()
#include <fstream> 

// To avoid copying target str and language locale complexity
// Trie does not preprocess by lower/uppercasing target str
template<typename T = char>

class Trie {
public:
	// Get char that Node codes thru its parent's children map
	template<typename T = T>
	class Node {
		bool  isWord;
		Node* parent;
		std::unordered_map<T, Node*> children;

		friend Trie<T>;

	public:
		Node(bool isWord = false, Node* parent = nullptr) :
			isWord(isWord), parent(parent) {}

		// Re: Node* at end of str's longest prefix found
		//	   Compute, without returning, that prefix's depth 
		Node* find(const std::basic_string<T>& str, size_t& depth) {
			Node* current = this;
			for (size_t len = str.size(); depth < len; depth++) {
				auto it = current->children.find(str[depth]);
				if (it == current->children.end()) break;
				current = it->second;
			}
			return current;
		}

		// Re: True if str is marked in Trie
		bool has(const std::basic_string<T>& str) {
			size_t depth = 0;
			return find(str, depth)->isWord && depth == str.size();
		}

		// Re: True if str--unmarked--can be added
		bool add(const std::basic_string<T>& str) {
			if (str.empty()) return false;

			// Find longest present prefix; count its depth
			size_t depth = 0;
			Node*  current = find(str, depth);

			// If prefix is short of str, extend to str's end
			for (size_t len = str.size(); depth < len; depth++) {
				current = current->append(str[depth], false);
			}

			if (current->isWord) return false;
			current->isWord = true;
			return true;
		}

		// Re: Newly-added child from which to easily extend
		Node* append(T c, bool isWord = false) {
			Node* child = new Node(isWord, this);
			children.emplace(c, child);
			return child;
		}

		// Re: True if str--marked--can be unmarked
		//
		// Do: If unmark, delete Nodes along str's suffix with
		// no further need as prefix. Delete least-deep such   
		// Node; its destructor deletes the rest recursively
		bool remove(const std::basic_string<T>& str) {
			size_t depth = 0; // Try to find str
			Node* current = find(str, depth);
			if (depth < str.size() || !current->isWord) {
				return false;
			}

			if (!current->children.empty()) {
				current->isWord = false;
			}

			// Delete highest ancestor that meets conditions
			else {
				// depth > 1: At child of root, don't move up
				//			  to or mark root for removal
				// size()< 2: If no path other than this one to 
				//			  shorten needs current as prefix
				while (depth > 1 && !current->parent->isWord &&
					   current->parent->children.size() < 2) {
					current = current->parent;
					depth -= 1;
				}

				// depth - 1: depth (1 + x) node holds str[0 + x]
				current->parent->children.erase(str[depth - 1]);
				delete current;
			}

			return true;
		}

		// [Recursive] Add strs marked, prefixing base (usually "")
		//template<class Container>
		//void addWords(
		//	Container& words, std::basic_string<T>& base) {
		//  if (isWord) words.insert(base);
		//	for (auto& [c, child] : children) {
		//		base += c; // Pass c to child
		//		child->addWords(words, base);
		//		base.pop_back(); // Revoke c from child
		//	}
		//}

		// [Iterative] Put Trie's marked strs into Container, prefixed
		//			   with base. Suggested Container: <set> if desire
		//			   lexicographical sort; else, <unordered_set>
		template<class Container>
		void addWords(Container& words, std::basic_string<T> base = "") {
			// Implement backtrack: keep own stack rather than use
			// callstack. Need to store only Node's key (char) or '\0'
			std::stack<T> stack;
			Node* current = this;
			do {
				auto it = current->children.begin();
				if (it == current->children.end()) {
					while (!stack.empty()) {
						// Go to sibling thru parent. Else backtrack
						current = current->parent; 
						base.pop_back();

						T sibling = stack.top(); stack.pop();
						if (sibling != '\0') {
							current = current->children[sibling];
							base += sibling;

							if (current->isWord) words.emplace(base);
							break; // Stop backtrack to traverse down
						}
					}
				}
				else {
					auto& [c, child] = *it; // Record it before ++

					// Record trail to backtrack to when current lacks children 
					// '\0'     : Backtrack to parent last (first-in-last-out)
					// it->first: Once child and its subtree finish, backtrack
					//			  to its siblings before current's parent
					stack.push('\0');
					for (it++; it != current->children.end(); it++) {
						stack.push(it->first);
					}

					base += c;
					if (child->isWord) words.emplace(base);
					current = child; // Traverse child
				}
			} while (current != this);
			// Stop upon backtrack to root (no other child to traverse)
		}

		~Node() {
			for (auto& [c, child] : children) delete child;
		}
	};

	Trie(): root(new Node<T>(false, nullptr)), wordCount(0) {}

	template<typename Iter> // Initialize with strs in range
	Trie(Iter it, Iter end) : Trie() { add(it, end); }
	Trie(std::initializer_list<std::basic_string<T>> strs):
		Trie() { add(strs); }

	// Pathfile's data must adhere to format of write()
	Trie(const std::string& path) :
		root(new Node<T>()), wordCount(read(path)) {}

	// Re: True if str is marked in Trie
	bool has(const std::basic_string<T>& str) {
		return root->has(str);
	}

	// Re: True if str--unmarked--can be added
	bool add(const std::basic_string<T>& str) {
		if (root->add(str)) {
			wordCount++;
			return true;
		}
		else return false;
	}

	// 1: For any of template<T, Alloc = allocator<T>>: vector, list 
	//template<
	//	template<class, class> class Container, 
	//	class Allocator = std::allocator<std::basic_string<T>>>
	//size_t add(const Container<std::basic_string<T>, Allocator>& strs) {
	//
	////2: For Container that start with T: vector, map, unordered_set
	//template<
	//	template<class, class...> class Container,
	//	class...Other>
	//size_t add(const Container<std::basic_string<T>, Other...>& strs) {
	// 
	//	size_t n = 0;
	//	for (auto const& str : strs) n = n + this->add(str);
	//	return n;
	//}

	// Re: # of strs within range newly added
	template<typename Iter>
	size_t add(Iter it, Iter end) {
		size_t n = 0;
		for (; it != end; it++) n += this->add(*it);
		return n;
	}
	size_t add(std::initializer_list<std::basic_string<T>> strs) {
		size_t n = 0;
		for (const auto& str : strs) n += this->add(str);
		return n;
	}

	// Re: True if str--marked--can be unmarked
	bool remove(const std::basic_string<T>& str) {
		// Removing empty str should fail; success otherwise 
		// could make size go below zero. Likewise for add()
		if (root->remove(str)) {
			wordCount--;
			return true;
		}
		else return false;
	}

	// Re: # of strs within range found and removed 
	template<typename Iter>
	size_t remove(Iter it, Iter end) {
		size_t n = 0;
		for (; it != end; it++) n += this->remove(*it);
		return n;
	}
	size_t remove(std::initializer_list<std::basic_string<T>> strs) {
		size_t n = 0;
		for (const auto& str : strs) n += this->remove(str);
		return n;
	}

	size_t getWordCount() { return wordCount; }

	// Re: All strs stored in Trie
	std::unordered_set<std::basic_string<T>> getWords() {
		std::unordered_set<std::basic_string<T>> words(wordCount);
		root->addWords(words, std::basic_string<T>());
		return words;
	}

	// Re: # of bytes written into path file from data in rt
	//
	// read() explains use of delimiters (ie "()*") in encoding 
	// User should ensure path filename hints type of str data
	// stored so on read of data back from [path], user knows which
	// T to assign (ie ".trie(1|2|4)" for (char|char16_t|char32_t))
	size_t write(const std::string path, Node<T>* rt) {
		if (rt->children.empty()) return 0;
		std::ofstream file(path, std::ios::binary);
		if (!file) return 0;

		signed int i;					  // Index of byte in T
		signed int nBytesPer = sizeof(T); // Pad to this many bytes

		std::vector<char> bytes;
		bytes.reserve(2 * nBytesPer * this->wordCount);

		std::stack<T> stack;
		Node<T>* current = rt;
		do {
			auto it = current->children.begin();
			if (it == current->children.end()) {
				while (!stack.empty()) {
					current = current->parent;

					// If sibling, traverse it. Else, close branch
					T sibling = stack.top(); stack.pop();
					if (sibling == '\0') {
						bytes.push_back(')');
						for (i = 1; i < nBytesPer; i++) bytes.push_back('\0');
					}
					else {
						current = current->children[sibling];
						for (i = 0; i < nBytesPer; i++) {
							bytes.push_back(sibling >> (8 * i));
						}

						// Node capping str should get '*', but if it
						// lacks child and still remains in trie, know 
						// it mark str. Thus explicit '*' is extraneous
						if (current->isWord && !current->children.empty()) {
							bytes.push_back('*');
							for (i = 1; i < nBytesPer; i++) bytes.push_back('\0');
						}
						break; // Stop backtrack to traverse down
					}
				}
			}
			else {
				auto& [c, child] = *it;

				stack.push('\0'); // If no more sibling, full backtrack 
				for (it++; it != current->children.end(); it++) {
					stack.push(it->first);
				}

				bytes.push_back('(');
				for (i = 1; i < nBytesPer; i++) {
					bytes.push_back('\0');
				}

				for (i = 0; i < nBytesPer; i++) {
					bytes.push_back(c >> (8 * i));
				}

				if (child->isWord && !child->children.empty()) {
					bytes.push_back('*');
					for (i = 1; i < nBytesPer; i++) {
						bytes.push_back('\0');
					}
				}
				current = child; // Traverse 1st child
			}
		} while (current != rt);
		// Stop upon backtrack to root (no other child to traverse)

		file.write((char*)&bytes[0], bytes.size());
		return bytes.size();
	}
	size_t write(const std::string path) { return write(path, root); }

	// Re: # of strs from path put into Node* current (ie root)
	size_t read(const std::string& path, Node<T>* current) {
		std::ifstream file(path, std::ios::binary | std::ios::ate);
		if (!file || file.tellg() == 0) return 0;

		size_t nBytes = file.tellg();
		size_t nBytesPer = sizeof(T);
		size_t nChars = nBytes / nBytesPer;

		if (nBytes % nBytesPer != 0) return 0;
		char* bytes = new char[nBytes];

		// Set cursor at start, read bytes as as chars
		file.seekg(std::ios::beg);
		file.read(&bytes[0], nBytes);
		T* chars = reinterpret_cast<T*>(bytes);

		size_t wordCount = 0;
		Node<T>* child;
		// Assume chars[0] to be '(': open children of root
		// Only letter (ie non-delimiter) can follow '('
		for (size_t i = 1; i < nChars; i++) {

			// ')' can be followed only by ')' or letter
			// If chars[i] != ')', it must be letter
			while (chars[i] != ')') {
				child = new Node<T>(false, current);
				current->children.emplace(chars[i], child);

				// Marked preceding Node/letter as word
				if (chars[i + 1] == '*') {
					child->isWord = true;
					wordCount++;
					i++;
				}

				// On '(', go down child, continue along same "while" 
				// loop for child. After each failed check against ')',
				// will backtrack to parent until re-encounter letter
				// and re-enter while loop with that letter
				if (chars[i + 1] == '(') {
					current = child;
					i++;
				}
				// Not followed by '(', known leaf must be marked
				else {
					child->isWord = true;
					wordCount++;
				}
				i++;
			}
			current = current->parent; // Backtrack upon ')'
		}

		if (nBytesPer == 1) delete[] chars;
		return wordCount;
	}
	size_t read(const std::string path) { return read(path, root); }

	// Do: Reset Trie for reuse. Delete all data
	void clear() {
		for (auto& [c, child] : root->children) delete child;
		root->children.clear();
		wordCount = 0;
	}
	~Trie() { delete root; }

private:
	Node<T>* root;
	size_t   wordCount;
};