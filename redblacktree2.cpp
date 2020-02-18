#include <iostream> 
using namespace std; 
  
enum COLOR { RED, BLACK }; 

struct Slice {
    uint8_t size;
    char *data;
};

struct Node { 
  Slice key;
  Slice data;
  COLOR color; 
  Node *left, *right, *parent; 
  
  Node(Slice key, Slice data) : key(key), data(data) { 
    parent = left = right = NULL; 
    color = RED; 
  } 
  
  // returns pointer to uncle 
  Node *uncle() { 
    // If no parent or grandparent, then no uncle 
    if (parent == NULL or parent->parent == NULL) 
      return NULL; 
  
    if (parent->isOnLeft()) 
      // uncle on right 
      return parent->parent->right; 
    else
      // uncle on left 
      return parent->parent->left; 
  } 
  
  // check if node is left child of parent 
  bool isOnLeft() { return this == parent->left; } 
  
  // returns pointer to sibling 
  Node *sibling() { 
    // sibling null if no parent 
    if (parent == NULL) 
      return NULL; 
  
    if (isOnLeft()) 
      return parent->right; 
  
    return parent->left; 
  } 
  
  // moves node down and moves given node in its place 
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
  
  // left rotates the given node 
  void leftRotate(Node *x) { 
    // new parent will be node's right child 
    Node *nParent = x->right; 
  
    // update root if current node is root 
    if (x == root) 
      root = nParent; 
  
    x->moveDown(nParent); 
  
    // connect x with new parent's left element 
    x->right = nParent->left; 
    // connect new parent's left element with node 
    // if it is not null 
    if (nParent->left != NULL) 
      nParent->left->parent = x; 
  
    // connect new parent with x 
    nParent->left = x; 
  } 
  
  void rightRotate(Node *x) { 
    // new parent will be node's left child 
    Node *nParent = x->left; 
  
    // update root if current node is root 
    if (x == root) 
      root = nParent; 
  
    x->moveDown(nParent); 
  
    // connect x with new parent's right element 
    x->left = nParent->right; 
    // connect new parent's right element with node 
    // if it is not null 
    if (nParent->right != NULL) 
      nParent->right->parent = x; 
  
    // connect new parent with x 
    nParent->right = x; 
  } 
  
  void swapColors(Node *x1, Node *x2) { 
    COLOR temp; 
    temp = x1->color; 
    x1->color = x2->color; 
    x2->color = temp; 
  } 
  
  void swapkeyues(Node *u, Node *v) { 
    int temp; 
    temp = u->key; 
    u->key = v->key; 
    v->key = temp; 
  } 
  
  // fix red red at given node 
  void fixRedRed(Node *x) { 
    // if x is root color it black and return 
    if (x == root) { 
      x->color = BLACK; 
      return; 
    } 
  
    // initialize parent, grandparent, uncle 
    Node *parent = x->parent, *grandparent = parent->parent, 
         *uncle = x->uncle(); 
  
    if (parent->color != BLACK) { 
      if (uncle != NULL && uncle->color == RED) { 
        // uncle red, perform recoloring and recurse 
        parent->color = BLACK; 
        uncle->color = BLACK; 
        grandparent->color = RED; 
        fixRedRed(grandparent); 
      } else { 
        // Else perform LR, LL, RL, RR 
        if (parent->isOnLeft()) { 
          if (x->isOnLeft()) { 
            // for left right 
            swapColors(parent, grandparent); 
          } else { 
            leftRotate(parent); 
            swapColors(x, grandparent); 
          } 
          // for left left and left right 
          rightRotate(grandparent); 
        } else { 
          if (x->isOnLeft()) { 
            // for right left 
            rightRotate(parent); 
            swapColors(x, grandparent); 
          } else { 
            swapColors(parent, grandparent); 
          } 
  
          // for right right and right left 
          leftRotate(grandparent); 
        } 
      } 
    } 
  } 
  
  // find node that do not have a left child 
  // in the subtree of the given node 
  Node *successor(Node *x) { 
    Node *temp = x; 
  
    while (temp->left != NULL) 
      temp = temp->left; 
  
    return temp; 
  } 
  
  // find node that replaces a deleted node in BST 
  Node *BSTreplace(Node *x) { 
    // when node have 2 children 
    if (x->left != NULL and x->right != NULL) 
      return successor(x->right); 
  
    // when leaf 
    if (x->left == NULL and x->right == NULL) 
      return NULL; 
  
    // when single child 
    if (x->left != NULL) 
      return x->left; 
    else
      return x->right; 
  } 
  
