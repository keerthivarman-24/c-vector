#pragma once

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

static void cvec_init(c_vector* arr, const size_t element_size) {
    if (!arr) custom_die("cvec_init: arr is NULL");
    if (element_size == 0) custom_die("cvec_init: element_size must be > 0");
    arr->element_size = element_size;
    arr->size = 0;
    arr->capacity = 8;

    if (arr->capacity > SIZE_MAX / arr->element_size) {
        custom_die("cvec_init: allocation size overflow");
    }

    arr->data = malloc(element_size * arr->capacity);
    if (!arr->data) die("cvec_init: Failed to allocate memory");
}

static void cvec_delete(c_vector* arr) {
    if (!arr) custom_die("cvec_delete: arr is NULL");
    if (arr->data) free(arr->data);
    arr->data = nullptr;
    arr->size = 0;
    arr->capacity = 0;
    arr->element_size = 0;
}

static void* cvec_get(c_vector* arr, const size_t index) {
    if (!arr) custom_die("cvec_get: arr is NULL");
    if (index >= arr->size) custom_die("cvec_get: index is out of range");

    return (uint8_t*)arr->data + index * arr->element_size;
}

static void cvec_set(c_vector* arr, const size_t index, const void* value) {
    if (!arr || !value) custom_die("cvec_set: invalid args");
    if (index >= arr->size) custom_die("cvec_set: index is out of range");

    memcpy(cvec_get(arr, index), value,  arr->element_size);
}

static void cvec_append(c_vector* arr, const void* value) {
    if (!arr || !value) custom_die("cvec_append: invalid args");

    if (arr->size >= arr->capacity) {
        if (arr->capacity > SIZE_MAX - (arr->capacity >> 1)) {
            custom_die("cvec_append: capacity overflow");
        }
        arr->capacity += arr->capacity >> 1;
        if (arr->capacity > SIZE_MAX / arr->element_size) {
            custom_die("cvec_append: allocation size overflow");
        }
        void* tmp = realloc(arr->data, arr->element_size * arr->capacity);
        if (!tmp) die("cvec_append: Failed to reallocate memory");
        arr->data = tmp;
    }

    memcpy((uint8_t*)arr->data + arr->size * arr->element_size, value, arr->element_size);
    arr->size++;
}

static bool cvec_is_empty(const c_vector* arr) {
    if (!arr) custom_die("cvec_is_empty: arr is NULL");
    return arr->size == 0;
}

static void* cvec_front(c_vector* arr) {
    if (!arr) custom_die("cvec_front: arr is NULL");
    if (arr->size == 0) custom_die("cvec_front: vector is empty");
    return arr->data;
}

static void* cvec_back(c_vector* arr) {
    if (!arr) custom_die("cvec_back: arr is NULL");
    if (arr->size == 0) custom_die("cvec_back: vector is empty");
    return cvec_get(arr, arr->size - 1);
}

static void cvec_remove(c_vector* arr, const size_t index) {
    if (!arr) custom_die("cvec_remove: arr is NULL");
    if (index >= arr->size) custom_die("cvec_remove: index is out of range");

    if (arr->size - index - 1 > 0) {
        memmove(
            (uint8_t*)arr->data + index * arr->element_size,
            (uint8_t*)arr->data + (index + 1) * arr->element_size,
            (arr->size - index - 1) * arr->element_size
        );
    }
    arr->size--;
}

static void cvec_clear(c_vector* arr) {
    if (!arr) custom_die("cvec_clear: arr is NULL");
    arr->size = 0;
}

static void cvec_reserve(c_vector* arr, const size_t capacity) {
    if (!arr) custom_die("cvec_reserve: arr is NULL");
    if (capacity == 0) custom_die("cvec_reserve: capacity must be > 0");
    if (arr->element_size == 0) custom_die("cvec_reserve: element_size must be > 0");
    if (capacity <= arr->capacity) return;

    if (capacity > SIZE_MAX / arr->element_size) {
        custom_die("cvec_reserve: allocation size overflow");
    }

    void *tmp = realloc(arr->data, arr->element_size * capacity);
    if (!tmp) die("cvec_reserve: Failed to reallocate memory");
    arr->data = tmp;
    arr->capacity = capacity;
}

#define cvec(array) cvec_from_array((array), sizeof(array) / sizeof((array)[0]), sizeof((array)[0]))

static c_vector cvec_from_array(void* data, size_t count, size_t element_size) {
    if (!data) custom_die("cvec_from_array: data is NULL");
    if (count == 0) custom_die("cvec_from_array: count must be > 0");
    if (element_size == 0) custom_die("cvec_from_array: element_size must be > 0");

    c_vector arr;
    cvec_init(&arr, element_size);
    cvec_reserve(&arr, count);  // Reserve exact capacity to avoid reallocations

    for (size_t i = 0; i < count; i++) {
        cvec_append(&arr, (uint8_t*)data + i * element_size);
    }
    return arr;
}

static void* cvec_to_array(c_vector* arr) {
    if (!arr) custom_die("cvec_to_array: arr is NULL");
    if (arr->size == 0) custom_die("cvec_to_array: arr is empty");

    if (arr->size > SIZE_MAX / arr->element_size) {
        custom_die("cvec_to_array: allocation size overflow");
    }

    void* array = malloc(arr->element_size * arr->size);
    if (!array) die("cvec_to_array: Failed to allocate memory");
    memcpy(array, arr->data, arr->element_size * arr->size);
    return array;
}

static void cvec_shrink_to_fit(c_vector* arr) {
    if (!arr) custom_die("cvec_shrink_to_fit: arr is NULL");
    if (arr->size == arr->capacity) return;

    void* tmp = realloc(arr->data, arr->element_size * arr->capacity);
    if (!tmp) { die("cvec_shrink_to_fit: Failed to reallocate memory"); }
    arr->data = tmp;
    arr->capacity = arr->size;
}

static void cvec_insert(c_vector* arr, const size_t index, const void* value) {
    if (!arr || !value) custom_die("cvec_insert: invalid args");
    if (index > arr->size) custom_die("cvec_insert: index is out of range");

    if (arr->size >= arr->capacity) {
        if (arr->capacity > SIZE_MAX - (arr->capacity >> 1)) {
            custom_die("cvec_insert: capacity overflow");
        }
        arr->capacity += arr->capacity >> 1;
        if (arr->capacity > SIZE_MAX / arr->element_size) {
            custom_die("cvec_insert: allocation size overflow");
        }
        void* tmp = realloc(arr->data, arr->element_size * arr->capacity);
        if (!tmp) die("failed to reallocate memory");
        arr->data = tmp;
    }

    if (index < arr->size) {
        memmove(
            (uint8_t*)arr->data + (index + 1) * arr->element_size,
            (uint8_t*)arr->data + index * arr->element_size,
            (arr->size - index) * arr->element_size
        );
    }

    memcpy((uint8_t*)arr->data + index * arr->element_size, value, arr->element_size);
    arr->size++;
}

#define cvec_at(arr, index, type) (*(type*)cvec_get(&(arr), (index)))

#endif //C_VECTOR