// Advanced operations: reserve, get/set, to_array
#include <stdio.h>
#include <stdlib.h>

#define C_VECTOR
#include "../include/vector.h"

int main() {
    c_vector vec;
    
    printf("=== Advanced Operations Example ===\n\n");
    
    // Initialize with custom element size
    cvec_init(&vec, sizeof(double));
    
    // Pre-allocate capacity to avoid multiple reallocations
    printf("Pre-allocating capacity for 100 elements\n");
    cvec_reserve(&vec, 100);
    printf("Initial - Size: %zu, Capacity: %zu\n\n", vec.size, vec.capacity);
    
    // Add elements
    printf("Adding 10 elements\n");
    for (int i = 0; i < 10; i++) {
        double value = i * 1.5;
        cvec_append(&vec, &value);
    }
    printf("After append - Size: %zu, Capacity: %zu\n\n", vec.size, vec.capacity);
    
    // Safe element access with cvec_get
    printf("Using cvec_get for safe access:\n");
    for (size_t i = 0; i < vec.size; i++) {
        double* value_ptr = (double*)cvec_get(&vec, i);
        printf("vec[%zu] = %.2f\n", i, *value_ptr);
    }
    printf("\n");
    
    // Modify elements using cvec_set
    printf("Doubling all values using cvec_set\n");
    for (size_t i = 0; i < vec.size; i++) {
        double* val = (double*)cvec_get(&vec, i);
        double doubled = (*val) * 2;
        cvec_set(&vec, i, &doubled);
    }
    
    printf("After doubling:\n");
    for (size_t i = 0; i < vec.size; i++) {
        printf("vec[%zu] = %.2f\n", i, cvec_at(vec, i, double));
    }
    printf("\n");
    
    // Convert to regular array
    printf("Converting vector to regular array\n");
    double* array = (double*)cvec_to_array(&vec);
    
    printf("Array contents: ");
    for (size_t i = 0; i < vec.size; i++) {
        printf("%.2f ", array[i]);
    }
    printf("\n");
    
    // Don't forget to free the array!
    free(array);
    printf("Array freed\n\n");
    
    // Cleanup
    cvec_delete(&vec);
    printf("Vector deleted successfully\n");
    
    return 0;
}
