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

    int idx;         // index of the first character of this string
    int len;         // length of the string data contained in this node
    char *value;     // value associated with the key ending at this trie node
    bool numofEnds;  // if a string was ending at this node
    TrieNode **p;    // array of TrieNode pointers

   private:
    int getIndex(char c) {
        if ('A' <= c)
            return (c - 'A') + (RANGE / 2);
        return c - 'a';
    }

    void split() {
    }

   public:
    TrieNode(char *str, int len) {
        idx = getIndex(*str);
        this->len = len;
        p = (TrieNode **)calloc(sizeof(TrieNode *) * RANGE, 1);
        numofEnds = false;  // ?
    }

    // return whether value was overwritten or not
    bool insert(char *s, int len, char *value) {
        int idx = getIndex(*s);

        if (this->p[idx]) {
            // if the transition already exists
            if (this->p[idx]->len == 1)
                // if the transition stores only the first character
                this->p[idx].insert(s + 1, len - 1);
            else
                ;
        } else {
            // if the transition doesn't exist
            this->p = new TrieNode(s, len);
        }
    }

    /**
     * Precondition: s exists in Trie before erasure
     */
    void erase(char *s) {
        // need to rewrite
    }
};