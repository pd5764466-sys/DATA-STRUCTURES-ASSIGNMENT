#include <stdio.h>
#include <string.h>

#define N 100000
#define BAD_SIZE 1000
#define GOOD_SIZE 131071

const char *states[] = {
    "TN", "KA", "KL", "AP", "MH", "DL",
    "GJ", "RJ", "UP", "MP", "WB", "PB"
};

unsigned bad_hash(char *s)
{
    return (s[0] + s[1]) % BAD_SIZE;
}

unsigned good_hash(char *s)
{
    unsigned h = 0;

    while (*s)
        h = h * 31 + (unsigned char)*s++;

    return h % GOOD_SIZE;
}

int main(void)
{
    static int bad[BAD_SIZE] = {0};
    static int good[GOOD_SIZE] = {0};

    char reg[20];
    int state, region, number;
    int bad_max = 0, good_max = 0;
    int bad_used = 0, good_used = 0;

    for (int i = 0; i < N; i++)
    {
        state = i % 12;
        region = (i / 12) % 100;
        number = i % 10000;

        snprintf(reg, sizeof(reg), "%s%02dAB%04d",
                 states[state], region, number);

        bad[bad_hash(reg)]++;
        good[good_hash(reg)]++;
    }

    for (int i = 0; i < BAD_SIZE; i++)
    {
        if (bad[i] > 0)
        {
            bad_used++;
            if (bad[i] > bad_max)
                bad_max = bad[i];
        }
    }

    for (int i = 0; i < GOOD_SIZE; i++)
    {
        if (good[i] > 0)
        {
            good_used++;
            if (good[i] > good_max)
                good_max = good[i];
        }
    }

    printf("Total registrations: %d\n\n", N);

    printf("Poor Hash Function\n");
    printf("Table size: %d\n", BAD_SIZE);
    printf("Used buckets: %d\n", bad_used);
    printf("Longest chain: %d\n", bad_max);
    printf("Average non-empty chain length: %.2f\n\n",
           (double)N / bad_used);

    printf("Improved Hash Function\n");
    printf("Table size: %d\n", GOOD_SIZE);
    printf("Used buckets: %d\n", good_used);
    printf("Longest chain: %d\n", good_max);
    printf("Average non-empty chain length: %.2f\n",
           (double)N / good_used);

    return 0;
}
