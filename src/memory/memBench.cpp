#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>

int N = 1000;
int FIXED_SIZE = 100;
int ITERS = 100;

// do something random with the memory
void processMem(char* ptr) {
    for (int i = 0; i < FIXED_SIZE; i++)
        *(ptr + i) = (char)(rand() % 50 + 40);
    for (int i = 1; i < FIXED_SIZE; i++)
        *(ptr + i) = (char)(*(ptr + i) - *(ptr + i - 1));
    std::ofstream f;
    f.open("out.txt", std::ofstream::out);

    for (int i = 0; i < FIXED_SIZE; i++)
        f << *(ptr + i);
    f << "\n";
}

double timer(struct timespec& t) {
    return t.tv_nsec / 1e9 + t.tv_sec;
}

double benchmark(bool premem = false) {
    struct timespec st, en;

    clock_gettime(CLOCK_MONOTONIC_RAW, &st);

    if (premem) {
        for (int i = 0; i < N; i++) {
            char* a = (char*)malloc(FIXED_SIZE);
            processMem(a);
        }
    } else {
        char* big = (char*)malloc(N * FIXED_SIZE);
        for (int i = 0; i < N; i++) {
            processMem(big);
            big += FIXED_SIZE;
        }
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &en);

    return timer(en) - timer(st);
}

int main() {
    srand(time(NULL));

    int best = INT32_MAX, bestpre = INT32_MAX;
    // idx 1 is for premem, 0 for normal mem
    double beatTimes[] = {0,
                          0};  // cumulative margin by which one beat the other
    int wins[] = {0, 0};       // number of times one beat the other

    for (int i = 0; i < ITERS; i++) {
        double premem, mem;

        // randomly decide order in which to run
        if (rand() & 1) {
            premem = benchmark(true);
            mem = benchmark();
        } else {
            mem = benchmark();
            premem = benchmark(true);
        }

        printf("%.8lf %.8lf\n", mem, premem);
        bool whoWon = mem >= premem;
        beatTimes[whoWon] += abs(mem - premem);
        wins[whoWon]++;
    }

    printf("----\n");
    printf("%.8lf %.8lf\n", beatTimes[0], beatTimes[1]);
    printf("%d %d\n", wins[0], wins[1]);

    return 0;
}