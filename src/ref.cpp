/* Author: Prakhar Jain */
#include <stdio.h>
#include <stdlib.h>
 
#define RED		1
#define BLACK	2
 
struct node {
	int key;
	struct node *left, *right, *p;
	int color;
	int size;
};
 
typedef struct node *NODEPTR;
struct node NIL;
NODEPTR NILPTR = &NIL;
 
NODEPTR search(NODEPTR root, int k) {
	if (root == NILPTR || root->key == k)
		return root;
	if (k < root->key)
		return search(root->left, k);
	else
		return search(root->right, k);
}
 
NODEPTR minimum(NODEPTR root) {
	while (root->left != NILPTR)
		root = root->left;
	return root;
}
 
NODEPTR maximum(NODEPTR root) {
	while (root->right != NILPTR)
		root = root->right;
	return root;
}
 
void leftrotate(NODEPTR *treeroot, NODEPTR x) {
	NODEPTR y = x->right;
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
	x->size = x->left->size + x->right->size +1;
	/* Changes End */
}
 
void rightrotate(NODEPTR *treeroot, NODEPTR y) {
	NODEPTR x = y->left;
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
 
void rbinsertfixup(NODEPTR *treeroot, NODEPTR z) {
	while (z->p->color == RED) {
		if (z->p == z->p->p->left) {
			NODEPTR y = z->p->p->right;
			if (y->color == RED) {
				z->p->color = BLACK;
				y->color = BLACK;
				z->p->p->color = RED;
				z = z->p->p;
			}
			else {
				if (z == z->p->right) {
					z = z->p;
					leftrotate(treeroot,z);
				}
				z->p->color = BLACK;
				z->p->p->color = RED;
				rightrotate(treeroot,z->p->p);
			}
		}
		else {
			NODEPTR y = z->p->p->left;
			if (y->color == RED) {
				z->p->color = BLACK;
				y->color = BLACK;
				z->p->p->color = RED;
				z = z->p->p;
			}
			else {
				if (z == z->p->left) {
					z = z->p;
					rightrotate(treeroot,z);
				}
				z->p->color = BLACK;
				z->p->p->color = RED;
				leftrotate(treeroot,z->p->p);
			}
		}
	}
	(*treeroot)->color = BLACK;
}
 
void rbinsert(NODEPTR *treeroot, int z) {
	NODEPTR Z = (NODEPTR) malloc(sizeof(struct node));
	Z->key = z;
	NODEPTR y = NILPTR;
	NODEPTR x = *treeroot;
	while (x != NILPTR) {
		y = x;
		/* Changes */	
		y->size++;
		/* Changes End */
		if (Z->key < x->key)
			x = x->left;
		else if (Z->key > x->key)
			x = x->right;
		else {
			/* if the value to be inserted is already in the tree, undo the changes */
			/* Changes */
			NODEPTR temp = x;
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
	else if (Z->key < y->key)
		y->left = Z;
	else
		y->right = Z;
	Z->left = NILPTR;
	Z->right = NILPTR;
	Z->color = RED;
	Z->size = 1;
	rbinsertfixup(treeroot,Z);
}
 
void rbtransplant(NODEPTR *treeroot, NODEPTR u, NODEPTR v) {
	if (u->p == NILPTR)
		*treeroot = v;
	else if (u == u->p->left)
		u->p->left = v;
	else
		u->p->right = v;
	v->p = u->p;
}
 
void rbdeletefixup(NODEPTR *treeroot, NODEPTR x) {
	while (x != *treeroot && x->color == BLACK) {
		if (x == x->p->left) {
			NODEPTR w = x->p->right;
			if (w->color == RED) {
				w->color = BLACK;
				x->p->color = RED;
				leftrotate(treeroot,x->p);
				w = x->p->right;
			}
			if (w->left->color == BLACK && w->right->color == BLACK) {
				w->color = RED;
				x = x->p;
			}
			else {
			 	if (w->right->color == BLACK) {
					w->left->color = BLACK;
					w->color = RED;
					rightrotate(treeroot,w);
					w = x->p->right;
				}
				w->color = x->p->color;
				x->p->color = BLACK;
				w->right->color = BLACK;
				leftrotate(treeroot,x->p);
				x = *treeroot;
			}
		}
		else {
			NODEPTR w = x->p->left;
			if (w->color == RED) {
				w->color = BLACK;
				x->p->color = RED;
				rightrotate(treeroot,x->p);
				w = x->p->left;
			}
			if (w->left->color == BLACK && w->right->color == BLACK) {
				w->color = RED;
				x = x->p;
			}
			else {
				if (w->left->color == BLACK) {
					w->right->color = BLACK;
					w->color = RED;
					leftrotate(treeroot,w);
					w = x->p->left;
				}
				w->color = x->p->color;
				x->p->color = BLACK;
				w->left->color = BLACK;
				rightrotate(treeroot,x->p);
				x = *treeroot;
			}
		}
	}
	x->color = BLACK;
}
 
void rbdelete(NODEPTR *treeroot, int z) {
	NODEPTR Z = search(*treeroot, z);
 
	if (Z == NILPTR) {
		return;
	}
	/* Phase1: Changes */
	if (Z != *treeroot) {
		NODEPTR tmp = Z->p;
		while (tmp != *treeroot) {
			tmp->size--;
			tmp = tmp->p;
		}
		(*treeroot)->size--;
	}
	/* Changes End */
	NODEPTR y = Z;
	int yoc = y->color;
	NODEPTR x;
	if (Z->left == NILPTR) {
		x = Z->right;
		rbtransplant(treeroot,Z,Z->right);
	}
	else if (Z->right == NILPTR) {
		x = Z->left;
		rbtransplant(treeroot,Z,Z->left);
	}
	else {
		y = minimum(Z->right);
		yoc = y->color;
		x = y->right;
		if (y->p == Z) {
			x->p = y;
		}
		else {
			rbtransplant(treeroot,y,y->right);
			y->right = Z->right;
			y->right->p = y;
			/* Phase2: Changes */
			NODEPTR tmp = x->p;
			while (tmp != y) {
				tmp->size--;
				tmp = tmp->p;
			}
			y->size = y->left->size + 1;
			/* Changes End */
		}
		rbtransplant(treeroot,Z,y);
		y->left = Z->left;
		y->left->p = y;
		y->color = Z->color;
		/* Phase2: Changes */
		y->size = y->left->size + y->right->size + 1;
		/* Changes End */
	}
	if (yoc == BLACK)
		rbdeletefixup(treeroot,x);
}
 
NODEPTR kth(NODEPTR treeroot, int K) {
	int currrank = treeroot->left->size + 1;
	NODEPTR y = treeroot;
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
 
int cnt(NODEPTR treeroot, int x) {
	int ans = 0;
	NODEPTR y = treeroot;
	while (y != NILPTR) {
		if (y->key > x)
			y = y->left;
		else if (y->key < x) {
			ans += y->left->size + 1;
			y = y->right;
		}
		else 
			return ans + y->left->size;
	}
	return ans;
}
 
main()
{
	NIL.left = NIL.right = NIL.p = NILPTR;
	NIL.color = BLACK;
	NIL.size = 0;
	NODEPTR tree = NILPTR;
	int Q;
	int x, k;
	NODEPTR temp;
	char c[2];
	scanf("%d", &Q);
	getchar();
	while (Q--) {
		scanf("%s", c);
		switch (c[0]) {
			case 'I':
				scanf("%d", &x);
				rbinsert(&tree, x);
				break;
			case 'D':
				scanf("%d", &x);
				rbdelete(&tree, x);
				break;
			case 'K':
				scanf("%d", &k);
				temp = kth(tree, k);
				if (temp != NILPTR)
					printf("%d\n", temp->key);
				else
					printf("invalid\n");
				break;
			case 'C':
				scanf("%d", &x);
				printf("%d\n", cnt(tree,x));
				break;
			default:
				break;
		}
	}
	return 0;
}
