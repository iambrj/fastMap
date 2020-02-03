// #include "../src/fast_map.h"
#include "../src/naive.cpp"

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <algorithm>

const int TEST_SIZE = 10;
const int OP_COUNT = 5;
const int STORE_SIZE = 100000;
const int KEY_SIZE = 32;
const int VAL_SIZE = 32;

/* https://stackove1rflow.com/a/12468109/12158779 */
std::string random_string(size_t length) {
    auto randchar = []() -> char {
        const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[rand() % max_index];
    };

    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);
    return str;
}

int main() {
    Slice key_slice, val_slice;
    key_slice.size = KEY_SIZE;
    val_slice.size = KEY_SIZE;

    kvStore kv(STORE_SIZE);
    std::map<Slice, Slice> m;

    for (int i = 0; i < TEST_SIZE; i++) {
        switch (rand() % 5) {
            case 0:
                // test kvStore->get
                std::cout << "[GET] ";
                if (m.size()) {
                    key_slice.data = m[m.begin() + rand() % m.size() - 1];
                    if (kv.get(key_slice, val_slice)) {
                        if (m[key_slice] == val_slice)
                            std::cout << "key: " << key_slice.data
                                      << " val: " << val_slice.data
                                      << " succeeded\n";
                        else
                            std::cout << "key: " << key_slice.data
                                      << " val: " << val_slice.data
                                      << " mismatch\n";
                    } else {
                        std::cout << "key: " << key_slice.data
                                  << " val: " << val_slice.data << " failed\n";
                    }
                } else {
                    std::cout << "store empty\n";
                }
            case 1:
                // test kvStore->put
                key_slice.data = random_string(KEY_SIZE);
                val_slice.data = random_string(KEY_SIZE);

                if (kv.put(key_slice, val_slice)) {
                    std::cout << "[PUT] key: " << key_slice.data
                              << " val: " << val_slice.data << " succeeded\n";
                    m[key_slice] = val_slice;
                } else {
                    std::cout << "[PUT] key: " << key_slice.data
                              << " val: " << val_slice.data << " failed\n";
                }
            case 2:
                // test kvStore->del
                if (m.size()) {
                    key_slice.data = m[m.begin() + rand() % m.size() - 1];
                    if (kv.del(key_slice)) {
                        std::cout << "[DEL] key: " << key_slice.data
                                  << " val: " << val_slice.data
                                  << " succeeded\n";
                        m.erase(key_slice);
                    } else {
                        std::cout << "[DEL] key: " << key_slice.data
                                  << " val: " << val_slice.data << " failed\n";
                    }
                } else {
                    std::cout << "[DEL] store empty\n";
                }
            case 3:
                // test kvStore->get at index
                if (m.size()) {
                    int get_index = rand() % n.size();
                    if (kv.get(get_index, key_slice, val_slice)) {
                        if (m[m.begin() + get_index - 1] == val_slice)
                            std::cout << "[GET] "
                                      << "index: " << get_index
                                      << " key: " << key_slice.data
                                      << " val: " << val_slice.data
                                      << " succeeded\n";
                        else
                            std::cout << "[GET] "
                                      << "index: " << get_index
                                      << " key: " << key_slice.data
                                      << " val: " << val_slice.data
                                      << " mismatch\n";
                    } else {
                        std::cout << "[GET] "
                                  << "index: " << get_index
                                  << " key: " << key_slice.data
                                  << " val: " << val_slice.data << " failed\n";
                    }
                } else {
                    std::cout << "[DEL] store empty\n";
                }
            case 4:
                // test kvStore->del at index
                if (m.size()) {
                    int del_index = rand() % m.size();
                    // kv->get needed for debug logs
                    if (kv.get(del_index, key_slice, val_slice) &&
                        kv.del(del_index)) {
                        std::cout << "[DEL] "
                                  << "index: " << get_index
                                  << " key: " << key_slice.data
                                  << " val: " << val_slice.data
                                  << " succeeded\n";
                        m.erase(m.begin() + del_index - 1);
                    }
                } else {
                    std::cout << "[DEL] "
                              << "index: " << get_index
                              << " key: " << key_slice.data
                              << " val: " << val_slice.data << " failed\n";
                }
        }
    }
}
