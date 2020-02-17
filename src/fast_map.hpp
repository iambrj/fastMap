#include "./trie.hpp"
#include <cstring>

struct Slice {
    uint8_t size;
    char *data;
};

class kvStore {
   private:
    TrieNode *root;

   public:
    kvStore(uint64_t max_entries) {
        root = new TrieNode();
    }
    ~kvStore() {
        delete root;
    }
    // returns false if key didn’t exist
    bool get(Slice &key, Slice &value) {
        TrieNode *found = root->lookup(key.data, key.size, value.data);
        if (!found)
            return false;
        value.size = strlen(value.data);
        return true;
    }
    // returns true if value overwritten
    bool put(Slice &key, Slice &value) {
        return root->insert(key.data, value.size, value.data);
    }
    bool del(Slice &key) {
        Slice value;
        TrieNode *found = root->lookup(key.data, key.size, value.data);
        if (!found)
            return false;
        root->erase(key.data);
        return true;
    }
    // returns Nth key-value pair
    bool get(int N, Slice &key, Slice &value);
    // delete Nth key-value pair
    bool del(int N);
};
