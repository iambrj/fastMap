#include "ctrie.hpp"
#include "util.hpp"
#include<cassert>

using namespace std;

CompressedTrie::CompressedTrie() {
    root = new CompressedTrieNode();
    root->parent = nullptr;
}

void updateChildren(BSTNode *r, CompressedTrieNode *val) {
    if (!r) return;
    updateChildren(r->left, val);
    updateChildren(r->right, val);
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

        curr_node->value = new Slice(value.data, value.size);
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
                    curr_node->value = new Slice(value.data, value.size);
                    inc(curr_node, !should);
                    return should;
                }
                    // j remaining - split word into 2
                else {
                    char *rem_word = wtc;
                    auto *newnode = new CompressedTrieNode();
                    newnode->edgelabel = rem_word;
                    newnode->edgeLabelSize = wtcSize - j;
                    newnode->isLeaf = curr_node->isLeaf;
                    newnode->parent = curr_node;
                    newnode->num_leafs = curr_node->num_leafs;
                    if (curr_node->isLeaf) {
                        newnode->value = curr_node->value;
                    }

                    newnode->sucs.root = curr_node->sucs.root;

                    updateChildren(newnode->sucs.getRoot(), newnode);

                    curr_node->edgelabel = word_to_cmp;
                    curr_node->edgeLabelSize = j;

                    curr_node->isLeaf = true;
                    curr_node->sucs.root = nullptr;

                    curr_node->sucs.getOrInsert(rem_word[0])->data = newnode;

                    curr_node->value = new Slice(value.data, value.size);
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
                    curr_node->value = new Slice(value.data, value.size);
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

                auto *newnode = new CompressedTrieNode();

                newnode->isLeaf = curr_node->isLeaf;
                newnode->num_leafs = curr_node->num_leafs;
                if (curr_node->isLeaf) {
                    newnode->value = curr_node->value;
                }

                newnode->sucs.root = curr_node->sucs.root;
                curr_node->sucs.root = nullptr;
                newnode->edgelabel = rem_word_j;
                newnode->edgeLabelSize = wtcSize - j;
                newnode->parent = curr_node;
                updateChildren(newnode->sucs.getRoot(), newnode);

                auto *newnode2 = new CompressedTrieNode();
                newnode2->isLeaf = true;
                newnode2->num_leafs++;
                newnode2->edgelabel = rem_word_i;
                newnode2->edgeLabelSize = key.size - i;
                newnode2->parent = curr_node;
                newnode2->value = new Slice(value.data, value.size);

                curr_node->isLeaf = false;
                curr_node->edgelabel = match_word;
                curr_node->edgeLabelSize = j;

                curr_node->sucs.root = nullptr;
                curr_node->sucs.getOrInsert(rem_word_j[0])->data =
                        newnode;

                auto node = curr_node->sucs.getOrInsert(*rem_word_i);
                node->data = newnode2;

                inc(curr_node, 1);

                return false;
            }
        }
    }

    return true;
}

bool searchKidsHelper(BSTNode *r, char *keyPointer, int keySize, Slice &A, Slice &B, int &remaining, char *kOrg) {
    if (!r) return false;

    if (searchKidsHelper(r->left, keyPointer, keySize, A, B, remaining, kOrg)) return true;

    auto trieNode = r->data;

    if (trieNode->num_leafs < remaining) {
        remaining -= trieNode->num_leafs;
        return searchKidsHelper(r->right, keyPointer, keySize, A, B, remaining, kOrg);
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

    return searchKidsHelper(trieNode->sucs.getRoot(), keyPointer, keySize, A, B, remaining, kOrg);
}

bool CompressedTrie::search(const int &N, Slice &A, Slice &B) {
    int left = N;

    char *keyPointer = (char *) malloc(65), *kOrg = keyPointer;
    int keySize = 0;

    return searchKidsHelper(root->sucs.getRoot(), keyPointer, keySize, A, B, left, kOrg);
}

bool delKidsHelper(BSTNode *r, int &remaining) {
    if (!r) return false;

    if (delKidsHelper(r->left, remaining)) return true;

    auto trieNode = r->data;

    if (trieNode->num_leafs < remaining) {
        remaining -= trieNode->num_leafs;
        return delKidsHelper(r->right, remaining);
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

    return delKidsHelper(trieNode->sucs.getRoot(), remaining);
}

bool CompressedTrie::del(const int &N) {
    int left = N;

    return delKidsHelper(root->sucs.getRoot(), left);
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
    Slice value{};
    return searchDelWrapper(key, value, IS_DEL);
}

