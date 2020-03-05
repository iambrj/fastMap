#include "ctrie.hpp"
#include <memory>

using namespace std;

CompressedTrie::CompressedTrie() {
    root = make_unique<CompressedTrieNode>();
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
    // if(root->children.find(word[0]) == root->children.end()) {
    BSTNode *bstnode = root->sucs.search(*keyPointer);
    if (!bstnode) {
        bstnode = root->sucs.getOrInsert(*keyPointer);
        // root->children[word[0]] = make_unique<CompressedTrieNode>();
        bstnode->data = make_unique<CompressedTrieNode>();

        CompressedTrieNode *curr_node = bstnode->data.get();
        curr_node->edgelabel = keyPointer;
        curr_node->edgeLabelSize = key.size;
        curr_node->isLeaf = true;
        curr_node->parent = root.get();

        curr_node->value = make_unique<Slice>();
        Slice *newSlice = curr_node->value.get();
        newSlice->size = value.size;
        newSlice->data = value.data; // new char[value.size + 1];
        inc(curr_node, 1);
    } else {
        int i = 0, j = 0;
        // CompressedTrieNode* curr_node = root->children[word[0]].get();
        auto bstnode = root->sucs.getOrInsert(*keyPointer);
        auto curr_node = bstnode->data.get();

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
                    curr_node->isLeaf = true;
                    curr_node->value = make_unique<Slice>();
                    Slice *newSlice = curr_node->value.get();
                    newSlice->size = value.size;//*0x5587d6d62b20
                    newSlice->data = value.data;
                    inc(curr_node, 1);
                }
                    // j remaining - split word into 2
                else {
                    char *rem_word = wtc;
                    unique_ptr<CompressedTrieNode> newnode =
                            make_unique<CompressedTrieNode>();
                    newnode->edgelabel = rem_word;
                    newnode->edgeLabelSize = wtcSize - j;
                    newnode->isLeaf = curr_node->isLeaf;
                    newnode->parent = curr_node;
                    newnode->num_leafs = curr_node->num_leafs;
                    if (curr_node->isLeaf) {
                        newnode->value = std::move(curr_node->value);
                    }

                    newnode->sucs.root = curr_node->sucs.root;

                    update_kids(newnode->sucs.getRoot(), newnode.get());

                    curr_node->edgelabel = word_to_cmp;
                    curr_node->edgeLabelSize = j;

                    curr_node->isLeaf = true;
                    // curr_node->children.clear();
                    curr_node->sucs.root = nullptr;

                    // curr_node->children[rem_word[0]] = std::move(newnode);

                    curr_node->sucs.getOrInsert(rem_word[0])->data = std::move(newnode);

                    curr_node->value = make_unique<Slice>();
                    Slice *newSlice = curr_node->value.get();
                    newSlice->size = value.size;
                    newSlice->data = value.data; //  new char[value.size + 1];
                    inc(curr_node, 1);
                }
            }
                // i not complete, j complete
            else if (j == wtcSize) {
                // no remaining edge
                // if (curr_node->children.find(word[i]) ==
                // curr_node->children.end()) {
                if (!curr_node->sucs.search(*keyPointer)) {
                    CompressedTrieNode *curr_parent = curr_node;
                    // curr_node->children[word[i]] =
                    // make_unique<CompressedTrieNode>();

                    auto node = curr_node->sucs.getOrInsert(*keyPointer);
                    node->data = make_unique<CompressedTrieNode>();

                    //curr_node = curr_node->children[word[i]].get();
                    curr_node = node->data.get();
                    curr_node->edgelabel = keyPointer;
                    curr_node->edgeLabelSize = key.size - i;
                    curr_node->isLeaf = true;
                    /* curr_node->num_leafs++; */
                    curr_node->parent = curr_parent;
                    i = key.size;
                    curr_node->value = make_unique<Slice>();
                    Slice *newSlice = curr_node->value.get();
                    newSlice->size = value.size;
                    newSlice->data = value.data; // new char[value.size + 1];
                    inc(curr_node, 1);

                } else {
                    // remaining edge - continue with matching
                    // curr_node = curr_node->children[word[i]].get();
                    bstnode = curr_node->sucs.getOrInsert(*keyPointer);
                    curr_node = bstnode->data.get();
                }
            }
                // i not complete & j not complete. Split into two and insert
            else {
                char *rem_word_i = keyPointer; // word.substr(i);
                char *rem_word_j = wtc; // word_to_cmp.substr(j);
                char *match_word = word_to_cmp; // word_to_cmp.substr(0, j);

                unique_ptr<CompressedTrieNode> newnode =
                        make_unique<CompressedTrieNode>();
                newnode->isLeaf = curr_node->isLeaf;
                newnode->num_leafs = curr_node->num_leafs;
                if (curr_node->isLeaf) {
                    newnode->value = std::move(curr_node->value);
                }
                // newnode->children = std::move(curr_node->children);
                // newnode->sucs = std::move(curr_node->sucs);
                newnode->sucs.root = curr_node->sucs.root;
                curr_node->sucs.root = nullptr;
                newnode->edgelabel = rem_word_j;
                newnode->edgeLabelSize = wtcSize - j;
                newnode->parent = curr_node;
                update_kids(newnode->sucs.getRoot(), newnode.get());

                unique_ptr<CompressedTrieNode> newnode2 = make_unique<CompressedTrieNode>();
                newnode2->isLeaf = true;
                newnode2->num_leafs++;
                newnode2->edgelabel = rem_word_i;
                newnode2->edgeLabelSize = key.size - i;
                newnode2->value = make_unique<Slice>();
                newnode2->parent = curr_node;
                Slice *newSlice = newnode2->value.get();
                newSlice->size = value.size;
                newSlice->data = value.data; // new char[value.size + 1];

                curr_node->isLeaf = false;
                curr_node->edgelabel = match_word;
                curr_node->edgeLabelSize = j;

                // curr_node->children.clear();
                curr_node->sucs.root = nullptr;
//                printf("%p", &curr_node->sucs);
                // curr_node->children[rem_word_j[0]] = std::move(newnode);
                curr_node->sucs.getOrInsert(rem_word_j[0])->data =
                        std::move(newnode);

                //curr_node->children[rem_word_i[0]] = std::move(newnode2);
                char x = rem_word_i[0];
                auto node = curr_node->sucs.getOrInsert(x);
                node->data = std::move(newnode2);

                inc(curr_node, 1);

                i = key.size;
            }
        }
    }

    return true;
}

