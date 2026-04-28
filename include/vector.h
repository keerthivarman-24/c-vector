#pragma once

#if defined(C_VECTOR)
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

typedef enum : uint8_t {
    CVEC_OK = 0,
    CVEC_ERROR_NULL_POINTER,
    CVEC_ERROR_OUT_OF_BOUNDS,
    CVEC_ERROR_ALLOCATION_FAILED,
    CVEC_ERROR_OVERFLOW,
    CVEC_ERROR_INVALID_SIZE,
    CVEC_ERROR_EMPTY_VECTOR,
    CVEC_ERROR_NOT_INITIALIZED,
    CVEC_ERROR_NOT_FOUND,
    CVEC_ERROR_NOT_SUPPORTED,
    CVEC_ERROR_INVALID_INDEX,
    CVEC_ERROR_INVALID_VALUE,
    CVEC_ERROR_INVALID_OPERATION,
    CVEC_ERROR_DUE_TO_PREVIOUS_OPERATION
} cvec_error_code;

typedef struct {
    void* data;
    size_t element_size;
    size_t size;
    size_t capacity;
} c_vector_t;

#ifndef CVEC_INITIAL_CAPACITY
#define CVEC_INITIAL_CAPACITY ((size_t)8)
#endif

#define cvec_array_count(arr) (sizeof(arr) / sizeof((arr)[0]))
#define cvec_from_c_array(out_vec, arr) \
    cvec_from_array((out_vec), (arr), cvec_array_count(arr), sizeof((arr)[0]))

static inline const char* cvec_error_message(const cvec_error_code code) {
    switch (code) {
        case CVEC_OK: return "ok";
        case CVEC_ERROR_NULL_POINTER: return "null pointer";
        case CVEC_ERROR_OUT_OF_BOUNDS: return "out of bounds";
        case CVEC_ERROR_ALLOCATION_FAILED: return "allocation failed";
        case CVEC_ERROR_OVERFLOW: return "overflow";
        case CVEC_ERROR_INVALID_SIZE: return "invalid size";
        case CVEC_ERROR_EMPTY_VECTOR: return "empty vector";
        case CVEC_ERROR_NOT_INITIALIZED: return "not initialized";
        case CVEC_ERROR_NOT_FOUND: return "not found";
        case CVEC_ERROR_NOT_SUPPORTED: return "not supported";
        case CVEC_ERROR_INVALID_INDEX: return "invalid index";
        case CVEC_ERROR_INVALID_VALUE: return "invalid value";
        case CVEC_ERROR_INVALID_OPERATION: return "invalid operation";
        case CVEC_ERROR_DUE_TO_PREVIOUS_OPERATION: return "due to previous operation";
        default: return "unknown error";
    }
}

static inline bool cvec_is_empty(const c_vector_t* vec) {
    return (vec == nullptr) || (vec->size == 0);
}

static inline bool cvec__mul_overflow(const size_t a, const size_t b, size_t* out) {
    if (!out) return true;
    if (a != 0 && b > SIZE_MAX / a) return true;
    *out = a * b;
    return false;
}

static inline cvec_error_code cvec__bytes_for(const size_t count, const size_t elem_size, size_t* out_bytes) {
    if (elem_size == 0) return CVEC_ERROR_INVALID_SIZE;
    if (cvec__mul_overflow(count, elem_size, out_bytes)) return CVEC_ERROR_OVERFLOW;
    return CVEC_OK;
}

static inline cvec_error_code cvec_init(c_vector_t* vec, const size_t element_size) {
    if (!vec) return CVEC_ERROR_NULL_POINTER;
    if (element_size == 0) return CVEC_ERROR_INVALID_SIZE;
    size_t bytes = 0;
    cvec_error_code status = cvec__bytes_for(CVEC_INITIAL_CAPACITY, element_size, &bytes);
    if (status != CVEC_OK) {
        vec->data = nullptr;
        vec->capacity = 0;
        vec->size = 0;
        vec->element_size = 0;
        return status;
    }
    void* data = malloc(bytes);
    if (!data) {
        vec->data = nullptr;
        vec->capacity = 0;
        vec->size = 0;
        vec->element_size = 0;
        return CVEC_ERROR_ALLOCATION_FAILED;
    }

    vec->data = data;
    vec->element_size = element_size;
    vec->size = 0;
    vec->capacity = CVEC_INITIAL_CAPACITY;


    return CVEC_OK;
}

