#include <bits/stdc++.h>
#include <stdint.h>

using namespace std;

struct Slice {
    uint8_t size;
    char* data;
};

class kvStore {
   public:
    kvStore(uint64_t max_entries){};

    bool get(Slice& key, Slice& value) {
        // Your Code Here
        return false;
    }

    bool put(Slice& key, Slice& value) {
        // Your Code Here
        return false;
    }

    bool del(Slice& key) {
        // Your Code Here
        return false;
    }

    bool get(int N, Slice& key, Slice& value) {
        // Your Code Here
        return false;
    }

    bool del(int N) {
        // Your Code Here
        return true;
    }
};