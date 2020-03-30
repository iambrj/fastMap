#include <cstdlib>

template<typename T>
void resetPointer(T *&x) {
    if (x) {
        free(x);
        x = NULL;
    }
}

