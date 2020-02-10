#include <vector>
using namespace std;

#define RANGE 52

template <typename T>
class TrieNode {
    int val;
    int numofEnds;  // num of values that ended at this node
    vector<TrieNode *> p;

   private:
    int getIndex(char c) {
        if ('A' <= c)
            return c - 'A';
        return c - 'a';
    }

   public:
    TrieNode(int value) {
        val = value;
        p = vector<TrieNode *>(RANGE);
        for (int i = 0; i < RANGE; i++)
            this->p[i] = NULL;
    }

    void insert(string s) {
        TrieNode *currNode = this;
        int i = 0;
        while (i < sz(s)) {
            int a = getIndex(s[i]);
            if (!currNode->p[a])
                currNode->p[a] = new TrieNode(s[i]);
            currNode = currNode->p[a];
            i++;
        }
        currNode->numofEnds++;
    }

    /**
     * Precondition: s exists in Trie before erasure
     */
    void erase(string s) {
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