struct Slice{
    uint8_t size;
    char*   data;
};

class kvStore {
    public:
        kvStore(uint64_t max_entries);
        bool get(Slice &key, Slice &value); //returns false if key didn’t exist
        bool put(Slice &key, Slice &value); //returns true if value overwritten
        bool del(Slice &key);			
        bool get(int N, Slice &key, Slice &value); //returns Nth key-value pair
        bool del(int N); //delete Nth key-value pair
};
