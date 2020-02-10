#include <vector>
using namespace std;

const int RANGE = 52;

template <typename T>
class TrieNode {
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

   public:
    TrieNode(char *str, int len) {
        idx = getIndex(*str);
        this->len = len;
        p = (TrieNode **)calloc(sizeof(TrieNode *) * RANGE, 1);
        numofEnds = false;  // ?
    }

    void insert(char *s, int len, char *value) {
        int i = 0, idx = getIndex(*s);

        // figure this out?!
        if (this->p[idx]) {
            this->p[idx].insert(s + 1, len - 1);
        } else {
            this->p = new TrieNode(s, len);
        }

        currNode->numofEnds++;
    }

    /**
     * Precondition: s exists in Trie before erasure
     */
    void erase(char *s) {
        TrieNode *currNode = this;
        TrieNode *prevNode = this;

        int i = 0, a;

        while (i < sz(s)) {
            a = getIndex(s[i]);
            prevNode = currNode;
            currNode = currNode->p[a];
            i++;
        }

        currNode->numofEnds--;
        if (currNode->numofEnds == 0)
            prevNode->p[a] = NULL;
    }
};