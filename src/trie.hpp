#include <cassert>
#include <cstdlib>

#define RANGE 52

class BSTNode;
class BST;
class TrieNode;


class BSTNode {
    public:
        char c;
        TrieNode* data;
        BSTNode* left;
        BSTNode* right;

        BSTNode(char c);
        ~BSTNode();
};

class BST {
    private:
        BSTNode* root;
        BSTNode* _insert(BSTNode* cur, char c);
        TrieNode* _get(BSTNode* cur, char c);
        BSTNode* _del(BSTNode* cur, char c);

    public:

        BST();
        ~BST();
        TrieNode* getOrInsert(char c);
        TrieNode* search(char c);
        void remove(char c);
        BSTNode* getRoot();

};

class TrieNode {
   public:
    int numofEnds;  // num of values that ended at this node
    BST* p;
    char *value;

    int getIndex(char c) {
        if (c < 'a')
            return c - 'A';
        return (c - 'a') + (RANGE / 2);
    }

    char getChar(int idx) {
        if (RANGE / 2 <= idx)
            return (char)((idx - RANGE / 2) + 'a');

        return (char)(idx + 'A');
    }

    TrieNode()
        : numofEnds(0),
          p(new BST()),
          value(nullptr) {
    }

    ~TrieNode() {
        delete this->p;
        this->p = nullptr;
    }

    bool insert(char *s, int sLen, char *valueToInsert) {
        if (!sLen) {
            bool isOverwrite = false;
            isOverwrite = this->value;
            this->numofEnds += !this->value;
            this->value = valueToInsert;
            return isOverwrite;
        }

        bool isOverwrite = this->p->getOrInsert(*s)->insert(s + 1, sLen - 1, valueToInsert);
        this->numofEnds += !isOverwrite;

        return isOverwrite;
    }

    char *lookup(char *s, int sLen) {
        TrieNode *currNode = this;
        int i = 0;

        while (i < sLen) {
            TrieNode* temp = currNode->p->search(s[i]);
            if (!temp) {
                return nullptr;
            }
            currNode = temp;
            i++;
        }

        return currNode->value;
    }

    bool erase(char *s, int sLen) {
        if (!sLen) {
            if (this->value) {
                free(this->value);
                this->value = nullptr;
                this->numofEnds--;

                return true;
            } else {
                return false;
            }
        }

        TrieNode* temp = this->p->search(*s);
        if (!temp) {
            return false;
        }

        if (temp->erase(s + 1, sLen - 1)) {
            this->numofEnds--;
            return true;
        }

        return false;
    }

    bool recurseInorderOne(BSTNode* cur, int& cnt, int& N, TrieNode*& curr, char*& keyPointer) {
        if (!cur) {
            return false;
        }

        if (recurseInorderOne(cur->left, cnt, N, curr, keyPointer))
            return true;

        TrieNode* trans = cur->data;
        if (cnt + trans->numofEnds < N) {
            cnt += trans->numofEnds;
        } else {
            curr = trans;
            N -= cnt;
            cnt = 0;

            *keyPointer = cur->c;
            keyPointer++;
            return true;
        }

        if (recurseInorderOne(cur->right, cnt, N, curr, keyPointer))
            return true;

        return false;
    }

    bool lookup(int N, char **key, char **valuePointer) {
        int cnt = 0;
        char *keyPointer = (char *)calloc(65, 1), *kOrg = keyPointer;

        TrieNode *curr = this;

        while (N) {
            if (curr->value) {
                N--;

                if (!N) {
                    *valuePointer = curr->value;
                    continue;
                }
            }

            if (this->recurseInorderOne(curr->p->getRoot(), cnt, N, curr, keyPointer))
                continue;
            return false;
        }

        *key = kOrg;
        return true;
    }

    bool recurseInorderTwo(BSTNode* cur, int& cnt, int& N, TrieNode*& curr) {
        if (!cur) {
            return false;
        }

        if (recurseInorderTwo(cur->left, cnt, N, curr))
            return true;

        TrieNode* trans = cur->data;
        if (cnt + trans->numofEnds < N) {
            cnt += trans->numofEnds;
        } else {
            curr = trans;
            N -= cnt;
            cnt = 0;
            return true;
        }

        if (recurseInorderTwo(cur->right, cnt, N, curr))
            return true;

        return false;
    }

    bool recurseInorderThree(BSTNode* cur, int& cnt, int& N, TrieNode*& curr) {
        if (!cur) {
            return false;
        }

        if (recurseInorderThree(cur->left, cnt, N, curr))
            return true;

        TrieNode* trans = cur->data;
        if (cnt + trans->numofEnds < N) {
            cnt += trans->numofEnds;
        } else {
            curr->numofEnds--;
            curr = trans;
            N -= cnt;
            cnt = 0;
            return true;
        }

        if (recurseInorderThree(cur->right, cnt, N, curr))
            return true;

        return false;
    }

    bool erase(int N) {
        int cnt = 0;

        TrieNode *curr = this;
        int Norg = N;

        // first determine whether string is present or not
        while (N) {
            if (curr->value) {
                N--;

                if (!N) {
                    continue;
                }
            }

            if (this->recurseInorderTwo(curr->p->getRoot(), cnt, N, curr))
                continue;
            return false;
        }

        // if string was found then deecrement num of ends
        // and free the value
        curr = this;
        cnt = 0;
        N = Norg;

        while (N) {
            if (curr->value) {
                N--;

                if (!N) {
                    curr->numofEnds--;
                    free(curr->value);
                    curr->value = nullptr;
                    continue;
                }
            }

            if (this->recurseInorderThree(curr->p->getRoot(), cnt, N, curr))
                continue;

            return false;
        }

        return true;
    }
};


