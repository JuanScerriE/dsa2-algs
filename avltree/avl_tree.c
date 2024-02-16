#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct avl_node avl_node;

typedef struct {
	avl_node **array;	
	uint32_t size;
	int64_t top;
} avl_node_stack;

avl_node_stack stack_create(uint32_t size) {
	avl_node_stack stack = {
		.array = (avl_node **)malloc(sizeof(avl_node *) * size),
		.size = size,
		.top = -1
	};

	return stack;
}

void stack_free(avl_node_stack *stack) {
	if (stack->array != NULL) {
		free(stack->array);
	}

	stack->size = 0;
	stack->top = -1;
}

avl_node *stack_peek(avl_node_stack *stack) {
	if (stack->top == -1) {
		return NULL;
	}

	return stack->array[stack->top];
}

avl_node *stack_pop(avl_node_stack *stack) {
	if (stack->top == -1) {
		return NULL;
	}

	avl_node *node = stack->array[stack->top];

	stack->array[(stack->top)--] = NULL;

	return node;
}

int stack_push(avl_node_stack *stack, avl_node* node) {
	if (stack->top + 1 >= stack->size) {
		return -1;
	}

	stack->array[++(stack->top)] = node;

	return 0;
}

bool stack_is_empty(avl_node_stack *stack) {
	return stack->top == -1;
}

struct avl_node {
  	int value;
  	int32_t left_height;
  	int32_t right_height;
  	avl_node *left;
  	avl_node *right;
};

typedef struct {
  	avl_node* root;
	uint64_t size;
} avl_tree;

avl_tree avl_tree_create(int value) {
	avl_node *node = (avl_node *)malloc(sizeof(avl_node));
	node->value = value;
	node->left_height = 0;
	node->right_height = 0;
	node->left = NULL;
	node->right = NULL;

	avl_tree tree = { .root = node, .size = 1 };

	return tree;
}

void avl_tree_free(avl_tree* tree) {
	avl_node_stack node_stack = stack_create(tree->size); 
	avl_node_stack temp_stack = stack_create(tree->size);

	stack_push(&temp_stack, tree->root);

	while (!stack_is_empty(&temp_stack)) {
		avl_node *node = stack_pop(&temp_stack);	

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

int avl_rotate_ll(avl_node *x, avl_node *y) {
	avl_node temp_x = *x;
	avl_node temp_y = *y;

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

	return max(x->left_height, x->right_height);
}

int avl_rotate_lr(avl_node *x, avl_node *y) {
	avl_node *z = y->right;

	avl_node temp_x = *x;
	avl_node temp_y = *y;
	avl_node temp_z = *z;

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

	return max(x->left_height, x->right_height);
}

int avl_rotate_rr(avl_node *x, avl_node *y) {
	avl_node temp_x = *x;
	avl_node temp_y = *y;
	
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

	return max(x->left_height, x->right_height);
}

int avl_rotate_rl(avl_node *x, avl_node *y) {
	avl_node *z = y->left;

	avl_node temp_x = *x;
	avl_node temp_y = *y;
	avl_node temp_z = *z;

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

	return max(x->left_height, x->right_height);
}

int avl_node_insert(avl_node *node, avl_node *new_node) {
	// insert left
	if (new_node->value < node->value) {
		if (node->left == NULL) {
			node->left = new_node;
			node->left_height = 1;

			// there is no need for detecting a
			// balancing violations when adding the
			// leaf at the end because the parent
			// node cannot have more than a child
			// node on the right. this is because
			// having anything more would imply that
			// without adding the new vertex the
			// tree would have already been
			// imbalanced. More mathematically, if
			// |new_h_l - h_r| >= 1 =>
			// |h_l - h_r| >= 2 which violates the
			// fact that the tree should be balanced.
			
			return max(node->left_height, node->right_height);
		} else {
			int32_t height_of_left_subtree = avl_node_insert(node->left, new_node);
			node->left_height = 1 + height_of_left_subtree;

			// detect balancing violation 
			if (abs(node->left_height - node->right_height) > 1) {
				// ll rotation
				if (node->left->left_height > node->left->right_height) {
					return avl_rotate_ll(node, node->left);
				// lr rotation
				} else {
					return avl_rotate_ll(node, node->left);
				}				

			} else {
				return max(node->left_height, node->right_height);
			}
		}
	// insert right
	} else {
		if (node->right == NULL) {
			node->right = new_node;
			node->right_height = 1;
				
			return max(node->left_height, node->right_height);
		} else {
			int32_t height_of_right_subtree = avl_node_insert(node->right, new_node);
			node->right_height = 1 + height_of_right_subtree;
 
			// detect balancing violation 
			if (abs(node->left_height - node->right_height) > 1) {
				// rr rotation
				if (node->right->right_height > node->right->left_height) {
					return avl_rotate_rr(node, node->right);
				// rl rotation
				} else {
					return avl_rotate_rl(node, node->right);
				}				
			} else {
				return max(node->left_height, node->right_height);
			}

		}

	}

}

void avl_tree_print(avl_tree* tree) {
	printf("Tree Size: %lu, ", tree->size);

	avl_node_stack node_stack = stack_create(tree->size); 
	avl_node_stack temp_stack = stack_create(tree->size);

	stack_push(&temp_stack, tree->root);

	while (!stack_is_empty(&temp_stack)) {
		avl_node *node = stack_pop(&temp_stack);	

		if (node->left != NULL) {
			stack_push(&temp_stack, node->left);
		}

		if (node->right != NULL) {
			stack_push(&temp_stack, node->right);
		}

		stack_push(&node_stack, node);
	}


	while (!stack_is_empty(&node_stack)) {
		avl_node *node = stack_pop(&node_stack);

		printf("(%d)", node->value);
	}

	printf("\n");

	stack_free(&node_stack);
	stack_free(&temp_stack);
}

int avl_tree_insert(avl_tree *tree, int value) {
	avl_node *node = (avl_node *)malloc(sizeof(avl_node));
	node->value = value;
	node->left_height = 0;
	node->right_height = 0;
	node->left = NULL;
	node->right = NULL;

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

bool avl_tree_find();

avl_node *avl_tree_remove();

int main() {
	avl_tree tree = avl_tree_create(10);
	avl_tree_insert(&tree, 20);
	avl_tree_print(&tree);
	avl_tree_insert(&tree, 30);
	avl_tree_print(&tree);
	avl_tree_insert(&tree, 25);
	avl_tree_print(&tree);
	avl_tree_insert(&tree, 30);
	avl_tree_print(&tree);
	avl_tree_free(&tree);
	return 0;
}
