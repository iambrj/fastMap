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

class CompressedTrieNode;

class BSTNode {
   public:
    char c;
    unique_ptr<CompressedTrieNode> data;
    BSTNode* left;
    BSTNode* right;

    explicit BSTNode(char c);
    ~BSTNode();
};

class BST {
    BSTNode* root;
    BSTNode* _insert(BSTNode* cur, char c);
    BSTNode* _get(BSTNode* cur, char c);
    BSTNode* _del(BSTNode* cur, char c);

   public:
    BST();
    ~BST();
    BSTNode* getOrInsert(char c);
    BSTNode* search(char c);
    void remove(char c);
    BSTNode* getRoot();
    void clear();
};

struct CompressedTrieNode {
    map<char, unique_ptr<CompressedTrieNode> > children;
    BST sucs;
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
    bool search(Slice& key, Slice& value) const;
    bool del(const Slice& key);
    bool del(const int& N);
    bool search(const int& N, Slice& A, Slice& B);
};

#endif /* trie_h */
