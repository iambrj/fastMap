#include <iostream>
#include <cstring>
using namespace std;

enum COLOR { RED, BLACK };

struct Slice {
    uint8_t size;
    char *data;
};

bool operator==(const Slice &a, const Slice &b) {
    return b.size == a.size && strcmp(a.data, b.data) == 0;
}

bool operator<(const Slice &a, const Slice &b) {
    return strcmp(a.data, b.data) < 0;
}

bool operator>(const Slice &a, const Slice &b) {
    return strcmp(a.data, b.data) > 0;
}

struct Node {
    Slice key;
    Slice data;
    COLOR color;
    Node *left, *right, *parent;

    Node(Slice key, Slice data) : key(key), data(data) {
        parent = left = right = NULL;
        color = RED;
    }

    Node *uncle() {
        if (parent == NULL or parent->parent == NULL)
            return NULL;

        if (parent->isOnLeft())
            return parent->parent->right;
        else
            return parent->parent->left;
    }

    bool isOnLeft() {
        return this == parent->left;
    }

    Node *sibling() {
        if (parent == NULL)
            return NULL;

        if (isOnLeft())
            return parent->right;

        return parent->left;
    }

    void moveDown(Node *nParent) {
        if (parent != NULL) {
            if (isOnLeft()) {
                parent->left = nParent;
            } else {
                parent->right = nParent;
            }
        }
        nParent->parent = parent;
        parent = nParent;
    }

    bool hasRedChild() {
        return (left != NULL and left->color == RED) or
               (right != NULL and right->color == RED);
    }
};

class RBTree {
    Node *root;

    void leftRotate(Node *x) {
        Node *nParent = x->right;

        if (x == root)
            root = nParent;

        x->moveDown(nParent);

        x->right = nParent->left;

        if (nParent->left != NULL)
            nParent->left->parent = x;

        nParent->left = x;
    }

    void rightRotate(Node *x) {
        Node *nParent = x->left;

        if (x == root)
            root = nParent;

        x->moveDown(nParent);

        x->left = nParent->right;

        if (nParent->right != NULL)
            nParent->right->parent = x;

        nParent->right = x;
    }

    void swapColors(Node *x1, Node *x2) {
        COLOR temp;
        temp = x1->color;
        x1->color = x2->color;
        x2->color = temp;
    }

    void swapkeyues(Node *u, Node *v) {
        Slice temp;
        temp = u->key;
        u->key = v->key;
        v->key = temp;
    }

    void fixRedRed(Node *x) {
        if (x == root) {
            x->color = BLACK;
            return;
        }

        Node *parent = x->parent, *grandparent = parent->parent,
             *uncle = x->uncle();

        if (parent->color != BLACK) {
            if (uncle != NULL && uncle->color == RED) {
                parent->color = BLACK;
                uncle->color = BLACK;
                grandparent->color = RED;
                fixRedRed(grandparent);
            } else {
                if (parent->isOnLeft()) {
                    if (x->isOnLeft()) {
                        swapColors(parent, grandparent);
                    } else {
                        leftRotate(parent);
                        swapColors(x, grandparent);
                    }
                    rightRotate(grandparent);
                } else {
                    if (x->isOnLeft()) {
                        rightRotate(parent);
                        swapColors(x, grandparent);
                    } else {
                        swapColors(parent, grandparent);
                    }

                    leftRotate(grandparent);
                }
            }
        }
    }

    Node *successor(Node *x) {
        Node *temp = x;

        while (temp->left != NULL)
            temp = temp->left;

        return temp;
    }

    Node *BSTreplace(Node *x) {
        if (x->left != NULL and x->right != NULL)
            return successor(x->right);

        if (x->left == NULL and x->right == NULL)
            return NULL;

        if (x->left != NULL)
            return x->left;
        else
            return x->right;
    }

    void deleteNode(Node *v) {
        Node *u = BSTreplace(v);

        bool uvBlack =
            ((u == NULL or u->color == BLACK) and (v->color == BLACK));
        Node *parent = v->parent;

        if (u == NULL) {
            if (v == root) {
                root = NULL;
            } else {
                if (uvBlack) {
                    fixDoubleBlack(v);
                } else {
                    if (v->sibling() != NULL)
                        v->sibling()->color = RED;
                }

                if (v->isOnLeft()) {
                    parent->left = NULL;
                } else {
                    parent->right = NULL;
                }
            }
            delete v;
            return;
        }

        if (v->left == NULL or v->right == NULL) {
            if (v == root) {
                v->key = u->key;
                v->left = v->right = NULL;
                delete u;
            } else {
                if (v->isOnLeft()) {
                    parent->left = u;
                } else {
                    parent->right = u;
                }
                delete v;
                u->parent = parent;
                if (uvBlack) {
                    fixDoubleBlack(u);
                } else {
                    u->color = BLACK;
                }
            }
            return;
        }

        swapkeyues(u, v);
        deleteNode(u);
    }

