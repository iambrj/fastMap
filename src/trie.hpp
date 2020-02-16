#include <vector>
#include <assert.h>
#include <stdlib.h>
using namespace std;

const int RANGE = 52;

class TrieNode {
    /*
     * Each trie node attempts to store the values of the transition edges
     * The transition "c" from a->b is represented by storing "c" on the node b,
     * such that node b is at the index c[0]
     */

    int len;  // length of the string "data" contained in this node
    char *data;
    char *value;   // value associated with the key ending at this trie node
    TrieNode **p;  // array of TrieNode pointers

   private:
    int getIndex(char c) {
        if ('a' <= c)
            return (c - 'a');
        return (c - 'A') + (RANGE / 2);
    }

    TrieNode **getNewTransitionsArray() {
        return (TrieNode **)calloc(sizeof(TrieNode *) * RANGE, 1);
    }

    // need not be a leaf node
    // index will be a part of the newly created trie node
    void split(int index) {
        assert(index < this->len && index >= 0);

        int splittedOutLen = len - index;

        TrieNode *splittedOut = new TrieNode();

        splittedOut->data = data + index;
        splittedOut->len = splittedOutLen;
        // transfer value to the splitted out node
        splittedOut->value = value;
        value = nullptr;

        // this node's remaining length
        len -= splittedOutLen;

        //  pointer remapping
        int newTransitionIndex = getIndex(*(splittedOut->data));
        TrieNode **previousTransitions = this->p;
        this->p = getNewTransitionsArray();
        this->p[newTransitionIndex] = splittedOut;
        splittedOut->p = previousTransitions;
    }

    /* I DON'T THINK IT IS NECESSARY THOUGH */
    // @description
    // if this trie node has only one outward
    // edge at the "index", then merge it back inside
    void merge() {
    }

   public:
    TrieNode() {
        p = getNewTransitionsArray();
        data = nullptr;
        value = nullptr;
        len = 0;
    }
    TrieNode(char *str, int sLen, char *valueAssign) {
        this->len = sLen;
        p = getNewTransitionsArray();
        data = str;
        this->value = valueAssign;
    }

    // finds the key in the trie, sets value and len accordingly
    void lookup(char *key, int keyLen, char *&value, int &valueLen) {
        TrieNode *curr = this;
        int currIndex = 0;
        char *dataStr = curr->data;

        for (int i = 0; i < keyLen; i++) {
            while (i < keyLen && currIndex < curr->len && *dataStr == *key) {
                key++;
                currIndex++;
                i++;
            }

            if (i == keyLen) {
                // stopped middway in a key's data
                if (currIndex < curr->len) {
                    valueLen = 0;
                    return;
                }
            }
        }
    }

    // return whether value was overwritten or not
    bool insert(char *s, int sLen, char *valueAssign) {
        int i = 0;
        char *pointer = this->data;

        while (i < sLen && i < len && *pointer == *s) {
            s++;
            i++;
            pointer++;
            sLen--;
        }

        // The existing transition completely encloses my transition
        // so I need to split this node
        if (sLen == 0) {
            // both ending at same point, overwrite!
            if (i == len) {
                this->value = valueAssign;
                return true;
            }

            this->split(i);
            int idx = getIndex(*s);
            this->p[idx] = new TrieNode(s, sLen, valueAssign);
            return false;
        } else if (i != 0)
            this->split(i);

        int idx = getIndex(*s);
        if (!this->p[idx]) {
            this->p[idx] = new TrieNode(s, sLen, valueAssign);
            return false;
        }
        return this->p[idx]->insert(s, sLen, valueAssign);
    }

    /**
     * Precondition: s exists in Trie before erasure
     */
    void erase(char *s) {
        // need to rewrite
    }
};