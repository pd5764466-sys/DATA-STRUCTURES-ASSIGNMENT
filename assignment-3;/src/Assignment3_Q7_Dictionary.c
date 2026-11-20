#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define WORD_COUNT 1000
#define WORD_LEN 16
#define ALPHABET 26
#define MAX_RESULTS 20

typedef struct TrieNode {
    struct TrieNode *child[ALPHABET];
    int isEnd;
} TrieNode;

char words[WORD_COUNT][WORD_LEN];
TrieNode *root;
size_t trieNodes = 0;

TrieNode *newNode(void) {
    TrieNode *node = (TrieNode *)calloc(1, sizeof(TrieNode));
    if (!node) { printf("Memory allocation failed.\n"); exit(1); }
    trieNodes++;
    return node;
}

int validWord(const char *s) {
    if (!s || !*s) return 0;
    for (int i = 0; s[i]; i++)
        if (s[i] < 'a' || s[i] > 'z') return 0;
    return 1;
}

void trieInsert(const char *word) {
    TrieNode *cur = root;
    for (int i = 0; word[i]; i++) {
        int index = word[i] - 'a';
        if (!cur->child[index]) cur->child[index] = newNode();
        cur = cur->child[index];
    }
    cur->isEnd = 1;
}

int trieSearch(const char *word) {
    TrieNode *cur = root;
    for (int i = 0; word[i]; i++) {
        int index = word[i] - 'a';
        if (!cur->child[index]) return 0;
        cur = cur->child[index];
    }
    return cur->isEnd;
}

int compareWords(const void *a, const void *b) {
    return strcmp((const char *)a, (const char *)b);
}

void makeWord(int value, char *out) {
    char temp[WORD_LEN];
    int n = 0;
    do { temp[n++] = (char)('a' + value % 26); value /= 26; } while (value > 0);
    for (int i = 0; i < n; i++) out[i] = temp[n - i - 1];
    out[n] = '\0';
}

void collectWords(TrieNode *node, char *buffer, int depth, int *shown) {
    if (!node || *shown >= MAX_RESULTS) return;
    if (node->isEnd) {
        buffer[depth] = '\0';
        printf("%s ", buffer);
        (*shown)++;
    }
    for (int i = 0; i < ALPHABET && *shown < MAX_RESULTS; i++) {
        if (node->child[i]) {
            buffer[depth] = (char)('a' + i);
            collectWords(node->child[i], buffer, depth + 1, shown);
        }
    }
}

void autocomplete(const char *prefix) {
    TrieNode *cur = root;
    for (int i = 0; prefix[i]; i++) {
        int index = prefix[i] - 'a';
        if (!cur->child[index]) { printf("No suggestions"); return; }
        cur = cur->child[index];
    }
    char buffer[WORD_LEN];
    strcpy(buffer, prefix);
    int shown = 0;
    collectWords(cur, buffer, (int)strlen(prefix), &shown);
    if (!shown) printf("No suggestions");
}

int lowerBound(const char *key) {
    int lo = 0, hi = WORD_COUNT;
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (strcmp(words[mid], key) < 0) lo = mid + 1;
        else hi = mid;
    }
    return lo;
}

void rangeList(const char *first, const char *last) {
    int start = lowerBound(first), shown = 0;
    for (int i = start; i < WORD_COUNT && strcmp(words[i], last) <= 0 && shown < MAX_RESULTS; i++) {
        printf("%s ", words[i]);
        shown++;
    }
    if (!shown) printf("No words in range");
}

int main(void) {
    root = newNode();
    for (int i = 0; i < WORD_COUNT; i++) {
        makeWord(i + 1, words[i]);
        trieInsert(words[i]);
    }
    qsort(words, WORD_COUNT, sizeof(words[0]), compareWords);

    printf("Q7 - DICTIONARY USING TRIE + SORTED ARRAY\n");
    printf("Dictionary loaded: %d words\n", WORD_COUNT);

    const char *exact = words[321];
    printf("\nExact lookup '%s': %s\n", exact, trieSearch(exact) ? "FOUND" : "NOT FOUND");
    printf("Exact lookup 'zzzzz': %s\n", trieSearch("zzzzz") ? "FOUND" : "NOT FOUND");

    printf("\nAutocomplete for prefix 'b': ");
    autocomplete("b");
    printf("\nAutocomplete for prefix 'xyz': ");
    autocomplete("xyz");
    printf("\n\nAlphabetical range '%s' to '%s': ", words[100], words[110]);
    rangeList(words[100], words[110]);
    printf("\n");

    clock_t start = clock();
    int found = 0;
    for (int i = 0; i < 100000; i++) found += trieSearch(words[i % WORD_COUNT]);
    double lookupMs = 1000.0 * (clock() - start) / CLOCKS_PER_SEC;

    start = clock();
    for (int i = 0; i < 10000; i++) (void)lowerBound(words[i % WORD_COUNT]);
    double rangeMs = 1000.0 * (clock() - start) / CLOCKS_PER_SEC;

    size_t trieBytes = trieNodes * sizeof(TrieNode);
    size_t arrayBytes = sizeof(words);
    printf("\nPerformance test:\n");
    printf("100000 exact lookups: %.3f ms (%d found)\n", lookupMs, found);
    printf("10000 range start searches: %.3f ms\n", rangeMs);
    printf("Trie nodes: %zu\n", trieNodes);
    printf("Approx. trie memory: %zu bytes\n", trieBytes);
    printf("Sorted word array memory: %zu bytes\n", arrayBytes);
    printf("Combined estimated memory: %zu bytes\n", trieBytes + arrayBytes);
    printf("Verification: %s\n", (trieSearch(exact) && !trieSearch("zzzzz")) ? "PASS" : "FAIL");
    printf("\nDesign: Trie for exact lookup/autocomplete; sorted array for alphabetical range listing.\n");
    return 0;
}