    void fixDoubleBlack(Node *x) {
        if (x == root)
            return;

        Node *sibling = x->sibling(), *parent = x->parent;
        if (sibling == NULL) {
            fixDoubleBlack(parent);
        } else {
            if (sibling->color == RED) {
                parent->color = RED;
                sibling->color = BLACK;
                if (sibling->isOnLeft()) {
                    rightRotate(parent);
                } else {
                    leftRotate(parent);
                }
                fixDoubleBlack(x);
            } else {
                if (sibling->hasRedChild()) {
                    if (sibling->left != NULL and sibling->left->color == RED) {
                        if (sibling->isOnLeft()) {
                            sibling->left->color = sibling->color;
                            sibling->color = parent->color;
                            rightRotate(parent);
                        } else {
                            sibling->left->color = parent->color;
                            rightRotate(sibling);
                            leftRotate(parent);
                        }
                    } else {
                        if (sibling->isOnLeft()) {
                            sibling->right->color = parent->color;
                            leftRotate(sibling);
                            rightRotate(parent);
                        } else {
                            sibling->right->color = sibling->color;
                            sibling->color = parent->color;
                            leftRotate(parent);
                        }
                    }
                    parent->color = BLACK;
                } else {
                    sibling->color = RED;
                    if (parent->color == BLACK)
                        fixDoubleBlack(parent);
                    else
                        parent->color = BLACK;
                }
            }
        }
    }

    void inorder(Node *x) {
        if (x == NULL)
            return;
        inorder(x->left);
        cout << x->key.data << "-" << x->data.data << " ";
        inorder(x->right);
    }

   public:
    RBTree() {
        root = NULL;
    }

    Node *getRoot() {
        return root;
    }

    Node *search(Slice &key) {
        Node *temp = root;
        while (temp != NULL) {
            if (key < temp->key) {
                if (temp->left == NULL)
                    break;
                else
                    temp = temp->left;
            } else if (key == temp->key) {
                break;
            } else {
                if (temp->right == NULL)
                    break;
                else
                    temp = temp->right;
            }
        }
        return temp;
    }

    // search for the required benchmark interface
    bool get(Slice &key, Slice &value) {
        Node *temp = root;
        while (temp != NULL) {
            if (key < temp->key) {
                if (temp->left == NULL)
                    break;
                else
                    temp = temp->left;
            } else if (key == temp->key) {
                break;
            } else {
                if (temp->right == NULL)
                    break;
                else
                    temp = temp->right;
            }
        }
        if (key == temp->key) {
            value = temp->data;
            return true;
        } else {
            return false;
        }
    }

    bool put(Slice &A, Slice &B) {
        Node *newNode = new Node(A, B);
        if (root == NULL) {
            newNode->color = BLACK;
            root = newNode;
        } else {
            Node *temp = search(A);

            if (temp->key == A) {
                return false;
            }

            newNode->parent = temp;

            if (A < temp->key)
                temp->left = newNode;
            else
                temp->right = newNode;

            fixRedRed(newNode);
        }
        return true;
    }

    bool del(Slice key) {
        if (root == NULL)
            return false;

        Node *v = search(key);

        if (v->key == key)
            ;
        else {
            return false;
        }

        deleteNode(v);
        return true;
    }

    void printInOrder() {
        cout << "Inorder: " << endl;
        if (root == NULL)
            cout << "Tree is empty" << endl;
        else
            inorder(root);
        cout << endl;
    }
};

class kvStore {
   public:
    RBTree tree;
    kvStore(uint64_t max_entries) {
    }
    bool get(Slice &key, Slice &value) {
        return tree.get(key, value);
    }
    bool put(Slice &key, Slice &value) {
        return tree.put(key, value);
    }
    bool del(Slice &key) {
        return tree.del(key);
    }
    bool get(int N, Slice &key, Slice &value);
    bool del(int N, Slice &key, Slice &value);
};

int main() {
    kvStore K(1000);

    Slice arr[11] = {
        {1, "a"}, {1, "b"}, {1, "c"}, {1, "d"}, {1, "e"}, {1, "f"},
        {1, "g"}, {1, "h"}, {1, "i"}, {1, "j"}, {1, "k"},
    };

    K.put(arr[0], arr[1]);
    K.put(arr[1], arr[2]);
    K.put(arr[2], arr[3]);
    K.put(arr[10], arr[0]);
    K.put(arr[7], arr[8]);
    cout << K.tree.search(arr[1])->data.data << endl;

    K.tree.printInOrder();

    K.tree.del(arr[10]);

    K.tree.printInOrder();

    cout << "exited" << endl;
    return 0;
}
