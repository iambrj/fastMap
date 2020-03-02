# Where are we allocating memory?

Precisely on

- `sizeof(TrieNode)` = `new TrieNode()` (twice)
- `(TrieNode **)calloc(sizeof(TrieNode *), RANGE)` for the pointers array
- `65` - when returning the key value for `lookup`

Always of these fixed sizes. Here's a [nice blog post](https://thinkingeek.com/2017/11/19/simple-memory-pool/) for memory poolers.