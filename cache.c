#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MIN_ELEMENT 256  // Minimum number of elements (1KB for int array)
#define SIZE_ORDER 17    // Array size ranges from 1KB to 64MB
#define INIT_STRIDE 8
#define AVERAGE 10       // Number of iterations for averaging
#define NUM_OPERATIONS (1 << 25)

int measureTime(int arraySize, int stride);
int calculateOneRound(int arraySize, int stride);

int calculateOneRound(int arraySize, int stride) {
    struct timespec start, end;
    int *array = (int *)malloc(sizeof(int) * arraySize);
    if (array == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    memset(array, 0, sizeof(int) * arraySize);

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (unsigned int i = 0; i < NUM_OPERATIONS; i++) {
        array[(i * stride) & (arraySize - 1)] += 1;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    unsigned long long interval = (end.tv_sec - start.tv_sec) * 1000000LL + 
                                  (end.tv_nsec - start.tv_nsec) / 1000LL;

    free(array);
    return (int)interval;
}

int measureTime(int arraySize, int stride) {
    int total = 0;
    for (int i = 0; i < AVERAGE; i++) {
        total += calculateOneRound(arraySize, stride);
    }
    return total / AVERAGE;
}

int main() {
    FILE *file = fopen("Data.txt", "w");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file\n");
        return EXIT_FAILURE;
    }

    for (int j = 0; j < 5; j++) {
        int stride = INIT_STRIDE << j;
        for (int i = 0; i < SIZE_ORDER; i++) {
            int arraySize = MIN_ELEMENT * (1 << i);
            int cost = measureTime(arraySize, stride);
            fprintf(file, "%d,%d,%d\n", i, stride, cost);
        }
    }

    fclose(file);
    printf("Successfully output the data!\n");
    return 0;
}
