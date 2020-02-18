#include <iostream>
#include <cstring>
using namespace std;

struct Slice {
    uint8_t size;
    char *data;
};

bool operator==(const Slice &a, const Slice &b) {
    return b.size == a.size && strcmp(a.data, b.data) == 0;
}

bool operator<(const Slice &a, const Slice &b) {
    return strcmp(a.data, b.data);
}

bool operator>(const Slice &a, const Slice &b) {
    return strcmp(b.data, a.data);
}

enum Color {RED, BLACK}; 
  
struct Node 
{ 
    Slice data; 
    Slice store;
    bool color; 
    Node *left, *right, *parent; 
  
    Node(Slice data, Slice store) 
    { 
       this->data = data; 
       this->store = store;
       left = right = parent = NULL; 
       this->color = RED; 
    } 
};
  
Node* root = NULL;

void rotateLeft(Node *&, Node *&); 
void rotateRight(Node *&, Node *&); 
void fixViolation(Node *&, Node *&); 

void insert(const int &n); 
void inorder(); 
void levelOrder(); 
  
void inorderHelper(Node *root) 
{ 
    if (root == NULL) 
        return; 
  
    inorderHelper(root->left); 
    cout << root->data.data << "  "; 
    inorderHelper(root->right); 
} 
  
Node* BSTInsert(Node* root, Node *pt) 
{ 
    if (root == NULL) 
       return pt; 
  
    if (pt->data < root->data) 
    { 
        root->left  = BSTInsert(root->left, pt); 
        root->left->parent = root; 
    } 
    else if (pt->data > root->data) 
    { 
        root->right = BSTInsert(root->right, pt); 
        root->right->parent = root; 
    } 
  
    return root; 
} 
  
struct qnode{
    Node * val;
    qnode * next;
};

qnode * qhead;
qnode * qtail;

void levelOrderHelper(Node *root) 
{ 
    if (root == NULL) 
        return; 
  
    qhead = (qnode*)malloc(sizeof(qnode));
    qtail = qhead;
    qhead->val = root;
    qhead->next = NULL;

    while (qhead->next)
    { 
        Node *temp = qhead->val; 
        cout << temp->data.data << "  "; 
        
        qnode* tmp = qhead;
        qhead = qhead->next;
        free(tmp);
  
        if (temp->left != NULL) {
            qnode* newnode;
            newnode = (qnode*)malloc(sizeof(qnode));
            newnode->val = temp->left;
            newnode->next = NULL;
            qtail->next = newnode;
            qtail = newnode;
        }
  
        if (temp->right != NULL) {
            qnode* newnode;
            newnode = (qnode*)malloc(sizeof(qnode));
            newnode->val = temp->right;
            newnode->next = NULL;
            qtail->next = newnode;
            qtail = newnode;
        }
    } 
} 
  
void rotateLeft(Node *&root, Node *&pt) 
{ 
    Node *pt_right = pt->right; 
  
    pt->right = pt_right->left; 
  
    if (pt->right != NULL) 
        pt->right->parent = pt; 
  
    pt_right->parent = pt->parent; 
  
    if (pt->parent == NULL) 
        root = pt_right; 
  
    else if (pt == pt->parent->left) 
        pt->parent->left = pt_right; 
  
    else
        pt->parent->right = pt_right; 
  
    pt_right->left = pt; 
    pt->parent = pt_right; 
} 
  
void rotateRight(Node *&root, Node *&pt) 
{ 
    Node *pt_left = pt->left; 
  
    pt->left = pt_left->right; 
  
    if (pt->left != NULL) 
        pt->left->parent = pt; 
  
    pt_left->parent = pt->parent; 
  
    if (pt->parent == NULL) 
        root = pt_left; 
  
    else if (pt == pt->parent->left) 
        pt->parent->left = pt_left; 
  
    else
        pt->parent->right = pt_left; 
  
    pt_left->right = pt; 
    pt->parent = pt_left; 
} 
  
// This function fixes violations caused by BST insertion 
void fixViolation(Node *&root, Node *&pt) 
{ 
    Node *parent_pt = NULL; 
    Node *grand_parent_pt = NULL; 
  
    while ((pt != root) && (pt->color != BLACK) && 
           (pt->parent->color == RED)) 
    { 
  
        parent_pt = pt->parent; 
        grand_parent_pt = pt->parent->parent; 
  
        /*  Case : A 
            Parent of pt is left child of Grand-parent of pt */
        if (parent_pt == grand_parent_pt->left) 
        { 
  
            Node *uncle_pt = grand_parent_pt->right; 
  
            /* Case : 1 
               The uncle of pt is also red 
               Only Recoloring required */
            if (uncle_pt != NULL && uncle_pt->color == RED) 
            { 
                grand_parent_pt->color = RED; 
                parent_pt->color = BLACK; 
                uncle_pt->color = BLACK; 
                pt = grand_parent_pt; 
            } 
  
            else
            { 
                /* Case : 2 
                   pt is right child of its parent 
                   Left-rotation required */
                if (pt == parent_pt->right) 
                { 
                    rotateLeft(root, parent_pt); 
                    pt = parent_pt; 
                    parent_pt = pt->parent; 
                } 
  
                /* Case : 3 
                   pt is left child of its parent 
                   Right-rotation required */
                rotateRight(root, grand_parent_pt); 
                swap(parent_pt->color, grand_parent_pt->color); 
                pt = parent_pt; 
            } 
        } 
  
        /* Case : B 
           Parent of pt is right child of Grand-parent of pt */
        else
        { 
            Node *uncle_pt = grand_parent_pt->left; 
  
            /*  Case : 1 
                The uncle of pt is also red 
                Only Recoloring required */
            if ((uncle_pt != NULL) && (uncle_pt->color == RED)) 
            { 
                grand_parent_pt->color = RED; 
                parent_pt->color = BLACK; 
                uncle_pt->color = BLACK; 
                pt = grand_parent_pt; 
            } 
            else
            { 
                /* Case : 2 
                   pt is left child of its parent 
                   Right-rotation required */
                if (pt == parent_pt->left) 
                { 
                    rotateRight(root, parent_pt); 
                    pt = parent_pt; 
                    parent_pt = pt->parent; 
                } 
  
                /* Case : 3 
                   pt is right child of its parent 
                   Left-rotation required */
                rotateLeft(root, grand_parent_pt); 
                swap(parent_pt->color, grand_parent_pt->color); 
                pt = parent_pt; 
            } 
        } 
    } 
  
    root->color = BLACK; 
} 
  
// Function to insert a new node with given data 
void insert(const Slice &data, const Slice &store) 
{ 
    Node *pt = new Node(data, store);
  
    // Do a normal BST insert 
    root = BSTInsert(root, pt); 
  
    // fix Red Black Tree violations 
    fixViolation(root, pt); 
} 
  
// Function to do inorder and level order traversals 
void inorder()     {  inorderHelper(root);} 
void levelOrder()  {  levelOrderHelper(root); } 

int main(){
    Slice A;
    A.size = 5;
    strcpy(A.data, "hello");
    insert(A, A);
    inorder();
}
