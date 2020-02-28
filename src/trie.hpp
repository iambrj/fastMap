#include <cassert>
#include <stdlib.h>

#define RANGE 52

class TrieNode {
public:
    int numofEnds;  // num of values that ended at this node
    TrieNode **p;
    char *value;

    int getIndex(char c) {
        if (c < 'a')
            return c - 'A';
        return (c - 'a') + (RANGE / 2);
    }

    char getChar(int idx) {
        if (RANGE / 2 <= idx)
            return (char) ((idx - RANGE / 2) + 'a');

        return (char) (idx + 'A');
    }

    TrieNode()
            : numofEnds(0),
              p((TrieNode **) calloc(sizeof(TrieNode *), RANGE)),
              value(nullptr) {
    }

    ~TrieNode() {
        for (int i = 0; i < RANGE; i++) {
            delete p[i];
        }

        free(p);
        p = NULL;
    }

    bool insert(char *s, int sLen, char *valueToInsert) {
        if (sLen == 0) {
            bool isOverwrite = false;
            if (this->value) {
                isOverwrite = true;
            } else {
                this->numofEnds++;
            }
            this->value = valueToInsert;
            return isOverwrite;
        }

        int idx = getIndex(*s);
        if (!this->p[idx])
            this->p[idx] = new TrieNode();
        bool isOverwrite = this->p[idx]->insert(s + 1, sLen - 1, valueToInsert);
        if (!isOverwrite) {
            this->numofEnds++;
        }

        return isOverwrite;
    }

    char *lookup(char *s, int sLen) {
        TrieNode *currNode = this;
        int i = 0;

        while (i < sLen) {
            int a = getIndex(s[i]);

            if (!currNode->p[a])
                return nullptr;

            currNode = currNode->p[a];
            i++;
        }

        return currNode->value;
    }

    bool erase(char *s, int sLen) {
        if (sLen == 0) {
            if (this->value) {
                free(this->value);
                this->value = NULL;
                this->numofEnds--;

                return true;
            } else {
                return false;
            }
        }

        int idx = getIndex(*s);
        if (!this->p[idx])
            return false;

        if (this->p[idx]->erase(s + 1, sLen - 1)) {
            this->numofEnds--;
            return true;
        }

        return false;
    }

    bool lookup(int N, char **key, char **valuePointer) {
        int cnt = 0;
        char *keyPointer = (char *) calloc(65, 1), *kOrg = keyPointer;

        TrieNode *curr = this;

        while (N) {
            if (curr->value) {
                N--;

                if (N == 0) {
                    *valuePointer = curr->value;
                    goto end;
                }
            }

            for (int i = 0; i < RANGE; i++) {
                TrieNode *trans = curr->p[i];

                if (trans) {
                    if (cnt + trans->numofEnds < N) {
                        cnt += trans->numofEnds;
                    } else {
                        curr = trans;
                        N -= cnt;
                        cnt = 0;

                        *keyPointer = getChar(i);
                        keyPointer++;
                        goto end;
                    }
                }
            }
            return false;
            end:;
        }

        *key = kOrg;
        return true;
    }

    bool erase(int N) {
        assert(false);

        return false;
    }
};