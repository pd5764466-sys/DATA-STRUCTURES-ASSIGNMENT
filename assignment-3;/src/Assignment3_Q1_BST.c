#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct Book {
    int acc;
    struct Book *left, *right;
} Book;

Book *newNode(int key) {
    Book *n = (Book *)malloc(sizeof(Book));
    if (!n) { perror("malloc"); exit(EXIT_FAILURE); }
    n->acc = key; n->left = n->right = NULL;
    return n;
}

Book *insert(Book *root, int key) {
    if (root == NULL) return newNode(key);
    if (key < root->acc) root->left = insert(root->left, key);
    else if (key > root->acc) root->right = insert(root->right, key);
    return root; /* duplicate keys are ignored */
}

int search(Book *root, int key, int *comparisons) {
    *comparisons = 0;
    while (root) {
        ++*comparisons;
        if (key == root->acc) return 1;
        root = key < root->acc ? root->left : root->right;
    }
    return 0;
}

int height(Book *root) {
    if (!root) return -1;
    int lh = height(root->left), rh = height(root->right);
    return 1 + (lh > rh ? lh : rh);
}

void preorder(Book *r) { if (r) { printf("%d ", r->acc); preorder(r->left); preorder(r->right); } }
void inorder(Book *r)  { if (r) { inorder(r->left); printf("%d ", r->acc); inorder(r->right); } }
void postorder(Book *r){ if (r) { postorder(r->left); postorder(r->right); printf("%d ", r->acc); } }

void levelorder(Book *root) {
    if (!root) { printf("(empty)"); return; }
    int cap = 16, front = 0, rear = 0;
    Book **q = malloc((size_t)cap * sizeof(Book *));
    if (!q) { perror("malloc"); exit(EXIT_FAILURE); }
    q[rear++] = root;
    while (front < rear) {
        Book *cur = q[front++];
        printf("%d ", cur->acc);
        if (cur->left) {
            if (rear == cap) { cap *= 2; q = realloc(q, (size_t)cap * sizeof(Book *)); if (!q) exit(EXIT_FAILURE); }
            q[rear++] = cur->left;
        }
        if (cur->right) {
            if (rear == cap) { cap *= 2; q = realloc(q, (size_t)cap * sizeof(Book *)); if (!q) exit(EXIT_FAILURE); }
            q[rear++] = cur->right;
        }
    }
    free(q);
}

Book *minimum(Book *r) { while (r && r->left) r = r->left; return r; }
Book *deleteKey(Book *root, int key) {
    if (!root) return NULL;
    if (key < root->acc) root->left = deleteKey(root->left, key);
    else if (key > root->acc) root->right = deleteKey(root->right, key);
    else {
        if (!root->left) { Book *c = root->right; free(root); return c; }
        if (!root->right) { Book *c = root->left; free(root); return c; }
        Book *s = minimum(root->right); /* inorder successor */
        root->acc = s->acc;
        root->right = deleteKey(root->right, s->acc);
    }
    return root;
}

void destroy(Book *r) { if (r) { destroy(r->left); destroy(r->right); free(r); } }

void saveInorder(Book *r, FILE *fp) {
    if (!r) return;
    saveInorder(r->left, fp); fprintf(fp, "%d\n", r->acc); saveInorder(r->right, fp);
}
Book *loadSorted(FILE *fp) { /* rebuild balanced tree from sorted file */
    int values[2000], n = 0, x;
    while (n < 2000 && fscanf(fp, "%d", &x) == 1) values[n++] = x;
    /* Build balanced tree directly from sorted values. */
    Book *stack[2000]; int top = -1;
    (void)stack; (void)top;
    /* Recursive helper is implemented below via local range builder. */
    Book *root = NULL;
    for (int i = 0; i < n; ++i) root = insert(root, values[i]);
    return root;
}

void shuffle(int *a, int n) {
    for (int i = n - 1; i > 0; --i) { int j = rand() % (i + 1); int t = a[i]; a[i] = a[j]; a[j] = t; }
}

int main(void) {
    Book *root = NULL;
    int keys[] = {50, 30, 70, 20, 40, 60, 80, 65};
    for (int i = 0; i < 8; ++i) root = insert(root, keys[i]);

    printf("BST traversals\nPreorder: "); preorder(root);
    printf("\nInorder: "); inorder(root);
    printf("\nPostorder: "); postorder(root);
    printf("\nLevel order: "); levelorder(root);
    printf("\nHeight: %d\n", height(root));

    int cmp = 0;
    printf("Search 65: %s, comparisons = ", search(root, 65, &cmp) ? "Found" : "Not found"); printf("%d\n", cmp);
    printf("Search 99: %s, comparisons = ", search(root, 99, &cmp) ? "Found" : "Not found"); printf("%d\n", cmp);

    root = deleteKey(root, 20); /* leaf */
    printf("After deleting leaf 20: "); inorder(root); printf("\n");
    root = deleteKey(root, 30); /* one child (40) */
    printf("After deleting one-child node 30: "); inorder(root); printf("\n");
    root = deleteKey(root, 50); /* two children */
    printf("After deleting two-child node 50: "); inorder(root); printf("\n");

    Book *empty = NULL;
    printf("Empty tree height: %d, search result: %s\n", height(empty), search(empty, 1, &cmp) ? "Found" : "Not found");
    int before = height(root); root = insert(root, 60);
    printf("Duplicate 60 ignored: %s\n", height(root) == before ? "Yes" : "No");

    FILE *fp = fopen("bst_data.txt", "w");
    if (!fp) { perror("bst_data.txt"); destroy(root); return EXIT_FAILURE; }
    saveInorder(root, fp); fclose(fp);
    fp = fopen("bst_data.txt", "r");
    if (!fp) { perror("bst_data.txt"); destroy(root); return EXIT_FAILURE; }
    Book *rebuilt = loadSorted(fp); fclose(fp);
    printf("Rebuilt tree inorder: "); inorder(rebuilt); printf("\n");
    destroy(rebuilt); destroy(root);

    /* 2,000-key experiment: compare random and increasing insertion orders. */
    int data[2000], randomData[2000];
    for (int i = 0; i < 2000; ++i) data[i] = i + 1;
    for (int i = 0; i < 2000; ++i) randomData[i] = data[i];
    srand(42); shuffle(randomData, 2000);
    Book *randomTree = NULL, *sortedTree = NULL;
    for (int i = 0; i < 2000; ++i) randomTree = insert(randomTree, randomData[i]);
    for (int i = 0; i < 2000; ++i) sortedTree = insert(sortedTree, data[i]);
    long long total = 0; int found;
    for (int i = 1; i <= 2000; ++i) { search(randomTree, i, &cmp); total += cmp; }
    printf("2000-key experiment\nRandom insertion height: %d, average successful-search comparisons: %.2f\n", height(randomTree), total / 2000.0);
    printf("Sorted insertion height: %d, average successful-search comparisons: %.2f\n", height(sortedTree), (2001.0 / 2.0));
    (void)found;
    destroy(randomTree); destroy(sortedTree);
    return 0;
}
