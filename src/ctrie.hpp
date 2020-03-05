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

struct BSTNode {

public:
    char c;
    unique_ptr<CompressedTrieNode> data;
    BSTNode* left;
    BSTNode* right;

    explicit BSTNode(char c);
    ~BSTNode();
};

class BST {

public:
    BSTNode* root;
    BSTNode* _insert(BSTNode* cur, char c);
    BSTNode* _get(BSTNode* cur, char c);

    BST();
    ~BST();
    //BST(const BST&);
    BST& operator=(BST b){
        root = b.root;
        return *this;
    }
    BST (BST&&) noexcept;
    BSTNode* getOrInsert(char c);
    BSTNode* search(char c);
    BSTNode* getRoot();
    void clear();
};

struct CompressedTrieNode {
public:
    map<char, unique_ptr<CompressedTrieNode> > children;
    BST sucs;
    string edgelabel;
    bool isLeaf;
    int num_leafs;
    CompressedTrieNode* parent;
    unique_ptr<Slice> value;
    CompressedTrieNode() : num_leafs(0){};
    ~CompressedTrieNode(){
        sucs.clear();
    }
};

class CompressedTrie {
public:
    unique_ptr<CompressedTrieNode> root;

    CompressedTrie();
    ~CompressedTrie(){
       root.reset();
    }
    bool insert(const Slice& key, const Slice& value);
    bool search(Slice& key, Slice& value) const;
    bool del(const Slice& key);
    bool del(const int& N);
    bool search(const int& N, Slice& A, Slice& B);
};

#endif /* trie_h */
