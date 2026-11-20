#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct Node {
    int key, h;
    struct Node *l, *r;
} Node;

long long rotations = 0;

int ht(Node *n) { return n ? n->h : 0; }
int max(int a, int b) { return a > b ? a : b; }
void fix(Node *n) { n->h = 1 + max(ht(n->l), ht(n->r)); }
int balance(Node *n) { return n ? ht(n->l) - ht(n->r) : 0; }

Node *rotate_right(Node *y)
{
    Node *x = y->l;
    Node *t2 = x->r;
    x->r = y;
    y->l = t2;
    fix(y);
    fix(x);
    rotations++;
    return x;
}

Node *rotate_left(Node *x)
{
    Node *y = x->r;
    Node *t2 = y->l;
    y->l = x;
    x->r = t2;
    fix(x);
    fix(y);
    rotations++;
    return y;
}

Node *insert(Node *n, int key)
{
    if (!n) {
        Node *p = malloc(sizeof *p);
        if (!p) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        p->key = key;
        p->h = 1;
        p->l = p->r = NULL;
        return p;
    }

    if (key < n->key) n->l = insert(n->l, key);
    else if (key > n->key) n->r = insert(n->r, key);
    else return n;

    fix(n);
    int bal = balance(n);

    if (bal > 1 && key < n->l->key) return rotate_right(n); /* LL */
    if (bal < -1 && key > n->r->key) return rotate_left(n); /* RR */

    if (bal > 1 && key > n->l->key) {                      /* LR */
        n->l = rotate_left(n->l);
        return rotate_right(n);
    }
    if (bal < -1 && key < n->r->key) {                     /* RL */
        n->r = rotate_right(n->r);
        return rotate_left(n);
    }
    return n;
}

void preorder(Node *n)
{
    if (!n) return;
    printf("%d ", n->key);
    preorder(n->l);
    preorder(n->r);
}

int verify(Node *n)
{
    if (!n) return 1;
    int b = balance(n);
    if (b < -1 || b > 1) return 0;
    return verify(n->l) && verify(n->r);
}

void destroy(Node *n)
{
    if (n) {
        destroy(n->l);
        destroy(n->r);
        free(n);
    }
}

/* Ordinary BST used only for the 100,000-key height comparison. */
typedef struct BNode {
    int key;
    struct BNode *l, *r;
} BNode;

BNode *bst_insert(BNode *n, int key)
{
    if (!n) {
        BNode *p = malloc(sizeof *p);
        if (!p) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        p->key = key;
        p->l = p->r = NULL;
        return p;
    }
    if (key < n->key) n->l = bst_insert(n->l, key);
    else if (key > n->key) n->r = bst_insert(n->r, key);
    return n;
}

int bst_height(BNode *n)
{
    if (!n) return 0;
    return 1 + max(bst_height(n->l), bst_height(n->r));
}

void bst_destroy(BNode *n)
{
    if (n) {
        bst_destroy(n->l);
        bst_destroy(n->r);
        free(n);
    }
}

int main(void)
{
    int prices[] = {10, 20, 30, 40, 50, 25};
    Node *root = NULL;

    printf("AVL tree after each insertion:\n");
    for (int i = 0; i < 6; i++) {
        root = insert(root, prices[i]);
        printf("After %d: ", prices[i]);
        preorder(root);
        printf("\n");
    }

    printf("\nFinal preorder: ");
    preorder(root);
    printf("\nheight = %d\n", ht(root));
    printf("Balance verification: %s\n", verify(root) ? "PASS" : "FAIL");
    destroy(root);

    /* Compare an ordinary BST and AVL tree for 100,000 sorted keys. */
    const int N = 100000;
    BNode *bst = NULL;
    Node *avl = NULL;
    rotations = 0;

    clock_t start_bst = clock();
    for (int i = 1; i <= N; i++) bst = bst_insert(bst, i);
    clock_t end_bst = clock();

    clock_t start_avl = clock();
    for (int i = 1; i <= N; i++) avl = insert(avl, i);
    clock_t end_avl = clock();

    printf("\n100,000 sorted keys:\n");
    printf("BST height = %d\n", bst_height(bst));
    printf("AVL height = %d\n", ht(avl));
    printf("AVL rotations = %lld\n", rotations);
    printf("AVL balance verification: %s\n", verify(avl) ? "PASS" : "FAIL");
    printf("BST insertion time = %.3f seconds\n",
           (double)(end_bst - start_bst) / CLOCKS_PER_SEC);
    printf("AVL insertion time = %.3f seconds\n",
           (double)(end_avl - start_avl) / CLOCKS_PER_SEC);

    bst_destroy(bst);
    destroy(avl);
    return 0;
}
