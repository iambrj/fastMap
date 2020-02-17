#define SEED 10  // no elements to be inserted at beginning
#define MAX_STR_LEN 64

#define OP_TYPES 3
#define LOOKUP_OP 0
#define INSERT_OP 1
#define ERASE_OP 2
#define OP_COUNT 1000

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

string get_newstring() {
    return alpha_rand(rand() % MAX_STR_LEN + 1);
}

string rand_string_wrapper() {
    string result = alpha_rand(rand() % MAX_STR_LEN + 1);
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
        string newstring = get_newstring();
        inserted.push_back(newstring);
        printf("%s %s\n", newstring.c_str(), get_newstring().c_str());
    }
    int rand_idx, op;
    for (int i = 0; i < OP_COUNT; i++) {
        op = rand() % OP_TYPES;
        string newstring = get_newstring();
        if (!inserted.size())
            op = INSERT_OP;
        else
            rand_idx = rand() % inserted.size();
        printf("%d ", op);
        switch (op) {
            case INSERT_OP:
                inserted.push_back(newstring);
                printf("%s %s\n", rand_string_wrapper().c_str(),
                       get_newstring().c_str());
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
