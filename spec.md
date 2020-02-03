# Spec for SPoP

## Memory requirements analysis

- Our key size is only 64 bytes, where each distinct letter in the key matches `[a-zA-Z]`. Each of these 52 possibilities take 6 bits to represent. Thus, worst case memory usage is 10 million keys times 52 bits per key = $10^7 \times 52= 65 \times 10^6$ bytes, which is approximately $65$ MB for storing the keys.

- Each value can have 256 characters, as one ASCII character takes one byte (0-255 range), and max value size is 256 bytes. Thus, total memory usage is around $10^7\times 256=2.56\times10^9$ bytes $=2.56$ giga bytes. ISN'T THIS HUGE?

- Finally, since we also plan to augment each trie node with some integers representing number of valid children the trie node has, the memory requirement is expected to shoot up by?

## Multithreading ideas

- We intend to keep a mutex to ensure that no more than one transaction occurs simultaneously, i.e., no race conditions.
- **Is our application parallelized??**

## Data augmentation

- how can we augment our data to better suit our needs?

## Trie based approach

- A trie is a data structure that keeps pointers to the its children, and each transition to a child represents one letter being added to the currently processed string. 

- We propose representing each key in a simple trie. The trie nodes will be augmented with two values: number of children leading further from that trie node, as well as, the value for the key at that trie node (if it was set).
- The height of the trie is at max 64, since each key has max length 64.

- How this will work in each case? Let we denote $n$=number of keys in the map so far, $l_k=\text{len}(k)$ and $l_v=\text{len}(k)$. Note that $l_k\le 64$.
  - `get(Key)` - this will, within at most $l_k$ iterations, return the value, or claim that none exists.
  - `put(Key, value)` - within at most $l_k$ iterations, this will reach the memory address for the key and put the value there.
  - `delete(Key)` -  within at most $l_k$ iterations, this will reach the memory address for the key and set its value to `nullptr`.
  - `get(int n)` - within $\log(52)\times\log(n)$ steps it will converge to the correct key, since each trie node maintains count of number of its children, and we will need to binary search over the cumulative sum count for the nodes.
  - `delete(int n)` - same as above

- Each of these methods may also have significant overhead due to having to convert to and from a `Slice` object. **FIGURE THIS OUT?**

## Memory pooling?

Kuch likho

## Btree?

In B+tree, you store the values in random order in some memory area, and then index the keys in a **b**alanced **tree**. So, at each step, you have two separators and three children, and then you go into one of the children depending on which side of the separators you lie at.
What's the problem with this? Why can't we use it?

- It is possible to quickly define the comparator required for the ordering of the key in the indexes?