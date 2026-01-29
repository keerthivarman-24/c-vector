#include <stdio.h>

#define C_VECTOR
#include "include/vector.h"

int main() {
    c_vector vec;

    cvec_init(&vec, sizeof(int));
    int a = 10, b = 20, c = 30;
    cvec_append(&vec, &a);
    cvec_append(&vec, &b);
    cvec_append(&vec, &c);
    cvec_append(&vec, &a);
    cvec_append(&vec, &b);
    cvec_append(&vec, &c);

    for (int i = 0; i < vec.size; i++) {
        printf("vector[%d] = %d\n", i, cvec_at(vec, i, int));
    }

    cvec_remove(&vec, 1);

    for (int i = 0; i < vec.size; i++) {
        printf("vector[%d] = %d\n", i, cvec_at(vec, i, int));
    }

    cvec_delete(&vec);
    return 0;
}