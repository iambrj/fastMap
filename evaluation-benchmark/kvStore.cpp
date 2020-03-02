#include <cassert>
#include "trieFinal.hpp"
#include <cstring>

struct Slice {
    int size;
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
        int len;
        char *found = root->lookup(key.data, key.size, len);
        if (!found)
            return false;
        value.data = found;
        value.size = len;
        return true;
    }

    // returns true if value overwritten
    bool put(Slice &key, Slice &value) {
        return root->insert(key.data, key.size, value.data, value.size);
    }

    bool del(Slice &key) {
        return root->erase(key.data, key.size);
    }

    // returns Nth key-value pair
    bool get(int N, Slice &key, Slice &value) {
        int x = 0, y = 0;
        bool found = root->lookupN(N, &key.data, &value.data, x, y);

        if (!found)
            return false;

        key.size = x;
        value.size = y;
        return true;
    }

    // delete Nth key-value pair
    bool del(int N) {
        return root->erase(N);
    }
};
