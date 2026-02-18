// Basic usage example for c_vector_t library
#include <stdio.h>

#define C_VECTOR
#include "../include/vector.h"

int main() {
    c_vector_t vec;
    
    // Initialize vector for integers
    cvec_init(&vec, sizeof(int));
    
    printf("=== Basic Vector Operations ===\n\n");
    
    // Check if empty
    printf("Is vector empty? %s\n", cvec_is_empty(&vec) ? "Yes" : "No");
    printf("Initial - Size: %zu, Capacity: %zu\n\n", vec.size, vec.capacity);
    
    // Add elements
    printf("Adding elements: 10, 20, 30, 40, 50\n");
    int values[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) {
        cvec_append(&vec, &values[i]);
    }
    
    // Display all elements
    printf("Vector contents: ");
    for (size_t i = 0; i < vec.size; i++) {
        printf("%d ", cvec_at(vec, i, int));
    }
    printf("\n");
    printf("Size: %zu, Capacity: %zu\n", vec.size, vec.capacity);
    printf("Is empty now? %s\n\n", cvec_is_empty(&vec) ? "Yes" : "No");
    
    // Access front and back
    printf("First element: %d\n", *(int*)cvec_front(&vec));
    printf("Last element: %d\n\n", *(int*)cvec_back(&vec));
    
    // Remove element at index 2 (value 30)
    printf("Removing element at index 2\n");
    cvec_remove(&vec, 2);
    
    printf("Vector after removal: ");
    for (size_t i = 0; i < vec.size; i++) {
        printf("%d ", cvec_at(vec, i, int));
    }
    printf("\n");
    printf("Size: %zu, Capacity: %zu\n\n", vec.size, vec.capacity);
    
    // Clear the vector
    printf("Clearing vector\n");
    cvec_clear(&vec);
    printf("Size after clear: %zu, Capacity: %zu\n", vec.size, vec.capacity);
    printf("Is empty after clear? %s\n\n", cvec_is_empty(&vec) ? "Yes" : "No");
    
    // Cleanup
    cvec_delete(&vec);
    printf("Vector deleted successfully\n");
    
    return 0;
}
