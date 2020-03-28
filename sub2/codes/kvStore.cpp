#ifndef FASTER
#define FASTER

#include <cassert>
#include "ctrie.hpp"
#include <cstring>

/* struct Slice { */
/*     int size; */
/*     char *data; */
/* }; */
class kvStore {
   private:
    CompressedTrie T;
    pthread_mutex_t lock;

   public:
    kvStore(uint64_t max_entries) { }

    // returns false if key didn’t exist
    bool get(Slice &key, Slice &value) {
        pthread_mutex_lock(&lock);
        auto result = T.search(key, value);
        pthread_mutex_unlock(&lock);
        return result;
    }

    // returns true if value overwritten
    bool put(Slice &key, Slice &value) {
        pthread_mutex_lock(&lock);
        auto result = T.insert(key, value);
        pthread_mutex_unlock(&lock);
        return result;
    }

    bool del(Slice &key) {
        pthread_mutex_lock(&lock);
        auto result = T.del(key);
        pthread_mutex_unlock(&lock);
        return result;
    }

    // N in benchmark.cpp is zero-indexed
    // N in trieFinal.hpp is one-indexed

    // returns Nth key-value pair
    bool get(int N, Slice &key, Slice &value) {
        pthread_mutex_lock(&lock);
        auto result = T.search(N + 1, key, value);
        pthread_mutex_unlock(&lock);
        return result;
    }

    // delete Nth key-value pair
    bool del(int N) {
        /* return root->erase(N + 1); */
        pthread_mutex_lock(&lock);
        auto result = T.del(N + 1);
        pthread_mutex_unlock(&lock);
        return result;
    }
};
#endif
