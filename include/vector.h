#pragma once
#define C_VECTOR

#if defined(C_VECTOR)
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    void*  data;
    size_t element_size;
    size_t size;
    size_t capacity;
} c_vector;

static void die(const char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

static void custom_die(const char* msg) {
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

void cvec_init(c_vector* arr, const size_t element_size) {
    if (!arr || element_size == 0) custom_die("init_array: invalid args");
    arr->element_size = element_size;
    arr->size = 0;
    arr->capacity = 8;

    arr->data = malloc(element_size * arr->capacity);
    if (!arr->data) die("failed to allocate memory");
}

static void cvec_delete(c_vector* arr) {
    if (!arr) return;
    free(arr->data);
    arr->data = nullptr;
    arr->size = 0;
    arr->capacity = 0;
    arr->element_size = 0;
}

static void cvec_append(c_vector* arr, const void* value) {
    if (!arr || !value) custom_die("append_array: invalid args");

    if (arr->size == arr->capacity) {
        arr->capacity *= 2;
        void* tmp = realloc(arr->data, arr->element_size * arr->capacity);
        if (!tmp) die("failed to reallocate memory");
        arr->data = tmp;
    }

    memcpy((uint8_t*)arr->data + arr->size * arr->element_size, value, arr->element_size);
    arr->size++;
}

#define cvec_at(arr, index, type) (*(type*)((uint8_t*)(arr).data + (index * arr.element_size)))

#endif //C_VECTOR