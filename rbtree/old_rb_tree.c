#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct rb_node rb_node_t;

// START OF STACK IMPLEMENTATION

typedef struct rb_node_stack {
  rb_node_t **array;
  uint32_t size;
  int64_t top;
} rb_node_stack_t;

rb_node_stack_t stack_create(uint32_t size) {
  rb_node_stack_t stack = {.array =
                               (rb_node_t **)malloc(sizeof(rb_node_t *) * size),
                           .size = size,
                           .top = -1};

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

int stack_push(rb_node_stack_t *stack, rb_node_t *node) {
  if (stack->top + 1 >= stack->size) {
    return -1;
  }

  stack->array[++(stack->top)] = node;

  return 0;
}

bool stack_is_empty(rb_node_stack_t *stack) { return stack->top == -1; }

// END OF STACK IMPLEMENTATION

typedef enum rb_color { BLACK, RED } rb_color_t;

#define LEFT 0
#define RIGHT 1
#define DIR(PARENT, CHILD) ((PARENT->child[LEFT] == CHILD) ? LEFT : RIGHT)
#define LCHILD(PARENT) (PARENT->child[LEFT])
#define RCHILD(PARENT) (PARENT->child[RIGHT])
// #define left child[LEFT]
// #define right child[RIGHT]

struct rb_node {
  int value;
  rb_node_t *parent;
  rb_node_t *child[2];
  rb_color_t color;
};

// TAKEN FROM THE INTERNET
//
// https://gist.github.com/ximik777/e04e5a9f0548a2f41cb09530924bdd9a/

// printing tree in ascii

typedef struct asciinode_struct asciinode;

struct asciinode_struct {
  asciinode *left, *right;

  // length of the edge from this node to its children
  int edge_length;

  int height;

  int lablen;

  //-1=I am left, 0=I am root, 1=right
  int parent_dir;

  // max supported unit32 in dec, 10 digits max
  char label[11];
};

#define MAX_HEIGHT 1000
int lprofile[MAX_HEIGHT];
int rprofile[MAX_HEIGHT];
#define INFINITY (1 << 20)

// adjust gap between left and right nodes
int gap = 3;

// used for printing next node in the same level,
// this is the x coordinate of the next char printed
int print_next;

int MIN(int X, int Y) { return ((X) < (Y)) ? (X) : (Y); }

int MAX(int X, int Y) { return ((X) > (Y)) ? (X) : (Y); }

asciinode *build_ascii_tree_recursive(rb_node_t *t) {
  asciinode *node;

  if (t == NULL)
    return NULL;

  node = malloc(sizeof(asciinode));
  node->left = build_ascii_tree_recursive(LCHILD(t));
  node->right = build_ascii_tree_recursive(RCHILD(t));

  if (node->left != NULL) {
    node->left->parent_dir = -1;
  }

  if (node->right != NULL) {
    node->right->parent_dir = 1;
  }

  if (t->color == BLACK) {
    sprintf(node->label, "B%d", t->value);
  } else {
    sprintf(node->label, "R%d", t->value);
  }
  node->lablen = (int)strlen(node->label);

  return node;
}

// Copy the tree into the ascii node structure
asciinode *build_ascii_tree(rb_node_t *t) {
  asciinode *node;
  if (t == NULL)
    return NULL;
  node = build_ascii_tree_recursive(t);
  node->parent_dir = 0;
  return node;
}

// Free all the nodes of the given tree
void free_ascii_tree(asciinode *node) {
  if (node == NULL)
    return;
  free_ascii_tree(node->left);
  free_ascii_tree(node->right);
  free(node);
}

// The following function fills in the lprofile array for the given tree.
// It assumes that the centre of the label of the root of this tree
// is located at a position (x,y).  It assumes that the edge_length
// fields have been computed for this tree.
void compute_lprofile(asciinode *node, int x, int y) {
  int i, isleft;
  if (node == NULL)
    return;
  isleft = (node->parent_dir == -1);
  lprofile[y] = MIN(lprofile[y], x - ((node->lablen - isleft) / 2));
  if (node->left != NULL) {
    for (i = 1; i <= node->edge_length && y + i < MAX_HEIGHT; i++) {
      lprofile[y + i] = MIN(lprofile[y + i], x - i);
    }
  }
  compute_lprofile(node->left, x - node->edge_length - 1,
                   y + node->edge_length + 1);
  compute_lprofile(node->right, x + node->edge_length + 1,
                   y + node->edge_length + 1);
}

void compute_rprofile(asciinode *node, int x, int y) {
  int i, notleft;
  if (node == NULL)
    return;
  notleft = (node->parent_dir != -1);
  rprofile[y] = MAX(rprofile[y], x + ((node->lablen - notleft) / 2));
  if (node->right != NULL) {
    for (i = 1; i <= node->edge_length && y + i < MAX_HEIGHT; i++) {
      rprofile[y + i] = MAX(rprofile[y + i], x + i);
    }
  }
  compute_rprofile(node->left, x - node->edge_length - 1,
                   y + node->edge_length + 1);
  compute_rprofile(node->right, x + node->edge_length + 1,
                   y + node->edge_length + 1);
}

// This function fills in the edge_length and
// height fields of the specified tree
void compute_edge_lengths(asciinode *node) {
  int h, hmin, i, delta;
  if (node == NULL)
    return;
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

    // If the node has two children of height 1, then we allow the
    // two leaves to be within 1, instead of 2
    if (((node->left != NULL && node->left->height == 1) ||
         (node->right != NULL && node->right->height == 1)) &&
        delta > 4) {
      delta--;
    }

    node->edge_length = ((delta + 1) / 2) - 1;
  }

  // now fill in the height of node
  h = 1;
  if (node->left != NULL) {
    h = MAX(node->left->height + node->edge_length + 1, h);
  }
  if (node->right != NULL) {
    h = MAX(node->right->height + node->edge_length + 1, h);
  }
  node->height = h;
}

