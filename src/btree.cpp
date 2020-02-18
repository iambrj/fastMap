#include <iostream>
#include <string.h>

#define MIN(x, y) (y ^ ((x ^ y) & -(x < y)))

using namespace std;

struct Slice {
    uint8_t size;
    char *data;
};

struct Point {
    Slice key;
    Slice val;
};

void printSlice(const Slice &s) {
    for (int i = 0; i < s.size; i++) {
        cout << s.data[i];
    }
    cout << '\n';
}

void printPoint(const Point &p) {
    for (int i = 0; i < p.key.size; i++) {
        cout << p.key.data[i];
    }
    cout << "\n";
    for (int i = 0; i < p.val.size; i++) {
        cout << p.val.data[i];
    }
    cout << "\n--------\n";
}

int strComp(Slice &a, Slice &b) {
    int minn = MIN(a.size, b.size);

    int res = strncmp(a.data, b.data, minn);

    if (res != 0) {
        return res;
    }

    return a.size - b.size;
}

// A BTree node
class BTreeNode {
    Point *points;  // An array of data points
    int t;          // Minimum degree (defines the range for number of keys)
    BTreeNode **C;  // An array of child pointers
    int n;          // Current number of data points
    bool leaf;      // Is true when node is leaf. Otherwise false
    int *endingAtIthChild;

public:
    BTreeNode(int _t, bool _leaf);  // Constructor

    // A function to traverse all nodes in a subtree rooted with this node
    void traverse();

    // A function to search a key in the subtree rooted with this node.
    Point *search(Slice &k);  // returns NULL if k is not present.

    // A function to search for N-th key value pair
    Point *search(int N);  // returns NULL if N > tree size

    // A utility function to insert a new point in the subtree rooted with
    // this node. The assumption is, the node must be non-full when this
    // function is called
    void insertNonFull(Point &p);

    // A utility function to split the child y of this node. i is index of y in
    // child array C[].  The Child y must be full when this function is called
    void splitChild(int i, BTreeNode *y);

    // A function that returns the index of the first key that is greater
    // or equal to k
    int findKey(Slice &k);

    // A wrapper function to remove the key k in subtree rooted with
    // this node.
    bool remove(Slice &k);

    // A function to remove the point present in idx-th position in
    // this node which is a leaf
    void removeFromLeaf(int idx);

    // A function to remove the point present in idx-th position in
    // this node which is a non-leaf node
    void removeFromNonLeaf(int idx);

    // A function to get the predecessor of the key- where the key
    // is present in the idx-th position in the node
    Point getPred(int idx);

    // A function to get the successor of the key- where the key
    // is present in the idx-th position in the node
    Point getSucc(int idx);

    // A function to fill up the child node present in the idx-th
    // position in the C[] array if that child has less than t-1 points
    void fill(int idx);

    // A function to borrow a point from the C[idx-1]-th node and place
    // it in C[idx]th node
    void borrowFromPrev(int idx);

    // A function to borrow a point from the C[idx+1]-th node and place it
    // in C[idx]th node
    void borrowFromNext(int idx);

    // A function to merge idx-th child of the node with (idx+1)th child of
    // the node
    void merge(int idx);

    // Make the BTree friend of this so that we can access private members of
    // this class in BTree functions
    friend class BTree;
};

// A BTree
class BTree {
    BTreeNode *root;  // Pointer to root node
    int t;            // Minimum degree
public:
    // Constructor (Initializes tree as empty)
    BTree(int _t) {
        root = NULL;
        t = _t;
    }

    // function to traverse the tree
    void traverse() {
        if (root != NULL)
            root->traverse();
    }

    // function to search a key in this tree
    Point *search(Slice &k) {
        return (root == NULL) ? NULL : root->search(k);
    }

    // function to search for N-th key value pair
    Point *search(int N) {
        return (root == NULL) ? NULL : root->search(N);
    }

    // The main function that inserts a new point in this B-Tree
    void insert(Point &p);

