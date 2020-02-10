#include "../src/fast_map.h"
#include "../src/naive.cpp"

#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <assert.h>

const int TEST_SIZE = 10;
const int OP_COUNT = 5;
const int STORE_SIZE = 100000;
const int KEY_SIZE = 32;
const int VAL_SIZE = 32;

const char charset[] =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";
const size_t max_index = (sizeof(charset) - 1);

#define fail               \
    {                      \
        cout << "failed!"; \
        exit(1);           \
    }

/* https://stackoverflow.com/a/12468109/12158779 */
std::string random_string(size_t length) {
    auto randchar = []() -> char { return charset[rand() % max_index]; };

    assert(length > 0);
    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);
    return str;
}
template <typename T>
auto getNthIterator(const map<T, T> &mp, int n) {
    int c = 0;

    for (auto it = mp.begin(); it != mp.end(); it++) {
        if (c == n) {
            return it;
        }
        c++;
    }

    return mp.end();
}

template <typename T>
int getRandomIndex(const map<T, T> &mp) {
    if (mp.empty())
        return -1;
    return rand() % ((int)mp.size());
}

template <typename T>
auto getRandomIterator(const map<T, T> &mp) {
    return getNthIterator(mp, getRandomIndex(mp));
}

int main() {
    srand(0);

    kvStoreBaseline kvOriginal(STORE_SIZE);
    kvStore kv(STORE_SIZE);
    int get_index, del_index;

    for (int i = 0; i < TEST_SIZE; i++) {
        vector<Slice> org(2), mod(2);
        for (auto &x : org) {
            x.size = KEY_SIZE;
        }

        switch (rand() % 5) {
            case 0:
                // test kvStore->get
                std::cout << "[GET] ";

                if (m.size()) {
                    auto it = getRandomIterator(m);
                    org[0].data = (it->first).data;

                    std::cout << "key: " << org[0].data;

                    bool r1 = kvOriginal.get(org[0], org[1]);
                    bool r2 = kv.get(mod[0], mod[1]);

                    assert(r1 == r2);
                    if (strcmp(mod[1].data, org[1].data) == 0) {
                        std::cout << "succeeded\n";
                    } else {
                        std::cout << "mismatch\n";

                        std::cout << "val original: " << org[1].data << " ";
                        std::cout << "your val: " << mod[1].data << " ";
                        fail;
                    }
                } else {
                    std::cout << "store empty\n";
                }
                break;
            case 1: {
                // test kvStore->put
                string k = random_string(KEY_SIZE);
                string v = random_string(KEY_SIZE);

                org[0].data = &k[0];
                org[1].data = &v[0];

                mod[0].data = &k[0];
                mod[1].data = &v[0];

                std::cout << "[PUT] key: " << org[0].data
                          << " val: " << mod[1].data << " ";

                bool r1 = kvOriginal.put(org[0], org[1]);
                bool r2 = kv.put(mod[0], mod[1]);

                assert(r1 == r2);

                if (r1) {
                    std::cout << "rewrite\n";
                } else {
                    std::cout << "new write\n";
                }
                break;
            }
            case 2:
                // test kvStore->del
                std::cout << "[DEL] key: " << org[0].data
                          << " val: " << org[1].data;

                if (m.size()) {
                    auto it = getRandomIterator(m);
                    org[0].data = it->second.data;

                    bool r1 = kvOriginal.del(org[0]);
                    bool r2 = kv.del(mod[0]);

                    assert(r1 == r2);

                    if (r1) {
                        std::cout << "succeeded\n";
                    } else {
                        std::cout << "failed\n";
                    }
                } else {
                    std::cout << "[DEL] store empty\n";
                }
                break;
            case 3:
                // test kvStore->get at index
                get_index = getRandomIndex(m);

                if (m.size()) {
                    std::cout << "[GET] "
                              << "index: " << get_index;

                    bool r1 = kvOriginal.get(get_index, org[0], org[1]);
                    bool r2 = kv.get(get_index, mod[0], mod[1]);
                    assert(r1 == r2);

                    if (r1) {
                        cout << " key: " << org[0].data
                             << " val: " << org[1].data << " ";

                        if (mod[1] == org[1])
                            std::cout << "succeeded\n";
                        else {
                            std::cout << "mismatch\n";
                            cout << " key: " << mod[0].data
                                 << " val: " << mod[1].data << " ";
                        }
                    } else {
                        fail;
                    }
                } else {
                    std::cout << "[GET] store empty\n";
                }
                break;
            case 4:
                // test kvStore->del at index
                del_index = getRandomIndex(m);
                std::cout << "[DEL] "
                          << "index: " << del_index;

                if (m.size()) {
                    bool r1 = kvOriginal.del(del_index);
                    bool r2 = kv.del(del_index);
                    assert(r1 == r2);

                    if (r1) {
                        std::cout << "succeeded\n";
                    } else {
                        fail;
                    }
                } else {
                    std::cout << "Empty map\n";
                }
        }
    }
}
