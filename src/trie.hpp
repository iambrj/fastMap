#define RANGE 52

template <typename T>
class TrieNode {
    ll val;
    ll numofEnds;  // num of values that ended at this node
    vector<TrieNode *> p;

   private:
    vector<char> getVectorOfString(const string &s) {
        vector<char> vc;
        for (const auto &c : s)
            vc.push_back(c);
        return move(vc);
    }

    ll getIndex(char c) {
        if ('A' <= c)
            return c - 'A';
        return c - 'a';
    }

   public:
    TrieNode(ll value) {
        val = value;
        p = vector<TrieNode *>(RANGE);
        for (ll i = 0; i < RANGE; i++)
            this->p[i] = NULL;
    }

    void insert(const vector<T> &vals) {
        TrieNode *currNode = this;
        ll i = 0;
        while (i < sz(vals)) {
            ll a = getIndex(vals[i]);
            if (!currNode->p[a])
                currNode->p[a] = new TrieNode(vals[i]);
            currNode = currNode->p[a];
            i++;
        }
        currNode->numofEnds++;
    }
    void insert(string s) {
        insert(getVectorOfString(s));
    }

    /**
     * Precondition: vals exists in Trie before erasure
     */
    void erase(const vector<T> &vals) {
        TrieNode *currNode = this;
        TrieNode *prevNode = this;
        ll i = 0, a;
        while (i < sz(vals)) {
            a = getIndex(vals[i]);
            prevNode = currNode;
            currNode = currNode->p[a];
            i++;
        }
        currNode->numofEnds--;
        if (currNode->numofEnds == 0)
            prevNode->p[a] = NULL;
    }
    void erase(string s) {
        erase(getVectorOfString(s));
    }
};