// This function prints the given level of the given tree, assuming
// that the node has the given x coordinate.
void print_level(asciinode *node, int x, int level) {
  int i, isleft;
  if (node == NULL)
    return;
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
    print_level(node->left, x - node->edge_length - 1,
                level - node->edge_length - 1);
    print_level(node->right, x + node->edge_length + 1,
                level - node->edge_length - 1);
  }
}

// prints ascii tree for given Tree structure
void print_ascii_tree(rb_node_t *t) {
  asciinode *proot;
  int xmin, i;
  if (t == NULL)
    return;
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
    printf("(This tree is taller than %d, and may be drawn incorrectly.)\n",
           MAX_HEIGHT);
  }
  free_ascii_tree(proot);
}

// THE ABOVE WAS TAKEN FROM THE INTERNET

static inline void rb_print_node(rb_node_t *node) {
  if (node == NULL) {
    printf("(nil)");
  } else {
    printf("%p {\n"
           "  .value=%d\n"
           "  .parent=%p\n"
           "  .child=[%p,%p]\n"
           "  .color=%s\n"
           "}\n",
           node, node->value, node->parent, node->child[LEFT],
           node->child[RIGHT], (node->color == BLACK) ? "BLACK" : "RED");
  }
}

typedef struct rb_tree {
  rb_node_t *root;
  uint64_t size;
} rb_tree_t;

#define NIL (NULL)

rb_tree_t rb_tree_create(int value) {
  rb_node_t *node = (rb_node_t *)malloc(sizeof(rb_node_t));
  node->value = value;
  node->parent = NULL;
  LCHILD(node) = NIL;
  RCHILD(node) = NIL;
  node->color = BLACK;

  rb_tree_t tree = {.root = node, .size = 1};

  return tree;
}

