#include <cassert>
#include <cstdlib>
#include <semaphore.h>

// Bithack to compute minimum branchlessly
#define MIN(x, y) ((y) ^ (((x) ^ (y)) & -((x) < (y))))
#define RANGE 52

int readercount = 0;

class TrieNode {
   public:
    int numofEnds;  // num of values that ended at this node
    TrieNode **p;
    char *value;
    sem_t reader_sem, writer_sem;

    int getIndex(char c) {
        return c - MIN(MIN(c, 'a'), 'A') - (-(c > 'Z') & 6);
    }

    char getChar(int idx) {
        if (RANGE / 2 <= idx)
            return (char)((idx - RANGE / 2) + 'a');

        return (char)(idx + 'A');
    }

    TrieNode()
        : numofEnds(0),
          p((TrieNode **)calloc(sizeof(TrieNode *), RANGE)),
          value(nullptr) {
            sem_init(&reader_sem, 0, 1);
            sem_init(&writer_sem, 0, 1);
    }

    ~TrieNode() {
        for (int i = 0; i < RANGE; i++) {
            delete p[i];
        }

        free(p);
        p = nullptr;
    }

    bool insert(char *s, int sLen, char *valueToInsert) {
        if (!sLen) {
            bool isOverwrite = false;
            sem_wait(&reader_sem);
            readercount++;
            if(readercount==1) sem_wait(&writer_sem);
            sem_post(&reader_sem);
            isOverwrite = this->value;
            sem_wait(&reader_sem);
            readercount--;
            if(readercount==0) sem_post(&writer_sem);
            sem_post(&reader_sem);

            sem_wait(&writer_sem);
            this->numofEnds += !this->value;
            this->value = valueToInsert;
            sem_post(&writer_sem);
            return isOverwrite;
        }

        int idx = getIndex(*s);
        sem_wait(&writer_sem);
        if (!this->p[idx])
            this->p[idx] = new TrieNode();
        bool isOverwrite = this->p[idx]->insert(s + 1, sLen - 1, valueToInsert);
        this->numofEnds += !isOverwrite;
        sem_post(&writer_sem);

        return isOverwrite;
    }

    char *lookup(char *s, int sLen) {
        TrieNode *currNode = this;
        int i = 0;

        while (i < sLen) {
            int a = getIndex(s[i]);

            sem_wait(&reader_sem);
            readercount++;
            if(readercount == 1) sem_wait(&writer_sem);
            sem_post(&reader_sem);
            if (!currNode->p[a]) {
                sem_wait(&reader_sem);
                readercount--;
                if(readercount == 0) sem_post(&writer_sem);
                sem_post(&reader_sem);
                return nullptr;
            }

            currNode = currNode->p[a];
            i++;
        }

        sem_wait(&reader_sem);
        readercount--;
        if(readercount == 0) sem_post(&writer_sem);
        sem_post(&reader_sem);
        return currNode->value;
    }

    bool erase(char *s, int sLen) {
        if (!sLen) {
            sem_wait(&writer_sem);
            if (this->value) {
                free(this->value);
                this->value = nullptr;
                this->numofEnds--;

                sem_post(&writer_sem);
                return true;
            } else {
                sem_post(&writer_sem);
                return false;
            }
        }

        int idx = getIndex(*s);
        sem_wait(&reader_sem);
        readercount++;
        if(readercount==1) sem_wait(&writer_sem);
        sem_post(&reader_sem);
        if (!this->p[idx]) {
            sem_wait(&reader_sem);
            readercount--;
            if(readercount==0) sem_post(&writer_sem);
            sem_post(&reader_sem);
            return false;
        }
        sem_wait(&reader_sem);
        readercount--;
        if(readercount==0) sem_post(&writer_sem);
        sem_post(&reader_sem);

        sem_wait(&writer_sem);
        if (this->p[idx]->erase(s + 1, sLen - 1)) {
            this->numofEnds--;
            sem_post(&writer_sem);
            return true;
        }

        sem_post(&writer_sem);
        return false;
    }

    bool lookup(int N, char **key, char **valuePointer) {
        int cnt = 0;
        char *keyPointer = (char *)calloc(65, 1), *kOrg = keyPointer;

        sem_wait(&reader_sem);
        readercount++;
        if(readercount==1) sem_wait(&writer_sem);
        sem_post(&reader_sem);
        sem_wait(&reader_sem);
        TrieNode *curr = this;

        while (N) {
            if (curr->value) {
                N--;

                if (!N) {
                    *valuePointer = curr->value;
                    readercount--;
                    if(readercount==0) sem_post(&writer_sem);
                    sem_post(&reader_sem);
                    goto end;
                }
            }

            for (int i = 0; i < RANGE; i++) {
                TrieNode *trans = curr->p[i];

                if (trans) {
                    if (cnt + trans->numofEnds < N) {
                        cnt += trans->numofEnds;
                    } else {
                        curr = trans;
                        N -= cnt;
                        cnt = 0;

                        *keyPointer = getChar(i);
                        keyPointer++;
                        readercount--;
                        if(readercount==0) sem_post(&writer_sem);
                        sem_post(&reader_sem);
                        goto end;
                    }
                }
            }
            readercount--;
            if(readercount==0) sem_post(&writer_sem);
            sem_post(&reader_sem);
            return false;
end:;
        }

        *key = kOrg;
        readercount--;
        if(readercount==0) sem_post(&writer_sem);
        sem_post(&reader_sem);
        return true;
    }

    bool erase(int N) {
        int cnt = 0;

        sem_wait(&reader_sem);
        readercount++;
        if(readercount==1) sem_wait(&writer_sem);
        sem_post(&reader_sem);
        sem_wait(&reader_sem);
        TrieNode *curr = this;
        int Norg = N;

        // first determine whether string is present or not
        while (N) {
            if (curr->value) {
                N--;

                if (!N) {
                    goto end;
                }
            }

            for (int i = 0; i < RANGE; i++) {
                TrieNode *trans = curr->p[i];

                if (trans) {
                    if (cnt + trans->numofEnds < N) {
                        cnt += trans->numofEnds;
                    } else {
                        curr = trans;
                        N -= cnt;
                        cnt = 0;
                        goto end;
                    }
                }
            }

            readercount--;
            if(readercount==0) sem_post(&writer_sem);
            sem_post(&reader_sem);
            return false;
end:;
        }

        // if string was found then deecrement num of ends
        // and free the value
        curr = this;
        cnt = 0;
        N = Norg;

        while (N) {
            if (curr->value) {
                N--;

                if (!N) {
                    curr->numofEnds--;
                    free(curr->value);
                    curr->value = nullptr;
                    goto end2;
                }
            }

            for (int i = 0; i < RANGE; i++) {
                TrieNode *trans = curr->p[i];

                if (trans) {
                    if (cnt + trans->numofEnds < N) {
                        cnt += trans->numofEnds;
                    } else {
                        curr->numofEnds--;
                        curr = trans;
                        N -= cnt;
                        cnt = 0;

                        goto end2;
                    }
                }
            }

            readercount--;
            if(readercount==0) sem_post(&writer_sem);
            sem_post(&reader_sem);
            return false;
end2:;
        }

        readercount--;
        if(readercount==0) sem_post(&writer_sem);
        sem_post(&reader_sem);
        return true;
    }
};
