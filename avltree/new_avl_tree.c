#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct avl_node avl_node_t;

/* ---------------------------------------------- */

typedef struct avl_node_stack {
	avl_node_t **array;	
	uint32_t size;
	int64_t top;
} avl_node_stack_t;

avl_node_stack_t stack_create(uint32_t size) {
	avl_node_stack_t stack = {
		.array = (avl_node_t **)malloc(sizeof(avl_node_t *) * size),
		.size = size,
		.top = -1
	};

	return stack;
}

void stack_free(avl_node_stack_t *stack) {
	if (stack->array != NULL) {
		free(stack->array);
	}

	stack->size = 0;
	stack->top = -1;
}

avl_node_t *stack_peek(avl_node_stack_t *stack) {
	if (stack->top == -1) {
		return NULL;
	}

	return stack->array[stack->top];
}

avl_node_t *stack_pop(avl_node_stack_t *stack) {
	if (stack->top == -1) {
		return NULL;
	}

	avl_node_t *node = stack->array[stack->top];

	stack->array[(stack->top)--] = NULL;

	return node;
}

int stack_push(avl_node_stack_t *stack, avl_node_t* node) {
	if (stack->top + 1 >= stack->size) {
		return -1;
	}

	stack->array[++(stack->top)] = node;

	return 0;
}

bool stack_is_empty(avl_node_stack_t *stack) {
	return stack->top == -1;
}

/* ---------------------------------------------- */

struct avl_node {
  	int value;
	avl_node_t *parent;
  	avl_node_t *left;
  	avl_node_t *right;
  	int32_t left_height;
  	int32_t right_height;
};


void avl_print_node(avl_node_t *node) {
	printf(
	"%p {\n"
	"  .value=%d\n"
	"  .parent=%p\n"
	"  .left=%p\n"
	"  .right=%p\n"
	"  .left_height=%d\n"
	"  .right_height=%d\n"
	"}\n",
		node,
		node->value,
		node->parent,
		node->left,
		node->right,
		node->left_height,
		node->right_height);
}

typedef struct avl_tree {
  	avl_node_t* root;
	uint64_t size;
} avl_tree_t;

avl_tree_t avl_tree_create(int value) {
	avl_node_t *node = (avl_node_t *)malloc(sizeof(avl_node_t));
	node->value = value;
	node->parent = NULL;
	node->left = NULL;
	node->right = NULL;
	node->left_height = 0;
	node->right_height = 0;

	avl_tree_t tree = { .root = node, .size = 1 };

	return tree;
}

void avl_tree_free(avl_tree_t* tree) {
	avl_node_stack_t node_stack = stack_create(tree->size); 
	avl_node_stack_t temp_stack = stack_create(tree->size);

	stack_push(&temp_stack, tree->root);

	while (!stack_is_empty(&temp_stack)) {
		avl_node_t *node = stack_pop(&temp_stack);	

		if (node->left != NULL) {
			stack_push(&temp_stack, node->left);
		}

		if (node->right != NULL) {
			stack_push(&temp_stack, node->right);
		}

		stack_push(&node_stack, node);
	}


	while (!stack_is_empty(&node_stack)) {
		free(stack_pop(&node_stack));	
	}

	stack_free(&node_stack);
	stack_free(&temp_stack);

	tree->root = NULL;
	tree->size = 0;
}

static inline int max(int32_t x, int32_t y) {
	return x < y ? y : x;
}

void avl_rotate_ll(avl_node_t *x, avl_node_t *y) {
	avl_node_t temp_x = *x;
	avl_node_t temp_y = *y;

	x->value = temp_y.value;
	y->value = temp_x.value;

	x->right = y;
	x->left = temp_y.left;
	y->right = temp_x.right;
	y->left = temp_y.right;

	y->left_height = temp_y.right_height;
	y->right_height = temp_x.right_height;
	x->left_height = temp_y.left_height;
	x->right_height = 1 + max(y->left_height, y->right_height);
}

