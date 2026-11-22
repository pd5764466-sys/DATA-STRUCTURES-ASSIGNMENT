#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ORDERS 100000
#define TABLE_SIZE 262147

typedef struct Order {
    int id;
    int restaurant;
    int amount;
    int promised_time;
    int wait_time;
    int active;
    struct Order *next;
} Order;

Order *table[TABLE_SIZE];
Order *heap[MAX_ORDERS];
int heap_size = 0;
int next_id = 1;

unsigned hash_id(int id)
{
    return (unsigned)id % TABLE_SIZE;
}

void swap(Order **a, Order **b)
{
    Order *t = *a;
    *a = *b;
    *b = t;
}

void heap_up(int i)
{
    while (i > 0) {
        int p = (i - 1) / 2;
        if (heap[p]->wait_time >= heap[i]->wait_time)
            break;
        swap(&heap[p], &heap[i]);
        i = p;
    }
}

void heap_down(int i)
{
    while (1) {
        int left = 2 * i + 1;
        int right = left + 1;
        int largest = i;

        if (left < heap_size &&
            heap[left]->wait_time > heap[largest]->wait_time)
            largest = left;

        if (right < heap_size &&
            heap[right]->wait_time > heap[largest]->wait_time)
            largest = right;

        if (largest == i)
            break;

        swap(&heap[i], &heap[largest]);
        i = largest;
    }
}

void heap_push(Order *order)
{
    if (heap_size >= MAX_ORDERS)
        return;
    heap[heap_size] = order;
    heap_up(heap_size);
    heap_size++;
}

Order *heap_pop(void)
{
    if (heap_size == 0)
        return NULL;

    Order *top = heap[0];
    heap_size--;

    if (heap_size > 0) {
        heap[0] = heap[heap_size];
        heap_down(0);
    }
    return top;
}

Order *find_order(int id)
{
    unsigned index = hash_id(id);
    Order *current = table[index];

    while (current != NULL) {
        if (current->id == id && current->active)
            return current;
        current = current->next;
    }
    return NULL;
}

int place_order(int restaurant, int amount, int promised_time, int wait_time)
{
    Order *order = malloc(sizeof(Order));
    if (order == NULL)
        return -1;

    order->id = next_id++;
    order->restaurant = restaurant;
    order->amount = amount;
    order->promised_time = promised_time;
    order->wait_time = wait_time;
    order->active = 1;

    unsigned index = hash_id(order->id);
    order->next = table[index];
    table[index] = order;

    heap_push(order);
    return order->id;
}

int cancel_order(int id)
{
    Order *order = find_order(id);
    if (order == NULL)
        return 0;

    /* Lazy deletion: mark inactive; heap skips it when popped. */
    order->active = 0;
    return 1;
}

Order *assign_next_order(void)
{
    Order *order;

    while ((order = heap_pop()) != NULL) {
        if (order->active) {
            order->active = 0;
            return order;
        }
    }
    return NULL;
}

int compare_orders(const void *a, const void *b)
{
    const Order *x = *(const Order * const *)a;
    const Order *y = *(const Order * const *)b;

    if (x->restaurant != y->restaurant)
        return x->restaurant - y->restaurant;
    if (x->amount != y->amount)
        return x->amount - y->amount;
    return x->id - y->id;
}

int make_report(Order **list, int capacity)
{
    int count = 0;

    for (int b = 0; b < TABLE_SIZE; b++) {
        for (Order *p = table[b]; p != NULL; p = p->next) {
            if (p->active && count < capacity)
                list[count++] = p;
        }
    }

    qsort(list, (size_t)count, sizeof(list[0]), compare_orders);
    return count;
}

void clear_all(void)
{
    for (int b = 0; b < TABLE_SIZE; b++) {
        Order *p = table[b];
        while (p != NULL) {
            Order *next = p->next;
            free(p);
            p = next;
        }
        table[b] = NULL;
    }
    heap_size = 0;
    next_id = 1;
}

int main(void)
{
    const int sizes[] = {50000, 100000};

    for (int test = 0; test < 2; test++) {
        int n = sizes[test];
        clock_t start = clock();

        for (int i = 0; i < n; i++)
            place_order((i % 50) + 1, (i * 37) % 2000 + 100,
                        60 + (i % 30), i % 120);

        clock_t placed = clock();

        int lookups = 0;
        for (int i = 1; i <= n; i++)
            if (find_order(i) != NULL)
                lookups++;

        clock_t searched = clock();

        int cancelled = 0;
        for (int i = 10; i <= n; i += 10)
            cancelled += cancel_order(i);

        clock_t cancelled_at = clock();

        Order *assigned = assign_next_order();
        clock_t assigned_at = clock();

        Order **report = malloc((size_t)n * sizeof(Order *));
        int report_count = report ? make_report(report, n) : 0;
        clock_t reported_at = clock();

        double place_ms = 1000.0 * (placed - start) / CLOCKS_PER_SEC;
        double search_ms = 1000.0 * (searched - placed) / CLOCKS_PER_SEC;
        double cancel_ms = 1000.0 * (cancelled_at - searched) / CLOCKS_PER_SEC;
        double assign_ms = 1000.0 * (assigned_at - cancelled_at) / CLOCKS_PER_SEC;
        double report_ms = 1000.0 * (reported_at - assigned_at) / CLOCKS_PER_SEC;

        printf("Simulation: %d orders\n", n);
        printf("Placed: %d | Lookups found: %d | Cancelled: %d\n",
               n, lookups, cancelled);
        if (assigned != NULL)
            printf("Next assigned: Order %d (wait %d)\n",
                   assigned->id, assigned->wait_time);
        printf("Active orders in report: %d\n", report_count);
        printf("Time (ms): place %.2f, lookup %.2f, cancel %.2f, assign %.2f, report %.2f\n\n",
               place_ms, search_ms, cancel_ms, assign_ms, report_ms);

        free(report);
        clear_all();
    }

    return 0;
}