    // The main function that removes a key from this B-Tree
    bool remove(Slice &k);
};

// Constructor for BTreeNode class
BTreeNode::BTreeNode(int _t, bool _leaf) {
    // Copy the given minimum degree and leaf property
    t = _t;
    leaf = _leaf;

    // Allocate memory for maximum number of possible keys
    // and child pointers
    points = new Point[2 * t - 1];
    endingAtIthChild = new int[2 * t];
    C = new BTreeNode *[2 * t];

    // Initialize the number of data points as 0
    n = 0;
}

// Function to traverse all nodes in a subtree rooted with this node
void BTreeNode::traverse() {
    // There are n points and n+1 children, traverse through n points
    // and first n children
    int i;

    if (!leaf) {
        for (i = 0; i < n; i++) {
            // If this is not leaf, then before printing key[i],
            // traverse the subtree rooted with child C[i].
            C[i]->traverse();
            printPoint(points[i]);
        }
    } else {
        for (i = 0; i < n; i++) {
            printPoint(points[i]);
        }
    }

    // Traverse the subtree rooted with last child
    if (leaf == false)
        C[i]->traverse();
}

// Function to search key k in subtree rooted with this node
Point *BTreeNode::search(Slice &k) {
    // Find the first key greater than or equal to k
    int i = 0;
    while (i < n && strComp(k, points[i].key) > 0)
        i++;

    // If the found key is equal to k, return this point
    if (i < n && strComp(k, points[i].key) == 0)
        return &points[i];

    // If the key is not found here and this is a leaf node
    if (leaf == true)
        return NULL;

    // Go to the appropriate child
    return C[i]->search(k);
}

Point *BTreeNode::search(int N) {
    // Find the first key greater than or equal to k
    // for n data points I have n+1 children
    int i = 0, cnt = 0;

    for (; i <= n; i++) {
        if (cnt + endingAtIthChild[i] < N) {
            cnt += endingAtIthChild[i];
        } else {
            return C[i]->search(N - cnt);
        }

        if (i < n) {
            if (cnt + 1 < N) {
                cnt++;
            } else {
                return &points[i];
            }
        }
    }

    return NULL;
}

// The main function that inserts a new point in this B-Tree
void BTree::insert(Point &p) {
    // If tree is empty
    if (root == NULL) {
        // Allocate memory for root
        root = new BTreeNode(t, true);
        root->points[0] = p;  // Insert point
        root->n = 1;          // Update number of keys in root
    } else {                  // If tree is not empty
        // If root is full, then tree grows in height
        if (root->n == 2 * t - 1) {
            // Allocate memory for new root
            BTreeNode *s = new BTreeNode(t, false);

            // Make old root as child of new root
            s->C[0] = root;

            // Split the old root and move 1 point to the new root
            s->splitChild(0, root);

            // New root has two children now.  Decide which of the
            // two children is going to have new point
            int i = 0;
            if (strComp(s->points[0].key, p.key) < 0)
                i++;
            s->C[i]->insertNonFull(p);

            // Change root
            root = s;
        } else  // If root is not full, call insertNonFull for root
            root->insertNonFull(p);
    }
}

// A utility function to insert a new point in this node
// The assumption is, the node must be non-full when this
// function is called
void BTreeNode::insertNonFull(Point &p) {
    // Initialize index as index of rightmost element
    int i = n - 1;

    // If this is a leaf node
    if (leaf == true) {
        // The following loop does two things
        // a) Finds the location of new point to be inserted
        // b) Moves all greater points to one place ahead
        while (i >= 0 && strComp(points[i].key, p.key) > 0) {
            points[i + 1] = points[i];
            i--;
        }

        // Insert the new point at found location
        points[i + 1] = p;
        n = n + 1;
    } else  // If this node is not leaf
    {
        // Find the child which is going to have the new point
        while (i >= 0 && strComp(points[i].key, p.key) > 0)
            i--;

        // See if the found child is full
        if (C[i + 1]->n == 2 * t - 1) {
            // If the child is full, then split it
            splitChild(i + 1, C[i + 1]);

            // After split, the middle key of C[i] goes up and
            // C[i] is splitted into two.  See which of the two
            // is going to have the new point
            if (strComp(points[i + 1].key, p.key) < 0)
                i++;
        }
        C[i + 1]->insertNonFull(p);
    }
}

