#include <vector>
using namespace std;

const int RANGE = 52;

template <typename T>
class TrieNode {
    /*
     * Each trie node attempts to store the values of the transition edges
     * The transition "c" from a->b is represented by storing "c" on the node b,
     * such that node b is at the index c[0]
     */

    int len;  // length of the string data contained in this node
    char *data;
    char *value;   // value associated with the key ending at this trie node
    TrieNode **p;  // array of TrieNode pointers

   private:
    int getIndex(char c) {
        if ('A' <= c)
            return (c - 'A') + (RANGE / 2);
        return c - 'a';
    }

    TrieNode **getNewTransitionsArray() {
        return (TrieNode **)calloc(sizeof(TrieNode *) * RANGE, 1);
    }

    // need not be a leaf node
    // index will be a part of the newly created trie node
    void split(int index) {
        assert(index < this->len && index >= 0);

        int splittedOutLen = len - index;

        TrieNode *splittedOut =
            new TrieNode(s + index, splittedOutLen, this->value);

        len -= splittedOutLen;
        // transfer value to the splitted out node
        splittedOut->value = value;
        value = nullptr;

        //  pointer remapping
        int newTransitionIndex = getIndex(s[index]);
        TrieNode **previousTransitions = this->p;
        this->p = getNewTransitionsArray();
        this->p[newTransitionIndex] = splittedOut;
        splittedOut->p = previousTransitions;
    }

   public:
    TrieNode(char *str, int len, char *value) {
        this->len = len;
        p = getNewTransitionsArray();
        data = str;
        this->value = value;
    }

    // return whether value was overwritten or not
    bool insert(char *s, int len, char *value) {
        int idx = getIndex(*s);

        if (this->p[idx]) {
            int i = 0;
            int len2 = this->p[idx]->len;
            char *pointer = this->p[idx]->data;

            while (i < len && i < len2 && *pointer == *s) {
                s++;
                i++;
                pointer++;
                len--;
            }

            if (i == len) {
                // the existing transition completely encloses my transition
                // so i need to split this node
                this->p[idx]->split();
            }

            return this->p[idx]->insert(s, len, value);
        } else {
            // if the transition doesn't exist
            this->p = new TrieNode(s, len, value);
        }
    }

    /**
     * Precondition: s exists in Trie before erasure
     */
    void erase(char *s) {
        // need to rewrite
    }
};