#include "ctrie.hpp"
#include<cassert>

using namespace std;

template<typename T>
void resetPointer(T *&x) {
    if (x) {
        free(x);
        x = NULL;
    }
}

CompressedTrie::CompressedTrie() {
    root = new CompressedTrieNode();
    root->parent = nullptr;
}

void update_kids(BSTNode *r, CompressedTrieNode *val) {
    if (!r) return;
    update_kids(r->left, val);
    update_kids(r->right, val);
    r->data->parent = val;
}

void inc(CompressedTrieNode *curr_node, const int &val) {
    while (curr_node) {
        curr_node->num_leafs += val;
        curr_node = curr_node->parent;
    }
}

bool CompressedTrie::insert(const Slice &key, const Slice &value) {
    char *keyPointer = key.data;

    if (key.size == 0)
        return false;
    // No matching edge present, just insert entire word
    BSTNode *bstnode = root->sucs.search(*keyPointer);

    if (!bstnode) {
        bstnode = root->sucs.getOrInsert(*keyPointer);
        bstnode->data = new CompressedTrieNode();

        CompressedTrieNode *curr_node = bstnode->data;
        curr_node->edgelabel = keyPointer;
        curr_node->edgeLabelSize = key.size;
        curr_node->isLeaf = true;
        curr_node->parent = root;

        curr_node->value = new Slice();
        Slice *newSlice = curr_node->value;
        newSlice->size = value.size;
        newSlice->data = value.data;
        inc(curr_node, 1);
        return false;
    } else {
        int i = 0, j = 0;
        auto bstnode = root->sucs.getOrInsert(*keyPointer);
        auto curr_node = bstnode->data;

        while (i < key.size) {
            char *word_to_cmp = curr_node->edgelabel;
            int wtcSize = curr_node->edgeLabelSize;
            char *wtc = word_to_cmp;

            j = 0;
            while (i < key.size && j < wtcSize &&
                   *keyPointer == *wtc) {
                i++;
                keyPointer++;
                j++;
                wtc++;
            }
            // i complete
            if (i == key.size) {
                // j also complete - mark this as leaf node
                if (j == wtcSize) {
                    bool should = false;
                    if (curr_node->isLeaf)
                        should = true;
                    curr_node->isLeaf = true;
                    curr_node->value = new Slice();
                    Slice *newSlice = curr_node->value;
                    newSlice->size = value.size;//*0x5587d6d62b20
                    newSlice->data = value.data;
                    inc(curr_node, !should);
                    return should;
                }
                    // j remaining - split word into 2
                else {
                    char *rem_word = wtc;
                    CompressedTrieNode *newnode = new CompressedTrieNode();
                    newnode->edgelabel = rem_word;
                    newnode->edgeLabelSize = wtcSize - j;
                    newnode->isLeaf = curr_node->isLeaf;
                    newnode->parent = curr_node;
                    newnode->num_leafs = curr_node->num_leafs;
                    if (curr_node->isLeaf) {
                        newnode->value = std::move(curr_node->value);
                    }

                    newnode->sucs.root = curr_node->sucs.root;

                    update_kids(newnode->sucs.getRoot(), newnode);

                    curr_node->edgelabel = word_to_cmp;
                    curr_node->edgeLabelSize = j;

                    curr_node->isLeaf = true;
                    curr_node->sucs.root = nullptr;

                    curr_node->sucs.getOrInsert(rem_word[0])->data = std::move(newnode);

                    curr_node->value = new Slice();
                    Slice *newSlice = curr_node->value;
                    newSlice->size = value.size;
                    newSlice->data = value.data;
                    inc(curr_node, 1);
                    return false;

                }
            }
                // i not complete, j complete
            else if (j == wtcSize) {
                // no remaining edge
                if (!curr_node->sucs.search(*keyPointer)) {
                    CompressedTrieNode *curr_parent = curr_node;

                    auto node = curr_node->sucs.getOrInsert(*keyPointer);
                    node->data = new CompressedTrieNode();

                    curr_node = node->data;
                    curr_node->edgelabel = keyPointer;
                    curr_node->edgeLabelSize = key.size - i;
                    curr_node->isLeaf = true;
                    curr_node->parent = curr_parent;
                    i = key.size;
                    curr_node->value = new Slice();
                    Slice *newSlice = curr_node->value;
                    newSlice->size = value.size;
                    newSlice->data = value.data; // new char[value.size + 1];
                    inc(curr_node, 1);

                } else {
                    // remaining edge - continue with matching
                    // curr_node = curr_node->children[word[i]];
                    bstnode = curr_node->sucs.getOrInsert(*keyPointer);
                    curr_node = bstnode->data;
                }
            }
                // i not complete & j not complete. Split into two and insert
            else {
                char *rem_word_i = keyPointer; // word.substr(i);
                char *rem_word_j = wtc; // word_to_cmp.substr(j);
                char *match_word = word_to_cmp; // word_to_cmp.substr(0, j);

                CompressedTrieNode *newnode = new CompressedTrieNode();

                newnode->isLeaf = curr_node->isLeaf;
                newnode->num_leafs = curr_node->num_leafs;
                if (curr_node->isLeaf) {
                    newnode->value = std::move(curr_node->value);
                }

                newnode->sucs.root = curr_node->sucs.root;
                curr_node->sucs.root = nullptr;
                newnode->edgelabel = rem_word_j;
                newnode->edgeLabelSize = wtcSize - j;
                newnode->parent = curr_node;
                update_kids(newnode->sucs.getRoot(), newnode);

                CompressedTrieNode *newnode2 = new CompressedTrieNode();
                newnode2->isLeaf = true;
                newnode2->num_leafs++;
                newnode2->edgelabel = rem_word_i;
                newnode2->edgeLabelSize = key.size - i;
                newnode2->value = new Slice();
                newnode2->parent = curr_node;
                Slice *newSlice = newnode2->value;
                newSlice->size = value.size;
                newSlice->data = value.data;

                curr_node->isLeaf = false;
                curr_node->edgelabel = match_word;
                curr_node->edgeLabelSize = j;

                curr_node->sucs.root = nullptr;
                curr_node->sucs.getOrInsert(rem_word_j[0])->data =
                        std::move(newnode);

                char x = rem_word_i[0];
                auto node = curr_node->sucs.getOrInsert(x);
                node->data = std::move(newnode2);

                inc(curr_node, 1);

                return false;
            }
        }
    }

    return true;
}

