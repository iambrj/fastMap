#define SEED 10  // no elements to be inserted at beginning
#define MAX_KEY_LEN 64
#define MAX_VALUE_LEN 256

#define OP_TYPES 3
#define LOOKUP_OP 0
#define INSERT_OP 1
#define ERASE_OP 2
#define OP_COUNT 10000000

#include <vector>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <cstdio>
#include <cstring>

using namespace std;

vector<string> inserted;

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
    int size = rand() % MAX_KEY_LEN + 1;
#ifdef MAX_OUT
    size = isValue ? MAX_VALUE_LEN : MAX_KEY_LEN;
#endif

    string result = alpha_rand(size);

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

int main() {
    srand(time(NULL));

    int totalOps = SEED + OP_COUNT;
    printf("%d\n", totalOps);

    for (int i = 0; i < SEED; i++) {
        printf("%d ", INSERT_OP);
        string newstring = rand_string_wrapper();
        inserted.push_back(newstring);
        printf("%s %s\n", newstring.c_str(), rand_string_wrapper().c_str());
    }

    int rand_idx, op;
    for (int i = 0; i < OP_COUNT; i++) {
        op = rand() % OP_TYPES;
        string newstring = rand_string_wrapper();

        if (!inserted.size())
            op = INSERT_OP;
        else
            rand_idx = rand() % inserted.size();

#ifdef MAX_OUT
        op = INSERT_OP;
#endif

        printf("%d ", op);
        switch (op) {
            case INSERT_OP:
                inserted.push_back(newstring);
                printf("%s %s\n", newstring.c_str(), rand_string_wrapper(true).c_str());
                break;
            case LOOKUP_OP:
                printf("%s\n", inserted[rand_idx].c_str());
                break;
            case ERASE_OP:
                printf("%s\n", inserted[rand_idx].c_str());
                inserted.erase(inserted.begin() + rand_idx);
                break;
        }
    }
}