void avl_rotate_lr(avl_node_t *x, avl_node_t *y) {
	avl_node_t *z = y->right;

	avl_node_t temp_x = *x;
	avl_node_t temp_y = *y;
	avl_node_t temp_z = *z;

	x->value = temp_z.value;
	z->value = temp_x.value;

	x->right = z;
	z->right = temp_x.right;
	z->left = temp_z.right;
	y->right = temp_z.left;

	y->right_height = temp_z.left_height;
	z->right_height = temp_x.right_height;
	z->left_height = temp_z.right_height;
	x->right_height = 1 + max(z->left_height, z->right_height);
	x->left_height = 1 + max(y->left_height, y->right_height);
}

void avl_rotate_rr(avl_node_t *x, avl_node_t *y) {
	avl_node_t temp_x = *x;
	avl_node_t temp_y = *y;
	
	x->value = temp_y.value;
	y->value = temp_x.value;

	x->left = y;
	x->right = temp_y.right;
	y->left = temp_x.left;
	y->right = temp_y.left;

	y->right_height = temp_y.left_height;
	y->left_height = temp_x.left_height;
	x->right_height = temp_y.right_height;
	x->left_height = 1 + max(y->left_height, y->right_height);
}

void avl_rotate_rl(avl_node_t *x, avl_node_t *y) {
	avl_node_t *z = y->left;

	avl_node_t temp_x = *x;
	avl_node_t temp_y = *y;
	avl_node_t temp_z = *z;

	x->value = temp_z.value;
	z->value = temp_x.value;

	x->left = z;
	z->left = temp_x.left;
	z->right = temp_z.left;
	y->left = temp_z.right;

	y->left_height = temp_z.right_height;
	z->left_height = temp_x.left_height;
	z->right_height = temp_z.left_height;
	x->left_height = 1 + max(z->left_height, z->right_height);
	x->right_height = 1 + max(y->left_height, y->right_height);
}

bool avl_can_step(avl_node_t *node, avl_node_t *new_node) {
	// left
	if (node->value > new_node->value) {
		if (node->left == NULL) {
			return false;
		}
	// right
	} else {
		if (node->right == NULL) {
			return false;
		}
	}

	return true;
}

static inline int32_t avl_height_diff(avl_node_t *node) {
	return node->left_height - node->right_height;
}

static inline bool avl_is_left(avl_node_t *node, avl_node_t *other) {
	return node->left == other;
}

void avl_tree_update(avl_node_t *node) {
	if (!node->left) {
		node->left_height = 0;
	} else {
		node->left_height = 1 + max(node->left->left_height, node->left->right_height);
	}

	if (!node->right) {
		node->right_height = 0;
	} else {
		node->right_height = 1 + max(node->right->left_height, node->right->right_height);
	}

	avl_node_t *parent_node = node->parent;

	while (parent_node) {
		if (avl_is_left(parent_node, node)) {
			parent_node->left_height
				= 1 + max(node->left_height, node->right_height);	
		} else {
			parent_node->right_height
				= 1 + max(node->left_height, node->right_height);
		}

		int32_t height_diff = avl_height_diff(parent_node);

		if (height_diff > 1) { // left heavy
			// ll rotate
			if (parent_node->left->left_height > parent_node->left->right_height) {
				avl_rotate_ll(parent_node, parent_node->left);
			// lr rotate
			} else {
				avl_rotate_lr(parent_node, parent_node->left);
			}
		}

		if (height_diff < -1) { // right heavy
			// rr rotate
			if (parent_node->right->right_height > parent_node->right->left_height) {
				avl_rotate_rr(parent_node, parent_node->right);
			// rl rotate
			} else {
				avl_rotate_rl(parent_node, parent_node->right);

			}
		}

		node = parent_node; parent_node = parent_node->parent;
	}
}

