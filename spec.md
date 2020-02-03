# Spec for SPoP

## Memory requirements analysis

- Our key size is only 64 bytes, where each distinct letter in the key matches `[a-zA-Z]`. Each of these 52 possibilities take 6 bits to represent. Thus, worst case memory usage is 10 million keys times 52 bits per key = $10^7 \times 52= 65 \times 10^6$ bytes, which is approximately $65$ MB for storing the keys.

- Each value can have 256 characters, as one ASCII character takes one byte (0-255 range), and max value size is 256 bytes. Thus, total memory usage is around $10^7\times 256=2.56\times10^9$ bytes $=2.56$ giga bytes. ISN'T THIS HUGE?

## Trie based approach

- A trie is a data structure that keeps pointers to the its children, and each transition to a child represents one letter being added to the currently processed string. 

- 

- 