#include<pthread.h>

class monitor {
private:
    // no. of readers 
    int readerCount;

    // no. of writers 
    int writerCount;

    // no. of readers waiting 
    int readerWaitingCount;

    // no. of writers waiting 
    int writerWaitingCount;

    // condition variable to check whether reader can read 
    pthread_cond_t canread;

    // condition variable to check whether writer can write 
    pthread_cond_t canwrite;

    // mutex for synchronisation 
    pthread_mutex_t condlock;

public:
    monitor() {
        readerCount = 0;
        writerCount = 0;
        readerWaitingCount = 0;
        writerWaitingCount = 0;

        int a = pthread_cond_init(&canread, NULL);
        a += pthread_cond_init(&canwrite, NULL);
        a += pthread_mutex_init(&condlock, NULL);
        if (a != 0) {
        }
    }

    // mutex provide synchronisation so that no other thread 
    // can change the value of data 
    void beginread() {
        pthread_mutex_lock(&condlock);

        // if there are active or waiting writers 
        if (writerCount == 1 || writerWaitingCount > 0) {
            // incrementing waiting readers 
            readerWaitingCount++;

            // reader suspended 
            pthread_cond_wait(&canread, &condlock);
            readerWaitingCount--;
        }

        // else reader reads the resource 
        readerCount++;
        pthread_mutex_unlock(&condlock);
        pthread_cond_broadcast(&canread);
    }

    void endread() {
        // if there are no readers left then writer enters monitor 
        pthread_mutex_lock(&condlock);

        if (--readerCount == 0)
            pthread_cond_signal(&canwrite);

        pthread_mutex_unlock(&condlock);
    }

    void beginwrite() {
        pthread_mutex_lock(&condlock);

        // a writer can enter when there are no active 
        // or waiting readers or other writer 
        if (writerCount == 1 || readerCount > 0) {
            ++writerWaitingCount;
            pthread_cond_wait(&canwrite, &condlock);
            --writerWaitingCount;
        }
        writerCount = 1;
        pthread_mutex_unlock(&condlock);
    }

    void endwrite() {
        pthread_mutex_lock(&condlock);
        writerCount = 0;

        // if any readers are waiting, threads are unblocked 
        if (readerWaitingCount > 0)
            pthread_cond_signal(&canread);
        else
            pthread_cond_signal(&canwrite);
        pthread_mutex_unlock(&condlock);
    }
};
