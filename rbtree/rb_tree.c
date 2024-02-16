#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct rb_node rb_node_t;

typedef struct rb_node_stack {
	rb_node_t **array;	
	uint32_t size;
	int64_t top;
} rb_node_stack_t;

rb_node_stack_t stack_create(uint32_t size) {
	rb_node_stack_t stack = {
		.array = (rb_node_t **)malloc(sizeof(rb_node_t *) * size),
		.size = size,
		.top = -1
	};

	return stack;
}

void stack_free(rb_node_stack_t *stack) {
	if (stack->array != NULL) {
		free(stack->array);
	}

	stack->size = 0;
	stack->top = -1;
}

rb_node_t *stack_peek(rb_node_stack_t *stack) {
	if (stack->top == -1) {
		return NULL;
	}

	return stack->array[stack->top];
}

rb_node_t *stack_pop(rb_node_stack_t *stack) {
	if (stack->top == -1) {
		return NULL;
	}

	rb_node_t *node = stack->array[stack->top];

	stack->array[(stack->top)--] = NULL;

	return node;
}

int stack_push(rb_node_stack_t *stack, rb_node_t* node) {
	if (stack->top + 1 >= stack->size) {
		return -1;
	}

	stack->array[++(stack->top)] = node;

	return 0;
}

bool stack_is_empty(rb_node_stack_t *stack) {
	return stack->top == -1;
}


typedef enum rb_color {
	BLACK,
	RED
} rb_color_t;

struct rb_node {
  	int value;
	rb_node_t *parent;
  	rb_node_t *left;
  	rb_node_t *right;
	rb_color_t color;
};


void rb_print_node(rb_node_t *node) {
	printf("%p {\n  .value=%d\n  .parent=%p\n  .left=%p\n  .right=%p\n  .color=%d\n}\n", node, node->value, node->parent, node->left, node->right, node->color);
}

typedef struct rb_tree {
  	rb_node_t* root;
	uint64_t size;
} rb_tree_t;

#define RB_LEAF NULL

rb_tree_t rb_tree_create(int value) {
	rb_node_t *node = (rb_node_t *)malloc(sizeof(rb_node_t));
	node->value = value;
	node->color = BLACK;
	node->left = RB_LEAF;
	node->right = RB_LEAF;

	rb_tree_t tree = { .root = node, .size = 1 };

	return tree;
}

