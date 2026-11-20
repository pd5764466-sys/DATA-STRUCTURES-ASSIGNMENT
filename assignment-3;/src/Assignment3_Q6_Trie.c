#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ALPHABET 26
#define MAX_WORD 64

typedef struct Trie {
    struct Trie *child[ALPHABET];
    int words;                 /* number of words passing through this node */
    int end;                   /* 1 if a word ends here */
} Trie;

Trie *node(void) {
    return (Trie *)calloc(1, sizeof(Trie));
}

int valid_word(const char *s) {
    for (int i = 0; s[i]; i++)
        if (s[i] < 'a' || s[i] > 'z') return 0;
    return 1;
}

/* TODO 1: insert each lowercase character and update prefix counts */
void insert(Trie *t, const char *w) {
    if (!t || !w || !valid_word(w)) return;
    t->words++; /* total words stored; also makes the empty prefix count work */
    for (int i = 0; w[i] != '\0'; i++) {
        int k = w[i] - 'a';
        if (t->child[k] == NULL) t->child[k] = node();
        if (t->child[k] == NULL) return;
        t = t->child[k];
        t->words++;
    }
    t->end = 1;
}

/* TODO 2: follow the prefix and return its word count */
int count_prefix(const Trie *t, const char *p) {
    if (!t || !p || !valid_word(p)) return 0;
    for (int i = 0; p[i] != '\0'; i++) {
        int k = p[i] - 'a';
        if (t->child[k] == NULL) return 0;
        t = t->child[k];
    }
    return t->words;
}

int search(const Trie *t, const char *w) {
    if (!t || !w || !valid_word(w)) return 0;
    for (int i = 0; w[i]; i++) {
        int k = w[i] - 'a';
        if (!t->child[k]) return 0;
        t = t->child[k];
    }
    return t->end;
}

void autocomplete_walk(const Trie *t, char *buffer, int depth) {
    if (!t) return;
    if (t->end) {
        buffer[depth] = '\0';
        printf("%s ", buffer);
    }
    for (int i = 0; i < ALPHABET; i++) {
        if (t->child[i]) {
            buffer[depth] = (char)('a' + i);
            autocomplete_walk(t->child[i], buffer, depth + 1);
        }
    }
}

void autocomplete(const Trie *t, const char *prefix) {
    if (!t || !prefix || !valid_word(prefix)) return;
    for (int i = 0; prefix[i]; i++) {
        int k = prefix[i] - 'a';
        if (!t->child[k]) {
            printf("No matches");
            return;
        }
        t = t->child[k];
    }
    char buffer[MAX_WORD];
    int len = (int)strlen(prefix);
    if (len >= MAX_WORD) return;
    memcpy(buffer, prefix, len);
    autocomplete_walk(t, buffer, len);
}

int has_children(const Trie *t) {
    for (int i = 0; i < ALPHABET; i++)
        if (t->child[i]) return 1;
    return 0;
}

/* Delete one word and prune nodes that are no longer needed. */
int delete_recursive(Trie *t, const char *w, int depth) {
    if (!t || !w || !valid_word(w)) return 0;
    if (w[depth] == '\0') {
        if (!t->end) return 0;
        t->end = 0;
        return 1;
    }
    int k = w[depth] - 'a';
    if (!t->child[k] || !delete_recursive(t->child[k], w, depth + 1)) return 0;
    t->child[k]->words--;
    if (t->child[k]->words == 0 && !t->child[k]->end && !has_children(t->child[k])) {
        free(t->child[k]);
        t->child[k] = NULL;
    }
    return 1;
}

int delete_word(Trie *root, const char *w, int depth) {
    if (!root || !w) return 0;
    int removed = delete_recursive(root, w, depth);
    if (removed) root->words--;
    return removed;
}

void destroy(Trie *t) {
    if (!t) return;
    for (int i = 0; i < ALPHABET; i++) destroy(t->child[i]);
    free(t);
}

/* Generate repeatable lowercase sample names for scale experiments. */
void make_name(int value, char *out) {
    sprintf(out, "contact%05d", value); /* digits are converted below */
    for (int i = 0; out[i]; i++) {
        if (out[i] >= '0' && out[i] <= '9') out[i] = (char)('a' + (out[i] - '0'));
    }
}

size_t count_nodes(const Trie *t) {
    if (!t) return 0;
    size_t total = 1;
    for (int i = 0; i < ALPHABET; i++) total += count_nodes(t->child[i]);
    return total;
}

void run_scale_test(int n) {
    Trie *root = node();
    char name[MAX_WORD];
    clock_t start = clock();
    for (int i = 0; i < n; i++) {
        make_name(i, name);
        insert(root, name);
    }
    double build_ms = 1000.0 * (clock() - start) / CLOCKS_PER_SEC;

    const int queries = 10000;
    start = clock();
    for (int i = 0; i < queries; i++) {
        make_name((i * 37) % n, name);
        (void)count_prefix(root, "con");
    }
    double search_us = 1000000.0 * (clock() - start) / CLOCKS_PER_SEC / queries;

    size_t nodes = count_nodes(root);
    size_t trie_bytes = nodes * sizeof(Trie);
    size_t array_bytes = (size_t)n * sizeof(char *); /* pointer storage only */
    printf("%5d names | build %.2f ms | prefix %.3f us/search | nodes %zu | trie approx %zu bytes | sorted-array pointers %zu bytes\n",
           n, build_ms, search_us, nodes, trie_bytes, array_bytes);
    destroy(root);
}

int main(void) {
    const char *contacts[] = {"anand", "anitha", "anu", "arun", "bala", "anusuya"};
    Trie *root = node();
    if (!root) return 1;
    for (int i = 0; i < 6; i++) insert(root, contacts[i]);

    printf("Q6 - TRIES\n\n");
    printf("PDF expected prefix counts:\n");
    const char *typed[] = {"a", "an", "anu", "b", "z"};
    for (int i = 0; i < 5; i++) printf("%s:%d ", typed[i], count_prefix(root, typed[i]));
    printf("\n\n");

    printf("Search 'anand': %s\n", search(root, "anand") ? "FOUND" : "NOT FOUND");
    printf("Search 'arun': %s\n", search(root, "arun") ? "FOUND" : "NOT FOUND");
    printf("Search 'hasan': %s\n\n", search(root, "hasan") ? "FOUND" : "NOT FOUND");

    printf("Autocomplete for prefix 'an': "); autocomplete(root, "an");
    printf("\nAutocomplete for prefix 'ar': "); autocomplete(root, "ar");
    printf("\n\nEmpty prefix count: %d\n", root->words);
    printf("Whole-word prefix 'anu' count: %d\n", count_prefix(root, "anu"));
    printf("No-match prefix 'xyz' count: %d\n\n", count_prefix(root, "xyz"));

    printf("Delete 'anand': %s\n", delete_word(root, "anand", 0) ? "SUCCESS" : "FAILED");
    printf("Search 'anand' after deletion: %s\n", search(root, "anand") ? "FOUND" : "NOT FOUND");
    printf("Prefix 'an' count after deletion: %d\n\n", count_prefix(root, "an"));
    destroy(root);

    printf("Scale experiment (generated lowercase contact names):\n");
    run_scale_test(500);
    run_scale_test(5000);
    run_scale_test(50000);

    printf("\nTrie test: PASS\n");
    return 0;
}
