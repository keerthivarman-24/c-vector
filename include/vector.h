#pragma once

#if defined(C_VECTOR)
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Error codes
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
    CVEC_ERROR_NOT_IMPLEMENTED,
    CVEC_ERROR_NOT_SUPPORTED,
    CVEC_ERROR_INVALID_INDEX,
    CVEC_ERROR_INVALID_VALUE,
    CVEC_ERROR_INVALID_OPERATION,
    CVEC_ERROR_DIVIDE_BY_ZERO,
    CVEC_ERROR_DUE_TO_PREVIOUS_OPERATION,
} cvec_error_code;

typedef struct {
    void*  data;
    size_t element_size;
    size_t size;
    size_t capacity;
} c_vector_t;

struct cvec_error {
    cvec_error_code error_code;
    const char* error_msg;
};

typedef struct {
    bool success;
    struct cvec_error error;
} cvec_result;

typedef struct {
    bool success;
    union {
        bool value;
        struct cvec_error error;
    } data;
} cvec_bool_result;

typedef struct {
    bool success;
    union {
        void* ptr;
        struct cvec_error error;
    } data;
} cvec_vector_ptr;

typedef struct {
    bool success;
    union {
        c_vector_t vector;
        struct cvec_error error;
    } data;
} cvec_vector;

static cvec_vector_ptr cvec_vector_ptr_error(const cvec_error_code code, const char* msg) {
    cvec_vector_ptr result = { .success = false, .data.error = {code, msg} };
    return result;
}

static cvec_vector_ptr cvec_ptr_ok(void* ptr) {
    const cvec_vector_ptr result = { .success = true, .data.ptr = ptr };
    return result;
}

static cvec_vector cvec_vector_error(const cvec_error_code code, const char* msg) {
    const cvec_vector result = { .success = false, .data.error = {code, msg} };
    return result;
}

static cvec_vector cvec_vector_ok(const c_vector_t vec) {
    const cvec_vector result = { .success = true, .data.vector = vec };
    return result;
}

static cvec_result cvec_error(const cvec_error_code code, const char* msg) {
    const cvec_result result = { .success = false, .error = {code, msg} };
    return result;
}

static cvec_result cvec_ok(void) {
    const cvec_result result = { .success = true, .error = {CVEC_OK, NULL} };
    return result;
}

static cvec_bool_result cvec_bool_error(const cvec_error_code code, const char* msg) {
    cvec_bool_result result = { .success = false, .data.error = {code, msg} };
    return result;
}

static cvec_bool_result cvec_bool_ok(const bool value) {
    cvec_bool_result result = { .success = true, .data.value = value };
    return result;
}

static cvec_result cvec_init(c_vector_t* arr, const size_t element_size) {
    if (!arr) return cvec_error(
        CVEC_ERROR_NULL_POINTER,
        "cvec_init: vector is null"
        );
    if (element_size == 0) return cvec_error(
        CVEC_ERROR_INVALID_SIZE,
        "cvec_init: element_size is must not be equal to zero"
        );

    arr->element_size = element_size;
    arr->size = 0;
    arr->capacity = 8;

    if (arr->capacity > SIZE_MAX / arr->element_size) {
        return cvec_error(
            CVEC_ERROR_OVERFLOW,
            "cvec_init: allocation size is overflow"
            );
    }

    arr->data = malloc(element_size * arr->capacity);
    if (!arr->data) return cvec_error(
        CVEC_ERROR_ALLOCATION_FAILED,
        "cvec_init: Failed allocation memory"
        );

    return cvec_ok();
}

static cvec_result cvec_delete(c_vector_t* arr) {
    if (!arr) return cvec_error(
        CVEC_ERROR_NULL_POINTER,
        "cvec_delete: vector is null"
        );
    if (!arr->data || arr->capacity == 0) return cvec_error(
            CVEC_ERROR_NOT_INITIALIZED,
            "cvec_delete: vector is not initialised"
        );
    
    free(arr->data);
    arr->data = NULL;
    arr->size = 0;
    arr->capacity = 0;
    arr->element_size = 0;

    return cvec_ok();
}

