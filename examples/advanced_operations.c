// Advanced operations: reserve, get/set, to_array, cvec macro, shrink_to_fit
#include <stdio.h>
#include <stdlib.h>

#define C_VECTOR
#include "../include/vector.h"

int main() {
    c_vector vec;
    
    printf("=== Advanced Operations Example ===\n\n");
    
    // Demonstrate cvec() macro - create vector from array
    printf("1. Creating vector from array using cvec() macro:\n");
    int source_array[] = {5, 10, 15, 20, 25, 30};
    c_vector vec_from_array = cvec(source_array);
    
    printf("   Array elements copied to vector: ");
    for (size_t i = 0; i < vec_from_array.size; i++) {
        printf("%d ", cvec_at(vec_from_array, i, int));
    }
    printf("\n");
    printf("   Size: %zu, Capacity: %zu\n\n", vec_from_array.size, vec_from_array.capacity);
    cvec_delete(&vec_from_array);
    
    // Initialize with custom element size
    cvec_init(&vec, sizeof(double));
    
    // Pre-allocate capacity to avoid multiple reallocations
    printf("2. Pre-allocating capacity for 100 elements\n");
    cvec_reserve(&vec, 100);
    printf("   After reserve - Size: %zu, Capacity: %zu\n\n", vec.size, vec.capacity);
    
    // Add elements
    printf("3. Adding 10 elements\n");
    for (int i = 0; i < 10; i++) {
        double value = i * 1.5;
        cvec_append(&vec, &value);
    }
    printf("   After append - Size: %zu, Capacity: %zu\n", vec.size, vec.capacity);
    printf("   Notice: No reallocation needed due to reserve!\n\n");
    
    // Safe element access with cvec_get
    printf("4. Using cvec_get for safe access:\n");
    for (size_t i = 0; i < 3; i++) {  // Show only first 3
        double* value_ptr = (double*)cvec_get(&vec, i);
        printf("   vec[%zu] = %.2f\n", i, *value_ptr);
    }
    printf("   ...\n");
    printf("\n");
    
    // Modify elements using cvec_set
    printf("5. Doubling all values using cvec_set\n");
    for (size_t i = 0; i < vec.size; i++) {
        double* val = (double*)cvec_get(&vec, i);
        double doubled = (*val) * 2;
        cvec_set(&vec, i, &doubled);
    }
    
    printf("   After doubling: ");
    for (size_t i = 0; i < vec.size; i++) {
        printf("%.1f ", cvec_at(vec, i, double));
    }
    printf("\n");
    printf("\n");
    
    // Shrink to fit - reduce capacity to match size
    printf("6. Shrinking capacity to fit size\n");
    printf("   Before shrink - Size: %zu, Capacity: %zu\n", vec.size, vec.capacity);
    cvec_shrink_to_fit(&vec);
    printf("   After shrink  - Size: %zu, Capacity: %zu\n", vec.size, vec.capacity);
    printf("   Freed %zu slots of unused capacity!\n\n", 100 - vec.size);
    
    // Convert to regular array
    printf("7. Converting vector to regular array\n");
    double* array = (double*)cvec_to_array(&vec);
    
    printf("   Array contents (first 5): ");
    for (size_t i = 0; i < 5; i++) {
        printf("%.1f ", array[i]);
    }
    printf("...\n");
    
    // Don't forget to free the array!
    free(array);
    printf("   Array freed\n\n");
    
    // Cleanup
    cvec_delete(&vec);
    printf("Vector deleted successfully\n");
    
    return 0;
}
