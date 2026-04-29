// Basic usage example for c_vector_t library
#include <stdio.h>

#define C_VECTOR
#include "../include/vector.h"

int main() {
    c_vector_t vec;
    cvec_error_code status;
    
    // Initialize vector for integers
    cvec_init(&vec, sizeof(int));

    printf("=== Basic Vector Operations ===\n\n");
    
    // Check if empty
    printf("Is vector empty? %s\n", cvec_is_empty(&vec) ? "Yes" : "No");
    printf("Initial - Size: %zu, Capacity: %zu\n\n", cvec_size(&vec), cvec_capacity(&vec));
    
    // Add elements
    printf("Adding elements: 10, 20, 30, 40, 50\n");
    int values[] = {10, 20, 30, 40, 50};
    cvec_from_array(&vec, values, sizeof(values) / sizeof(values[0]),sizeof(values[0]));
    
    // Display all elements
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
    printf("Size: %zu, Capacity: %zu\n", cvec_size(&vec), cvec_capacity(&vec));
    printf("Is empty now? %s\n\n", cvec_is_empty(&vec) ? "Yes" : "No");
    
    // Access front and back
    void* ptr = nullptr;
    status = cvec_front(&vec, &ptr);
    if (status != CVEC_OK) {
        fprintf(stderr, "front failed: %s\n", cvec_error_message(status));
        cvec_delete(&vec);
        return 1;
    }
    printf("First element: %d\n", *(int*)ptr);

    status = cvec_back(&vec, &ptr);
    if (status != CVEC_OK) {
        fprintf(stderr, "back failed: %s\n", cvec_error_message(status));
        cvec_delete(&vec);
        return 1;
    }
    printf("Last element: %d\n\n", *(int*)ptr);
    
    // Remove element at index 2 (value 30)
    printf("Removing element at index 2\n");
    status = cvec_remove(&vec, 2);
    if (status != CVEC_OK) {
        fprintf(stderr, "remove failed: %s\n", cvec_error_message(status));
        cvec_delete(&vec);
        return 1;
    }
    
    printf("Vector after removal: ");
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
    printf("Size: %zu, Capacity: %zu\n\n", cvec_size(&vec), cvec_capacity(&vec));
    
    // Clear the vector
    printf("Clearing vector\n");
    cvec_clear(&vec);
    printf("Size after clear: %zu, Capacity: %zu\n", cvec_size(&vec), cvec_capacity(&vec));
    printf("Is empty after clear? %s\n\n", cvec_is_empty(&vec) ? "Yes" : "No");
    
    // Cleanup
    cvec_delete(&vec);
    printf("Vector deleted successfully\n");
    
    return 0;
}
