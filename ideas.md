# Thinking out loud

Core: a compressed trie with lazy updates.

Rejected:

## Btree

In B+tree, you store the values in random order in some memory area, and then index the keys in a **b**alanced **tree**. So, at each step, you have two separators and three children, and then you go into one of the children depending on which side of the separators you lie at.

problems:

1. It is not possible for the comparator to quickly compare keys (which are themselves strings)?
2. higher space complexity at each particular node (storing the entire string)
3. also a redundant cost of strcmp at every step.

## **FST**

1. Too tough implementation
2.  also we are not sure where we would be able to store the value if the node structure is something like NODE <=> NODE <=> NODE (total four strings).

## **FST with hashmap**

1. This resolves the above issue, since we can use a separate unordered map to associate keys to values, but it's quite pointless because you will have to store everything double in space $\approx 6.5$ GB of memory requirement.