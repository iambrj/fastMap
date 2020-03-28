struct CompressedTrieNode;

struct BSTNode {

public:
    char c;
    CompressedTrieNode *data;
    BSTNode *left;
    BSTNode *right;

    explicit BSTNode(char c);

    ~BSTNode();
};

class BST {

public:
    BSTNode *root;

    static BSTNode *_insert(BSTNode *cur, char c);

    static BSTNode *_get(BSTNode *cur, char c);

    BST();

    ~BST();

    BST &operator=(const BST& b) {
        root = b.root;
        return *this;
    }

    BSTNode *getOrInsert(char c);

    BSTNode *search(char c);

    BSTNode *getRoot();

    void clear();
};

