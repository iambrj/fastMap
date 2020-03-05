#ifndef FASTER
#define FASTER

#include <cassert>
#include "ctrie.hpp"
#include <cstring>
#include "monitor.cpp"

/* struct Slice { */
/*     int size; */
/*     char *data; */
/* }; */
class kvStore {
   private:
    CompressedTrie T;
    monitor M;

   public:
    kvStore(uint64_t max_entries) { }

    // returns false if key didn’t exist
    bool get(Slice &key, Slice &value) {
        /*
        int len;
        char *found = root->lookup(key.data, key.size, len);
        if (!found)
            return false;
        value.data = found;
        value.size = len;
        return true;
        */
        M.beginread();
        auto result = T.search(key, value);
        M.endread();
        return result;
    }

    // returns true if value overwritten
    bool put(Slice &key, Slice &value) {
        /* return root->insert(key.data, key.size, value.data, value.size); */
        M.beginwrite();
        auto result = T.insert(key, value);
        M.endwrite();
        return result;
    }

    bool del(Slice &key) {
        /* return root->erase(key.data, key.size); */
        M.beginwrite();
        auto result = T.del(key);
        M.endwrite();
        return result;
    }

    // N in benchmark.cpp is zero-indexed
    // N in trieFinal.hpp is one-indexed

    // returns Nth key-value pair
    bool get(int N, Slice &key, Slice &value) {
        /*
        int x = 0, y = 0;
        bool found = root->lookupN(N + 1, &key.data, &value.data, x, y);

        if (!found)
            return false;

        key.size = x;
        value.size = y;
        return true;
        */
        M.beginread();
        auto result = T.search(N + 1, key, value); 
        M.endread();
        return result;
    }

    // delete Nth key-value pair
    bool del(int N) {
        /* return root->erase(N + 1); */
        M.beginwrite();
        auto result = T.del(N + 1);
        M.endwrite();
        return result;
    }
};
#endif