// BSTNode FUNCTIONS
BSTNode::BSTNode(char c)
    : c(c),
      data(new TrieNode()),
      left(nullptr),
      right(nullptr) {
}

BSTNode::~BSTNode() {
    delete this->data;
    delete this->left;
    delete this->right;

    this->data = nullptr;
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

TrieNode* BST::_get(BSTNode* cur, char c) {
    if (!cur) {
        return nullptr;
    }

    if (cur->c < c) {
        return _get(cur->right, c);
    } else if (cur->c > c) {
        return _get(cur->left, c);
    } else {
        return cur->data;  // least likely, at the end
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

BST::BST() : root(nullptr) {}

BST::~BST() {
    delete this->root;
    this->root = nullptr;
}

TrieNode* BST::getOrInsert(char c) {
    this->root = this->_insert(this->root, c);
    return _get(this->root, c);
}

TrieNode* BST::search(char c) {
    return _get(this->root, c);
}

void BST::remove(char c) {
    this->root = _del(this->root, c);
}

BSTNode* BST::getRoot() {
    return this->root;
}
// class TrieNode {
//    public:
//     int numofEnds;  // num of values that ended at this node
//     TrieNode **p;
//     char *value;

//     int getIndex(char c) {
//         if (c < 'a')
//             return c - 'A';
//         return (c - 'a') + (RANGE / 2);
//     }

//     char getChar(int idx) {
//         if (RANGE / 2 <= idx)
//             return (char)((idx - RANGE / 2) + 'a');

//         return (char)(idx + 'A');
//     }

//     TrieNode()
//         : numofEnds(0),
//           p((TrieNode **)calloc(sizeof(TrieNode *), RANGE)),
//           value(nullptr) {
//     }

//     ~TrieNode() {
//         for (int i = 0; i < RANGE; i++) {
//             delete p[i];
//         }

//         free(p);
//         p = nullptr;
//     }

//     bool insert(char *s, int sLen, char *valueToInsert) {
//         if (!sLen) {
//             bool isOverwrite = false;
//             isOverwrite = this->value;
//             this->numofEnds += !this->value;
//             this->value = valueToInsert;
//             return isOverwrite;
//         }

//         int idx = getIndex(*s);
//         if (!this->p[idx])
//             this->p[idx] = new TrieNode();
//         bool isOverwrite = this->p[idx]->insert(s + 1, sLen - 1, valueToInsert);
//         this->numofEnds += !isOverwrite;

//         return isOverwrite;
//     }

//     char *lookup(char *s, int sLen) {
//         TrieNode *currNode = this;
//         int i = 0;

//         while (i < sLen) {
//             int a = getIndex(s[i]);

//             if (!currNode->p[a])
//                 return nullptr;

//             currNode = currNode->p[a];
//             i++;
//         }

//         return currNode->value;
//     }

//     bool erase(char *s, int sLen) {
//         if (!sLen) {
//             if (this->value) {
//                 free(this->value);
//                 this->value = nullptr;
//                 this->numofEnds--;

//                 return true;
//             } else {
//                 return false;
//             }
//         }

//         int idx = getIndex(*s);
//         if (!this->p[idx])
//             return false;

//         if (this->p[idx]->erase(s + 1, sLen - 1)) {
//             this->numofEnds--;
//             return true;
//         }

//         return false;
//     }

//     bool lookup(int N, char **key, char **valuePointer) {
//         int cnt = 0;
//         char *keyPointer = (char *)calloc(65, 1), *kOrg = keyPointer;

//         TrieNode *curr = this;

//         while (N) {
//             if (curr->value) {
//                 N--;

//                 if (!N) {
//                     *valuePointer = curr->value;
//                     goto end;
//                 }
//             }

//             for (int i = 0; i < RANGE; i++) {
//                 TrieNode *trans = curr->p[i];

//                 if (trans) {
//                     if (cnt + trans->numofEnds < N) {
//                         cnt += trans->numofEnds;
//                     } else {
//                         curr = trans;
//                         N -= cnt;
//                         cnt = 0;

//                         *keyPointer = getChar(i);
//                         keyPointer++;
//                         goto end;
//                     }
//                 }
//             }
//             return false;
//         end:;
//         }

//         *key = kOrg;
//         return true;
//     }

//     bool erase(int N) {
//         int cnt = 0;

//         TrieNode *curr = this;
//         int Norg = N;

//         // first determine whether string is present or not
//         while (N) {
//             if (curr->value) {
//                 N--;

//                 if (!N) {
//                     goto end;
//                 }
//             }

//             for (int i = 0; i < RANGE; i++) {
//                 TrieNode *trans = curr->p[i];

//                 if (trans) {
//                     if (cnt + trans->numofEnds < N) {
//                         cnt += trans->numofEnds;
//                     } else {
//                         curr = trans;
//                         N -= cnt;
//                         cnt = 0;
//                         goto end;
//                     }
//                 }
//             }

//             return false;
//         end:;
//         }

//         // if string was found then deecrement num of ends
//         // and free the value
//         curr = this;
//         cnt = 0;
//         N = Norg;

//         while (N) {
//             if (curr->value) {
//                 N--;

//                 if (!N) {
//                     curr->numofEnds--;
//                     free(curr->value);
//                     curr->value = nullptr;
//                     goto end2;
//                 }
//             }

//             for (int i = 0; i < RANGE; i++) {
//                 TrieNode *trans = curr->p[i];

//                 if (trans) {
//                     if (cnt + trans->numofEnds < N) {
//                         cnt += trans->numofEnds;
//                     } else {
//                         curr->numofEnds--;
//                         curr = trans;
//                         N -= cnt;
//                         cnt = 0;

//                         goto end2;
//                     }
//                 }
//             }

//             return false;
//         end2:;
//         }

//         return true;
//     }
// };
