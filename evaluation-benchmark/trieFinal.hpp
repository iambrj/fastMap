#include <cassert>
#include <cstdlib>

#define RANGE 52

int min(int a, int b) {
    return a < b ? a : b;
}

class TrieNode {
public:
    int numofEnds;  // num of values that ended at this node
    TrieNode **p;
    char *value;
    int valueLen;

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
        p = nullptr;
    }

    bool insert(char *s, int sLen, char *valueToInsert, int valueLen) {
        if (!sLen) {
            bool isOverwrite = false;
            isOverwrite = this->value;
            this->numofEnds += !this->value;
            this->value = valueToInsert;
            this->valueLen = valueLen;
            return isOverwrite;
        }

        int idx = getIndex(*s);
        if (!this->p[idx])
            this->p[idx] = new TrieNode();
        bool isOverwrite =
                this->p[idx]->insert(s + 1, sLen - 1, valueToInsert, valueLen);
        this->numofEnds += !isOverwrite;

        return isOverwrite;
    }

    char *lookup(char *s, int sLen, int &valLen) {
        TrieNode *currNode = this;
        int i = 0;

        while (i < sLen) {
            int a = getIndex(s[i]);

            if (!currNode->p[a])
                return nullptr;

            currNode = currNode->p[a];
            i++;
        }

        valLen = currNode->valueLen;
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

    bool lookupN(int N, char **key, char **valuePointer, int &ksize,
                 int &vsize) {
        int cnt = 0;
        char *keyPointer = (char *) calloc(65, 1), *kOrg = keyPointer;

        TrieNode *curr = this;

        while (N) {
            if (curr->value) {
                N--;

                if (!N) {
                    *valuePointer = curr->value;
                    vsize = curr->valueLen;
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
                        ksize++;
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
