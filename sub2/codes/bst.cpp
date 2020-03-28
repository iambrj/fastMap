#include "bst.h"
#include "util.hpp"
BSTNode::BSTNode(char cc) : c(cc), left(nullptr), right(nullptr) {
}

BSTNode::~BSTNode() {
    if (this->left) {
        delete this->left;
        this->left = nullptr;
    }
    if (this->right) {
        delete this->right;
        this->right = nullptr;
    }
    if (this->data) {
        resetPointer(this->data);
        this->data = nullptr;
    }
}

// BST FUNCTIONS
BSTNode *BST::_insert(BSTNode *cur, char c) {
    if (!cur) {  // 50% of all lookups
        return new BSTNode(c);
    }

    if (cur->c < c) {
        cur->right = _insert(cur->right, c);
    } else if (cur->c > c) {
        cur->left = _insert(cur->left, c);
    }

    return cur;
}

BSTNode *BST::_get(BSTNode *cur, char c) {
    if (!cur) {
        return nullptr;
    }

    if (cur->c < c) {
        return _get(cur->right, c);
    } else if (cur->c > c) {
        return _get(cur->left, c);
    } else {
        return cur;  // least likely, at the end
    }
}

BST::BST() : root(nullptr) {
}

BST::~BST() {
    if (this->root) {
        delete this->root;
        this->root = nullptr;
    }
}

BSTNode *BST::getOrInsert(char c) {
    this->root = this->_insert(this->root, c);
    return _get(this->root, c);
}

BSTNode *BST::search(char c) {
    return _get(this->root, c);
}

BSTNode *BST::getRoot() {
    return this->root;
}

void BST::clear() {
    if (this->root) {
        delete this->root;
        this->root = nullptr;
    }
}
