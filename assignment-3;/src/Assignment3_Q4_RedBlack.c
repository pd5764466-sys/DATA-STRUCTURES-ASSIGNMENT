#include <stdio.h>
#include <stdlib.h>

typedef enum { RED, BLACK } Color;

typedef struct Node {
    int key;
    Color color;
    struct Node *left, *right, *parent;
} Node;

Node *root = NULL;
long long rotations = 0;

Node *new_node(int key)
{
    Node *n = malloc(sizeof *n);
    if (!n) {
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    n->key = key;
    n->color = RED;
    n->left = n->right = n->parent = NULL;
    return n;
}

Color color_of(Node *n) { return n ? n->color : BLACK; }

void left_rotate(Node *x)
{
    Node *y = x->right;
    x->right = y->left;
    if (y->left) y->left->parent = x;
    y->parent = x->parent;

    if (!x->parent) root = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;

    y->left = x;
    x->parent = y;
    rotations++;
}

void right_rotate(Node *y)
{
    Node *x = y->left;
    y->left = x->right;
    if (x->right) x->right->parent = y;
    x->parent = y->parent;

    if (!y->parent) root = x;
    else if (y == y->parent->left) y->parent->left = x;
    else y->parent->right = x;

    x->right = y;
    y->parent = x;
    rotations++;
}

void fix_insert(Node *z)
{
    while (z->parent && z->parent->color == RED) {
        Node *p = z->parent;
        Node *g = p->parent;

        if (p == g->left) {
            Node *u = g->right;
            if (color_of(u) == RED) {
                p->color = BLACK;
                u->color = BLACK;
                g->color = RED;
                z = g;
            } else {
                if (z == p->right) {
                    z = p;
                    left_rotate(z);
                    p = z->parent;
                    g = p->parent;
                }
                p->color = BLACK;
                g->color = RED;
                right_rotate(g);
            }
        } else {
            Node *u = g->left;
            if (color_of(u) == RED) {
                p->color = BLACK;
                u->color = BLACK;
                g->color = RED;
                z = g;
            } else {
                if (z == p->left) {
                    z = p;
                    right_rotate(z);
                    p = z->parent;
                    g = p->parent;
                }
                p->color = BLACK;
                g->color = RED;
                left_rotate(g);
            }
        }
    }
    root->color = BLACK;
}

void insert(int key)
{
    Node *z = new_node(key);
    Node *y = NULL, *x = root;

    while (x) {
        y = x;
        if (key == x->key) {
            free(z);
            return;
        }
        x = key < x->key ? x->left : x->right;
    }

    z->parent = y;
    if (!y) root = z;
    else if (key < y->key) y->left = z;
    else y->right = z;

    fix_insert(z);
}

Node *minimum(Node *n)
{
    if (!n) return NULL;
    while (n->left) n = n->left;
    return n;
}

int verify_red_black(Node *n, int black_count, int *expected)
{
    if (!n) {
        if (*expected == -1) *expected = black_count;
        return black_count == *expected;
    }

    if (n->color == BLACK) black_count++;

    if (n->color == RED &&
        (color_of(n->left) == RED || color_of(n->right) == RED))
        return 0;

    return verify_red_black(n->left, black_count, expected) &&
           verify_red_black(n->right, black_count, expected);
}

int valid_tree(void)
{
    if (root && root->color != BLACK) return 0;
    int expected = -1;
    return verify_red_black(root, 0, &expected);
}

int height(Node *n)
{
    if (!n) return 0;
    int l = height(n->left), r = height(n->right);
    return 1 + (l > r ? l : r);
}

void inorder(Node *n)
{
    if (!n) return;
    inorder(n->left);
    printf("%d(%c) ", n->key, n->color == RED ? 'R' : 'B');
    inorder(n->right);
}

void destroy(Node *n)
{
    if (!n) return;
    destroy(n->left);
    destroy(n->right);
    free(n);
}

int main(void)
{
    int sample[] = {10, 20, 30, 15, 25, 5};
    int n = sizeof(sample) / sizeof(sample[0]);

    printf("Red-Black Tree insertion:\n");
    for (int i = 0; i < n; i++) {
        insert(sample[i]);
        printf("After %d: ", sample[i]);
        inorder(root);
        printf("| valid: %s\n", valid_tree() ? "PASS" : "FAIL");
    }

    Node *smallest = minimum(root);
    printf("\nSmallest key (leftmost node): %d\n", smallest->key);
    printf("Tree height: %d\n", height(root));
    printf("Rotations for sample insertions: %lld\n", rotations);
    printf("Red-Black properties verification: %s\n",
           valid_tree() ? "PASS" : "FAIL");

    destroy(root);
    root = NULL;
    rotations = 0;

    printf("\nIncreasing-order insertion test (1 to 10000):\n");
    for (int i = 1; i <= 10000; i++) insert(i);
    printf("Nodes inserted: 10000\n");
    printf("Tree height: %d\n", height(root));
    printf("Rotations: %lld\n", rotations);
    printf("Red-Black properties verification: %s\n",
           valid_tree() ? "PASS" : "FAIL");
    printf("Smallest key: %d\n", minimum(root)->key);

    destroy(root);
    return 0;
}
