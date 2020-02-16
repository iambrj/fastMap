#include <string>
#include <string.h>
#include <vector>
#include "trie.hpp"
using namespace std;

int main() {
    TrieNode *root = new TrieNode();
    vector<char *> strs = {"abcd", "efgh", "abdef"};
    vector<char *> values = {"vals", "tree", "abcd"};

    for (int i = 0; i < strs.size(); i++) {
        int isOverwritten = root->insert(strs[i], strlen(strs[i]), values[i]);
        printf("%d\n", isOverwritten);
    }

    for (auto key : strs) {
        char *value = nullptr;
        root->lookup(key, strlen(key), value);

        if (value == nullptr) {
            printf("Not found!\n");
        } else {
            printf("%s\n", value);
        }

        root->erase(key);
        root->lookup(key, strlen(key), value);

        if (value == nullptr) {
            printf("Not found!\n");
        } else {
            printf("%s\n", value);
        }
    }
}