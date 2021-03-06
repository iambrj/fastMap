#include <fstream>
#include <map>

#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <cassert>
#include "fast_map.hpp"

using namespace std;
#define LOOKUP_OP 0
#define INSERT_OP 1
#define ERASE_OP 2
#define LOOKUPN_OP 3
#define ERASEN_OP 4
#define contSize(x) (int) x.size()

map<string, string> naive;
map<string, string>::iterator it;

char *getCharPointer(const string &s) {
    char *valueChar = (char *)malloc(s.size() + 1), *org = valueChar;
    for (auto ch : s) {
        *valueChar = ch;
        valueChar++;
    }
    *valueChar = 0;

    return org;
}

void setStringIntoSlice(string &s, Slice &slc) {
    slc.data = getCharPointer(s);
    slc.size = s.size();
}

_GLIBCXX_NORETURN void iteratorFail(int x, int y) {
    cout << naive.size() << " " << y << endl;
    for (auto iter : naive) {
        cout << iter.first << " " << iter.second << endl;
    }
    printf("Couldn't retrieve iterator - %d\n", x);
    exit(3);
}

map<string, string>::iterator getNth(int n) {
    auto iterator = naive.begin();

    while (iterator != naive.end() && (*iterator).second.empty())
        iterator++;

    if (iterator == naive.end()) {
        iteratorFail(0, n);
    }

    for (int i = 1; i < n; i++) {
        iterator++;

        while (iterator != naive.end() && (*iterator).second.empty())
            iterator++;

        if (iterator == naive.end()) {
            iteratorFail(1, n);
        }
    }

    return iterator;
}

#define fail(x)                                                              \
    {                                                                        \
        printf("Mismatch at operation %d, optype %d, index %d\n", i, op, x); \
        return;                                                              \
    }

void fileCheck() {
    const char *FILE_PATH = "../tests/genInp.txt";

    ifstream file(FILE_PATH);

    int opCount;
    file >> opCount;
    std::cout << opCount << endl;

    kvStore fastMap((uint64_t)opCount);

    for (int i = 1; i <= opCount; i++) {
        int op;
        file >> op;
        string key;
        string actual, value;
        int found, wasFound, actuallyFound, isOverwrite, nth;
        Slice x, y, z;

        switch (op) {
            case LOOKUP_OP:
                file >> key;
                actual = naive[key];
                actuallyFound = actual.size() > 0;

                setStringIntoSlice(key, x);

                wasFound = fastMap.get(x, y);

                if (wasFound != actuallyFound) {
                    fail(0);
                }

                if (wasFound) {
                    if (strcmp(actual.c_str(), y.data)) {
                        cout << actual << endl;
                        printf("%s\n", y.data);
                        fail(1);
                    }
                }

                // key was allocated new memory by trie.hpp
                // need to ree it since lookup successful
                //if (x.data)
                //    ;

                break;
            case INSERT_OP:
                file >> key;
                file >> value;

                isOverwrite = (naive[key].size() > 0);
                naive[key] = value;

                setStringIntoSlice(key, x);

                found = fastMap.get(x, y);

                if (found != isOverwrite) {
                    cout << found << " " << isOverwrite << endl;
                    fail(0);
                }

                setStringIntoSlice(value, y);

                fastMap.put(x, y);
                found = fastMap.get(x, z);

                if (!found) {
                    printf("%s\n", x.data);
                    printf("%s\n", y.data);
                    fail(1);
                }

                if (strcmp(value.c_str(), z.data)) {
                    cout << value << endl;
                    printf("%s\n", z.data);
                    fail(2);
                }

                ;
                // cannot free y, sicne it's a value being used by the trienode

                break;
            case ERASE_OP:
                file >> key;
                it = naive.find(key);
                found = it != naive.end() && (*it).second.size() > 0;
                if (found)
                    naive.erase(it);

                setStringIntoSlice(key, x);

                // check if erased correctly from fastmap
                wasFound = fastMap.del(x);

                if (found != wasFound) {
                    fail(0);
                }

                found = fastMap.get(x, y);

                if (found) {
                    cout << "not found\n";
                    fail(1);
                }


                break;
            case LOOKUPN_OP:
                file >> nth;
                wasFound = true;

                if (nth > contSize(naive)) {
                    wasFound = false;
                } else {
                    it = getNth(nth);
                    value = (*it).second;
                }

                found = fastMap.get(nth, x, y);

                if (found != wasFound) {
                    fail(0);
                }

                if (found && strcmp(value.c_str(), y.data)) {
                    cout << contSize(naive) << endl;

                    cout << (*it).first << endl;
                    cout << value << endl;

                    printf("%s\n", x.data);
                    printf("%s\n", y.data);
                    fail(1);
                }

                break;
            case ERASEN_OP:
                file >> nth;
                wasFound = true;
                if (nth > contSize(naive)) {
                    wasFound = false;
                    key = "";
                } else {
                    it = getNth(nth);
                    key = (*it).first;
                    naive.erase(it);
                }

                found = fastMap.del(nth);

                if (found != wasFound) {
                    fail(0);
                }

                if (!key.empty()) {
                    setStringIntoSlice(key, x);

                    found = fastMap.get(x, y);
                    if (found) {
                        fail(1);
                    }

                    ;
                }

                break;
            default:
                break;
        }

        x.data = NULL;
        y.data = NULL;
        z.data = NULL;

        if(i%(int)1e4 == 0)
                printf("Completed op %d\n", i);
    }

    // remove all unremoved values from map
    //while (fastMap.del(1))
     //   ;
}

int main() {
    fileCheck();
    printf("File check done\n");
    return 0;
}