  // deletes the given node 
  void deleteNode(Node *v) { 
    Node *u = BSTreplace(v); 
  
    // True when u and v are both black 
    bool uvBlack = ((u == NULL or u->color == BLACK) and (v->color == BLACK)); 
    Node *parent = v->parent; 
  
    if (u == NULL) { 
      // u is NULL therefore v is leaf 
      if (v == root) { 
        // v is root, making root null 
        root = NULL; 
      } else { 
        if (uvBlack) { 
          // u and v both black 
          // v is leaf, fix double black at v 
          fixDoubleBlack(v); 
        } else { 
          // u or v is red 
          if (v->sibling() != NULL) 
            // sibling is not null, make it red" 
            v->sibling()->color = RED; 
        } 
  
        // delete v from the tree 
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
      // v has 1 child 
      if (v == root) { 
        // v is root, assign the keyue of u to v, and delete u 
        v->key = u->key; 
        v->left = v->right = NULL; 
        delete u; 
      } else { 
        // Detach v from tree and move u up 
        if (v->isOnLeft()) { 
          parent->left = u; 
        } else { 
          parent->right = u; 
        } 
        delete v; 
        u->parent = parent; 
        if (uvBlack) { 
          // u and v both black, fix double black at u 
          fixDoubleBlack(u); 
        } else { 
          // u or v red, color u black 
          u->color = BLACK; 
        } 
      } 
      return; 
    } 
  
    // v has 2 children, swap keyues with successor and recurse 
    swapkeyues(u, v); 
    deleteNode(u); 
  } 
  
  void fixDoubleBlack(Node *x) { 
    if (x == root) 
      // Reached root 
      return; 
  
    Node *sibling = x->sibling(), *parent = x->parent; 
    if (sibling == NULL) { 
      // No sibiling, double black pushed up 
      fixDoubleBlack(parent); 
    } else { 
      if (sibling->color == RED) { 
        // Sibling red 
        parent->color = RED; 
        sibling->color = BLACK; 
        if (sibling->isOnLeft()) { 
          // left case 
          rightRotate(parent); 
        } else { 
          // right case 
          leftRotate(parent); 
        } 
        fixDoubleBlack(x); 
      } else { 
        // Sibling black 
        if (sibling->hasRedChild()) { 
          // at least 1 red children 
          if (sibling->left != NULL and sibling->left->color == RED) { 
            if (sibling->isOnLeft()) { 
              // left left 
              sibling->left->color = sibling->color; 
              sibling->color = parent->color; 
              rightRotate(parent); 
            } else { 
              // right left 
              sibling->left->color = parent->color; 
              rightRotate(sibling); 
              leftRotate(parent); 
            } 
          } else { 
            if (sibling->isOnLeft()) { 
              // left right 
              sibling->right->color = parent->color; 
              leftRotate(sibling); 
              rightRotate(parent); 
            } else { 
              // right right 
              sibling->right->color = sibling->color; 
              sibling->color = parent->color; 
              leftRotate(parent); 
            } 
          } 
          parent->color = BLACK; 
        } else { 
          // 2 black children 
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
    cout << x->key << " "; 
    inorder(x->right); 
  } 
  
public: 
  // constructor 
  // initialize root 
  RBTree() { root = NULL; } 
  
  Node *getRoot() { return root; } 
  
  // searches for given keyue 
  // if found returns the node (used for delete) 
  // else returns the last node while traversing (used in insert) 
  Node *search(int n) { 
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
  
  // inserts the given keyue to tree 
  void insert(int n) { 
    Node *newNode = new Node(n); 
    if (root == NULL) { 
      // when root is null 
      // simply insert keyue at root 
      newNode->color = BLACK; 
      root = newNode; 
    } else { 
      Node *temp = search(n); 
  
      if (temp->key == n) { 
        // return if keyue already exists 
        return; 
      } 
  
      // if keyue is not found, search returns the node 
      // where the keyue is to be inserted 
  
      // connect new node to correct node 
      newNode->parent = temp; 
  
      if (n < temp->key) 
        temp->left = newNode; 
      else
        temp->right = newNode; 
  
      // fix red red voilaton if exists 
      fixRedRed(newNode); 
    } 
  } 
  
  // utility function that deletes the node with given keyue 
  void deleteBykey(int n) { 
    if (root == NULL) 
      // Tree is empty 
      return; 
  
    Node *v = search(n); 
  
    if (v->key != n) { 
      cout << "No node found to delete with keyue:" << n << endl; 
      return; 
    } 
  
    deleteNode(v); 
  } 
  
  // prints inorder of the tree 
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
  
  tree.insert(7); 
  tree.insert(3); 
  tree.insert(18); 
  tree.insert(10); 
  tree.insert(22); 
  tree.insert(8); 
  tree.insert(11); 
  tree.insert(26); 
  tree.insert(2); 
  tree.insert(6); 
  tree.insert(13); 
  
  tree.printInOrder(); 
  
  cout<<endl<<"Deleting 18, 11, 3, 10, 22"<<endl; 
  
  tree.deleteBykey(18); 
  tree.deleteBykey(11); 
  tree.deleteBykey(3); 
  tree.deleteBykey(10); 
  tree.deleteBykey(22); 
  
  tree.printInOrder(); 
  return 0; 
} 
