#pragma once

#if defined(C_VECTOR)
#include <stdint.h>
#include <stdlib.h>
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
    CVEC_ERROR_INVALID_INDEX,
    CVEC_ERROR_INVALID_VALUE,
    CVEC_ERROR_INVALID_OPERATION,
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

/* Suppress deprecation warnings for internal library use */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#define __CVEC_INTERNAL \
    __attribute__((deprecated( \
    "Use public API instead of internal helpers " \
    "which is not safe to use" \
)))

#define __CVEC_DEPRCATED_WARNING(str) \
    __attribute__((deprecated(str)))

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
        case CVEC_ERROR_INVALID_INDEX: return "invalid index";
        case CVEC_ERROR_INVALID_VALUE: return "invalid value";
        case CVEC_ERROR_INVALID_OPERATION: return "invalid operation";
        default: return "unknown error";
    }
}

static inline bool cvec_is_empty(const c_vector_t* vec) {
    return (!vec) || (vec->size == 0);
}

static inline cvec_error_code cvec_reserve(c_vector_t * vec, size_t new_capacity);
static inline void cvec_delete(c_vector_t * vec);

__CVEC_INTERNAL static inline bool __cvec_mul_overflow(const size_t a, const size_t b, size_t* out) {
    if (!out) return true;
    if (a != 0 && b > SIZE_MAX / a) return true;
    *out = a * b;
    return false;
}

__CVEC_INTERNAL static inline cvec_error_code __cvec_bytes_for(const size_t count, const size_t elem_size, size_t* out_bytes) {
    if (__cvec_mul_overflow(count, elem_size, out_bytes)) return CVEC_ERROR_OVERFLOW;
    return CVEC_OK;
}

__CVEC_INTERNAL static inline cvec_error_code __cvec_reserve(c_vector_t* vec, const size_t new_capacity) {
    size_t bytes = 0;

    if (__cvec_bytes_for(new_capacity, vec->element_size, &bytes) != CVEC_OK) {
        return CVEC_ERROR_OVERFLOW;
    }

    void* new_data = realloc(vec->data, bytes);
    if (!new_data) return CVEC_ERROR_ALLOCATION_FAILED;

    vec->data = new_data;
    vec->capacity = new_capacity;
    return CVEC_OK;
}

__CVEC_INTERNAL static inline cvec_error_code __cvec_ensure_capacity(c_vector_t* vec, const size_t min_capacity) {
    size_t cap = 0;
    if (vec->capacity >= min_capacity) return CVEC_OK;

    cap = (vec->capacity == 0) ? CVEC_INITIAL_CAPACITY : vec->capacity;
    while (cap < min_capacity) {
        size_t grow = cap >> 1;
        if (grow == 0) grow = 1;
        if (cap > SIZE_MAX - grow) {
            return CVEC_ERROR_OVERFLOW;
        }
        size_t next = cap + grow;
        cap = next;
    }

    return __cvec_reserve(vec, cap);
}

__CVEC_INTERNAL static inline bool __cvec_is_all_zero(const void* value, size_t size) {

    static const uint8_t zero_block[256] = {0};
    if (size <= 256) return memcmp(value, zero_block, size) == 0;

    const uint8_t* p = (const uint8_t*)value;
    for (size_t i = 0; i < size; i++) {
        if (p[i] != 0) return false;
    }
    return true;
}

