## Features

1. Essential Trie's functions: has, find, add, remove, getWords

2. Trie is templated to use of **any char type** (ie UTF-8) that basic_string can store

3. Trie can write to and read from binary file **iteratively** (for efficiency)

4. Lots of comments

### Functions
| Core               | Utility                    |
|:------------------:|---------------------------|
| has                | getWords                  |
| add                | write                     |                  |
| remove             | read                      |
| find (Node only)   | append (Node only         |

### Notes

1. Must do preprocessing yourself before using Trie (ie convert to lowercase,
separate string into words by whitespace, consider special characters like - or ')

2. (, ), and are * reserved as delimiter for read and write