static inline void cvec_delete(c_vector_t* vec) {
    if (!vec) return;
    free(vec->data);
    vec->data = nullptr;
    vec->element_size = 0;
    vec->size = 0;
    vec->capacity = 0;
}

static inline cvec_error_code cvec_clear(c_vector_t* vec) {
    if (!vec) return CVEC_ERROR_NULL_POINTER;
    vec->size = 0;
    return CVEC_OK;
}

static inline size_t cvec_size(const c_vector_t* vec) {
    return (vec) ? vec->size : 0;
}

static inline size_t cvec_capacity(const c_vector_t* vec) {
    return (vec) ? vec->capacity : 0;
}

static inline cvec_error_code cvec_reserve(c_vector_t* vec, const size_t new_capacity) {
    size_t bytes = 0;

    if (!vec) return CVEC_ERROR_NULL_POINTER;
    if (vec->element_size == 0) return CVEC_ERROR_NOT_INITIALIZED;
    if (new_capacity <= vec->capacity) return CVEC_OK;

    if (cvec__bytes_for(new_capacity, vec->element_size, &bytes) != CVEC_OK) {
        return CVEC_ERROR_OVERFLOW;
    }

    void* new_data = realloc(vec->data, bytes);
    if (!new_data) return CVEC_ERROR_ALLOCATION_FAILED;

    vec->data = new_data;
    vec->capacity = new_capacity;
    return CVEC_OK;
}

static inline cvec_error_code cvec__ensure_capacity(c_vector_t* vec, const size_t min_capacity) {
    size_t cap = 0;
    size_t grow = 0;

    if (!vec) return CVEC_ERROR_NULL_POINTER;
    if (vec->element_size == 0) return CVEC_ERROR_NOT_INITIALIZED;
    if (vec->capacity >= min_capacity) return CVEC_OK;

    cap = (vec->capacity == 0) ? CVEC_INITIAL_CAPACITY : vec->capacity;
    while (cap < min_capacity) {
        grow = cap / 2;
        if (grow == 0) grow = 1;
        if (cap > SIZE_MAX - grow) return CVEC_ERROR_OVERFLOW;
        cap += grow;
    }

    return cvec_reserve(vec, cap);
}

static inline cvec_error_code cvec_shrink_to_fit(c_vector_t* vec) {
    size_t bytes = 0;
    void* new_data = nullptr;

    if (!vec) return CVEC_ERROR_NULL_POINTER;
    if (vec->element_size == 0) return CVEC_ERROR_NOT_INITIALIZED;

    if (vec->size == 0) {
        free(vec->data);
        vec->data = nullptr;
        vec->capacity = 0;
        return CVEC_OK;
    }

    if (vec->size == vec->capacity) return CVEC_OK;

    if (cvec__bytes_for(vec->size, vec->element_size, &bytes) != CVEC_OK) {
        return CVEC_ERROR_OVERFLOW;
    }

    new_data = realloc(vec->data, bytes);
    if (!new_data) return CVEC_ERROR_ALLOCATION_FAILED;

    vec->data = new_data;
    vec->capacity = vec->size;
    return CVEC_OK;
}

static inline cvec_error_code cvec_get(c_vector_t* vec, const size_t index, void** out_ptr) {
    if (!vec || !out_ptr) return CVEC_ERROR_NULL_POINTER;
    if (index >= vec->size) return CVEC_ERROR_OUT_OF_BOUNDS;

    *out_ptr = (uint8_t*)vec->data + (index * vec->element_size);
    return CVEC_OK;
}

static inline cvec_error_code cvec_get_const(const c_vector_t* vec, const size_t index, const void** out_ptr) {
    if (!vec || !out_ptr) return CVEC_ERROR_NULL_POINTER;
    if (index >= vec->size) return CVEC_ERROR_OUT_OF_BOUNDS;
    const uint8_t* base = (const uint8_t*)vec->data;
    *out_ptr = base + (index * vec->element_size);
    return CVEC_OK;
}

static inline cvec_error_code cvec_set(c_vector_t* vec, const size_t index, const void* value) {
    if (!vec || !value) return CVEC_ERROR_NULL_POINTER;
    if (index >= vec->size) return CVEC_ERROR_OUT_OF_BOUNDS;

    memcpy((uint8_t*)vec->data + (index * vec->element_size), value, vec->element_size);
    return CVEC_OK;
}