static cvec_vector_ptr cvec_get(c_vector_t* arr, const size_t index) {
    if (!arr) return cvec_vector_ptr_error(
        CVEC_ERROR_NULL_POINTER,
        "cvec_get: vector is null"
        );
    if (index >= arr->size) return cvec_vector_ptr_error(
        CVEC_ERROR_OUT_OF_BOUNDS,
        "cvec_get: index is out of range"
    );

    return cvec_ptr_ok((uint8_t*)arr->data + index * arr->element_size);
}

static cvec_vector cvec_set(c_vector_t* arr, const size_t index, const void* value) {
    if (!arr || !value) return cvec_vector_error(
        CVEC_ERROR_NULL_POINTER,
        "cvec_set: vector or value is null"
        );
    if (index >= arr->size) return cvec_vector_error(
        CVEC_ERROR_OUT_OF_BOUNDS,
        "cvec_set: index is out of range"
    );

    cvec_vector_ptr get_result = cvec_get(arr, index);
    if (!get_result.success) {
        return cvec_vector_error(get_result.data.error.error_code, get_result.data.error.error_msg);
    }

    memcpy(get_result.data.ptr, value, arr->element_size);
    return cvec_vector_ok(*arr);
}

static cvec_vector cvec_append(c_vector_t* arr, const void* value) {
    if (!arr || !value) return cvec_vector_error(
        CVEC_ERROR_NULL_POINTER,
        "cvec_append: vector or value is null"
        );

    if (arr->size == arr->capacity) {
        if (arr->capacity > SIZE_MAX - (arr->capacity >> 1)) return cvec_vector_error(
            CVEC_ERROR_OVERFLOW,
            "cvec_append: allocation size is overflow"
            );

        arr->capacity += arr->capacity >> 1;
        if (arr->capacity > SIZE_MAX / arr->element_size) return cvec_vector_error(
            CVEC_ERROR_OVERFLOW,
            "cvec_append: allocation size is overflow"
            );

        void* tmp = realloc(arr->data, arr->element_size * arr->capacity);
        if (!tmp) return cvec_vector_error(
            CVEC_ERROR_ALLOCATION_FAILED,
            "cvec_append: Failed allocation memory"
        );
        arr->data = tmp;
    }

    memcpy((uint8_t*)arr->data + arr->size * arr->element_size, value, arr->element_size);
    arr->size++;
    return cvec_vector_ok(*arr);
}

static bool cvec_is_empty(const c_vector_t* arr) {
    return arr ? arr->size == 0 : true;
}

static cvec_vector_ptr cvec_front(c_vector_t* arr) {
    if (!arr) return cvec_vector_ptr_error(
        CVEC_ERROR_NULL_POINTER,
        "cvec_front: vector is NULL"
        );
    if (arr->size == 0) return cvec_vector_ptr_error(
        CVEC_ERROR_EMPTY_VECTOR,
        "cvec_front: vector is empty"
        );

    return cvec_ptr_ok(arr->data);
}

static cvec_vector_ptr cvec_back(c_vector_t* arr) {
    if (!arr) return cvec_vector_ptr_error(
        CVEC_ERROR_NULL_POINTER,
        "cvec_back: vector is NULL"
        );
    if (arr->size == 0) return cvec_vector_ptr_error(
        CVEC_ERROR_EMPTY_VECTOR,
        "cvec_back: vector is empty"
        );

    return cvec_ptr_ok((uint8_t*)arr->data + (arr->size - 1) * arr->element_size);
}

static cvec_result cvec_remove(c_vector_t* arr, const size_t index) {
    if (!arr) return cvec_error(CVEC_ERROR_NULL_POINTER, "cvec_remove: arr is NULL");
    if (index >= arr->size) return cvec_error(CVEC_ERROR_OUT_OF_BOUNDS, "cvec_remove: index is out of range");

    if (arr->size - index - 1 > 0) {
        memmove(
            (uint8_t*)arr->data + index * arr->element_size,
            (uint8_t*)arr->data + (index + 1) * arr->element_size,
            (arr->size - index - 1) * arr->element_size
        );
    }

    arr->size--;
    return cvec_ok();
}

