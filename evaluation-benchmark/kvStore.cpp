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
    pthread_mutex_t lock;

public:
    kvStore(uint64_t max_entries) : root(new TrieNode()) {
        if (pthread_mutex_init(&lock, NULL) != 0) {
            printf("\n mutex init has failed\n");
            exit(1);
        }

    }

    ~kvStore() {
        delete root;
        pthread_mutex_destroy(&lock);
    }

    inline void cleanup() {
        pthread_mutex_unlock(&lock);
    }

    inline void gainControl() {
        pthread_mutex_lock(&lock);
    }

    // returns false if key didn’t exist
    bool get(Slice &key, Slice &value) {
        gainControl();
        int len;
        char *found = root->lookup(key.data, key.size, len);
        if (!found) {
            cleanup();
            return false;
        }

        value.data = found;
        value.size = len;
        cleanup();
        return true;
    }

    // returns true if value overwritten
    bool put(Slice &key, Slice &value) {
        gainControl();
        bool res = root->insert(key.data, key.size, value.data, value.size);
        cleanup();
        return res;
    }

    bool del(Slice &key) {
        gainControl();
        bool res = root->erase(key.data, key.size);
        cleanup();
        return res;
    }

    // N in benchmark.cpp is zero-indexed
    // N in trieFinal.hpp is one-indexed

    // returns Nth key-value pair
    bool get(int N, Slice &key, Slice &value) {
        gainControl();
        int x = 0, y = 0;
        bool found = root->lookupN(N + 1, &key.data, &value.data, x, y);

        if (!found) {
            cleanup();
            return false;
        }

        key.size = x;
        value.size = y;
        cleanup();
        return true;
    }

    // delete Nth key-value pair
    bool del(int N) {
        gainControl();
        bool ans = root->erase(N + 1);
        cleanup();
        return ans;
    }
};
