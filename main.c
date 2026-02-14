// Simple demonstration of c_vector library
#include <stdio.h>

#define C_VECTOR
#include "include/vector.h"

int main() {
    c_vector vec;
    // Initialize vector for integers
    cvec_init(&vec, sizeof(int));
    // Add some elements
    int values[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) {
        cvec_append(&vec, &values[i]);
    }
    // Display elements
    printf("Vector contents: ");
    for (size_t i = 0; i < vec.size; i++) {
        printf("%d ", cvec_at(vec, i, int));
    }

    printf("\n");
    printf("Reverse vector contents: ");
    cvec_reverse(&vec);
    for (size_t i = 0; i < vec.size; i++) {
        printf("%d ", cvec_at(vec, i, int));
    }
    printf("\n");

    c_vector vec1;
    cvec_init(&vec1, sizeof(int));
    cvec_reserve(&vec1, 10);
    cvec_fill(&vec1, &values[0]);

    for (size_t i = 0; i < vec1.size; i++) {
        printf("%d ", cvec_at(vec1, i, int));
    }
    // Clean up
    cvec_delete(&vec);
    cvec_delete(&vec1);
    return 0;
}