static cvec_result cvec_clear(c_vector_t* arr) {
    if (!arr) return cvec_error(CVEC_ERROR_NULL_POINTER, "cvec_clear: arr is NULL");
    arr->size = 0;
    return cvec_ok();
}

static cvec_result cvec_reserve(c_vector_t* arr, const size_t capacity) {
    if (!arr) return cvec_error(CVEC_ERROR_NULL_POINTER, "cvec_reserve: arr is NULL");
    if (arr->element_size == 0) return cvec_error(CVEC_ERROR_NOT_INITIALIZED, "cvec_reserve: element_size not initialized");
    if (capacity <= arr->capacity) return cvec_ok();

    if (capacity > SIZE_MAX / arr->element_size) {
        return cvec_error(CVEC_ERROR_OVERFLOW, "cvec_reserve: allocation size overflow");
    }

    void* tmp = realloc(arr->data, arr->element_size * capacity);
    if (!tmp) return cvec_error(CVEC_ERROR_ALLOCATION_FAILED, "cvec_reserve: Failed to reallocate memory");

    arr->data = tmp;
    arr->capacity = capacity;
    return cvec_ok();
}

static cvec_result cvec_reverse(c_vector_t* arr) {
    if (!arr) return cvec_error(CVEC_ERROR_NULL_POINTER, "cvec_reverse: arr is NULL");
    if (arr->size <= 1) return cvec_ok();
    
    void* tmp = malloc(arr->element_size);
    if (!tmp) return cvec_error(CVEC_ERROR_ALLOCATION_FAILED, "cvec_reverse: Failed to allocate temporary buffer");
    
    size_t left = 0;
    size_t right = arr->size - 1;
    while (left < right) {
        void* left_ptr = (uint8_t*)arr->data + left * arr->element_size;
        void* right_ptr = (uint8_t*)arr->data + right * arr->element_size;
        
        memcpy(tmp, left_ptr, arr->element_size);
        memcpy(left_ptr, right_ptr, arr->element_size);
        memcpy(right_ptr, tmp, arr->element_size);
        
        left++;
        right--;
    }
    
    free(tmp);
    return cvec_ok();
}

#define cvec(array) cvec_from_array((array), sizeof(array) / sizeof((array)[0]), sizeof((array)[0]))

static cvec_vector cvec_from_array(const void* data, const size_t count, const size_t element_size) {
    if (!data) return cvec_vector_error(CVEC_ERROR_NULL_POINTER, "cvec_from_array: data is NULL");
    if (count == 0) return cvec_vector_error(CVEC_ERROR_INVALID_SIZE, "cvec_from_array: count must be > 0");
    if (element_size == 0) return cvec_vector_error(CVEC_ERROR_INVALID_SIZE, "cvec_from_array: element_size must be > 0");

    c_vector_t arr;
    cvec_result init_result = cvec_init(&arr, element_size);
    if (!init_result.success) {
        return cvec_vector_error(init_result.error.error_code, init_result.error.error_msg);
    }

    cvec_result reserve_result = cvec_reserve(&arr, count);
    if (!reserve_result.success) {
        cvec_delete(&arr);
        return cvec_vector_error(reserve_result.error.error_code, reserve_result.error.error_msg);
    }

    memcpy(arr.data, data, count * element_size);
    arr.size = count;
    return cvec_vector_ok(arr);
}

