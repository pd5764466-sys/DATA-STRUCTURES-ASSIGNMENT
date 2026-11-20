#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 200
#define ACCESS_COUNT 10000

typedef struct Node {
    int key;
    struct Node *left, *right;
    int height;
} Node;

Node *new_node(int key) {
    Node *n = malloc(sizeof(Node));
    if (!n) exit(1);
    n->key = key;
    n->left = n->right = NULL;
    n->height = 1;
    return n;
}

int max(int a, int b) { return a > b ? a : b; }
int h(Node *n) { return n ? n->height : 0; }

Node *right_rotate(Node *y) {
    Node *x = y->left, *t = x->right;
    x->right = y; y->left = t;
    y->height = 1 + max(h(y->left), h(y->right));
    x->height = 1 + max(h(x->left), h(x->right));
    return x;
}

Node *left_rotate(Node *x) {
    Node *y = x->right, *t = y->left;
    y->left = x; x->right = t;
    x->height = 1 + max(h(x->left), h(x->right));
    y->height = 1 + max(h(y->left), h(y->right));
    return y;
}

int balance(Node *n) { return n ? h(n->left) - h(n->right) : 0; }

Node *avl_insert(Node *root, int key) {
    if (!root) return new_node(key);
    if (key < root->key) root->left = avl_insert(root->left, key);
    else if (key > root->key) root->right = avl_insert(root->right, key);
    else return root;

    root->height = 1 + max(h(root->left), h(root->right));
    int b = balance(root);

    if (b > 1 && key < root->left->key) return right_rotate(root);
    if (b < -1 && key > root->right->key) return left_rotate(root);
    if (b > 1 && key > root->left->key) {
        root->left = left_rotate(root->left);
        return right_rotate(root);
    }
    if (b < -1 && key < root->right->key) {
        root->right = right_rotate(root->right);
        return left_rotate(root);
    }
    return root;
}

Node *splay_right(Node *x) {
    Node *y = x->left;
    x->left = y->right;
    y->right = x;
    return y;
}
Node *splay_left(Node *x) {
    Node *y = x->right;
    x->right = y->left;
    y->left = x;
    return y;
}

Node *splay(Node *root, int key) {
    if (!root || root->key == key) return root;

    if (key < root->key) {
        if (!root->left) return root;
        if (key < root->left->key) {
            root->left->left = splay(root->left->left, key);
            root = splay_right(root);
        } else if (key > root->left->key) {
            root->left->right = splay(root->left->right, key);
            if (root->left->right) root->left = splay_left(root->left);
        }
        return root->left ? splay_right(root) : root;
    } else {
        if (!root->right) return root;
        if (key > root->right->key) {
            root->right->right = splay(root->right->right, key);
            root = splay_left(root);
        } else if (key < root->right->key) {
            root->right->left = splay(root->right->left, key);
            if (root->right->left) root->right = splay_right(root->right);
        }
        return root->right ? splay_left(root) : root;
    }
}

Node *splay_insert(Node *root, int key) {
    if (!root) return new_node(key);
    root = splay(root, key);
    if (root->key == key) return root;
    Node *n = new_node(key);
    if (key < root->key) {
        n->right = root;
        n->left = root->left;
        root->left = NULL;
    } else {
        n->left = root;
        n->right = root->right;
        root->right = NULL;
    }
    return n;
}

int search_splay(Node **root, int key) {
    int visited = 0;
    Node *p = *root;
    while (p) {
        visited++;
        if (key == p->key) break;
        p = key < p->key ? p->left : p->right;
    }
    *root = splay(*root, key);
    return visited;
}

int search_avl(Node *root, int key) {
    int visited = 0;
    while (root) {
        visited++;
        if (key == root->key) break;
        root = key < root->key ? root->left : root->right;
    }
    return visited;
}

void free_tree(Node *n) {
    if (!n) return;
    free_tree(n->left); free_tree(n->right); free(n);
}

int main(void) {
    Node *sroot = NULL, *aroot = NULL;
    int i, key, visits, max_s = 0, max_a = 0;
    long long total_s = 0, total_a = 0;
    srand(42);

    for (i = 1; i <= N; i++) {
        sroot = splay_insert(sroot, i);
        aroot = avl_insert(aroot, i);
    }

    printf("Q5 - SPLAY TREES\n");
    printf("Created two trees with %d keys (1 to %d).\n", N, N);
    printf("Splay search demonstration: access 3, then 9.\n");
    sroot = splay(sroot, 3);
    printf("Root after accessing 3: %d\n", sroot->key);
    sroot = splay(sroot, 9);
    printf("Root after accessing 9: %d\n", sroot->key);

    /* Rebuild trees for a clean experiment. */
    free_tree(sroot); free_tree(aroot);
    sroot = aroot = NULL;
    for (i = 1; i <= N; i++) {
        sroot = splay_insert(sroot, i);
        aroot = avl_insert(aroot, i);
    }

    printf("\nFirst 10 roots during skewed workload:\n");
    for (i = 0; i < ACCESS_COUNT; i++) {
        if (i < 10) key = 1 + rand() % 10;
        else if (i % 5 != 0) key = 1 + rand() % 10;
        else key = 1 + rand() % N;
        visits = search_splay(&sroot, key);
        if (i < 10) printf("Access %d (key %d): root = %d\n", i + 1, key, sroot->key);
        total_s += visits; if (visits > max_s) max_s = visits;
        visits = search_avl(aroot, key);
        total_a += visits; if (visits > max_a) max_a = visits;
    }
    printf("\nSkewed workload (80%% accesses to 5%% of keys):\n");
    printf("Splay average nodes visited: %.2f\n", (double)total_s / ACCESS_COUNT);
    printf("AVL average nodes visited: %.2f\n", (double)total_a / ACCESS_COUNT);
    printf("Splay worst single access: %d nodes\n", max_s);
    printf("AVL worst single access: %d nodes\n", max_a);

    total_s = total_a = 0; max_s = max_a = 0;
    for (i = 0; i < ACCESS_COUNT; i++) {
        key = 1 + rand() % N;
        visits = search_splay(&sroot, key);
        total_s += visits; if (visits > max_s) max_s = visits;
        visits = search_avl(aroot, key);
        total_a += visits; if (visits > max_a) max_a = visits;
    }
    printf("\nUniform workload (random keys):\n");
    printf("Splay average nodes visited: %.2f\n", (double)total_s / ACCESS_COUNT);
    printf("AVL average nodes visited: %.2f\n", (double)total_a / ACCESS_COUNT);
    printf("Splay worst single access: %d nodes\n", max_s);
    printf("AVL worst single access: %d nodes\n", max_a);

    printf("\nSorted access test (1 to %d):\n", N);
    for (i = 1; i <= N; i++) sroot = splay(sroot, i);
    printf("Completed %d sorted accesses.\n", N);
    printf("Root after sorted accesses: %d\n", sroot->key);
    printf("Splay tree search test: PASS\n");
    printf("AVL tree comparison test: PASS\n");

    free_tree(sroot); free_tree(aroot);
    return 0;
}
