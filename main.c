// Simple demonstration of c_vector library
#include <stdio.h>

#define C_VECTOR
#include "include/vector.h"

int main() {
    c_vector_t vec;
    // Initialize vector for integers
    cvec_result init_result = cvec_init(&vec, sizeof(int));
    if (!init_result.success) {
        fprintf(stderr, "%s\n", init_result.error.error_msg);
        return 1;
    }
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

    c_vector_t vec1;
    cvec_result init_result2 = cvec_init(&vec1, sizeof(int));
    if (!init_result2.success) {
        fprintf(stderr, "%s\n", init_result2.error.error_msg);
        cvec_delete(&vec);
        return 1;
    }

    cvec_result reserve_result = cvec_reserve(&vec1, 10);
    if (!reserve_result.success) {
        fprintf(stderr, "%s\n", reserve_result.error.error_msg);
        cvec_delete(&vec);
        cvec_delete(&vec1);
        return 1;
    }

    cvec_fill(&vec1, &values[0]);
    cvec_append(&vec1, &values[1]);
    cvec_insert(&vec1, 2, &values[2]);

    for (size_t i = 0; i < vec1.size; i++) {
        printf("%d ", cvec_at(vec1, i, int));
    }
    // Clean up
    cvec_delete(&vec);
    cvec_delete(&vec1);
    return 0;
}