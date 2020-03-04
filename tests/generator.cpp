#define SEED 10000  // no elements to be inserted at beginning
#define MAX_KEY_LEN 64
#define MAX_VALUE_LEN 240

#define OP_TYPES 5
#define LOOKUP_OP 0
#define INSERT_OP 1
#define ERASE_OP 2
#define LOOKUPN_OP 3
#define ERASEN_OP 4
#define OP_COUNT (int)1e5
//#define MAX_OUT
#define PREFIX_OVERLAP

#include <vector>
#include <iostream>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <set>

using namespace std;

set<string> inserted;
#define contSize(x) (int)x.size()

static const char alpha[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";

static int alpha_len = strlen(alpha);

string getNth(int n) {
    auto iterator = inserted.begin();

    for (int i = 1; i < n; i++) {
        iterator++;
    }

    return *iterator;
}

string alpha_rand(const int len) {
    string s;
    for (int i = 0; i < len; ++i) {
        s += alpha[rand() % (alpha_len)];
    }

    return s;
}

string rand_string_wrapper(bool isValue = false) {
    int modder = isValue ? MAX_VALUE_LEN : MAX_KEY_LEN;
    int sizeString = rand() % modder + 1;
#ifdef MAX_OUT
    sizeString = modder;
#endif

    string result = alpha_rand(sizeString);

#ifdef PREFIX_OVERLAP
    if (inserted.empty())
        return result;
    int randNth = rand() % contSize(inserted) + 1;
    string nth = getNth(randNth);
    int overlap = rand() % contSize(nth);
    string prefix = nth.substr(0, overlap);
    result = prefix.substr(0, sizeString) +
             result.substr(0, max(0, sizeString - overlap));
#endif

    return result;
}

void printNewKeyValue() {
    string newKey;

    while (1) {
        newKey = rand_string_wrapper();
        if (inserted.find(newKey) == inserted.end()) {
            inserted.insert(newKey);
            break;
        }
    }

    printf("%s %s\n", newKey.c_str(), rand_string_wrapper(true).c_str());
}

int main() {
    srand(42);  // time(0));

    int totalOps = SEED + OP_COUNT;
    printf("%d\n", totalOps);

    for (int i = 0; i < SEED; i++) {
        printf("%d ", INSERT_OP);
        printNewKeyValue();
    }

    int rand_idx, op, nth;
    string newstring, str;

    for (int i = 1; i <= OP_COUNT; i++) {
        op = rand() % OP_TYPES;

        if (inserted.empty())
            op = INSERT_OP;
        else {
            rand_idx = rand() % contSize(inserted);
            nth = rand_idx + 1;
        }

#ifdef MAX_OUT
        op = INSERT_OP;
#endif

        printf("%d ", op);
        switch (op) {
            case INSERT_OP:
                printNewKeyValue();
                break;
            case LOOKUP_OP:
                printf("%s\n", getNth(nth).c_str());
                break;
            case ERASE_OP:
                str = getNth(nth);
                printf("%s\n", str.c_str());
                inserted.erase(str);
                break;
            case LOOKUPN_OP:
                printf("%d\n", nth);
                break;
            case ERASEN_OP:
                str = getNth(nth);
                printf("%d\n", nth);
                inserted.erase(str);
                break;
            default:
                assert(false);
                break;
        }
    }
}