int avl_node_insert(avl_node_t *node, avl_node_t *new_node) {
	avl_node_t *current_node = node;

	for (;;) {
		bool can_step = avl_can_step(current_node, new_node);

		if (can_step == false) {
			break;
		}
		
		// right
		if (current_node->value <= new_node->value) {
			current_node = current_node->right;
		// left
		} else {
			current_node = current_node->left;
		}
	}
	
	new_node->parent = current_node;

	// left
	if (current_node->value > new_node->value) {
		current_node->left = new_node;			
	// right
	} else {
		current_node->right = new_node;			
	}

	// proceed to update the heights of each node and rotate
	// if necessary

	avl_tree_update(current_node);

	return 0;
}


int avl_tree_insert(avl_tree_t *tree, int value) {
	avl_node_t *node = (avl_node_t *)malloc(sizeof(avl_node_t));
	node->value = value;
	node->parent = NULL;
	node->left = NULL;
	node->right = NULL;
	node->left_height = 0;
	node->right_height = 0;

	if (tree->root != NULL) {
		if (avl_node_insert(tree->root, node) >= 0) {
			tree->size++;

			return 0;
		}

		return -1;
	}

	tree->root = node;
	tree->size = 1;

	return 0;
}

void avl_tree_print(avl_tree_t* tree) {
#ifdef __APPLE__ 
	printf("Tree Size: %llu, ", tree->size);
#elif __linux__    
	printf("Tree Size: %lu, ", tree->size);
#endif 

	avl_node_stack_t node_stack = stack_create(tree->size); 
	avl_node_stack_t temp_stack = stack_create(tree->size);

	stack_push(&temp_stack, tree->root);

	while (!stack_is_empty(&temp_stack)) {
		avl_node_t *node = stack_pop(&temp_stack);	

		if (node->left != NULL) {
			stack_push(&temp_stack, node->left);
		}

		if (node->right != NULL) {
			stack_push(&temp_stack, node->right);
		}

		stack_push(&node_stack, node);
	}


	while (!stack_is_empty(&node_stack)) {
		avl_node_t *node = stack_pop(&node_stack);

		printf("(%d)", node->value);
	}

	printf("\n");

	stack_free(&node_stack);
	stack_free(&temp_stack);
}

bool avl_is_leaf(avl_node_t *node) {
	return node->left == NULL && node->right == NULL;
}

avl_node_t *avl_find_max(avl_node_t *node) {
	avl_node_t *current_node = node;

	while (current_node->right) {
		current_node = current_node->right;
	}

	return current_node;
}

int avl_node_remove(avl_node_t *node, int value) {
	avl_node_t *current_node = node;

	bool found = false;

	while (!found && current_node) {
		if (current_node->value == value) {
			found = true;
		}

		// left
		if (current_node->value > value) {
			current_node = current_node->left;
		}

		// right
		if (current_node->value < value) {
			current_node = current_node->right;
		}
	}

	if (!found) {
		return -1;
	}

	avl_node_t *update_node;

	if (avl_is_leaf(current_node)) {
		update_node = current_node->parent;

		if (avl_is_left(update_node, current_node)) {
			update_node->left = NULL;
		} else {
			update_node->right = NULL;
		}

		free(current_node);
	} else {
		if (current_node->left) {
			avl_node_t *max_node = avl_find_max(current_node->left);

			current_node->value = max_node->value;

			if (max_node->left) {
				max_node->value = max_node->left->value;

				free(max_node->left);

				max_node->left = NULL;

				update_node = max_node;
			} else {
				max_node->parent->right = NULL;

				update_node = max_node->parent;

				free(max_node);
			}
		} else {
			avl_node_t *right_node = current_node->right;
			
			current_node->value = right_node->value;

			current_node->right = NULL;

			free(right_node);

			update_node = current_node;
		}
	}	

	avl_tree_update(update_node);
	
	return 0;
}

int avl_tree_remove(avl_tree_t *tree, int value) {
	if (tree->root == NULL) {
		return 0;
	}

	if (avl_node_remove(tree->root, value) >= 0) { // >= 0 => found
		tree->size--;

		if (tree->size == 0) {
			tree->root = NULL;
		}

		return 0;
	}

	return -1;
}

// TAKEN FROM THE INTERNET
//
// https://gist.github.com/ximik777/e04e5a9f0548a2f41cb09530924bdd9a/