static inline cvec_error_code cvec_init(c_vector_t* vec, const size_t element_size) {
    if (!vec) return CVEC_ERROR_NULL_POINTER;
    if (element_size == 0) return CVEC_ERROR_INVALID_SIZE;

    *vec = (c_vector_t){0};

    size_t bytes = 0;
    cvec_error_code status = __cvec_bytes_for(CVEC_INITIAL_CAPACITY, element_size, &bytes);
    if (status != CVEC_OK) {
        return status;
    }

    void* data = malloc(bytes);
    if (!data) {
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
    if (new_capacity <= vec->capacity) return CVEC_OK;

    cvec_error_code status = __cvec_bytes_for(new_capacity, vec->element_size, &bytes);
    if (status != CVEC_OK) return status;

    void* new_data = realloc(vec->data, bytes);
    if (!new_data) return CVEC_ERROR_ALLOCATION_FAILED;

    vec->data = new_data;
    vec->capacity = new_capacity;
    return CVEC_OK;
}

static inline cvec_error_code cvec_shrink_to_fit(c_vector_t* vec) {
    if (!vec) return CVEC_ERROR_NULL_POINTER;
    if (vec->element_size == 0) return CVEC_ERROR_NOT_INITIALIZED;
    if (vec->size == 0) {
        free(vec->data);
        vec->data = nullptr;
        vec->capacity = 0;
        return CVEC_OK;
    }

    if (vec->size == vec->capacity) return CVEC_OK;
    cvec_error_code status = __cvec_reserve(vec, vec->size);
    if (status != CVEC_OK) return status;

    return CVEC_OK;
}

static inline cvec_error_code cvec_resize(c_vector_t* vec, const size_t new_size, const void* default_value) {
    if (!vec) return CVEC_ERROR_NULL_POINTER;
    if (vec->element_size == 0) return CVEC_ERROR_NOT_INITIALIZED;
    if (new_size == vec->size) return CVEC_OK;

    if (new_size < vec->size) {
        vec->size = new_size;
        return CVEC_OK;
    }
    cvec_error_code status = CVEC_OK;
    if (new_size > vec->capacity) {
        status = __cvec_reserve(vec, new_size);
        if (status != CVEC_OK) return status;
    }

    if (default_value) {
        size_t num_new = new_size - vec->size;

        if (vec->element_size == 1) {
            size_t offset = 0;
            if (__cvec_mul_overflow(vec->size, vec->element_size, &offset)) {
                return CVEC_ERROR_OVERFLOW;
            }
            memset((uint8_t*)vec->data + offset,
                   *(const uint8_t*)default_value, num_new);
        } else if (__cvec_is_all_zero(default_value, vec->element_size)) {
            size_t offset = 0;
            if (__cvec_mul_overflow(vec->size, vec->element_size, &offset)) {
                return CVEC_ERROR_OVERFLOW;
            }
            memset((uint8_t*)vec->data + offset, 0, num_new * vec->element_size);
        } else {
            for (size_t i = vec->size; i < new_size; ++i) {
                size_t loop_offset = 0;
                if (__cvec_mul_overflow(i, vec->element_size, &loop_offset)) {
                    return CVEC_ERROR_OVERFLOW;
                }
                memcpy((uint8_t*)vec->data + loop_offset,
                       default_value, vec->element_size);
            }
        }
    }

    vec->size = new_size;
    return CVEC_OK;
}

static inline cvec_error_code cvec_get(c_vector_t* vec, const size_t index, void** out_ptr) {
    if (!vec || !out_ptr) return CVEC_ERROR_NULL_POINTER;
    if (index >= vec->size) return CVEC_ERROR_OUT_OF_BOUNDS;

    size_t offset = 0;
    if (__cvec_mul_overflow(index, vec->element_size, &offset)) {
        return CVEC_ERROR_OVERFLOW;
    }

    *out_ptr = (uint8_t*)vec->data + offset;
    return CVEC_OK;
}

static inline cvec_error_code cvec_get_const(const c_vector_t* vec, const size_t index, const void** out_ptr) {
    if (!vec || !out_ptr) return CVEC_ERROR_NULL_POINTER;
    if (index >= vec->size) return CVEC_ERROR_OUT_OF_BOUNDS;

    size_t offset = 0;
    if (__cvec_mul_overflow(index, vec->element_size, &offset)) {
        return CVEC_ERROR_OVERFLOW;
    }
    const uint8_t* base = vec->data;
    *out_ptr = base + offset;
    return CVEC_OK;
}

static inline cvec_error_code cvec_set(c_vector_t* vec, const size_t index, const void* value) {
    if (!vec || !value) return CVEC_ERROR_NULL_POINTER;
    if (index >= vec->size) return CVEC_ERROR_OUT_OF_BOUNDS;

    size_t offset = 0;
    if (__cvec_mul_overflow(index, vec->element_size, &offset)) {
        return CVEC_ERROR_OVERFLOW;
    }

    memcpy((uint8_t*)vec->data + offset, value, vec->element_size);
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

    size_t offset = 0;
    if (__cvec_mul_overflow(vec->size - 1, vec->element_size, &offset)) {
        return CVEC_ERROR_OVERFLOW;
    }

    *out_ptr = (uint8_t*)vec->data + offset;
    return CVEC_OK;
}

static inline cvec_error_code cvec_append(c_vector_t* vec, const void* value) {
    cvec_error_code status = CVEC_OK;
    if (!vec || !value) return CVEC_ERROR_NULL_POINTER;
    if (vec->element_size == 0) return CVEC_ERROR_NOT_INITIALIZED;

    status = __cvec_ensure_capacity(vec, vec->size + 1);
    if (status != CVEC_OK) return status;

    size_t offset = 0;
    if (__cvec_mul_overflow(vec->size, vec->element_size, &offset)) {
        return CVEC_ERROR_OVERFLOW;
    }

    memcpy((uint8_t*)vec->data + offset, value, vec->element_size);
    vec->size++;
    return CVEC_OK;
}

static inline cvec_error_code cvec_insert(c_vector_t* vec, const size_t index, const void* value) {
    cvec_error_code status = CVEC_OK;

    if (!vec || !value) return CVEC_ERROR_NULL_POINTER;
    if (vec->element_size == 0) return CVEC_ERROR_NOT_INITIALIZED;
    if (index > vec->size) return CVEC_ERROR_INVALID_INDEX;

    status = __cvec_ensure_capacity(vec, vec->size + 1);
    if (status != CVEC_OK) return status;

    if (index < vec->size) {
        memmove(
            (uint8_t*)vec->data + ((index + 1) * vec->element_size),
            (uint8_t*)vec->data + (index * vec->element_size),
            (vec->size - index) * vec->element_size
        );
    }

    size_t offset = 0;
    if (__cvec_mul_overflow(index, vec->element_size, &offset)) {
        return CVEC_ERROR_OVERFLOW;
    }

    memcpy((uint8_t*)vec->data + offset, value, vec->element_size);
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

    size_t offset = 0;
    if (__cvec_mul_overflow(vec->size - 1, vec->element_size, &offset)) {
        return CVEC_ERROR_OVERFLOW;
    }

    memcpy(value, (uint8_t*)vec->data + offset, vec->element_size);
    vec->size--;
    return CVEC_OK;
}

static inline cvec_error_code cvec_pop_discard(c_vector_t* vec) {
    if (!vec) return CVEC_ERROR_NULL_POINTER;
    if (vec->size == 0) return CVEC_ERROR_EMPTY_VECTOR;

    vec->size--;
    return CVEC_OK;
}

#define is_cvec(x) _Generic((x), \
    c_vector_t: true, \
    default: false \
)