bool search_kids_stuff(BSTNode *r, char *keyPointer, int keySize, Slice &A, Slice &B, int &remaining, char *kOrg) {
    if (!r) return false;

    if (search_kids_stuff(r->left, keyPointer, keySize, A, B, remaining, kOrg)) return true;

    auto trieNode = r->data.get();

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

    auto trieNode = r->data.get();

    if (trieNode->num_leafs < remaining) {
        remaining -= trieNode->num_leafs;
        return del_kids_stuff(r->right, remaining);
    }

    if (trieNode->isLeaf)
        remaining--;

    if (remaining == 0) {
        trieNode->isLeaf = false;
        trieNode->value.reset();
        inc(trieNode, -1);
        return true;
    }

    return del_kids_stuff(trieNode->sucs.getRoot(), remaining);
}

bool CompressedTrie::del(const int &N) {
    int left = N;

    return del_kids_stuff(root->sucs.getRoot(), left);
}

bool CompressedTrie::search(Slice &key, Slice &value) const {
    int i = 0, j = 0;
    char *keyPointer = key.data;

    if (key.size == 0)
        return false;

    if (!root->sucs.search(*keyPointer))
        return false;

    bool ispresent = false;
    BSTNode *bstnode = root->sucs.getOrInsert(*keyPointer);
    CompressedTrieNode *curr_node = bstnode->data.get();

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
                value.size = curr_node->value->size;
                value.data = curr_node->value->data;
            }
        }
            // match remaining
        else {
            // j completed
            if (j == wtcSize) {
                // nowhere to go
                if (!curr_node->sucs.search(*keyPointer)) {
                    ispresent = false;
                    break;
                } else {
                    // continue matching
                    curr_node =
                            curr_node->sucs.getOrInsert(*keyPointer)->data.get();
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

bool CompressedTrie::del(const Slice &key) {
    int i = 0, j = 0;
    char *keyPointer = key.data;

    if (key.size == 0)
        return false;

    BSTNode *bstnode = root->sucs.search(*keyPointer);

    if (!bstnode)
        return false;

    bool ispresent = false;
    // TODO: replace with result of search;
    CompressedTrieNode *curr_node = bstnode->data.get();

    // CompressedTrieNode* curr_node = root->children[word[0]].get();
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
                curr_node->isLeaf = false;
                curr_node->value.reset();
                inc(curr_node, -1);
            }
            break;
        }
            // match remaining
        else {
            // j completed
            if (j == wtcSize) {
                // // nowhere to go
                //if (curr_node->children.find(*keyPointer) ==
                //   curr_node->children.end()) {
                //  ispresent = false;
                // break;
                //     } else {
                // continue matching
                //    curr_node = curr_node->children[*keyPointer].get();
                //}
                BSTNode *bstnode = curr_node->sucs.search(*keyPointer);

                if (!bstnode) {
                    ispresent = false;
                    break;
                } else {
                    curr_node = bstnode->data.get();
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

// BSTNode FUNCTIONS
BSTNode::BSTNode(char c) : c(c), left(nullptr), right(nullptr) {
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
        this->data.reset();
        this->data = nullptr;
    }
}

// BST FUNCTIONS
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

/*BSTNode* BST::_del(BSTNode* cur, char c) {
    while (cur) {
        if (cur->c < c) {
            cur = cur->right;
        } else if (cur->c > c) {
            cur = cur->left;
        } else {
            delete cur;
            return nullptr;
        }
    }
    return nullptr;
}*/

BST::BST() : root(nullptr) {
}

BST::~BST() {
    if (this->root) {
        delete this->root;
        this->root = nullptr;
    }
}

/*
BST::BST(BST&& b) noexcept {
    root = b.root;
}

BST::BST(const BST& b) {
    root = b.root;
}*/

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