bool search_kids_stuff(BSTNode *r, char *keyPointer, int keySize, Slice &A, Slice &B, int &remaining, char *kOrg) {
    if (!r) return false;

    if (search_kids_stuff(r->left, keyPointer, keySize, A, B, remaining, kOrg)) return true;

    auto trieNode = r->data;

    if (trieNode->num_leafs < remaining) {
        remaining -= trieNode->num_leafs;
        return search_kids_stuff(r->right, keyPointer, keySize, A, B, remaining, kOrg);
    }

    // edge label loop
    char *edger = trieNode->edgelabel;
    for (int i = 0; i < trieNode->edgeLabelSize; i++) {
        *keyPointer = *edger;
        keyPointer++;
        edger++;
        keySize++;
    }

    if (trieNode->isLeaf)
        remaining--;

    if (remaining == 0) {
        A.data = kOrg;
        A.size = keySize;
        B.data = trieNode->value->data;
        B.size = trieNode->value->size;
        return true;
    }

    return search_kids_stuff(trieNode->sucs.getRoot(), keyPointer, keySize, A, B, remaining, kOrg);
}

bool CompressedTrie::search(const int &N, Slice &A, Slice &B) {
    int left = N;

    char *keyPointer = (char *) malloc(65), *kOrg = keyPointer;
    int keySize = 0;

    return search_kids_stuff(root->sucs.getRoot(), keyPointer, keySize, A, B, left, kOrg);
}

bool del_kids_stuff(BSTNode *r, int &remaining) {
    if (!r) return false;

    if (del_kids_stuff(r->left, remaining)) return true;

    auto trieNode = r->data;

    if (trieNode->num_leafs < remaining) {
        remaining -= trieNode->num_leafs;
        return del_kids_stuff(r->right, remaining);
    }

    if (trieNode->isLeaf)
        remaining--;

    if (remaining == 0) {
        bool should = trieNode->isLeaf;
        trieNode->isLeaf = false;
        resetPointer(trieNode->value);
        trieNode->value = nullptr;
        inc(trieNode, should ? -1 : 0);
        return true;
    }

    return del_kids_stuff(trieNode->sucs.getRoot(), remaining);
}