static inline cvec_error_code cvec_extend(c_vector_t* dst, const c_vector_t* src) {
    if (!dst || !src) return CVEC_ERROR_NULL_POINTER;
    if (dst->element_size != src->element_size) return CVEC_ERROR_INVALID_SIZE;
    if (src->size == 0) return CVEC_OK;
    size_t new_size = dst->size + src->size;
    if (new_size < dst->size) return CVEC_ERROR_OVERFLOW;
    cvec_error_code status = __cvec_ensure_capacity(dst, new_size);
    if (status != CVEC_OK) return status;
    memcpy(
        (uint8_t*)dst->data + dst->size * dst->element_size,
        src->data,
        src->size * src->element_size
    );
    dst->size = new_size;
    return CVEC_OK;
}

static inline cvec_error_code cvec_fill(c_vector_t* vec, const void* value, const size_t count) {
    cvec_error_code status = CVEC_OK;
    size_t i = 0;

    if (!vec || !value) return CVEC_ERROR_NULL_POINTER;
    if (vec->element_size == 0) return CVEC_ERROR_NOT_INITIALIZED;

    status = __cvec_ensure_capacity(vec, count);
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

    uint8_t stack_buffer[256];
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

    out_vec->element_size = element_size;
    status = __cvec_reserve(out_vec, count);
    if (status != CVEC_OK) {
        cvec_delete(out_vec);
        return status;
    }

    if (__cvec_bytes_for(count, element_size, &bytes) != CVEC_OK) {
        cvec_delete(out_vec);
        return CVEC_ERROR_OVERFLOW;
    }

    memcpy(out_vec->data, data, bytes);
    out_vec->size = count;
    return CVEC_OK;
}

static inline cvec_error_code cvec_to_array(const c_vector_t* vec, void** out_array) {
    size_t bytes = 0;

    if (!vec || !out_array) return CVEC_ERROR_NULL_POINTER;
    if (vec->size == 0) return CVEC_ERROR_EMPTY_VECTOR;
    if (vec->element_size == 0) return CVEC_ERROR_NOT_INITIALIZED;
    cvec_error_code status = __cvec_bytes_for(vec->size, vec->element_size, &bytes);
    if (status != CVEC_OK) return CVEC_ERROR_OVERFLOW;

    void* copy = malloc(bytes);
    if (!copy) return CVEC_ERROR_ALLOCATION_FAILED;

    memcpy(copy, vec->data, bytes);
    *out_array = copy;
    return CVEC_OK;
}

static inline cvec_error_code cvec_copy(c_vector_t *src, c_vector_t* dst) {
    cvec_error_code status = CVEC_OK;
    if (!src || !dst) return CVEC_ERROR_NULL_POINTER;
    if (src == dst) return CVEC_OK;
    if (src->size == 0) {
        if (dst->data != nullptr) free(dst->data);
        dst->data = nullptr;
        dst->size = 0;
        dst->capacity = 0;
        dst->element_size = src->element_size;
        return CVEC_OK;
    }

    size_t bytes = 0;
    status = __cvec_bytes_for(src->size, src->element_size, &bytes);
    if (status != CVEC_OK) return status;
    if (dst->data != nullptr) free(dst->data);

    dst->data = malloc(bytes);
    if (!dst->data) return CVEC_ERROR_ALLOCATION_FAILED;
    memcpy(dst->data, src->data, bytes);
    dst->size = src->size;
    dst->capacity = src->size;
    dst->element_size = src->element_size;

    return CVEC_OK;
}

static inline cvec_error_code cvec_move(c_vector_t *src, c_vector_t* dst) {
    if (!src || !dst) return CVEC_ERROR_NULL_POINTER;
    cvec_delete(dst);
    *dst = *src;
    *src = (c_vector_t){0};
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

typedef int (*cvec_compare_fn)(const void* a, const void* b);

static inline cvec_error_code cvec_sort(c_vector_t* vec, cvec_compare_fn compare) {
    if (!vec || !compare) return CVEC_ERROR_NULL_POINTER;
    if (vec->size <= 1) return CVEC_OK;

    qsort(vec->data, vec->size, vec->element_size, compare);
    return CVEC_OK;
}

#pragma GCC diagnostic pop
#endif /* C_VECTOR */