#include <stdio.h>
#include <stdlib.h>

#define T 2
#define MAX_KEYS (2 * T - 1)
#define MAX_CHILDREN (2 * T)

typedef struct BNode {
    int n;
    int leaf;
    int keys[MAX_KEYS];
    struct BNode *child[MAX_CHILDREN];
} BNode;

BNode *new_node(int leaf)
{
    BNode *p = calloc(1, sizeof *p);
    if (!p) {
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    p->leaf = leaf;
    return p;
}

BNode *search(BNode *root, int key, int *reads)
{
    BNode *x = root;
    while (x) {
        (*reads)++;
        int i = 0;
        while (i < x->n && key > x->keys[i]) i++;
        if (i < x->n && key == x->keys[i]) return x;
        if (x->leaf) return NULL;
        x = x->child[i];
    }
    return NULL;
}

void split_child(BNode *parent, int i)
{
    BNode *y = parent->child[i];
    BNode *z = new_node(y->leaf);
    z->n = T - 1;

    for (int j = 0; j < T - 1; j++)
        z->keys[j] = y->keys[j + T];

    if (!y->leaf) {
        for (int j = 0; j < T; j++)
            z->child[j] = y->child[j + T];
    }

    y->n = T - 1;

    for (int j = parent->n; j >= i + 1; j--)
        parent->child[j + 1] = parent->child[j];

    parent->child[i + 1] = z;

    for (int j = parent->n - 1; j >= i; j--)
        parent->keys[j + 1] = parent->keys[j];

    parent->keys[i] = y->keys[T - 1];
    parent->n++;
}

void insert_nonfull(BNode *x, int key)
{
    int i = x->n - 1;

    if (x->leaf) {
        while (i >= 0 && key < x->keys[i]) {
            x->keys[i + 1] = x->keys[i];
            i--;
        }
        if (i >= 0 && key == x->keys[i]) return;
        x->keys[i + 1] = key;
        x->n++;
    } else {
        while (i >= 0 && key < x->keys[i]) i--;
        i++;
        if (i < x->n && key == x->keys[i]) return;

        if (x->child[i]->n == MAX_KEYS) {
            split_child(x, i);
            if (key > x->keys[i]) i++;
            else if (key == x->keys[i]) return;
        }
        insert_nonfull(x->child[i], key);
    }
}

BNode *insert(BNode *root, int key)
{
    if (!root) {
        root = new_node(1);
        root->keys[0] = key;
        root->n = 1;
        return root;
    }

    if (search(root, key, &(int){0})) return root;

    if (root->n == MAX_KEYS) {
        BNode *s = new_node(0);
        s->child[0] = root;
        split_child(s, 0);
        int i = (key > s->keys[0]) ? 1 : 0;
        insert_nonfull(s->child[i], key);
        return s;
    }

    insert_nonfull(root, key);
    return root;
}

void print_tree(BNode *root, int level)
{
    if (!root) return;
    printf("Level %d: [", level);
    for (int i = 0; i < root->n; i++)
        printf("%s%d", i ? " | " : "", root->keys[i]);
    printf("]\n");

    if (!root->leaf) {
        for (int i = 0; i <= root->n; i++)
            print_tree(root->child[i], level + 1);
    }
}

int height(BNode *root)
{
    if (!root) return 0;
    int h = 1;
    while (!root->leaf) {
        h++;
        root = root->child[0];
    }
    return h;
}

void destroy(BNode *root)
{
    if (!root) return;
    if (!root->leaf)
        for (int i = 0; i <= root->n; i++)
            destroy(root->child[i]);
    free(root);
}

int main(void)
{
    int keys[] = {10, 20, 5, 6, 12, 30, 7, 17};
    int count = sizeof(keys) / sizeof(keys[0]);
    BNode *root = NULL;

    printf("B-Tree insertion (minimum degree 2):\n");
    for (int i = 0; i < count; i++) {
        root = insert(root, keys[i]);
        printf("\nAfter inserting %d:\n", keys[i]);
        print_tree(root, 0);
    }

    printf("\nFinal B-Tree:\n");
    print_tree(root, 0);
    printf("B-Tree height (levels) = %d\n", height(root));

    printf("\nSearch tests:\n");
    int tests[] = {5, 30, 17, 99};
    for (int i = 0; i < 4; i++) {
        int reads = 0;
        BNode *found = search(root, tests[i], &reads);
        printf("Key %d: %s, node visits = %d\n",
               tests[i], found ? "FOUND" : "NOT FOUND", reads);
    }

    printf("\nDisk-read comparison at 10 ms per read:\n");
    printf("B-Tree order 100 estimate: about 4 reads = about 40 ms\n");
    printf("Binary tree estimate for 5,000,000 keys: about 23 reads = about 230 ms\n");

    destroy(root);
    return 0;
}