//printing tree in ascii

typedef struct asciinode_struct asciinode;

struct asciinode_struct {
    asciinode *left, *right;

    //length of the edge from this node to its children
    int edge_length;

    int height;

    int lablen;

    //-1=I am left, 0=I am root, 1=right
    int parent_dir;

    //max supported unit32 in dec, 10 digits max
    char label[11];
};


#define MAX_HEIGHT 1000
int lprofile[MAX_HEIGHT];
int rprofile[MAX_HEIGHT];
#define INFINITY (1<<20)

//adjust gap between left and right nodes
int gap = 3;

//used for printing next node in the same level,
//this is the x coordinate of the next char printed
int print_next;

int MIN(int X, int Y) {
    return ((X) < (Y)) ? (X) : (Y);
}

int MAX(int X, int Y) {
    return ((X) > (Y)) ? (X) : (Y);
}

asciinode *build_ascii_tree_recursive(avl_node_t *t) {
    asciinode *node;

    if (t == NULL) return NULL;

    node = malloc(sizeof(asciinode));
    node->left = build_ascii_tree_recursive(t->left);
    node->right = build_ascii_tree_recursive(t->right);

    if (node->left != NULL) {
        node->left->parent_dir = -1;
    }

    if (node->right != NULL) {
        node->right->parent_dir = 1;
    }

    sprintf(node->label, "%d", t->value);
    node->lablen = (int) strlen(node->label);

    return node;
}


//Copy the tree into the ascii node structre
asciinode *build_ascii_tree(avl_node_t *t) {
    asciinode *node;
    if (t == NULL) return NULL;
    node = build_ascii_tree_recursive(t);
    node->parent_dir = 0;
    return node;
}

//Free all the nodes of the given tree
void free_ascii_tree(asciinode *node) {
    if (node == NULL) return;
    free_ascii_tree(node->left);
    free_ascii_tree(node->right);
    free(node);
}

//The following function fills in the lprofile array for the given tree.
//It assumes that the center of the label of the root of this tree
//is located at a position (x,y).  It assumes that the edge_length
//fields have been computed for this tree.
void compute_lprofile(asciinode *node, int x, int y) {
    int i, isleft;
    if (node == NULL) return;
    isleft = (node->parent_dir == -1);
    lprofile[y] = MIN(lprofile[y], x - ((node->lablen - isleft) / 2));
    if (node->left != NULL) {
        for (i = 1; i <= node->edge_length && y + i < MAX_HEIGHT; i++) {
            lprofile[y + i] = MIN(lprofile[y + i], x - i);
        }
    }
    compute_lprofile(node->left, x - node->edge_length - 1, y + node->edge_length + 1);
    compute_lprofile(node->right, x + node->edge_length + 1, y + node->edge_length + 1);
}

void compute_rprofile(asciinode *node, int x, int y) {
    int i, notleft;
    if (node == NULL) return;
    notleft = (node->parent_dir != -1);
    rprofile[y] = MAX(rprofile[y], x + ((node->lablen - notleft) / 2));
    if (node->right != NULL) {
        for (i = 1; i <= node->edge_length && y + i < MAX_HEIGHT; i++) {
            rprofile[y + i] = MAX(rprofile[y + i], x + i);
        }
    }
    compute_rprofile(node->left, x - node->edge_length - 1, y + node->edge_length + 1);
    compute_rprofile(node->right, x + node->edge_length + 1, y + node->edge_length + 1);
}