static inline cvec_error_code cvec_front(c_vector_t* vec, void** out_ptr) {
    if (!vec || !out_ptr) return CVEC_ERROR_NULL_POINTER;
    if (vec->size == 0) return CVEC_ERROR_EMPTY_VECTOR;

    *out_ptr = vec->data;
    return CVEC_OK;
}

static inline cvec_error_code cvec_back(c_vector_t* vec, void** out_ptr) {
    if (!vec || !out_ptr) return CVEC_ERROR_NULL_POINTER;
    if (vec->size == 0) return CVEC_ERROR_EMPTY_VECTOR;

    *out_ptr = (uint8_t*)vec->data + ((vec->size - 1) * vec->element_size);
    return CVEC_OK;
}

static inline cvec_error_code cvec_append(c_vector_t* vec, const void* value) {
    cvec_error_code status = CVEC_OK;

    if (!vec || !value) return CVEC_ERROR_NULL_POINTER;
    if (vec->element_size == 0) return CVEC_ERROR_NOT_INITIALIZED;

    status = cvec__ensure_capacity(vec, vec->size + 1);
    if (status != CVEC_OK) return status;

    memcpy((uint8_t*)vec->data + (vec->size * vec->element_size), value, vec->element_size);
    vec->size++;
    return CVEC_OK;
}

static inline cvec_error_code cvec_insert(c_vector_t* vec, const size_t index, const void* value) {
    cvec_error_code status = CVEC_OK;

    if (!vec || !value) return CVEC_ERROR_NULL_POINTER;
    if (vec->element_size == 0) return CVEC_ERROR_NOT_INITIALIZED;
    if (index > vec->size) return CVEC_ERROR_INVALID_INDEX;

    status = cvec__ensure_capacity(vec, vec->size + 1);
    if (status != CVEC_OK) return status;

    if (index < vec->size) {
        memmove(
            (uint8_t*)vec->data + ((index + 1) * vec->element_size),
            (uint8_t*)vec->data + (index * vec->element_size),
            (vec->size - index) * vec->element_size
        );
    }

    memcpy((uint8_t*)vec->data + (index * vec->element_size), value, vec->element_size);
    vec->size++;
    return CVEC_OK;
}

static inline cvec_error_code cvec_remove(c_vector_t* vec, const size_t index) {
    if (!vec) return CVEC_ERROR_NULL_POINTER;
    if (index >= vec->size) return CVEC_ERROR_OUT_OF_BOUNDS;

    if (index + 1 < vec->size) {
        memmove(
            (uint8_t*)vec->data + (index * vec->element_size),
            (uint8_t*)vec->data + ((index + 1) * vec->element_size),
            (vec->size - index - 1) * vec->element_size
        );
    }

    vec->size--;
    return CVEC_OK;
}

static inline cvec_error_code cvec_pop(c_vector_t* vec, void* value) {
    if (!vec || !value) return CVEC_ERROR_NULL_POINTER;
    if (vec->size == 0) return CVEC_ERROR_EMPTY_VECTOR;

    memcpy(value, (uint8_t*)vec->data + (vec->size - 1) * vec->element_size, vec->element_size);
    vec->size--;
    return CVEC_OK;
}

static inline cvec_error_code cvec_fill(c_vector_t* vec, const void* value, const size_t count) {
    cvec_error_code status = CVEC_OK;
    size_t i = 0;

    if (!vec || !value) return CVEC_ERROR_NULL_POINTER;
    if (vec->element_size == 0) return CVEC_ERROR_NOT_INITIALIZED;

    status = cvec__ensure_capacity(vec, count);
    if (status != CVEC_OK) return status;
    
    if (vec->element_size == 1) {
        memset(vec->data, *(const uint8_t*)value, count);
        vec->size = count;
        return CVEC_OK;
    }
    
    for (i = 0; i < count; ++i) {
        memcpy((uint8_t*)vec->data + (i * vec->element_size), value, vec->element_size);
    }
    vec->size = count;
    return CVEC_OK;
}

