// Example demonstrating vectors with different data types
#include <stdio.h>
#include <string.h>

#define C_VECTOR
#include "../include/vector.h"

int main() {
    cvec_error_code status;

    printf("=== Different Data Types Example ===\n\n");
    
    // Integer vector
    printf("1. Integer Vector:\n");
    c_vector_t int_vec = {0};
    cvec_init(&int_vec, sizeof(int));
    
    int nums[] = {100, 200, 300, 400, 500};
    for (int i = 0; i < 5; i++) {
        status = cvec_append(&int_vec, &nums[i]);
        if (status != CVEC_OK) goto fail;
    }
    
    printf("   Values: ");
    for (size_t i = 0; i < int_vec.size; i++) {
        int value = 0;
        status = cvec_at(&int_vec, i, &value);
        if (status != CVEC_OK) goto fail;
        printf("%d ", value);
    }
    printf("\n\n");
    
    // Character vector
    printf("2. Character Vector:\n");
    c_vector_t char_vec = {0};
    status = cvec_init(&char_vec, sizeof(char));
    if (status != CVEC_OK) goto fail;
    
    char letters[] = {'H', 'e', 'l', 'l', 'o'};
    for (int i = 0; i < 5; i++) {
        status = cvec_append(&char_vec, &letters[i]);
        if (status != CVEC_OK) goto fail;
    }
    
    printf("   Characters: ");
    for (size_t i = 0; i < char_vec.size; i++) {
        char ch = '\0';
        status = cvec_at(&char_vec, i, &ch);
        if (status != CVEC_OK) goto fail;
        printf("%c", ch);
    }
    printf("\n\n");
    
    // Float vector
    printf("3. Float Vector:\n");
    c_vector_t float_vec = {0};
    status = cvec_init(&float_vec, sizeof(float));
    if (status != CVEC_OK) goto fail;
    
    float decimals[] = {3.14f, 2.71f, 1.41f, 1.73f};
    for (int i = 0; i < 4; i++) {
        status = cvec_append(&float_vec, &decimals[i]);
        if (status != CVEC_OK) goto fail;
    }
    
    printf("   Values: ");
    for (size_t i = 0; i < float_vec.size; i++) {
        float value = 0.0f;
        status = cvec_at(&float_vec, i, &value);
        if (status != CVEC_OK) goto fail;
        printf("%.2f ", value);
    }
    printf("\n\n");
    
    // Pointer vector
    printf("4. Pointer Vector:\n");
    c_vector_t ptr_vec = {0};
    status = cvec_init(&ptr_vec, sizeof(char*));
    if (status != CVEC_OK) goto fail;
    
    char* strings[] = {"Apple", "Banana", "Cherry", "Date"};
    for (int i = 0; i < 4; i++) {
        status = cvec_append(&ptr_vec, &strings[i]);
        if (status != CVEC_OK) goto fail;
    }
    
    printf("   Strings: ");
    for (size_t i = 0; i < ptr_vec.size; i++) {
        char* text = NULL;
        status = cvec_at(&ptr_vec, i, &text);
        if (status != CVEC_OK) goto fail;
        printf("%s ", text);
    }
    printf("\n\n");
    
    // Cleanup all vectors
    cvec_delete(&int_vec);
    cvec_delete(&char_vec);
    cvec_delete(&float_vec);
    cvec_delete(&ptr_vec);
    
    printf("All vectors deleted successfully\n");
    
    return 0;

fail:
    fprintf(stderr, "operation failed: %s\n", cvec_error_message(status));
    cvec_delete(&int_vec);
    cvec_delete(&char_vec);
    cvec_delete(&float_vec);
    cvec_delete(&ptr_vec);
    return 1;
}
