#include <cassert>
#include "trieFinal.hpp"
#include <cstring>
#include"monitor.cpp"

struct Slice {
    int size;
    char *data;
};

class kvStore {
private:
    TrieNode *root;
    monitor M;

public:
    kvStore(uint64_t max_entries) : root(new TrieNode()), M(monitor()) {
    }

    ~kvStore() {
        delete root;
    }

    // returns false if key didn’t exist
    bool get(Slice &key, Slice &value) {
        M.beginread();

        int len;
        char *found = root->lookup(key.data, key.size, len);
        if (!found) {
            M.endread();
            return false;
        }

        value.data = found;
        value.size = len;
        M.endread();
        return true;
    }

    // returns true if value overwritten
    bool put(Slice &key, Slice &value) {
        M.beginwrite();
        bool res = root->insert(key.data, key.size, value.data, value.size);
        M.endwrite();
        return res;
    }

    bool del(Slice &key) {
        M.beginwrite();
        bool res = root->erase(key.data, key.size);
        M.endwrite();
        return res;
    }

    // N in benchmark.cpp is zero-indexed
    // N in trieFinal.hpp is one-indexed

    // returns Nth key-value pair
    bool get(int N, Slice &key, Slice &value) {
        M.beginread();
        int x = 0, y = 0;
        bool found = root->lookupN(N + 1, &key.data, &value.data, x, y);

        if (!found) {
            M.endread();
            return false;
        }

        key.size = x;
        value.size = y;
        M.endread();
        return true;
    }

    // delete Nth key-value pair
    bool del(int N) {
        M.beginwrite();
        bool ans = root->erase(N + 1);
        M.endwrite();
        return ans;
    }
};
