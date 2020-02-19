#include <stdlib.h>

#define RANGE 52

class TrieNode {
public:
    int numofEnds;  // num of values that ended at this node
    TrieNode **p;
    char *value;

    int getIndex(char c) {
        if ('a' <= c)
            return c - 'a';
        return (c - 'A') + (RANGE / 2);
    }

    int getChar(int idx) {
        if (RANGE / 2 <= idx)
            return (char) ((idx - RANGE / 2) + 'A');

        return (char) (idx + 'a');
    }

    TrieNode() {
        p = (TrieNode **) calloc(sizeof(TrieNode *), RANGE);
        value = nullptr;
    }

    bool insert(char *s, int sLen, char *value) {
        TrieNode *currNode = this;
        int i = 0;

        while (i < sLen) {
            int a = getIndex(s[i]);

            if (!currNode->p[a])
                currNode->p[a] = new TrieNode();

            currNode = currNode->p[a];
            i++;
        }

        int isOverwrite = true;
        if (!currNode->value) {
            isOverwrite = false;

            *currNode = this;
            i = 0;

            while (i < sLen) {
                currNode->numofEnds++;
                int a = getIndex(s[i]);

                if (!currNode->p[a])
                    currNode->p[a] = new TrieNode();

                currNode = currNode->p[a];
                i++;
            }
        }

        currNode->value = value;

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
        TrieNode *currNode = this;
        TrieNode *prevNode = this;
        int i = 0, a;

        while (i < sLen) {
            a = getIndex(s[i]);
            prevNode = currNode;
            if (!currNode->p[a]) {
                return false;
            }
            currNode = currNode->p[a];
            i++;
        }

        currNode->numofEnds--;

        if (currNode->numofEnds == 0)
            prevNode->p[a] = NULL;
        return true;
    }

    bool lookup(int N, char **key, char **value) {
        int cnt = 0;
        char *keyp = (char *) calloc(65, 1);

        TrieNode *curr = this;
        char *ko = keyp;

        while (N) {
            if (curr->value) {
                N--;

                if (N == 0) {
                    *value = curr->value;
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

                        N -= cnt;
                        *keyp = getChar(i);
                        keyp++;
                        goto end;
                    }
                }
            }
            return false;
            end:;
        }

        *key = ko;
        return true;
    }

    bool erase(int N) {
        return false;
    }
};