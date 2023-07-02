/*PRE: Build catch2 library with cmake (ie in /build)
  ie : "mkdir build", "cd build", "cmake ..", "make"

IF:Case_1, for cmake route, add 2 lines to build/CMakeLists.txt:
find_package(Catch2 REQUIRED)
target_link_libraries(foo PRIVATE Catch2::Catch2)
// Some catch2 functions need additional includes

IF:Case_2, for VS Solution, add as static library (steps below)
https://stackoverflow.com/questions/23882112/how-to-add-static-libraries-to-a-visual-studio-project

Run as x64 (Catch2 does not support x86)

(Project) Properties->Configuration_Properties: [TO : ADD]
"C/C++"/General/"Additional Include Directories" : 
[catch2_folder]/src
[catch2_folder]/build/generated-includes

Linker/Input/"Additional Dependencies" [TO : ADD]
[catch2_folder]/build/src/Debug/Catch2Maind.lib
[catch2_folder]/build/src/Debug/Catch2d.lib
*/
#ifndef CATCH_CONFIG_MAIN
#define CATCH_CONFIG_MAIN
 
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include "trie.h"

TEST_CASE("Trie", "") {
    // Other constructors tested/used in other sections
    SECTION("Trie(): size 0, no word") {
        Trie trie;
        REQUIRE(trie.getWordCount() == 0);
        REQUIRE(trie.getWords().empty());
    }

    // Tested/used in other section
    //SECTION("has(string&") {}

    SECTION("add(string&)") {
        Trie trie;

        SECTION("For empty str, return false, store unchanged") {
            REQUIRE(trie.add("") == false);
            REQUIRE(trie.getWordCount() == 0);
            REQUIRE(trie.getWords().empty());
        }

        REQUIRE(trie.add("hats") == true);
        REQUIRE(trie.add("hat")    == true);
        REQUIRE(trie.add("cat") == true);
        REQUIRE(trie.has("hats"));
        auto words = trie.getWords();
        REQUIRE(words.size() == 3);
        REQUIRE(words.count("hat"));

        SECTION("For duplicate word, return false, store unchanged") {
            REQUIRE(trie.add("hats") == false);
            REQUIRE(trie.getWordCount() == 3);
        }
    }

    SECTION("add(Iter it, Iter end)") {
        Trie trie;
        std::vector<std::string> words(
            { "app", "art", "arts", "basic", "car", "case", "cast" });
        REQUIRE(trie.add(words.begin(), words.end()) == words.size());

        for (const auto& word : words) {
            REQUIRE(trie.has(word));
        }
        REQUIRE(trie.add(words.begin(), words.end()) == 0);
    }

    SECTION("add(initializer_list<str>)") {
        Trie trie;
        std::vector<std::string> words({"art", "arts", "car", "cast"});
        REQUIRE(trie.add({"art", "arts", "car", "cast"}) == words.size());

        for (const auto& word : words) {
            REQUIRE(trie.has(word));
        }
        REQUIRE(trie.add(words.begin(), words.end()) == 0);
    }

    SECTION("remove(string&)") {
        Trie trie;

        SECTION("For empty str, return false, store unchanged") {
            REQUIRE(trie.remove("") == false);
            REQUIRE(trie.getWordCount() == 0);
        }

        trie.add("hats");
        trie.add("hat");
        REQUIRE(trie.remove("hats") == true);
        REQUIRE(trie.getWordCount() == 1);

        SECTION("For non-present str, return false, store unchanged") {
            REQUIRE(trie.remove("hatsful") == false);
            REQUIRE(trie.remove("hats") == false);
            REQUIRE(trie.getWordCount() == 1);
        }
    }

    SECTION("remove(initializer_list<str>)") {
        std::vector<std::string> words({ "art", "arts", "car", "cast" });
        Trie trie(words.begin(), words.end());
        REQUIRE(trie.remove({"art", "arts", "car", "cast", "me"}) == words.size());

        for (const auto& word : words) {
            REQUIRE(!trie.has(word));
        }
        REQUIRE(trie.remove(words.begin(), words.end()) == 0);
    }

    SECTION("has(string&)") {
        Trie trie;
        REQUIRE(trie.has("hats") == false);
        trie.add("hats");
        REQUIRE(trie.has("hat") == false);
        REQUIRE(trie.has("dog") == false);
    }

    SECTION("getWords()") {
        std::vector<std::string> vec =
        {"app", "arc", "art", "arts", "base", "basic", "care",
         "case", "cast", "dare", "dasher", "die" };
        Trie trie(vec.begin(), vec.end());

        auto words = trie.getWords();
        for (const auto& word : vec) {
            REQUIRE(words.find(word) != words.end());
        }
    }

    // TODO TODO
    SECTION("[write + read](string& path)") {
        
    if (!std::filesystem::exists("./Deletable")) {
         std::filesystem::create_directory("./Deletable");
    }

    SECTION("write([childless root] does not create file") {
        Trie trie;
        trie.write("./Deletable/MczBnLXbrK.txt");
        std::ifstream file("./Deletable/MczBnLXbrK.txt");
        REQUIRE(!file.good());
    }

    SECTION("read([empty file] should be error-free") {
        std::ofstream file("./Deletable/emptytmptp.dat");
        Trie trie("./Deletable/emptytmptp.dat");
        REQUIRE(trie.getWordCount() == 0);
        REQUIRE(trie.getWords().empty());
    }

    SECTION("ENG (T == char),   1 byte") {
        std::vector<std::string> words = {
            "app", "arc", "art", "arts", "base", "basic",
            "car", "case", "cast", "dare", "dasher", "die" };
        Trie wTrie(words.begin(), words.end());
        wTrie.write("./Deletable/trie_ENG.txt");
        
        Trie rTrie("./Deletable/trie_ENG.txt");
        for (auto& word : wTrie.getWords()) {
            REQUIRE(rTrie.has(word));
        }
    }

    SECTION("JAP (T != char), > 1 byte") {
        Trie<wchar_t> wTrie({
            L"ぁぱぱ", L"ぁらぎ", L"ぁらつ", L"ばせせ", L"ばせいい", L"ぢエ"});
        wTrie.write("./Deletable/trie_JAP.trie2");

        Trie<wchar_t> rTrie("./Deletable/trie_JAP.trie2");
        for (auto& word : wTrie.getWords()) {
            REQUIRE(rTrie.has(word));
        }
    }
    }

    SECTION("trie.clear()") {
        Trie trie;
        trie.add("hats");
        trie.add("hat");
        trie.add("dam");
        trie.add("dater");

        trie.clear();
        REQUIRE(trie.getWordCount() == 0);
        REQUIRE(trie.getWords().empty());

        SECTION("After clear(), can reuse like new") {
            REQUIRE(trie.has("hats") == false);

            REQUIRE(trie.remove("hat") == false);
            REQUIRE(trie.remove("date") == false);

            REQUIRE(trie.add("hat") == true);
            REQUIRE(trie.add("date") == true);
        }
    }
}

TEST_CASE("Trie:Node", "") {
    SECTION("root.remove(string&) deletes unneeded branch") {
        Trie<>::Node<> root(false, nullptr);
        root.add("hat");
        root.add("hats");

        // Delete up from end to "hat" prefix branch, word still needed
        root.remove("hats");
        size_t depth = 0;
        auto end = root.find("hat", depth);
        depth = 0;
        REQUIRE(root.find("hats", depth) == end);

        // Keep "hat" branch because longer branch "hatch" uses it
        root.add("hatch");
        root.remove("hat");
        depth = 0;
        REQUIRE(root.find("hat", depth) == end);

        // "hatcher" is not present, so no remove and no change
        root.remove("hatcher");
        depth = 0;
        root.find("hatcher", depth);
        REQUIRE(depth == std::string("hatch").size());

        // Delete unmarked "hat" branch because no longer branch uses it
        root.remove("hatch");
        depth = 0;
        REQUIRE(root.find("hat", depth) == &root);
    }
}

#endif