bool CompressedTrie::del(const int &N) {
    int left = N;

    return del_kids_stuff(root->sucs.getRoot(), left);
}

bool CompressedTrie::searchDelWrapper(const Slice &key, Slice &value, enum types type) const {
    int i = 0, j = 0;
    char *keyPointer = key.data;

    if (key.size == 0)
        return false;

    if (!root->sucs.search(*keyPointer))
        return false;

    bool ispresent = false;
    BSTNode *bstnode = root->sucs.getOrInsert(*keyPointer);
    CompressedTrieNode *curr_node = bstnode->data;

    while (i < key.size) {
        j = 0;

        char *word_to_match = curr_node->edgelabel;
        char *wtc = word_to_match;
        int wtcSize = curr_node->edgeLabelSize;

        while (i < key.size && j < wtcSize &&
               *keyPointer == *wtc) {
            i++;
            keyPointer++;
            wtc++;
            j++;
        }
        // completed matching
        if (i == key.size) {
            ispresent = j == wtcSize && curr_node->isLeaf;
            if (ispresent) {
                if (type == IS_SEARCH) {
                    value.size = curr_node->value->size;
                    value.data = curr_node->value->data;
                } else if (type == IS_DEL) {
                    bool should = curr_node->isLeaf;
                    curr_node->isLeaf = false;
                    resetPointer(curr_node->value);
                    curr_node->value = nullptr;
                    inc(curr_node, should ? -1 : 0);
                } else {
                    assert(false); // not implemented
                }
            }
        }
            // match remaining
        else {
            // j completed
            if (j == wtcSize) {
                auto bstnode = curr_node->sucs.search(*keyPointer);
                // nowhere to go
                if (!bstnode) {
                    ispresent = false;
                    break;
                } else {
                    // continue matching
                    curr_node = bstnode->data;
                }
            }
                // j remaining, no match
            else {
                ispresent = false;
                break;
            }
        }
    }
    return ispresent;
}

bool CompressedTrie::search(const Slice &key, Slice &value) const {
    return searchDelWrapper(key, value, IS_SEARCH);
}

bool CompressedTrie::del(const Slice &key) {
    Slice value;
    return searchDelWrapper(key, value, IS_DEL);
}


// BSTNode FUNCTIONS
BSTNode::BSTNode(char cc) : c(cc), left(nullptr), right(nullptr) {
}

BSTNode::~BSTNode() {
    if (this->left) {
        delete this->left;
        this->left = nullptr;
    }
    if (this->right) {
        delete this->right;
        this->right = nullptr;
    }
    if (this->data) {
        resetPointer(this->data);
        this->data = nullptr;
    }
}

// BST FUNCTIONS
BSTNode *BST::_insert(BSTNode *cur, char c) {
    if (!cur) {  // 50% of all lookups
        return new BSTNode(c);
    }

    if (cur->c < c) {
        cur->right = _insert(cur->right, c);
    } else if (cur->c > c) {
        cur->left = _insert(cur->left, c);
    }

    return cur;
}

BSTNode *BST::_get(BSTNode *cur, char c) {
    if (!cur) {
        return nullptr;
    }

    if (cur->c < c) {
        return _get(cur->right, c);
    } else if (cur->c > c) {
        return _get(cur->left, c);
    } else {
        return cur;  // least likely, at the end
    }
}

BST::BST() : root(nullptr) {
}

BST::~BST() {
    if (this->root) {
        delete this->root;
        this->root = nullptr;
    }
}

BSTNode *BST::getOrInsert(char c) {
    this->root = this->_insert(this->root, c);
    return _get(this->root, c);
}

BSTNode *BST::search(char c) {
    return _get(this->root, c);
}

BSTNode *BST::getRoot() {
    return this->root;
}

void BST::clear() {
    if (this->root) {
        delete this->root;
        this->root = nullptr;
    }
}
