// #include "../src/fast_map.h"
#include "../src/naive.cpp"

#include <cstdlib>
#include <iostream>
#include <map>
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

typedef std::map<Slice, Slice> mpss;

int getRandomIndex(const mpss &mp) {
    if (mp.empty())
        return -1;
    return rand() % (int)mp.size();
}

auto getRandomIterator(const mpss &mp) {
    return getNthIterator(mp, getRandomIndex(mp));
}

int main() {
    srand(0);

    kvStore kv(STORE_SIZE);
    mpss m;
    int get_index, del_index;

    for (int i = 0; i < TEST_SIZE; i++) {
        Slice key_slice, val_slice;
        key_slice.size = KEY_SIZE;
        val_slice.size = KEY_SIZE;

        switch (rand() % 5) {
            case 0:
                // test kvStore->get
                std::cout << "[GET] ";

                if (m.size()) {
                    auto it = getRandomIterator(m);
                    key_slice.data = (it->first).data;

                    std::cout << "key: " << key_slice.data
                              << " val: " << val_slice.data << " ";

                    if (kv.get(key_slice, val_slice)) {
                        if (m[key_slice] == val_slice)
                            std::cout << "succeeded\n";
                        else
                            std::cout << "mismatch\n";
                    } else {
                        std::cout << "failed\n";
                    }
                } else {
                    std::cout << "store empty\n";
                }
                break;
            case 1: {
                // test kvStore->put
                string k = random_string(KEY_SIZE);
                string v = random_string(KEY_SIZE);

                key_slice.data = &k[0];
                val_slice.data = &v[0];

                std::cout << "[PUT] key: " << key_slice.data
                          << " val: " << val_slice.data << " ";

                if (kv.put(key_slice, val_slice)) {
                    std::cout << "rewrite\n";
                } else {
                    std::cout << "new write\n";
                }

                m[key_slice] = val_slice;
                break;
            }
            case 2:
                // test kvStore->del
                std::cout << "[DEL] key: " << key_slice.data
                          << " val: " << val_slice.data;

                if (m.size()) {
                    auto it = getRandomIterator(m);
                    key_slice.data = it->second.data;

                    if (kv.del(key_slice)) {
                        std::cout << "succeeded\n";
                        m.erase(key_slice);
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
                              << "index: " << get_index
                              << " key: " << key_slice.data
                              << " val: " << val_slice.data << " ";
                    if (kv.get(get_index, key_slice, val_slice)) {
                        auto it = getNthIterator(m, get_index);

                        if (it->second == val_slice)
                            std::cout << "succeeded\n";
                        else
                            std::cout << "mismatch\n";
                    } else {
                        std::cout << "failed\n";
                    }
                } else {
                    std::cout << "[GET] store empty\n";
                }
                break;
            case 4:
                // test kvStore->del at index
                del_index = getRandomIndex(m);
                std::cout << "[DEL] "
                          << "index: " << del_index
                          << " key: " << key_slice.data
                          << " val: " << val_slice.data << " ";

                if (m.size()) {
                    // kv->get needed for debug logs
                    if (kv.get(del_index, key_slice, val_slice) &&
                        kv.del(del_index)) {
                        std::cout << "succeeded\n";
                        m.erase(getNthIterator(m, del_index));
                    }
                } else {
                    std::cout << "failed\n";
                }
        }
    }
}
