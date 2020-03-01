#include <cassert>
#include <cstdlib>

// Bithack to compute minimum branchlessly
#define MIN(x, y) ((y) ^ (((x) ^ (y)) & -((x) < (y))))
#define RANGE 52

class TrieNode {
   public:
    int numofEnds;  // num of values that ended at this node
    TrieNode **p;
    char *value;

    int getIndex(char c) {
        return c - MIN(MIN(c, 'a'), 'A') - (-(c > 'Z') & 6);
    }

    char getChar(int idx) {
        if (RANGE / 2 <= idx)
            return (char)((idx - RANGE / 2) + 'a');

        return (char)(idx + 'A');
    }

    TrieNode()
        : numofEnds(0),
          p((TrieNode **)calloc(sizeof(TrieNode *), RANGE)),
          value(nullptr) {
    }

    ~TrieNode() {
        for (int i = 0; i < RANGE; i++) {
            delete p[i];
        }

        free(p);
        p = nullptr;
    }

    bool insert(char *s, int sLen, char *valueToInsert) {
        if (!sLen) {
            bool isOverwrite = false;
            isOverwrite = this->value;
            this->numofEnds += !this->value;
            this->value = valueToInsert;
            return isOverwrite;
        }

        int idx = getIndex(*s);
        if (!this->p[idx])
            this->p[idx] = new TrieNode();
        bool isOverwrite = this->p[idx]->insert(s + 1, sLen - 1, valueToInsert);
        this->numofEnds += !isOverwrite;

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
        char *keyPointer = (char *)calloc(65, 1), *kOrg = keyPointer;

        TrieNode *curr = this;

        while (N) {
            if (curr->value) {
                N--;

                if (!N) {
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
        int cnt = 0;

        TrieNode *curr = this;
        int Norg = N;

        // first determine whether string is present or not
        while (N) {
            if (curr->value) {
                N--;

                if (!N) {
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
                        goto end;
                    }
                }
            }

            return false;
        end:;
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
                    goto end2;
                }
            }

            for (int i = 0; i < RANGE; i++) {
                TrieNode *trans = curr->p[i];

                if (trans) {
                    if (cnt + trans->numofEnds < N) {
                        cnt += trans->numofEnds;
                    } else {
                        curr->numofEnds--;
                        curr = trans;
                        N -= cnt;
                        cnt = 0;

                        goto end2;
                    }
                }
            }

            return false;
        end2:;
        }

        return true;
    }
};