// A utility function to split the child y of this node
// Note that y must be full when this function is called
void BTreeNode::splitChild(int i, BTreeNode *y) {
    // Create a new node which is going to store (t-1) points
    // of y
    BTreeNode *z = new BTreeNode(y->t, y->leaf);
    z->n = t - 1;

    // Copy the last (t-1) points of y to z
    for (int j = 0; j < t - 1; j++)
        z->points[j] = y->points[j + t];

    // Copy the last t children of y to z
    if (y->leaf == false) {
        for (int j = 0; j < t; j++)
            z->C[j] = y->C[j + t];
    }

    // Reduce the number of points in y
    y->n = t - 1;

    // Since this node is going to have a new child,
    // create space of new child
    for (int j = n; j >= i + 1; j--)
        C[j + 1] = C[j];

    // Link the new child to this node
    C[i + 1] = z;

    // A point of y will move to this node. Find the location of
    // new point and move all greater points one space ahead
    for (int j = n - 1; j >= i; j--)
        points[j + 1] = points[j];

    // Copy the middle point of y to this node
    points[i] = y->points[t - 1];

    // Increment count of points in this node
    n = n + 1;
}

// A utility function that returns the index of the first key that is
// greater than or equal to k
int BTreeNode::findKey(Slice &k) {
    int idx = 0;
    while (idx < n && strComp(points[idx].key, k) < 0)
        idx++;
    return idx;
}

// A wrapper function to remove the key k from the sub-tree rooted with this
// node
bool BTreeNode::remove(Slice &k) {
    int idx = findKey(k);

    // The key to be removed is present in this node
    if (idx < n && strComp(points[idx].key, k) == 0) {
        // If the node is a leaf node - removeFromLeaf is called
        // Otherwise, removeFromNonLeaf function is called
        if (leaf)
            removeFromLeaf(idx);
        else
            removeFromNonLeaf(idx);
        return true;
    } else {
        // If this node is a leaf node, then the key is not present in tree
        if (leaf) {
            // printSlice(k);
            // cout << "does not exist in the tree\n";
            return false;
        }

        // The key to be removed is present in the sub-tree rooted with this
        // node The flag indicates whether the key is present in the sub-tree
        // rooted with the last child of this node
        bool flag = (idx == n);

        // If the child where the key is supposed to exist has less that t keys,
        // we fill that child
        if (C[idx]->n < t)
            fill(idx);

        // If the last child has been merged, it must have merged with the
        // previous child and so we recurse on the (idx-1)th child. Else, we
        // recurse on the (idx)th child which now has atleast t keys
        if (flag && idx > n)
            return C[idx - 1]->remove(k);
        else
            return C[idx]->remove(k);
    }
}

// A function to remove the idx-th point from this node - which is a leaf node
void BTreeNode::removeFromLeaf(int idx) {
    // Move all the points after the idx-th pos one place backward
    for (int i = idx + 1; i < n; i++)
        points[i - 1] = points[i];

    // Reduce the count of keys
    n--;

    return;
}

