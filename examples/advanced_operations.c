// Advanced operations: reserve, get/set, to_array, cvec macro, shrink_to_fit
#include <stdio.h>
#include <stdlib.h>

#define C_VECTOR
#include "../include/vector.h"

int main() {
    c_vector_t vec;
    cvec_error_code status;
    
    printf("=== Advanced Operations Example ===\n\n");
    
    // Demonstrate creating vector from C array
    printf("1. Creating vector from C array:\n");
    int source_array[] = {5, 10, 15, 20, 25, 30};
    c_vector_t vec_from_array;
    status = cvec_from_c_array(&vec_from_array, source_array);
    if (status != CVEC_OK) {
        fprintf(stderr, "create from array failed: %s\n", cvec_error_message(status));
        return 1;
    }
    
    printf("   Array elements copied to vector: ");
    for (size_t i = 0; i < vec_from_array.size; i++) {
        int value = 0;
        status = cvec_at(&vec_from_array, i, &value);
        if (status != CVEC_OK) {
            fprintf(stderr, "read failed: %s\n", cvec_error_message(status));
            cvec_delete(&vec_from_array);
            return 1;
        }
        printf("%d ", value);
    }
    printf("\n");
    printf("   Size: %zu, Capacity: %zu\n\n", vec_from_array.size, vec_from_array.capacity);
    cvec_delete(&vec_from_array);
    
    // Initialize with custom element size
    cvec_init(&vec, sizeof(double));
    
    // Pre-allocate capacity to avoid multiple reallocations
    printf("2. Pre-allocating capacity for 100 elements\n");
    status = cvec_reserve(&vec, 100);
    if (status != CVEC_OK) {
        fprintf(stderr, "reserve failed: %s\n", cvec_error_message(status));
        cvec_delete(&vec);
        return 1;
    }
    printf("   After reserve - Size: %zu, Capacity: %zu\n\n", vec.size, vec.capacity);
    
    // Add elements
    printf("3. Adding 10 elements\n");
    for (int i = 0; i < 10; i++) {
        double value = i * 1.5;
        status = cvec_append(&vec, &value);
        if (status != CVEC_OK) {
            fprintf(stderr, "append failed: %s\n", cvec_error_message(status));
            cvec_delete(&vec);
            return 1;
        }
    }
    printf("   After append - Size: %zu, Capacity: %zu\n", vec.size, vec.capacity);
    printf("   Notice: No reallocation needed due to reserve!\n\n");
    
    // Safe element access with cvec_get
    printf("4. Using cvec_get for safe access:\n");
    for (size_t i = 0; i < 3; i++) {  // Show only first 3
        void* ptr = nullptr;
        status = cvec_get(&vec, i, &ptr);
        if (status != CVEC_OK) {
            fprintf(stderr, "get failed: %s\n", cvec_error_message(status));
            cvec_delete(&vec);
            return 1;
        }
        double* value_ptr = (double*)ptr;
        printf("   vec[%zu] = %.2f\n", i, *value_ptr);
    }
    printf("   ...\n");
    printf("\n");
    
    // Modify elements using cvec_set
    printf("5. Doubling all values using cvec_set\n");
    for (size_t i = 0; i < vec.size; i++) {
        void* ptr = nullptr;
        status = cvec_get(&vec, i, &ptr);
        if (status != CVEC_OK) {
            fprintf(stderr, "get failed: %s\n", cvec_error_message(status));
            cvec_delete(&vec);
            return 1;
        }
        double* val = (double*)ptr;
        double doubled = (*val) * 2;
        status = cvec_set(&vec, i, &doubled);
        if (status != CVEC_OK) {
            fprintf(stderr, "set failed: %s\n", cvec_error_message(status));
            cvec_delete(&vec);
            return 1;
        }
    }
    
    printf("   After doubling: ");
    for (size_t i = 0; i < vec.size; i++) {
        double value = 0;
        status = cvec_at(&vec, i, &value);
        if (status != CVEC_OK) {
            fprintf(stderr, "read failed: %s\n", cvec_error_message(status));
            cvec_delete(&vec);
            return 1;
        }
        printf("%.1f ", value);
    }
    printf("\n");
    printf("\n");
    
    // Shrink to fit - reduce capacity to match size
    printf("6. Shrinking capacity to fit size\n");
    printf("   Before shrink - Size: %zu, Capacity: %zu\n", vec.size, vec.capacity);
    status = cvec_shrink_to_fit(&vec);
    if (status != CVEC_OK) {
        fprintf(stderr, "shrink failed: %s\n", cvec_error_message(status));
        cvec_delete(&vec);
        return 1;
    }
    printf("   After shrink  - Size: %zu, Capacity: %zu\n", vec.size, vec.capacity);
    printf("   Freed %zu slots of unused capacity!\n\n", 100 - vec.size);
    
    // Convert to regular array
    printf("7. Converting vector to regular array\n");
    void* raw_array = nullptr;
    status = cvec_to_array(&vec, &raw_array);
    if (status != CVEC_OK) {
        fprintf(stderr, "to_array failed: %s\n", cvec_error_message(status));
        cvec_delete(&vec);
        return 1;
    }
    double* array = (double*)raw_array;
    
    printf("   Array contents (first 5): ");
    for (size_t i = 0; i < 5; i++) {
        printf("%.1f ", array[i]);
    }
    printf("...\n");

    free(array);
    printf("   Array freed\n\n");

    cvec_delete(&vec);
    printf("Vector deleted successfully\n");
    
    return 0;
}
