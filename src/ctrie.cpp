#include "ctrie.hpp"
#include <memory>
#include <cstring>

using namespace std;

CompressedTrie::CompressedTrie() {
    root = make_unique<CompressedTrieNode>();
}

void CompressedTrie::inc(CompressedTrieNode* curr_node, const int& val) {
    auto rooter = root.get();

    while (curr_node != rooter) {
        curr_node->num_leafs += val;
        curr_node = curr_node->parent;
    }
}

bool CompressedTrie::insert(const Slice& key, const Slice& value) {
    string word = string(key.data);

    if (word == "")
        return false;
    // No matching edge present, just insert entire word
    // if(root->children.find(word[0]) == root->children.end()) {
    if (!root->sucs.search(word[0])) {
        // root->children[word[0]] = make_unique<CompressedTrieNode>();
        BSTNode* bstnode = root->sucs.getOrInsert(word[0]);
        bstnode->data = make_unique<CompressedTrieNode>();

        CompressedTrieNode* curr_node = bstnode->data.get();
        curr_node->edgelabel = word;
        curr_node->isLeaf = true;
        curr_node->parent = root.get();

        curr_node->value = make_unique<Slice>();
        Slice* newSlice = curr_node->value.get();
        newSlice->size = value.size;
        newSlice->data = new char[value.size + 1];
        strncpy(newSlice->data, value.data, value.size + 1);
        inc(curr_node, 1);
    } else {
        size_t i = 0, j = 0;
        string word_to_cmp;
        // CompressedTrieNode* curr_node = root->children[word[0]].get();
        BSTNode* bstnode = root->sucs.getOrInsert(word[0]);
        CompressedTrieNode* curr_node = bstnode->data.get();
        while (i < word.length()) {
            word_to_cmp = curr_node->edgelabel;
            j = 0;
            while (i < word.length() && j < word_to_cmp.length() &&
                   word[i] == word_to_cmp[j]) {
                i++;
                j++;
            }
            // i complete
            if (i == word.length()) {
                // j also complete - mark this as leaf node
                if (j == word_to_cmp.length()) {
                    curr_node->isLeaf = true;
                    /* curr_node->num_leafs++; */
                    curr_node->value = make_unique<Slice>();
                    Slice* newSlice = curr_node->value.get();
                    newSlice->size = value.size;
                    newSlice->data = new char[value.size + 1];
                    strcpy(newSlice->data, value.data);
                    inc(curr_node, 1);
                }
                // j remaining - split word into 2
                else {
                    string rem_word = word_to_cmp.substr(j);
                    unique_ptr<CompressedTrieNode> newnode =
                        make_unique<CompressedTrieNode>();
                    newnode->edgelabel = rem_word;
                    newnode->isLeaf = curr_node->isLeaf;
                    newnode->parent = curr_node;
                    newnode->num_leafs = curr_node->num_leafs;
                    if (curr_node->isLeaf) {
                        newnode->value = std::move(curr_node->value);
                    }
                    // newnode->children = std::move(curr_node->children);
                    newnode->sucs = std::move(curr_node->sucs);

                    for (auto& baccha : newnode->children) {
                        baccha.second->parent = newnode.get();
                    }

                    curr_node->edgelabel = word_to_cmp.substr(0, j);

                    curr_node->isLeaf = true;
                    // curr_node->children.clear();
                    curr_node->sucs.clear();

                    // curr_node->children[rem_word[0]] = std::move(newnode);

                    BSTNode* bstnode = root->sucs.getOrInsert(rem_word[0]);
                    bstnode->data = std::move(newnode);

                    curr_node->value = make_unique<Slice>();
                    Slice* newSlice = curr_node->value.get();
                    newSlice->size = value.size;
                    newSlice->data = new char[value.size + 1];
                    strcpy(newSlice->data, value.data);
                    inc(curr_node, 1);
                }
            }
            // i not complete, j complete
            else if (j == word_to_cmp.length()) {
                // no remaining edge
                // if (curr_node->children.find(word[i]) ==
                // curr_node->children.end()) {
                if (!curr_node->sucs.search(word[i])) {
                    CompressedTrieNode* curr_parent = curr_node;
                    // curr_node->children[word[i]] =
                    // make_unique<CompressedTrieNode>();

                    BSTNode* bstnode = root->sucs.getOrInsert(word[i]);
                    bstnode->data = make_unique<CompressedTrieNode>();

                    // curr_node = curr_node->children[word[i]].get();
                    curr_node = bstnode->data.get();
                    curr_node->edgelabel = word.substr(i);
                    curr_node->isLeaf = true;
                    /* curr_node->num_leafs++; */
                    curr_node->parent = curr_parent;
                    i = word.length();
                    curr_node->value = make_unique<Slice>();
                    Slice* newSlice = curr_node->value.get();
                    newSlice->size = value.size;
                    newSlice->data = new char[value.size + 1];
                    strcpy(newSlice->data, value.data);
                    inc(curr_node, 1);

                } else {
                    // remaining edge - continue with matching
                    // curr_node = curr_node->children[word[i]].get();
                    BSTNode* bstnode = root->sucs.getOrInsert(word[i]);
                    curr_node = bstnode->data.get();
                }
            }
            // i not complete & j not complete. Split into two and insert
            else {
                string rem_word_i = word.substr(i);
                string rem_word_j = word_to_cmp.substr(j);
                string match_word = word_to_cmp.substr(0, j);

                unique_ptr<CompressedTrieNode> newnode =
                    make_unique<CompressedTrieNode>();
                newnode->isLeaf = curr_node->isLeaf;
                newnode->num_leafs = curr_node->num_leafs;
                if (curr_node->isLeaf) {
                    newnode->value = std::move(curr_node->value);
                }
                // newnode->children = std::move(curr_node->children);
                newnode->sucs = std::move(curr_node->sucs);
                newnode->edgelabel = rem_word_j;
                newnode->parent = curr_node;
                for (auto& baccha : newnode->children) {
                    baccha.second->parent = newnode.get();
                }

                unique_ptr<CompressedTrieNode> newnode2 =
                    make_unique<CompressedTrieNode>();
                newnode2->isLeaf = true;
                newnode2->num_leafs++;
                newnode2->edgelabel = rem_word_i;
                newnode2->value = make_unique<Slice>();
                newnode2->parent = curr_node;
                Slice* newSlice = newnode2->value.get();
                newSlice->size = value.size;
                newSlice->data = new char[value.size + 1];
                strcpy(newSlice->data, value.data);

                curr_node->isLeaf = false;
                curr_node->edgelabel = match_word;

                // curr_node->children.clear();
                curr_node->sucs.clear();
                // curr_node->children[rem_word_j[0]] = std::move(newnode);
                curr_node->sucs.getOrInsert(rem_word_j[0])->data =
                    std::move(newnode);

                // curr_node->children[rem_word_i[0]] = std::move(newnode2);
                curr_node->sucs.getOrInsert(rem_word_i[0])->data =
                    std::move(newnode2);

                inc(curr_node, 1);

                i = word.length();
            }
        }
    }

    return true;
}