// A function to remove the idx-th point from this node - which is a non-leaf
// node
void BTreeNode::removeFromNonLeaf(int idx) {
    Point p = points[idx];

    // If the child that precedes k (C[idx]) has atleast t keys,
    // find the predecessor 'pred' of k in the subtree rooted at
    // C[idx]. Replace p by pred. Recursively delete pred
    // in C[idx]
    if (C[idx]->n >= t) {
        Point pred = getPred(idx);
        points[idx] = pred;
        C[idx]->remove(pred.key);
    }

        // If the child C[idx] has less that t keys, examine C[idx+1].
        // If C[idx+1] has atleast t keys, find the successor 'succ' of k in
        // the subtree rooted at C[idx+1]
        // Replace p by succ
        // Recursively delete succ in C[idx+1]
    else if (C[idx + 1]->n >= t) {
        Point succ = getSucc(idx);
        points[idx] = succ;
        C[idx + 1]->remove(succ.key);
    }

        // If both C[idx] and C[idx+1] has less that t keys, merge p and all of
        // C[idx+1] into C[idx] Now C[idx] contains 2t-1 keys Free C[idx+1] and
        // recursively delete k from C[idx]
    else {
        merge(idx);
        C[idx]->remove(p.key);
    }
    return;
}

// A function to get predecessor of points[idx].key
Point BTreeNode::getPred(int idx) {
    // Keep moving to the right most node until we reach a leaf
    BTreeNode *cur = C[idx];
    while (!cur->leaf)
        cur = cur->C[cur->n];

    // Return the last key of the leaf
    return cur->points[cur->n - 1];
}

Point BTreeNode::getSucc(int idx) {
    // Keep moving the left most node starting from C[idx+1] until we reach a
    // leaf
    BTreeNode *cur = C[idx + 1];
    while (!cur->leaf)
        cur = cur->C[0];

    // Return the first key of the leaf
    return cur->points[0];
}

// A function to fill child C[idx] which has less than t-1 keys
void BTreeNode::fill(int idx) {
    // If the previous child(C[idx-1]) has more than t-1 keys, borrow a key
    // from that child
    if (idx != 0 && C[idx - 1]->n >= t)
        borrowFromPrev(idx);

        // If the next child(C[idx+1]) has more than t-1 keys, borrow a key
        // from that child
    else if (idx != n && C[idx + 1]->n >= t)
        borrowFromNext(idx);

        // Merge C[idx] with its sibling
        // If C[idx] is the last child, merge it with with its previous sibling
        // Otherwise merge it with its next sibling
    else {
        if (idx != n)
            merge(idx);
        else
            merge(idx - 1);
    }
    return;
}

// A function to borrow a point from C[idx-1] and insert it
// into C[idx]
void BTreeNode::borrowFromPrev(int idx) {
    BTreeNode *child = C[idx];
    BTreeNode *sibling = C[idx - 1];

    // The last point from C[idx-1] goes up to the parent and point[idx-1]
    // from parent is inserted as the first point in C[idx]. Thus, the sibling
    // loses one point and child gains one point

    // Moving all key in C[idx] one step ahead
    for (int i = child->n - 1; i >= 0; --i)
        child->points[i + 1] = child->points[i];

    // If C[idx] is not a leaf, move all its child pointers one step ahead
    if (!child->leaf) {
        for (int i = child->n; i >= 0; --i)
            child->C[i + 1] = child->C[i];
    }

    // Setting child's first point equal to points[idx-1] from the current node
    child->points[0] = points[idx - 1];

    // Moving sibling's last child as C[idx]'s first child
    if (!child->leaf)
        child->C[0] = sibling->C[sibling->n];

    // Moving the point from the sibling to the parent
    // This reduces the number of points in the sibling
    points[idx - 1] = sibling->points[sibling->n - 1];

    child->n += 1;
    sibling->n -= 1;

    return;
}

// A function to borrow a point from the C[idx+1] and place
// it in C[idx]
void BTreeNode::borrowFromNext(int idx) {
    BTreeNode *child = C[idx];
    BTreeNode *sibling = C[idx + 1];

    // points[idx] is inserted as the last key in C[idx]
    child->points[(child->n)] = points[idx];

    // Sibling's first child is inserted as the last child
    // into C[idx]
    if (!(child->leaf))
        child->C[(child->n) + 1] = sibling->C[0];

    // The first point from sibling is inserted into points[idx]
    points[idx] = sibling->points[0];

    // Moving all points in sibling one step behind
    for (int i = 1; i < sibling->n; ++i)
        sibling->points[i - 1] = sibling->points[i];

    // Moving the child pointers one step behind
    if (!sibling->leaf) {
        for (int i = 1; i <= sibling->n; ++i)
            sibling->C[i - 1] = sibling->C[i];
    }

    // Increasing and decreasing the point count of C[idx] and C[idx+1]
    // respectively
    child->n += 1;
    sibling->n -= 1;

    return;
}

