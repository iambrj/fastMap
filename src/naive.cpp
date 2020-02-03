#include <map>
#include <string.h>
using namespace std;

struct Slice {
    uint8_t size;
    char *data;
};

bool operator==(const Slice &a, const Slice &b) {
    return b.size == a.size && strcmp(a.data, b.data) == 0;
}

bool operator<(const Slice &a, const Slice &b) {
    return strcmp(a.data, b.data);
}
class kvStore {
    typedef map<string, string> mpss;

   private:
    mpss mp;

    string getString(const Slice &sc) {
        return string(sc.data, sc.data + sc.size);
    }

    char *getCharPointer(string &s) {
        char *valueChar = (char *)malloc(s.size());
        for (int i = 0; i < s.size(); i++) {
            *(valueChar + i) = s[i];
        }

        return valueChar;
    }

    mpss::iterator getNthIterator(int n) {
        int c = 1;
        for (auto it = mp.begin(); it != mp.end(); it++) {
            if (c == n) {
                return it;
            }
            c++;
        }

        return mp.end();
    }

    void setStringIntoSlice(string &s, Slice &slc) {
        slc.data = getCharPointer(s);
        slc.size = s.size();
    }

   public:
    kvStore(uint64_t max_entries) {
        // WE ARE NOT utilizing max_entries in this
    }

    // returns false if key didn’t exist
    bool get(Slice &key, Slice &value) {
        string k = getString(key);
        string v = mp[k];

        if (mp.find(k) == mp.end()) {
            return false;
        }

        setStringIntoSlice(v, value);

        return true;
    }

    // returns true if value overwritten
    bool put(const Slice &key, const Slice &value) {
        string v = getString(value);
        string k = getString(key);
        bool ret = mp.find(k) != mp.end();

        mp[k] = v;

        return ret;
    }

    bool del(Slice &key) {
        string k = getString(key);

        if (mp.find(k) == mp.end())
            return false;
        mp.erase(k);

        return true;
    }

    // returns Nth key-value pair
    bool get(int N, Slice &key, Slice &value) {
        auto it = getNthIterator(N);

        if (it != mp.end()) {
            string k = it->first;
            string v = it->second;
            setStringIntoSlice(k, key);
            setStringIntoSlice(v, value);
            return true;
        } else
            return false;
    }

    // delete Nth key-value pair
    bool del(int N) {
        auto it = getNthIterator(N);

        if (it != mp.end()) {
            mp.erase(it);
            return true;
        } else
            return false;
    }
};