bool CompressedTrie::search(const int& N, Slice& A, Slice& B) {
    CompressedTrieNode* curr_node = root.get();

    int left = N;

    char *keyPointer = (char*)malloc(65), *kOrg = keyPointer;
    int keySize = 0;

    while (true) {
        for (auto& edge : curr_node->children) {
            int child_cnt = edge.second->num_leafs;
            if (child_cnt == 0)
                continue;

            if (left > child_cnt) {
                left -= child_cnt;
            } else {
                for (auto& c : edge.second.get()->edgelabel) {
                    *keyPointer = c;
                    keyPointer++;
                    keySize++;
                }

                curr_node = edge.second.get();
                if (curr_node->isLeaf)
                    left--;

                if (left == 0) {
                    A.data = kOrg;
                    A.size = keySize;
                    B.data = curr_node->value->data;
                    B.size = curr_node->value->size;
                    return true;
                }

                break;
            }
        }
    }
}

bool CompressedTrie::del(const int& N) {
    CompressedTrieNode* curr_node = root.get();

    int left = N;

    while (true) {
        bool done = false;
        for (auto& edge : curr_node->children) {
            int child_cnt = edge.second->num_leafs;
            if (child_cnt == 0)
                continue;
            done = true;

            if (left > child_cnt) {
                left -= child_cnt;
            } else {
                curr_node = edge.second.get();
                if (curr_node->isLeaf)
                    left--;

                if (left == 0) {
                    curr_node->isLeaf = false;
                    curr_node->value.reset();
                    inc(curr_node, -1);
                    return true;
                }

                break;
            }
        }

        if (!done)
            return false;
    }
}

