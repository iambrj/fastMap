#include <iostream>
#include <stdlib.h>
#include <string.h>
using namespace std;
#define MIN(x, y) (y ^ ((x ^ y) & -(x < y)))

#define RED 1
#define BLACK 2

struct Slice {
    uint8_t size;
    char* data;
};

struct node {
    Slice key;
    Slice val;
    node *left, *right, *p;
    int color;
    int size;
};

node NIL;
node* NILPTR = &NIL;

void printSlice(const Slice& s) {
    for (int i = 0; i < s.size; i++) {
        cout << s.data[i];
    }
    cout << '\n';
}

int strComp(Slice* a, Slice* b) {
    // because the strings may not have '\0' at the end
    // if (a.size == b.size) { // not needed
    //     return strncmp(a.data, b.data, a.size);
    // }
    int minn = MIN(a->size, b->size);
    int res = strncmp(a->data, b->data, minn);
    if (res != 0) {
        return res;
    }

    return a->size - b->size;
}

node* search(node* root, Slice* k) {
    if (root == NILPTR)
        return root;

    int res = strComp(k, &(root->key));

    if (res == 0)
        return root;
    else if (res < 0)
        return search(root->left, k);
    else
        return search(root->right, k);
}

node* minimum(node* root) {
    while (root->left != NILPTR)
        root = root->left;
    return root;
}

node* maximum(node* root) {
    while (root->right != NILPTR)
        root = root->right;
    return root;
}

void leftrotate(node** treeroot, node* x) {
    node* y = x->right;
    x->right = y->left;
    if (y->left != NILPTR)
        y->left->p = x;
    y->p = x->p;
    if (x->p == NILPTR)
        *treeroot = y;
    else if (x->p->left == x)
        x->p->left = y;
    else
        x->p->right = y;
    y->left = x;
    x->p = y;
    /* Changes */
    y->size = x->size;
    x->size = x->left->size + x->right->size + 1;
    /* Changes End */
}

void rightrotate(node** treeroot, node* y) {
    node* x = y->left;
    y->left = x->right;
    if (x->right != NILPTR)
        x->right->p = y;
    x->p = y->p;
    if (y->p == NILPTR)
        *treeroot = x;
    else if (y->p->left == y)
        y->p->left = x;
    else
        y->p->right = x;
    x->right = y;
    y->p = x;
    /* Changes */
    x->size = y->size;
    y->size = y->left->size + y->right->size + 1;
    /* Changes End */
}

void rbinsertfixup(node** treeroot, node* z) {
    while (z->p->color == RED) {
        if (z->p == z->p->p->left) {
            node* y = z->p->p->right;
            if (y->color == RED) {
                z->p->color = BLACK;
                y->color = BLACK;
                z->p->p->color = RED;
                z = z->p->p;
            } else {
                if (z == z->p->right) {
                    z = z->p;
                    leftrotate(treeroot, z);
                }
                z->p->color = BLACK;
                z->p->p->color = RED;
                rightrotate(treeroot, z->p->p);
            }
        } else {
            node* y = z->p->p->left;
            if (y->color == RED) {
                z->p->color = BLACK;
                y->color = BLACK;
                z->p->p->color = RED;
                z = z->p->p;
            } else {
                if (z == z->p->left) {
                    z = z->p;
                    rightrotate(treeroot, z);
                }
                z->p->color = BLACK;
                z->p->p->color = RED;
                leftrotate(treeroot, z->p->p);
            }
        }
    }
    (*treeroot)->color = BLACK;
}

void rbinsert(node** treeroot, Slice* key, Slice* val) {
    node* Z = (node*)malloc(sizeof(node));
    Z->key.size = key->size;
    Z->key.data = key->data;
    Z->val.size = val->size;
    Z->val.data = val->data;

    node* y = NILPTR;
    node* x = *treeroot;
    while (x != NILPTR) {
        y = x;
        /* Changes */
        y->size++;
        /* Changes End */
        int res = strComp(&(Z->key), &(x->key));
        if (res < 0)
            x = x->left;
        else if (res > 0)
            x = x->right;
        else {
            /* if the value to be inserted is already in the tree, undo the
             * changes */
            /* Changes */
            node* temp = x;
            while (x != *treeroot) {
                x->size--;
                x = x->p;
            }
            x->size--;
            free(Z);
            return;
            /* Changes End */
        }
    }
    Z->p = y;
    if (y == NILPTR)
        *treeroot = Z;
    else if (strComp(&(Z->key), &(y->key)) < 0)
        y->left = Z;
    else
        y->right = Z;

    Z->left = NILPTR;
    Z->right = NILPTR;
    Z->color = RED;
    Z->size = 1;
    rbinsertfixup(treeroot, Z);
}

void rbtransplant(node** treeroot, node* u, node* v) {
    if (u->p == NILPTR)
        *treeroot = v;
    else if (u == u->p->left)
        u->p->left = v;
    else
        u->p->right = v;
    v->p = u->p;
}

