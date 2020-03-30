# fastMap

- C++ implementation of a fast, _in-memory_ key-value store
- supports get, put, delete; both by value (`get("foo")`) and by alphabetic index (`get(1)`)
- works for arbitrary-length strings keys and values (matching `[a-zA-Z]+`), as many as your RAM can fit in.
- **stores ten million entries** (max key length=64, max value length=256) in _less than 25 seconds_ (on a medium-end CPU)
- supports multiple thread calls
- well structured, modular code based on **compressed tries** and **binary search trees**

[Link to detailed implementation spec](https://docs.google.com/document/d/1YPywCODZPhzKSr9QRMuAZ-Gxe3JDBI5e4JqsmdUXx28/edit?usp=sharing)

### Scope for improvement:

PRs welcome!

- asynchronous non-blocking `get` calls
