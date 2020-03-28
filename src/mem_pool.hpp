#ifndef _MEM_POOL_
#define _MEM_POOL_

#include <cstdlib>
class mem_pool {
    typedef unsigned long long ull;
    private:
        char *head, *cur, *end;
        ull unit_size;
    public:
        mem_pool(ull MAX_BYTES, ull unit_size) {
            head = (char *) malloc(MAX_BYTES);
            cur = head;
            end = head + MAX_BYTES;
            this->unit_size = unit_size;
        }
        void *get_units(ull count) {
            if (cur + count * this->unit_size >= end)
                return NULL;
            
            void *units = cur;
            cur += count * this->unit_size;
            return units;
        }
        bool free_units(ull count) {
            if (cur - count * this->unit_size < head)
                return false;

            cur -= count * this->unit_size;
            return true;
        }
        void free_pool() {
            if(!head) {
                free(head);
                head = NULL;
                cur = NULL;
                end = NULL;
            }
        }
        ~mem_pool() {
            if(!head)
                free(head);
        }
};

#endif
