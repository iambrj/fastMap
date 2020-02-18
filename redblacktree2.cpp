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
  
  bool isOnLeft() { return this == parent->left; } 

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
  
    bool uvBlack = ((u == NULL or u->color == BLACK) and (v->color == BLACK)); 
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
  RBTree() { root = NULL; } 
  
  Node *getRoot() { return root; } 
  
  Node *search(Slice n) { 
    Node *temp = root; 
    while (temp != NULL) { 
      if (n < temp->key) { 
        if (temp->left == NULL) 
          break; 
        else
          temp = temp->left; 
      } else if (n == temp->key) { 
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
  
  void insert(const Slice &A, const Slice &B) { 
    Node *newNode = new Node(A, B); 
    if (root == NULL) { 
      newNode->color = BLACK; 
      root = newNode; 
    } else { 
      Node *temp = search(A); 
  
      if (temp->key == A) { 
        return; 
      } 
  
      newNode->parent = temp; 
  
      if (A < temp->key) 
        temp->left = newNode; 
      else
        temp->right = newNode; 
  
      fixRedRed(newNode); 
    } 
  } 
  
  void deleteBykey(Slice n) { 
    if (root == NULL) 
      return; 
  
    Node *v = search(n); 
  
    if (v->key == n);
    else{
      cout << "No node found to delete with keyue: " << n.data << endl; 
      return; 
    } 
  
    deleteNode(v); 
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
  
int main() { 
  RBTree tree; 
  
  Slice arr[11] = {
    {1, "a"},
    {2, "b"},
    {5, "c"},
    {7, "d"},
    {7, "e"},
    {15, "f"},
    {18, "g"},
    {11, "h"},
    {1, "i"},
    {8, "j"},
    {4, "k"},
  };

  tree.insert(arr[0], arr[1]); 
  tree.insert(arr[1], arr[2]); 
  tree.insert(arr[2], arr[3]); 
  tree.insert(arr[10], arr[0]); 
  tree.insert(arr[7], arr[8]); 
  cout << tree.search(arr[1])->data.data << endl;
  cout << "hello" << endl;
  /* tree.insert(8); */ 
  /* tree.insert(11); */ 
  /* tree.insert(26); */ 
  /* tree.insert(2); */ 
  /* tree.insert(6); */ 
  /* tree.insert(13); */ 
  
  tree.printInOrder(); 
  
  cout<<endl<<"Deleting 18, 11, 3, 10, 22"<<endl; 
  
  /* tree.deleteBykey(18); */ 
  /* tree.deleteBykey(11); */ 
  /* tree.deleteBykey(3); */ 
  /* tree.deleteBykey(10); */ 
  /* tree.deleteBykey(22); */ 
  
  tree.printInOrder(); 
  return 0; 
} 
