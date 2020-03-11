#ifndef trie_h
#define trie_h

#include "bst.h"
#include <iostream>
#include <map>

using namespace std;

struct Slice {
    uint8_t size;
    char *data;

    Slice(char* d, int s): size(s), data(d) {}
    Slice(){}
};

struct CompressedTrieNode {
public:
    map<char, CompressedTrieNode *> children;
    BST sucs;
    char *edgelabel;
    int edgeLabelSize;
    bool isLeaf;
    int num_leafs;
    CompressedTrieNode *parent;
    Slice *value;

    CompressedTrieNode() : num_leafs(0) {};

    ~CompressedTrieNode() {
        sucs.clear();
    }
};

enum types {
    IS_SEARCH, IS_DEL
};

class CompressedTrie {
public:
    CompressedTrieNode *root;

    CompressedTrie();

    ~CompressedTrie() {
        if (root) {
            free(root);
            root = nullptr;
        }
    }

    bool insert(const Slice &key, const Slice &value);


    bool search(const Slice &key, Slice &value) const;

    bool searchDelWrapper(const Slice &key, Slice &value, enum types type) const;

    bool del(const Slice &key);

    bool del(const int &N);

    bool search(const int &N, Slice &A, Slice &B);
};

#endif
