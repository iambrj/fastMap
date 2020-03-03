#ifndef trie_h
#define trie_h

#include <iostream>
#include <map>
#include <memory>
using namespace std;

struct Slice{
    uint8_t size;
    char*   data;
};

struct CompressedTrieNode {
    map<char, unique_ptr<CompressedTrieNode> > children;
    string edgelabel;
    bool isLeaf;
    int num_leafs;
    CompressedTrieNode* parent;
    unique_ptr<Slice> value;
    CompressedTrieNode() : num_leafs(0){};
};

class CompressedTrie {
    unique_ptr<CompressedTrieNode> root;
    void inc(CompressedTrieNode* curr_node, const int& val);
public:
    CompressedTrie();
    bool insert(const Slice& key, const Slice& value);
    Slice* search(const Slice& key) const;
    bool del(const Slice& key);
    bool del(const int& N);
    string search(const int& N);
};

#endif /* trie_h */