// A function to merge C[idx] with C[idx+1]
// C[idx+1] is freed after merging
void BTreeNode::merge(int idx) {
    BTreeNode *child = C[idx];
    BTreeNode *sibling = C[idx + 1];

    // Pulling a point from the current node and inserting it into (t-1)th
    // position of C[idx]
    child->points[t - 1] = points[idx];

    // Copying the points from C[idx+1] to C[idx] at the end
    for (int i = 0; i < sibling->n; ++i)
        child->points[i + t] = sibling->points[i];

    // Copying the child pointers from C[idx+1] to C[idx]
    if (!child->leaf) {
        for (int i = 0; i <= sibling->n; ++i)
            child->C[i + t] = sibling->C[i];
    }

    // Moving all points after idx in the current node one step before -
    // to fill the gap created by moving points[idx] to C[idx]
    for (int i = idx + 1; i < n; ++i)
        points[i - 1] = points[i];

    // Moving the child pointers after (idx+1) in the current node one
    // step before
    for (int i = idx + 2; i <= n; ++i)
        C[i - 1] = C[i];

    // Updating the key count of child and the current node
    child->n += sibling->n + 1;
    n--;

    // Freeing the memory occupied by sibling
    delete (sibling);
    return;
}

bool BTree::remove(Slice &k) {
    if (!root) {
        // cout << "The tree is empty\n";
        return false;
    }

    // Call the remove function for root
    bool ret = root->remove(k);

    // If the root node has 0 keys, make its first child as the new root
    //  if it has a child, otherwise set root as NULL
    if (root->n == 0) {
        BTreeNode *tmp = root;
        if (root->leaf)
            root = NULL;
        else
            root = root->C[0];

        // Free the old root
        delete tmp;
    }
    return ret;
}

class kvStore {
    BTree *tree;

public:
    kvStore(uint64_t max_entries) {
        tree = new BTree(10);
    }

    bool get(Slice &key, Slice &value) {
        Point *p = tree->search(key);
        if (p == NULL) {
            return false;
        }
        value = p->val;
        return true;
    }

    bool put(Slice &key, Slice &value) {
        bool rem = tree->remove(key);
        Point p;
        p.key = key;
        p.val = value;
        tree->insert(p);

        return rem;
    }

    bool del(Slice &key) {
        return tree->remove(key);
    }

    bool get(int N, Slice &key, Slice &value) {
        Point *p = tree->search(N);
        if (p == NULL) {
            return false;
        }

        key = p->key;
        value = p->val;
        return true;
    }

    bool del(int N) {
        return tree->removeNth(N);
    }
};

// int main(void) {
//     kvStore map(1000);
//     Slice key, val;
//     key.size = 78;
//     key.data = (char*)malloc(sizeof(char) * key.size);
//     strncpy(key.data, "BkSvXGxGeYTxJxJafLm
//     BQocDEJXBcleoDZnlyuPzNMImWKRhxGKNXnsDwQgBBLphMdSLXiKMuuaQF", 78);
//     val.size = 78;
//     val.data = (char*)malloc(sizeof(char) * val.size);
//     strncpy(val.data, "BkSvXGxGeYTxJxJafLm
//     BQocDEJXBcleoDZnlyuPzNMImWKRhxGKNXnsDwQgBBLphMdSLXiKMuuaQF", 78);
//     map.put(key, val);
//     Slice test;

//     map.get(key, test);
//     printSlice(test);
// }
