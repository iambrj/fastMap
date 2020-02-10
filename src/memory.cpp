#include <iostream>
#include <stdlib.h>
#include <assert.h>

const long long MX_BYTES = 1ll << 32;
char* spaceINeed = (char*)malloc(MX_BYTES);
const long long MX_CHECK = 100;

/*
    This program allocates a contiguous memory of 2^32 bytes
    and then randomly accesses different indices from it and checks if they have
   the same value as intended
   The intended value and indices are stored in a vec, vecVal arrays.
*/
int main() {
    srand(0);
    long long vec[MX_CHECK];
    long long vecVal[MX_CHECK];

    for (long long i = 0; i < MX_CHECK; i++) {
        long long ind = rand() % MX_BYTES;
        char val = rand() % 255;
        spaceINeed[ind] = val;
        vec[i] = ind;
        vecVal[i] = val;
    }

    for (long long i = 0; i < MX_CHECK; i++) {
        std::cout << vec[i] << " " << vecVal[i] << " "
                  << (int)spaceINeed[vec[i]] << std::endl;
        assert((int)spaceINeed[vec[i]] == vecVal[i]);
    }

    std::cout << "success allocation" << std::endl;

    return 0;
}