bool CompressedTrie::search(Slice& key, Slice& value) const {
    size_t i = 0, j = 0;
    char* keyPointer = key.data;

    if (key.size == 0)
        return false;

    if (!root->sucs.search(*keyPointer))
        return false;

    bool ispresent = false;
    BSTNode* bstnode = root->sucs.getOrInsert(*keyPointer);
    CompressedTrieNode* curr_node = bstnode->data.get();

    while (i < key.size) {
        j = 0;

        const string& word_to_match = curr_node->edgelabel;

        while (i < key.size && j < word_to_match.length() &&
               *keyPointer == word_to_match[j]) {
            i++;
            keyPointer++;
            j++;
        }
        // completed matching
        if (i == key.size) {
            ispresent = j == word_to_match.length() && curr_node->isLeaf;
            if (ispresent) {
                value.size = curr_node->value->size;
                value.data = curr_node->value->data;
            }
        }
        // match remaining
        else {
            // j completed
            if (j == word_to_match.length()) {
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

bool CompressedTrie::del(const Slice& key) {
    size_t i = 0, j = 0;
    char* keyPointer = key.data;

    if (key.size == 0)
        return false;

    if (!root->sucs.search(*keyPointer))
        return false;

    bool ispresent = false;
    BSTNode* bstnode = root->sucs.getOrInsert(*keyPointer);
    CompressedTrieNode* curr_node = bstnode->data.get();

    // CompressedTrieNode* curr_node = root->children[word[0]].get();
    while (i < key.size) {
        j = 0;
        const string& word_to_match = curr_node->edgelabel;
        while (i < key.size && j < word_to_match.length() &&
               *keyPointer == word_to_match[j]) {
            i++;
            keyPointer++;
            j++;
        }

        // completed matching
        if (i == key.size) {
            ispresent = j == word_to_match.length() && curr_node->isLeaf;
            if (ispresent) {
                curr_node->isLeaf = false;
                curr_node->value.reset();
                inc(curr_node, -1);
                ispresent = true;
            }
            break;
        }
        // match remaining
        else {
            // j completed
            if (j == word_to_match.length()) {
                // nowhere to go
                if (curr_node->children.find(*keyPointer) ==
                    curr_node->children.end()) {
                    ispresent = false;
                    break;
                } else {
                    // continue matching
                    curr_node = curr_node->children[*keyPointer].get();
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
    delete this->left;
    delete this->right;

    this->data.reset();
    this->left = nullptr;
    this->right = nullptr;
    // this node will automatically be deleted
}

// BST FUNCTIONS
BSTNode* BST::_insert(BSTNode* cur, char c) {
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

BSTNode* BST::_get(BSTNode* cur, char c) {
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

BSTNode* BST::_del(BSTNode* cur, char c) {
    if (!cur) {
        return nullptr;
    }

    if (cur->c < c) {
        cur->right = _del(cur->right, c);
        return cur;
    } else if (cur->c > c) {
        cur->left = _del(cur->left, c);
        return cur;
    }
    delete cur;
    return nullptr;
}

BST::BST() : root(nullptr) {
}

BST::~BST() {
    delete this->root;
    this->root = nullptr;
}

BSTNode* BST::getOrInsert(char c) {
    this->root = this->_insert(this->root, c);
    return _get(this->root, c);
}

BSTNode* BST::search(char c) {
    return _get(this->root, c);
}

void BST::remove(char c) {
    this->root = _del(this->root, c);
}

BSTNode* BST::getRoot() {
    return this->root;
}

void BST::clear() {
    delete this->root;
}
