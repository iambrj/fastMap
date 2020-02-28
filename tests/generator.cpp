#define SEED 10           // no elements to be inserted at beginning
#define MAX_KEY_LEN 10    // 64
#define MAX_VALUE_LEN 10  // 256

#define OP_TYPES 3
#define LOOKUP_OP 0
#define INSERT_OP 1
#define ERASE_OP 2
#define LOOKUPN_OP 3
#define ERASEN_OP 4
#define OP_COUNT 50000
//#define MAX_OUT

#include <vector>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cassert>

using namespace std;

vector<string> inserted;
int size;

static const char alpha[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";

static int alpha_len = strlen(alpha);

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
    size = modder;
#endif

    string result = alpha_rand(sizeString);

#ifdef PREFIX_OVERLAP
    if (inserted.empty())
        return result;
    int rand_idx = rand() % inserted.size();
    string prefix = inserted[rand_idx].substr(
        0, rand() % (rand() % inserted[rand_idx].length() + 1));
    result = prefix + result;
#endif

    return result;
}

void printNewKeyValue() {
    string newKey = rand_string_wrapper();
    inserted.push_back(newKey);
    size++;

    printf("%s %s\n", newKey.c_str(), rand_string_wrapper(true).c_str());
}

int main() {
    srand(time(0));

    int totalOps = SEED + OP_COUNT;
    printf("%d\n", totalOps);

    for (int i = 0; i < SEED; i++) {
        printf("%d ", INSERT_OP);
        printNewKeyValue();
    }

    int rand_idx, op;
    string newstring;

    for (int i = 0; i < OP_COUNT; i++) {
        op = rand() % OP_TYPES;

        if (!size)
            op = INSERT_OP;
        else
            rand_idx = rand() % size;

#ifdef MAX_OUT
        op = INSERT_OP;
#endif

        printf("%d ", op);
        switch (op) {
            case INSERT_OP:
                printNewKeyValue();
                break;
            case LOOKUP_OP:
                printf("%s\n", inserted[rand_idx].c_str());
                break;
            case ERASE_OP:
                printf("%s\n", inserted[rand_idx].c_str());
                size--;
                inserted.erase(inserted.begin() + rand_idx);
                break;
            case LOOKUPN_OP:
                printf("%d\n", rand_idx + 1);
                break;
            case ERASEN_OP:
                printf("%d\n", rand_idx + 1);
                size--;
                inserted.erase(inserted.begin() + rand_idx);
                break;
            default:
                assert(false);
                break;
        }
    }
}
