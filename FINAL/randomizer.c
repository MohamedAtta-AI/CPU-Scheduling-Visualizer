#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_PROCESSES 100
#define MIN_RANGE 0
#define MAX_RANGE 100

int randomInRange() {
    return rand() % (MAX_RANGE - MIN_RANGE + 1) + MIN_RANGE;
}

void generateRandomModel(int fileIndex) {
    char filename[20];
    sprintf(filename, "process_%d.txt", fileIndex);
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file.\n");
        exit(1);
    }

    srand(time(NULL));

    fprintf(file, "%d\n", NUM_PROCESSES);

    for (int i = 0; i < NUM_PROCESSES; i++) {
        fprintf(file, "P%d %d %d %d\n", i, randomInRange(), randomInRange(), randomInRange());
    }

    fclose(file);
}

int main() {
    // Generate 5 random files
    for (int i = 1; i <= 5; i++) {
        generateRandomModel(i);
        printf("Random model %d generated.\n", i);
    }
    return 0;
}