//This function fills in the edge_length and
//height fields of the specified tree
void compute_edge_lengths(asciinode *node) {
    int h, hmin, i, delta;
    if (node == NULL) return;
    compute_edge_lengths(node->left);
    compute_edge_lengths(node->right);

    /* first fill in the edge_length of node */
    if (node->right == NULL && node->left == NULL) {
        node->edge_length = 0;
    } else {
        if (node->left != NULL) {
            for (i = 0; i < node->left->height && i < MAX_HEIGHT; i++) {
                rprofile[i] = -INFINITY;
            }
            compute_rprofile(node->left, 0, 0);
            hmin = node->left->height;
        } else {
            hmin = 0;
        }
        if (node->right != NULL) {
            for (i = 0; i < node->right->height && i < MAX_HEIGHT; i++) {
                lprofile[i] = INFINITY;
            }
            compute_lprofile(node->right, 0, 0);
            hmin = MIN(node->right->height, hmin);
        } else {
            hmin = 0;
        }
        delta = 4;
        for (i = 0; i < hmin; i++) {
            delta = MAX(delta, gap + 1 + rprofile[i] - lprofile[i]);
        }

        //If the node has two children of height 1, then we allow the
        //two leaves to be within 1, instead of 2
        if (((node->left != NULL && node->left->height == 1) ||
             (node->right != NULL && node->right->height == 1)) && delta > 4) {
            delta--;
        }

        node->edge_length = ((delta + 1) / 2) - 1;
    }

    //now fill in the height of node
    h = 1;
    if (node->left != NULL) {
        h = MAX(node->left->height + node->edge_length + 1, h);
    }
    if (node->right != NULL) {
        h = MAX(node->right->height + node->edge_length + 1, h);
    }
    node->height = h;
}

//This function prints the given level of the given tree, assuming
//that the node has the given x cordinate.
void print_level(asciinode *node, int x, int level) {
    int i, isleft;
    if (node == NULL) return;
    isleft = (node->parent_dir == -1);
    if (level == 0) {
        for (i = 0; i < (x - print_next - ((node->lablen - isleft) / 2)); i++) {
            printf(" ");
        }
        print_next += i;
        printf("%s", node->label);
        print_next += node->lablen;
    } else if (node->edge_length >= level) {
        if (node->left != NULL) {
            for (i = 0; i < (x - print_next - (level)); i++) {
                printf(" ");
            }
            print_next += i;
            printf("/");
            print_next++;
        }
        if (node->right != NULL) {
            for (i = 0; i < (x - print_next + (level)); i++) {
                printf(" ");
            }
            print_next += i;
            printf("\\");
            print_next++;
        }
    } else {
        print_level(node->left,
                    x - node->edge_length - 1,
                    level - node->edge_length - 1);
        print_level(node->right,
                    x + node->edge_length + 1,
                    level - node->edge_length - 1);
    }
}

//prints ascii tree for given Tree structure
void print_ascii_tree(avl_node_t *t) {
    asciinode *proot;
    int xmin, i;
    if (t == NULL) return;
    proot = build_ascii_tree(t);
    compute_edge_lengths(proot);
    for (i = 0; i < proot->height && i < MAX_HEIGHT; i++) {
        lprofile[i] = INFINITY;
    }
    compute_lprofile(proot, 0, 0);
    xmin = 0;
    for (i = 0; i < proot->height && i < MAX_HEIGHT; i++) {
        xmin = MIN(xmin, lprofile[i]);
    }
    for (i = 0; i < proot->height; i++) {
        print_next = 0;
        print_level(proot, -xmin, i);
        printf("\n");
    }
    if (proot->height >= MAX_HEIGHT) {
        printf("(This tree is taller than %d, and may be drawn incorrectly.)\n", MAX_HEIGHT);
    }
    free_ascii_tree(proot);
}

// THE ABOVE WAS TAKEN FROM THE INTERNET

int main() {
	avl_tree_t tree = avl_tree_create(7);
	avl_tree_insert(&tree, 3);
	avl_tree_insert(&tree, 18);
	avl_tree_insert(&tree, 10);
	avl_tree_insert(&tree, 22);
	avl_tree_insert(&tree, 26);
	avl_tree_insert(&tree, 8);
	avl_tree_insert(&tree, 11);

	print_ascii_tree(tree.root);
	avl_tree_insert(&tree, 15);
	print_ascii_tree(tree.root);

	avl_tree_remove(&tree, 18);
	print_ascii_tree(tree.root);


	avl_tree_remove(&tree, 11);
	print_ascii_tree(tree.root);

	avl_tree_free(&tree);
	return 0;
}
