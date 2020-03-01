#include <cassert>
#include "trie.hpp"
#include <cstring>

#define strsize(x) (uint8_t) strlen(x)

struct Slice {
    uint8_t size;
    char *data;
};

class kvStore {
   private:
    TrieNode *root;

   public:
    kvStore(uint64_t max_entries) : root(new TrieNode()) {
    }

    ~kvStore() {
        delete root;
    }

    // returns false if key didn’t exist
    bool get(Slice &key, Slice &value) {
        char *found = root->lookup(key.data, key.size);
        if (!found)
            return false;
        value.data = found;
        value.size = strsize(found);
        return true;
    }

    // returns true if value overwritten
    bool put(Slice &key, Slice &value) {
        return root->insert(key.data, key.size, value.data);
    }

    bool del(Slice &key) {
        return root->erase(key.data, key.size);
    }

    // returns Nth key-value pair
    bool get(int N, Slice &key, Slice &value) {
        bool found = root->lookup(N, &key.data, &value.data);

        if (!found)
            return false;

        key.size = strsize(key.data);
        value.size = strsize(value.data);
        return true;
    }

    // delete Nth key-value pair
    bool del(int N) {
        return root->erase(N);
    }
};
