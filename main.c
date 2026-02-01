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
    
    // Clean up
    cvec_delete(&vec);
    return 0;
}