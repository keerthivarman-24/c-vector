// Example demonstrating vectors with different data types
#include <stdio.h>
#include <string.h>

#define C_VECTOR
#include "../include/vector.h"

int main() {
    printf("=== Different Data Types Example ===\n\n");
    
    // Integer vector
    printf("1. Integer Vector:\n");
    c_vector int_vec;
    cvec_init(&int_vec, sizeof(int));
    
    int nums[] = {100, 200, 300, 400, 500};
    for (int i = 0; i < 5; i++) {
        cvec_append(&int_vec, &nums[i]);
    }
    
    printf("   Values: ");
    for (size_t i = 0; i < int_vec.size; i++) {
        printf("%d ", cvec_at(int_vec, i, int));
    }
    printf("\n\n");
    
    // Character vector
    printf("2. Character Vector:\n");
    c_vector char_vec;
    cvec_init(&char_vec, sizeof(char));
    
    char letters[] = {'H', 'e', 'l', 'l', 'o'};
    for (int i = 0; i < 5; i++) {
        cvec_append(&char_vec, &letters[i]);
    }
    
    printf("   Characters: ");
    for (size_t i = 0; i < char_vec.size; i++) {
        printf("%c", cvec_at(char_vec, i, char));
    }
    printf("\n\n");
    
    // Float vector
    printf("3. Float Vector:\n");
    c_vector float_vec;
    cvec_init(&float_vec, sizeof(float));
    
    float decimals[] = {3.14f, 2.71f, 1.41f, 1.73f};
    for (int i = 0; i < 4; i++) {
        cvec_append(&float_vec, &decimals[i]);
    }
    
    printf("   Values: ");
    for (size_t i = 0; i < float_vec.size; i++) {
        printf("%.2f ", cvec_at(float_vec, i, float));
    }
    printf("\n\n");
    
    // Pointer vector
    printf("4. Pointer Vector:\n");
    c_vector ptr_vec;
    cvec_init(&ptr_vec, sizeof(char*));
    
    char* strings[] = {"Apple", "Banana", "Cherry", "Date"};
    for (int i = 0; i < 4; i++) {
        cvec_append(&ptr_vec, &strings[i]);
    }
    
    printf("   Strings: ");
    for (size_t i = 0; i < ptr_vec.size; i++) {
        printf("%s ", cvec_at(ptr_vec, i, char*));
    }
    printf("\n\n");
    
    // Cleanup all vectors
    cvec_delete(&int_vec);
    cvec_delete(&char_vec);
    cvec_delete(&float_vec);
    cvec_delete(&ptr_vec);
    
    printf("All vectors deleted successfully\n");
    
    return 0;
}
