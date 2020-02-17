#include <fstream>
#include <map>

#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include "fast_map.hpp"
using namespace std;
#define LOOKUP_OP 0
#define INSERT_OP 1
#define ERASE_OP 2

void naiveCheck() {
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

char *getCharPointer(string &s) {
    char *valueChar = (char *)malloc(s.size());
    for (int i = 0; i < s.size(); i++) {
        *(valueChar + i) = s[i];
    }

    return valueChar;
}

void setStringIntoSlice(string &s, Slice &slc) {
    slc.data = getCharPointer(s);
    slc.size = s.size();
}

#define fail                                     \
    {                                            \
        printf("Mismatch at operation %d: ", i); \
        return;                                  \
    }
void fileCheck() {
    map<string, string> naive;
    const char *FILE_PATH = "../tests/genInp.txt";

    ifstream file(FILE_PATH);

    int opCount;
    file >> opCount;
    // std::cout << opCount << endl;
    // return;

    kvStore fastMap((uint64_t)opCount);

    for (int i = 1; i <= opCount; i++) {
        int op;
        file >> op;
        string key;
        file >> key;
        string actual, value;
        int found, wasFound, actuallyFound, isOverwrite;
        Slice x, y, z;

        switch (op) {
            case LOOKUP_OP:
                actual = naive[key];
                actuallyFound = actual.size() > 0;

                setStringIntoSlice(key, x);

                wasFound = fastMap.get(x, y);

                if (wasFound != actuallyFound) {
                    fail;
                }

                if (wasFound) {
                    if (!strcmp(actual.c_str(), y.data)) {
                        fail;
                    }
                }

                break;
            case INSERT_OP:
                file >> value;

                isOverwrite = (naive[key].size() > 0);
                naive[key] = value;

                setStringIntoSlice(key, x);
                setStringIntoSlice(value, y);

                found = fastMap.get(x, y);

                if (found != isOverwrite)
                    fail;

                fastMap.put(x, y);

                found = fastMap.get(x, z);

                if (!found) {
                    fail;
                }

                if (!strcmp(value.c_str(), z.data)) {
                    fail;
                }

                break;
            case ERASE_OP:
                naive.erase(naive.find(key));

                setStringIntoSlice(key, x);

                // check if erased correctly from fastmap
                found = fastMap.del(x);

                if (!found)
                    fail;

                found = fastMap.get(x, y);

                if (found)
                    fail;

                break;

            default:
                break;
        }

        printf("Completed op number %d\n", i);
    }
}

int main() {
    fileCheck();
    return 0;
}