void rb_tree_free(rb_tree_t *tree) {
  rb_node_stack_t node_stack = stack_create(tree->size);
  rb_node_stack_t temp_stack = stack_create(tree->size);

  stack_push(&temp_stack, tree->root);

  while (!stack_is_empty(&temp_stack)) {
    rb_node_t *node = stack_pop(&temp_stack);

    if (LCHILD(node) != NULL) {
      stack_push(&temp_stack, LCHILD(node));
    }

    if (RCHILD(node) != NULL) {
      stack_push(&temp_stack, RCHILD(node));
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

/*

The below describes a left rotation.

   Parent (or NULL)        Parent (or NULL)
     \                         \
    Node                        \
      \                        Child
      Child        =>          /   \
      /  \                   Node   \
     /    \                    \     Grandchild
    T  Grandchild               T

*/

void rb_rotate(rb_node_t *node, int direction) {
  if (node == NULL)
    return;

  rb_node_t *child = node->child[1 - direction];

  if (child == NULL)
    return;

  rb_node_t *grandchild = child->child[1 - direction];

  if (node->parent == NULL) {
    child->parent = NULL;
  } else {
    child->parent = node->parent;

    node->parent->child[DIR(node->parent, node)] = child;
  }

  node->child[1 - direction] = child->child[direction];

  if (node->child[1 - direction] != NIL)
    node->child[1 - direction]->parent = node->child[1 - direction];

  child->child[direction] = node;

  node->parent = child;
}

#define VALUEDIR(X, Y) ((X->value < Y->value) ? RIGHT : LEFT)

bool rb_can_step(rb_node_t *node, rb_node_t *new_node) {
  return node->child[VALUEDIR(node, new_node)] != NIL;
}

bool rb_is_black(rb_node_t *node) {
  return node == NIL || node->color == BLACK;
}

rb_node_t *rb_get_sibling(rb_node_t *node) {
  if (node->parent == NULL) {
    return NULL;
  }

  return node->parent->child[1 - DIR(node->parent, node)];
}

bool rb_is_sibling_red(rb_node_t *node) {
  return !rb_is_black(rb_get_sibling(node));
}

void rb_node_insert(rb_tree_t *tree, rb_node_t *node, rb_node_t *x_node) {
  rb_node_t *current_node = node;

  while (rb_can_step(current_node, x_node)) {
    current_node = current_node->child[VALUEDIR(current_node, x_node)];
  }

  x_node->parent = current_node;

  current_node->child[VALUEDIR(current_node, x_node)] = x_node;

  rb_node_t *parent_node = current_node;

  // note: if the parent does not have a parent then it is the
  // root

  while (parent_node != NULL && parent_node->color != BLACK) {
    rb_node_t *grandparent_node = parent_node->parent;

    if (rb_is_sibling_red(parent_node)) { // i.e. uncle of x_node
      rb_node_t *sibling_node = rb_get_sibling(parent_node);

      sibling_node->color = BLACK;
      parent_node->color = BLACK;

      if (grandparent_node->parent != NULL) {
        grandparent_node->color = RED;
      }
    } else {
      int direction = DIR(parent_node, x_node);
      int parent_direction = DIR(grandparent_node, parent_node);

      if (parent_direction == direction) {
        rb_rotate(grandparent_node, 1 - parent_direction);

        grandparent_node->color = RED;
        parent_node->color = BLACK;

        if (parent_node->parent == NULL)
          tree->root = parent_node;
      } else {
        rb_rotate(parent_node, 1 - direction);
        rb_rotate(grandparent_node, 1 - parent_direction);

        grandparent_node->color = RED;
        x_node->color = BLACK;

        if (x_node->parent == NULL)
          tree->root = x_node;
      }

      return;
    }

    x_node = grandparent_node;
    parent_node = grandparent_node->parent;
  }
}

void rb_tree_insert(rb_tree_t *tree, int value) {
  rb_node_t *node = (rb_node_t *)malloc(sizeof(rb_node_t));
  node->value = value;
  node->parent = NULL;
  RCHILD(node) = NIL;
  LCHILD(node) = NIL;
  node->color = RED;

  if (tree->root != NULL) {
    rb_node_insert(tree, tree->root, node);

    tree->size++;

    return;
  }

  node->color = BLACK;

  tree->root = node;
  tree->size = 1;
}

void rb_tree_print(rb_tree_t *tree) {
#ifdef __APPLE__
  printf("Tree Size: %llu, ", tree->size);
#elif __linux__
  printf("Tree Size: %lu, ", tree->size);
#endif

  rb_node_stack_t node_stack = stack_create(tree->size);
  rb_node_stack_t temp_stack = stack_create(tree->size);

  stack_push(&temp_stack, tree->root);

  while (!stack_is_empty(&temp_stack)) {
    rb_node_t *node = stack_pop(&temp_stack);

    if (LCHILD(node) != NULL) {
      stack_push(&temp_stack, LCHILD(node));
    }

    if (RCHILD(node) != NULL) {
      stack_push(&temp_stack, RCHILD(node));
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

static inline bool rb_is_leaf(rb_node_t *node) {
  return LCHILD(node) == NIL && RCHILD(node) == NIL;
}

rb_node_t *rb_find_max(rb_node_t *node) {
  rb_node_t *current_node = node;

  while (RCHILD(current_node)) {
    current_node = RCHILD(current_node);
  }

  return current_node;
}

int rb_node_remove(rb_node_t *node, int value) {
  rb_node_t *current_node = node;

  bool found = false;

  while (!found && current_node) {
    if (current_node->value == value) {
      found = true;
    }

    current_node =
        current_node->child[current_node->value < value ? RIGHT : LEFT];
  }

  if (!found) {
    return -1;
  }

  rb_node_t *update_node;

  if (LCHILD(current_node) == NIL && RCHILD(current_node) == NIL) {
    if (current_node->color == RED) {
      rb_node_t *parent_node = current_node->parent;

      parent_node->child[DIR(parent_node, current_node)] = NIL;

      free(current_node);
    } else { // black leaf
      return 0;
    }
  }

  if (LCHILD(current_node) != NIL && RCHILD(current_node) != NIL) {
    rb_node_t *max_node = rb_find_max(LCHILD(current_node));

    current_node->value = max_node->value;

    if (LCHILD(max_node)) {
      max_node->value = LCHILD(max_node)->value;

      free(LCHILD(max_node));

      LCHILD(max_node) = NIL;
    } else { // max_node is a leaf node
      if (max_node->color == RED) {
        rb_node_t *parent_node = current_node->parent;

        parent_node->child[DIR(parent_node, max_node)] = NIL;

        free(max_node);
      } else { // black leaf
        return 0;
      }
    }
  }

#define NILDIR(X) ((LCHILD(X) == NIL) ? LEFT : RIGHT)
  


    {
      if (LCHILD(current_node) != NIL &&
          RCHILD(current_node) == NIL) { // left node only
        rb_node_t *left_node = LCHILD(current_node);

        current_node->value = left_node->value;
        LCHILD(current_node) = NIL;

        free(left_node);
      } else if (LCHILD(current_node) == NIL &&
                 RCHILD(current_node) != NIL) { // right node only
        rb_node_t *right_node = current_node->right;

        current_node->value = right_node->value;
        current_node->right = RB_LEAF;

        free(right_node);
      } else { // both children are present
        rb_node_t *max_node = rb_find_max(current_node->left);

        printf("Max\n");
        rb_print_node(max_node);

        current_node->value = max_node->value;

        if (max_node->left) {
          max_node->value = max_node->left->value;
          max_node->left = RB_LEAF;

          free(max_node->left);
        } else { // max_node is a leaf node
          if (max_node->color == RED) {
            rb_node_t *parent_node = max_node->parent;

            if (rb_is_left(parent_node, max_node)) {
              parent_node->left = RB_LEAF;
            } else {
              parent_node->right = RB_LEAF;
            }

            free(max_node);
          } else { // black leaf
          }
        }
      }
    }

    avl_tree_update(update_node);

    return 0;
  }

  int rb_tree_remove(rb_tree_t * tree, int value) {
    if (tree->root == NULL) {
      return 0;
    }

    if (rb_node_remove(tree->root, value) >= 0) { // >= 0 => found
      tree->size--;

      if (tree->size == 0) {
        tree->root = NULL;
      }

      return 0;
    }

    return -1;
  }

  int main() {
    rb_tree_t tree = rb_tree_create(7);
    print_ascii_tree(tree.root);
    rb_tree_insert(&tree, 3);
    rb_print_node(tree.root);
    print_ascii_tree(tree.root);
    rb_tree_insert(&tree, 18);
    print_ascii_tree(tree.root);
    rb_tree_insert(&tree, 10);
    print_ascii_tree(tree.root);
    rb_tree_insert(&tree, 22);
    print_ascii_tree(tree.root);
    rb_tree_insert(&tree, 26);
    print_ascii_tree(tree.root);
    rb_tree_insert(&tree, 8);
    print_ascii_tree(tree.root);
    rb_tree_insert(&tree, 11);
    rb_tree_print(&tree);

    printf("before first tree\n");
    print_ascii_tree(tree.root);
    rb_tree_insert(&tree, 15);
    print_ascii_tree(tree.root);

    rb_tree_free(&tree);
    return 0;
  }
