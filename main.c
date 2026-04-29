// Simple demonstration of c_vector library
#include <stdio.h>

#define C_VECTOR
#include "include/vector.h"

int main() {
    c_vector_t vec;
    cvec_error_code status;

    // Initialize vector for integers
    cvec_init(&vec, sizeof(int));

    // Add some elements
    int values[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) {
        status = cvec_append(&vec, &values[i]);
        if (status != CVEC_OK) {
            fprintf(stderr, "append failed: %s\n", cvec_error_message(status));
            cvec_delete(&vec);
            return 1;
        }
    }

    // Display elements
    printf("Vector contents: ");
    for (size_t i = 0; i < cvec_size(&vec); i++) {
        int value = 0;
        status = cvec_at(&vec, i, &value);
        if (status != CVEC_OK) {
            fprintf(stderr, "read failed: %s\n", cvec_error_message(status));
            cvec_delete(&vec);
            return 1;
        }
        printf("%d ", value);
    }

    printf("\n");
    printf("Reverse vector contents: ");
    status = cvec_reverse(&vec);
    if (status != CVEC_OK) {
        fprintf(stderr, "reverse failed: %s\n", cvec_error_message(status));
        cvec_delete(&vec);
        return 1;
    }

    for (size_t i = 0; i < cvec_size(&vec); i++) {
        int value = 0;
        status = cvec_at(&vec, i, &value);
        if (status != CVEC_OK) {
            fprintf(stderr, "read failed: %s\n", cvec_error_message(status));
            cvec_delete(&vec);
            return 1;
        }
        printf("%d ", value);
    }
    printf("\n");

    c_vector_t vec1;
    status = cvec_init(&vec1, sizeof(int));
    if (status != CVEC_OK) {
        fprintf(stderr, "init failed: %s\n", cvec_error_message(status));
        cvec_delete(&vec);
        return 1;
    }

    status = cvec_reserve(&vec1, 10);
    if (status != CVEC_OK) {
        fprintf(stderr, "reserve failed: %s\n", cvec_error_message(status));
        cvec_delete(&vec);
        cvec_delete(&vec1);
        return 1;
    }

    status = cvec_fill(&vec1, &values[0], 10);
    if (status != CVEC_OK) {
        fprintf(stderr, "fill failed: %s\n", cvec_error_message(status));
        cvec_delete(&vec);
        cvec_delete(&vec1);
        return 1;
    }

    status = cvec_append(&vec1, &values[1]);
    if (status != CVEC_OK) {
        fprintf(stderr, "append failed: %s\n", cvec_error_message(status));
        cvec_delete(&vec);
        cvec_delete(&vec1);
        return 1;
    }

    status = cvec_insert(&vec1, 2, &values[2]);
    if (status != CVEC_OK) {
        fprintf(stderr, "insert failed: %s\n", cvec_error_message(status));
        cvec_delete(&vec);
        cvec_delete(&vec1);
        return 1;
    }

    for (size_t i = 0; i < cvec_size(&vec1); i++) {
        int t = 0;
        status = cvec_at(&vec1, i, &t);
        if (status != CVEC_OK) {
            fprintf(stderr, "read failed: %s\n", cvec_error_message(status));
            cvec_delete(&vec);
            cvec_delete(&vec1);
            return 1;
        }
        printf("%d ", t);
    }

    printf("\n");

    // Clean up
    cvec_delete(&vec);
    cvec_delete(&vec1);
    return 0;
}