static inline cvec_error_code cvec_reverse(c_vector_t* vec) {
    size_t left = 0;
    size_t right = 0;

    if (!vec) return CVEC_ERROR_NULL_POINTER;
    if (vec->element_size == 0) return CVEC_ERROR_NOT_INITIALIZED;
    if (vec->size <= 1) return CVEC_OK;

    uint8_t stack_buffer[128];
    void* tmp = (vec->element_size <= sizeof(stack_buffer)) ? stack_buffer : malloc(vec->element_size);
    if (!tmp) return CVEC_ERROR_ALLOCATION_FAILED;

    right = vec->size - 1;
    while (left < right) {
        void* left_ptr = (uint8_t*)vec->data + (left * vec->element_size);
        void* right_ptr = (uint8_t*)vec->data + (right * vec->element_size);

        memcpy(tmp, left_ptr, vec->element_size);
        memcpy(left_ptr, right_ptr, vec->element_size);
        memcpy(right_ptr, tmp, vec->element_size);

        left++;
        right--;
    }

    if (tmp != (void*)stack_buffer) free(tmp);
    return CVEC_OK;
}

static inline cvec_error_code cvec_from_array(
    c_vector_t* out_vec,
    const void* data,
    const size_t count,
    const size_t element_size
) {
    cvec_error_code status = CVEC_OK;
    size_t bytes = 0;

    if (!out_vec || !data) return CVEC_ERROR_NULL_POINTER;
    if (count == 0 || element_size == 0) return CVEC_ERROR_INVALID_SIZE;
    if (out_vec->data != nullptr) cvec_delete(out_vec);

    status = cvec_reserve(out_vec, count);
    if (status != CVEC_OK) {
        cvec_delete(out_vec);
        return status;
    }

    if (cvec__bytes_for(count, element_size, &bytes) != CVEC_OK) {
        cvec_delete(out_vec);
        return CVEC_ERROR_OVERFLOW;
    }

    memcpy(out_vec->data, data, bytes);
    out_vec->size = count;
    out_vec->element_size = element_size;
    return CVEC_OK;
}

static inline cvec_error_code cvec_to_array(const c_vector_t* vec, void** out_array) {
    size_t bytes = 0;
    void* copy = nullptr;

    if (!vec || !out_array) return CVEC_ERROR_NULL_POINTER;
    if (vec->size == 0) return CVEC_ERROR_EMPTY_VECTOR;
    if (vec->element_size == 0) return CVEC_ERROR_NOT_INITIALIZED;

    if (cvec__bytes_for(vec->size, vec->element_size, &bytes) != CVEC_OK) {
        return CVEC_ERROR_OVERFLOW;
    }

    copy = malloc(bytes);
    if (!copy) return CVEC_ERROR_ALLOCATION_FAILED;

    memcpy(copy, vec->data, bytes);
    *out_array = copy;
    return CVEC_OK;
}

static inline cvec_error_code cvec_copy(c_vector_t *src, c_vector_t* dst) {
    cvec_error_code status = CVEC_OK;
    if (!src || !dst) return CVEC_ERROR_NULL_POINTER;
    if (src->size == 0) {
        dst->data = nullptr;
        dst->size = 0;
        dst->capacity = 0;
        dst->element_size = src->element_size; 
        return CVEC_OK;
    }
    if (src == dst) return CVEC_OK;

    size_t bytes = 0;
    status = cvec__bytes_for(src->size, src->element_size, &bytes);
    if (status != CVEC_OK) return status;
    if (dst->data != nullptr) free(dst->data);

    dst->data = malloc(bytes);
    if (!dst->data) return CVEC_ERROR_ALLOCATION_FAILED;
    memcpy(dst->data, src->data, bytes);
    dst->size = src->size;
    dst->capacity = src->capacity;
    dst->element_size = src->element_size;

    return CVEC_OK;
}

#define cvec_at(vec_ptr, index, out_value_ptr) \
    cvec_at_impl((vec_ptr), (index), (out_value_ptr), sizeof(*(out_value_ptr)))

static inline cvec_error_code cvec_at_impl(
    const c_vector_t* vec,
    const size_t index,
    void* out_value,
    const size_t out_size
) {
    if (!vec || !out_value) return CVEC_ERROR_NULL_POINTER;
    if (index >= vec->size) return CVEC_ERROR_OUT_OF_BOUNDS;
    if (out_size != vec->element_size) return CVEC_ERROR_INVALID_SIZE;

    memcpy(out_value, (const uint8_t*)vec->data + (index * vec->element_size), vec->element_size);
    return CVEC_OK;
}

#endif /* C_VECTOR */