void rbdeletefixup(node** treeroot, node* x) {
    while (x != *treeroot && x->color == BLACK) {
        if (x == x->p->left) {
            node* w = x->p->right;
            if (w->color == RED) {
                w->color = BLACK;
                x->p->color = RED;
                leftrotate(treeroot, x->p);
                w = x->p->right;
            }
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->p;
            } else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    rightrotate(treeroot, w);
                    w = x->p->right;
                }
                w->color = x->p->color;
                x->p->color = BLACK;
                w->right->color = BLACK;
                leftrotate(treeroot, x->p);
                x = *treeroot;
            }
        } else {
            node* w = x->p->left;
            if (w->color == RED) {
                w->color = BLACK;
                x->p->color = RED;
                rightrotate(treeroot, x->p);
                w = x->p->left;
            }
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->p;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    leftrotate(treeroot, w);
                    w = x->p->left;
                }
                w->color = x->p->color;
                x->p->color = BLACK;
                w->left->color = BLACK;
                rightrotate(treeroot, x->p);
                x = *treeroot;
            }
        }
    }
    x->color = BLACK;
}

void rbdelete(node** treeroot, Slice* key) {
    node* Z = search(*treeroot, key);

    if (Z == NILPTR) {
        return;
    }
    /* Phase1: Changes */
    if (Z != *treeroot) {
        node* tmp = Z->p;
        while (tmp != *treeroot) {
            tmp->size--;
            tmp = tmp->p;
        }
        (*treeroot)->size--;
    }
    /* Changes End */
    node* y = Z;
    int yoc = y->color;
    node* x;
    if (Z->left == NILPTR) {
        x = Z->right;
        rbtransplant(treeroot, Z, Z->right);
    } else if (Z->right == NILPTR) {
        x = Z->left;
        rbtransplant(treeroot, Z, Z->left);
    } else {
        y = minimum(Z->right);
        yoc = y->color;
        x = y->right;
        if (y->p == Z) {
            x->p = y;
        } else {
            rbtransplant(treeroot, y, y->right);
            y->right = Z->right;
            y->right->p = y;
            /* Phase2: Changes */
            node* tmp = x->p;
            while (tmp != y) {
                tmp->size--;
                tmp = tmp->p;
            }
            y->size = y->left->size + 1;
            /* Changes End */
        }
        rbtransplant(treeroot, Z, y);
        y->left = Z->left;
        y->left->p = y;
        y->color = Z->color;
        /* Phase2: Changes */
        y->size = y->left->size + y->right->size + 1;
        /* Changes End */
    }
    if (yoc == BLACK)
        rbdeletefixup(treeroot, x);
}

node* kth(node* treeroot, int K) {
    int currrank = treeroot->left->size + 1;
    node* y = treeroot;
    while (y != NILPTR && currrank != K) {
        if (K < currrank)
            y = y->left;
        else {
            K = K - currrank;
            y = y->right;
        }
        if (y == NILPTR)
            return NILPTR;
        currrank = y->left->size + 1;
    }
    return y;
}

int cnt(node* treeroot, Slice* key) {
    int ans = 0;
    node* y = treeroot;
    while (y != NILPTR) {
        int res = strComp(&(y->key), key);
        if (res > 0)
            y = y->left;
        else if (res < 0) {
            ans += y->left->size + 1;
            y = y->right;
        } else
            return ans + y->left->size;
    }
    return ans;
}

class kvStore {
    node* tree;

   public:
    kvStore(uint64_t max_entries) {
        NIL.left = NIL.right = NIL.p = NILPTR;
        NIL.color = BLACK;
        NIL.size = 0;
        tree = NILPTR;
    }

    bool get(Slice& key, Slice& value) {
        node* temp = search(tree, &key);
        if (temp == NILPTR) {
            return false;
        }
        value.size = temp->val.size;
        value.data = temp->val.data;
        return true;
    }
    bool del(Slice& key) {
        // searching because can't understand delete
        // also free memory in delete
        node* temp = search(tree, &key);
        if (temp == NILPTR) {
            return false;
        }
        rbdelete(&tree, &key);
        return true;
    }
    bool put(Slice& key, Slice& value) {
        bool res = del(key);
        rbinsert(&tree, &key, &value);
        return res;
    }
    bool get(int N, Slice& key, Slice& value) {
        node* temp = kth(tree, N);
        if (temp == NILPTR) {
            return false;
        }
        key.size = temp->key.size;
        key.data = temp->key.data;
        value.size = temp->val.size;
        value.data = temp->val.data;

        return true;
    }
    bool del(int N) {
        node* temp = kth(tree, N);
        if (temp == NILPTR) {
            return false;
        }

        rbdelete(&tree, &(temp->key));
        return true;
    }
};