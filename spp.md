# Spec for SPoP

## Memory requirements analysis

- Our key size is only 64 bytes, where each distinct letter in the key matches `[a-zA-Z]`. Each of these 52 possibilities take 6 bits to represent. Thus, worst case memory usage is 10 million keys times 52 bits per key = $10^7 \times 52= 65 \times 10^6$ bytes, which is approximately $65$ MB for storing the keys.

- Each value can have 256 characters, as one ASCII character takes one byte (0-255 range), and max value size is 256 bytes. Thus, total memory usage is around $10^7\times 256=2.56\times10^9$ bytes $=2.56$ giga bytes, at the very bare minimum. Moreover, each location will need a lot of pointers to be stored, moreover, we will need additional integers to augment the trie nodes. Therefore, we are expected to reach upto 4GB of memory usage.


## Multithreading ideas

- We intend to keep a mutex to ensure that at most one transaction occurs simultaneously, i.e., no race conditions.
- **Is our application parallelized??**

## Data augmentation

- How can we augment our data to better suit our needs?

## Trie based approach

- A trie is a data structure that keeps pointers to the its children, and each transition to a child represents one letter being added to the currently processed string.

- We propose representing each key in a simple trie. The trie nodes will be augmented with two values: number of children leading further from that trie node, as well as, the value for the key ending at that trie node (if it was set).

- The height of the trie is at max 64, since each key has max length 64.

- How this will work in each case? When searching for a key, we will traverse the key recursively or using a stack. Let us denote $n$=number of keys in the map so far, $l_k=\text{len}(k)$ and $l_v=\text{len}(v)$. Note that $l_k\le 64$.
  - `get(Key)` - this will, within at most $l_k$ iterations, return the value, or claim that none exists.
  - `put(Key, value)` - within at most $l_k$ iterations, this will reach the memory address for the key and put the value there.
  - `delete(Key)` -  within at most $l_k$ iterations, this will reach the memory address for the key and set its value to `nullptr`.
  - `get(int n)` - within $\log(52)\times\log(n)$ steps it will converge to the correct key, since each trie node maintains count of number of its children, and we will need to binary search over the cumulative sum count for the nodes.
  - `delete(int n)` - same as above

- Each of these methods may also have significant overhead due to having to convert to and from a `Slice` object. **FIGURE THIS OUT?**

- Struct:

    ```c++
    struct {
        node** children; // array of 52 elements, containing a pointer to the next child
        node* valuePtr = nullptr;
    } node;
    ```