void rb_tree_free(rb_tree_t* tree) {
	rb_node_stack_t node_stack = stack_create(tree->size); 
	rb_node_stack_t temp_stack = stack_create(tree->size);

	stack_push(&temp_stack, tree->root);

	while (!stack_is_empty(&temp_stack)) {
		rb_node_t *node = stack_pop(&temp_stack);	

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

void rb_rotate_ll(rb_node_t *x, rb_node_t *y) {
	rb_node_t *a = x;
	rb_node_t *b = y;

	rb_node_t temp_x = *x;
	rb_node_t temp_y = *y;

	a->value = temp_y.value;
	b->value = temp_x.value;

	a->right = b;
	a->left = temp_y.left;
	
	b->left = temp_y.right;
	b->right = temp_x.right;
}

void rb_rotate_lr(rb_node_t *x, rb_node_t *y) {
	rb_node_t *z = y->right;

	rb_node_t *a = x;
	rb_node_t *b = y;
	rb_node_t *c = z;

	rb_node_t temp_x = *x;
	rb_node_t temp_y = *y;
	rb_node_t temp_z = *z;

	a->value = temp_z.value;
	c->value = temp_x.value;
	
	a->left = b;
	a->right = c;

	b->right = temp_z.left;

	c->left = temp_z.right;
	c->right = temp_x.right;
}

void rb_rotate_rr(rb_node_t *x, rb_node_t *y) {
	rb_node_t *a = x;
	rb_node_t *b = y;

	rb_node_t temp_x = *x;
	rb_node_t temp_y = *y;

	a->value = temp_y.value;
	b->value = temp_x.value;

	a->right = temp_y.right;
	a->left = b;
	
	b->left = temp_x.left;
	b->right = temp_y.left;
}

void rb_rotate_rl(rb_node_t *x, rb_node_t *y) {
	rb_node_t *z = y->right;

	rb_node_t *a = x;
	rb_node_t *b = y;
	rb_node_t *c = z;

	rb_node_t temp_x = *x;
	rb_node_t temp_y = *y;
	rb_node_t temp_z = *z;

	a->value = temp_z.value;
	c->value = temp_x.value;
	
	a->right = b;
	a->left = c;

	b->left = temp_z.right;

	c->right = temp_z.left;
	c->left = temp_x.left;
}

bool rb_can_step(rb_node_t *node, rb_node_t *new_node) {
	// left
	if (node->value > new_node->value) {
		if (node->left == RB_LEAF) {
			return false;
		}
	// right
	} else {
		if (node->right == RB_LEAF) {
			return false;
		}
	}

	return true;
}

bool rb_is_black(rb_node_t *node) {
	if (node == RB_LEAF) {
		return true;
	} else {
		return node->color == BLACK;
	}
}

static inline rb_node_t *rb_get_sibling(rb_node_t *node) {
	if (node->parent == NULL) {
		return NULL;
	}

	return node->parent->left == node ? node->parent->right : node->parent->left;
}

bool rb_is_sibling_red(rb_node_t *node) {
	rb_node_t *uncle = rb_get_sibling(node); 

	if (uncle == NULL /* ( == RB_LEAF ) */ || uncle->color == BLACK) {
		return false;
	}

	return true;
}

bool rb_is_left(rb_node_t *node, rb_node_t *other) {
	return node->left == other;
}

int rb_node_insert(rb_node_t *node, rb_node_t *x_node) {
	rb_node_t *current_node = node;

	for (;;) {
		bool can_step = rb_can_step(current_node, x_node);

		if (can_step == false) {
			break;
		}
		
		// right
		if (current_node->value <= x_node->value) {
			current_node = current_node->right;
		// left
		} else {
			current_node = current_node->left;
		}
	}
	
	x_node->parent = current_node;

	// right
	if (current_node->value <= x_node->value) {
		current_node->right = x_node;			
	// left
	} else {
		current_node->left = x_node;			
	}

	rb_node_t *parent_node = x_node->parent;

	while (parent_node != NULL && parent_node->color != BLACK) {
		rb_node_t *grandparent_node = parent_node->parent;

		if (rb_is_sibling_red(parent_node)) { // i.e. uncle of x_node
			rb_node_t *sibling_node = rb_get_sibling(parent_node);

			sibling_node->color = BLACK;
			parent_node->color = BLACK;

			if (grandparent_node->parent != NULL) { // i.e. if the grandparent node is not the root
				grandparent_node->color = RED;
			}
		} else {
			// left
			if (rb_is_left(grandparent_node, parent_node)) {
				// ll
				if (rb_is_left(parent_node, x_node)) {
					rb_rotate_ll(grandparent_node, parent_node);
				// lr
				} else {
					rb_rotate_lr(grandparent_node, parent_node);
				}
			// right
			} else {
				// rl
				if (rb_is_left(parent_node, x_node)) {
					rb_rotate_rl(grandparent_node, parent_node);
				// rr
				} else {
					rb_rotate_rr(grandparent_node, parent_node);	
				}
			}

		}					

		x_node = grandparent_node;
		parent_node = grandparent_node->parent;
	}

	return 0;
}


int rb_tree_insert(rb_tree_t *tree, int value) {
	rb_node_t *node = (rb_node_t *)malloc(sizeof(rb_node_t));
	node->value = value;
	node->parent = NULL;
	node->left = RB_LEAF;
	node->right = RB_LEAF;
	node->color = RED;

	if (tree->root != NULL) {
		if (rb_node_insert(tree->root, node) >= 0) {
			tree->size++;

			return 0;
		}

		return -1;
	}

	node->color = BLACK;

	tree->root = node;
	tree->size = 1;

	return 0;
}

void rb_tree_print(rb_tree_t* tree) {
	printf("Tree Size: %llu, ", tree->size);

	rb_node_stack_t node_stack = stack_create(tree->size); 
	rb_node_stack_t temp_stack = stack_create(tree->size);

	stack_push(&temp_stack, tree->root);

	while (!stack_is_empty(&temp_stack)) {
		rb_node_t *node = stack_pop(&temp_stack);	

		if (node->left != NULL) {
			stack_push(&temp_stack, node->left);
		}

		if (node->right != NULL) {
			stack_push(&temp_stack, node->right);
		}

		stack_push(&node_stack, node);
	}


	while (!stack_is_empty(&node_stack)) {
		rb_node_t *node = stack_pop(&node_stack);

		if (node->color == BLACK) {
			printf("(B%d)", node->value);
		} else {
			printf("(R%d)", node->value);
		}
	}

	printf("\n");

	stack_free(&node_stack);
	stack_free(&temp_stack);
}

bool rb_tree_find();

rb_node_t *rb_tree_remove();

int main() {
	rb_tree_t tree = rb_tree_create(10);
	rb_tree_print(&tree);
	rb_tree_insert(&tree, 7);
	rb_tree_print(&tree);
	rb_tree_insert(&tree, 3);
	rb_tree_print(&tree);
	rb_tree_insert(&tree, 18);
	rb_tree_print(&tree);
	rb_tree_insert(&tree, 10);
	rb_tree_print(&tree);
	rb_tree_insert(&tree, 22);
	rb_tree_print(&tree);
	rb_tree_insert(&tree, 26);
	rb_tree_print(&tree);
	rb_tree_insert(&tree, 8);
	rb_tree_print(&tree);
	rb_tree_insert(&tree, 11);
	rb_tree_print(&tree);
	rb_tree_insert(&tree, 15);
	rb_tree_print(&tree);
	rb_tree_free(&tree);
	return 0;
}