static cvec_vector_ptr cvec_to_array(c_vector_t* arr) {
    if (!arr) return cvec_vector_ptr_error(CVEC_ERROR_NULL_POINTER, "cvec_to_array: arr is NULL");
    if (arr->size == 0) return cvec_vector_ptr_error(CVEC_ERROR_EMPTY_VECTOR, "cvec_to_array: arr is empty");

    if (arr->size > SIZE_MAX / arr->element_size) {
        return cvec_vector_ptr_error(CVEC_ERROR_OVERFLOW, "cvec_to_array: allocation size overflow");
    }

    void* array = malloc(arr->element_size * arr->size);
    if (!array) return cvec_vector_ptr_error(CVEC_ERROR_ALLOCATION_FAILED, "cvec_to_array: Failed to allocate memory");
    memcpy(array, arr->data, arr->element_size * arr->size);
    return cvec_ptr_ok(array);
}

static cvec_result cvec_shrink_to_fit(c_vector_t* arr) {
    if (!arr) return cvec_error(CVEC_ERROR_NULL_POINTER, "cvec_shrink_to_fit: arr is NULL");
    if (arr->size == arr->capacity) return cvec_ok();

    if (arr->size == 0) {
        free(arr->data);
        arr->data = NULL;
        arr->capacity = 0;
        return cvec_ok();
    }

    void* tmp = realloc(arr->data, arr->element_size * arr->size);
    if (!tmp) return cvec_error(CVEC_ERROR_ALLOCATION_FAILED, "cvec_shrink_to_fit: Failed to reallocate memory");
    arr->data = tmp;
    arr->capacity = arr->size;
    return cvec_ok();
}

static cvec_result cvec_fill(c_vector_t* arr, const void* value) {
    if (!arr) return cvec_error(CVEC_ERROR_NULL_POINTER, "cvec_fill: arr is NULL");
    if (!value) return cvec_error(CVEC_ERROR_NULL_POINTER, "cvec_fill: value is NULL");
    if (arr->element_size == 0) return cvec_error(CVEC_ERROR_NOT_INITIALIZED, "cvec_fill: element_size must not be zero");
    if (!arr->data && arr->capacity > 0) return cvec_error(CVEC_ERROR_NOT_INITIALIZED, "cvec_fill: data is NULL");

    arr->size = arr->capacity;
    for (size_t i = 0; i < arr->size; i++) {
        memcpy((uint8_t*)arr->data + (i * arr->element_size), value, arr->element_size);
    }

    return cvec_ok();
}

// TODO: Needs to works on proper display and enhance
static cvec_vector_ptr cvec_to_string(const c_vector_t* arr) {
    fprintf(stdout, "Warning: cvec_to_string is not fully supported\n");
    if (!arr) return cvec_vector_ptr_error(CVEC_ERROR_NULL_POINTER, "cvec_to_string: arr is NULL");
    
    if (arr->size == 0) {
        char* str = malloc(4);
        if (!str) return cvec_vector_ptr_error(CVEC_ERROR_ALLOCATION_FAILED, "cvec_to_string: failed to allocate memory");
        strcpy(str, "[]");
        return cvec_ptr_ok(str);
    }

    // Estimate buffer size: "[ " + (pointer_width * 2 + 2) per element + " ]" + null terminator
    size_t estimated_size = arr->size * 25 + 10;
    char* str = malloc(estimated_size);
    if (!str) return cvec_vector_ptr_error(CVEC_ERROR_ALLOCATION_FAILED, "cvec_to_string: failed to allocate memory");

    strcpy(str, "[ ");
    
    // Format each element as pointer address
    for (size_t i = 0; i < arr->size; i++) {
        int bytes_written = snprintf(str + strlen(str), estimated_size - strlen(str), 
                                     "%p%s",
                                     (uint8_t*)arr->data + (i * arr->element_size),
                                     i < arr->size - 1 ? ", " : "");
        if (bytes_written < 0) {
            free(str);
            return cvec_vector_ptr_error(CVEC_ERROR_INVALID_OPERATION, "cvec_to_string: buffer write failed");
        }
    }
    
    strcat(str, " ]");
    return cvec_ptr_ok(str);
}

#define cvec_at(arr, index, type) (*(type*)cvec_get(&(arr), (index)).data.ptr)

#endif //C_VECTOR