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
    CVEC_ERROR_NOT_INITIALIZED
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
    cvec_vector_ptr result = { .success = true, .data.ptr = ptr };
    return result;
}

static cvec_vector cvec_vector_error(const cvec_error_code code, const char* msg) {
    cvec_vector result = { .success = false, .data.error = {code, msg} };
    return result;
}

static cvec_vector cvec_vector_ok(const c_vector_t vec) {
    cvec_vector result = { .success = true, .data.vector = vec };
    return result;
}

static cvec_result cvec_error(const cvec_error_code code, const char* msg) {
    cvec_result result = { .success = false, .error = {code, msg} };
    return result;
}

static cvec_result cvec_ok(void) {
    cvec_result result = { .success = true, .error = {CVEC_OK, nullptr} };
    return result;
}

static void die(const char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

static void custom_die(const char* msg) {
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

static void cvec_init(c_vector_t* arr, const size_t element_size) {
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

static void cvec_delete(c_vector_t* arr) {
    if (!arr) custom_die("cvec_delete: arr is NULL");
    if (!arr->data || arr->capacity == 0) custom_die("cvec_delete: vector not initialized");
    
    free(arr->data);
    arr->data = nullptr;
    arr->size = 0;
    arr->capacity = 0;
    arr->element_size = 0;
}

static void* cvec_get(c_vector_t* arr, const size_t index) {
    if (!arr) custom_die("cvec_get: arr is NULL");
    if (index >= arr->size) custom_die("cvec_get: index is out of range");

    return (uint8_t*)arr->data + index * arr->element_size;
}

static void cvec_set(c_vector_t* arr, const size_t index, const void* value) {
    if (!arr || !value) custom_die("cvec_set: invalid args");
    if (index >= arr->size) custom_die("cvec_set: index is out of range");

    memcpy(cvec_get(arr, index), value,  arr->element_size);
}

static void cvec_append(c_vector_t* arr, const void* value) {
    if (!arr || !value) custom_die("cvec_append: invalid args");

    if (arr->size == arr->capacity) {
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

static bool cvec_is_empty(const c_vector_t* arr) {
    if (!arr) custom_die("cvec_is_empty: arr is NULL");
    return arr->size == 0;
}

static void* cvec_front(c_vector_t* arr) {
    if (!arr) custom_die("cvec_front: arr is NULL");
    if (arr->size == 0) custom_die("cvec_front: vector is empty");
    return arr->data;
}

static void* cvec_back(c_vector_t* arr) {
    if (!arr) custom_die("cvec_back: arr is NULL");
    if (arr->size == 0) custom_die("cvec_back: vector is empty");
    return cvec_get(arr, arr->size - 1);
}

static void cvec_remove(c_vector_t* arr, const size_t index) {
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

static void cvec_clear(c_vector_t* arr) {
    if (!arr) custom_die("cvec_clear: arr is NULL");
    arr->size = 0;
}

static void cvec_reserve(c_vector_t* arr, const size_t capacity, const size_t element_size) {
    if (!arr) custom_die("cvec_reserve: arr is NULL");
    // if (capacity <= arr->capacity) return;
    if (capacity > SIZE_MAX / arr->element_size) {
        custom_die("cvec_reserve: allocation size overflow");
    }
    void *tmp = nullptr;
    if (arr->element_size == 0) {
        arr->element_size = element_size;
        tmp = malloc(arr->element_size * capacity);
        if (!tmp) die("cvec_reserve: Failed to allocate memory");
    }else {
        tmp = realloc(arr->data, arr->element_size * capacity);
        if (!tmp) die("cvec_reserve: Failed to reallocate memory");
    }
    arr->data = tmp;
    arr->capacity = capacity;
}

static void cvec_reverse(c_vector_t* arr) {
    if (!arr) custom_die("cvec_reverse: arr is NULL");
    if (arr->size <= 1) return;
    
    void* tmp = malloc(arr->element_size);
    if (!tmp) die("cvec_reverse: Failed to allocate temporary buffer");
    
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
}

#define cvec(array) cvec_from_array((array), sizeof(array) / sizeof((array)[0]), sizeof((array)[0]))

static c_vector_t cvec_from_array(const void* data, const size_t count, const size_t element_size) {
    if (!data) custom_die("cvec_from_array: data is NULL");
    if (count == 0) custom_die("cvec_from_array: count must be > 0");
    if (element_size == 0) custom_die("cvec_from_array: element_size must be > 0");

    c_vector_t arr;
    cvec_init(&arr, element_size);
    cvec_reserve(&arr, count, element_size);
    memcpy(arr.data, data, count * element_size);
    arr.size = count;
    return arr;
}

static void* cvec_to_array(c_vector_t* arr) {
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

static void cvec_shrink_to_fit(c_vector_t* arr) {
    if (!arr) custom_die("cvec_shrink_to_fit: arr is NULL");
    if (arr->size == arr->capacity) return;

    if (arr->size == 0) {
        free(arr->data);
        arr->data = nullptr;
        arr->capacity = 0;
        return;
    }

    void* tmp = realloc(arr->data, arr->element_size * arr->size);
    if (!tmp) { die("cvec_shrink_to_fit: Failed to reallocate memory"); }
    arr->data = tmp;
    arr->capacity = arr->size;
}

static void cvec_fill(c_vector_t* arr, const void* value) {
    if (!arr) custom_die("cvec_fill: arr is NULL");
    if (!value) custom_die("cvec_fill: value is NULL");
    if (arr->element_size == 0) custom_die("cvec_fill: element_size must not be zero");
    if (!arr->data && arr->capacity > 0) custom_die("cvec_fill: data is NULL");

    arr->size = arr->capacity;
    for (size_t i = 0; i < arr->size; i++) {
        memcpy((uint8_t*)arr->data + (i * arr->element_size), value, arr->element_size);
    }
}

// TODO: Needs to works on proper display and enhance
static char* cvec_to_string(const c_vector_t* arr) {
    if (!arr) custom_die("cvec_to_string: arr is NULL");
    
    if (arr->size == 0) {
        char* str = malloc(4);
        if (!str) die("cvec_to_string: failed to allocate memory");
        strcpy(str, "[]");
        return str;
    }

    // Estimate buffer size: "[ " + (pointer_width * 2 + 2) per element + " ]" + null terminator
    size_t estimated_size = arr->size * 25 + 10;
    char* str = malloc(estimated_size);
    if (!str) die("cvec_to_string: failed to allocate memory");

    strcpy(str, "[ ");
    
    // Format each element as pointer address
    for (size_t i = 0; i < arr->size; i++) {
        int bytes_written = snprintf(str + strlen(str), estimated_size - strlen(str), 
                                     "%p%s",
                                     (uint8_t*)arr->data + (i * arr->element_size),
                                     i < arr->size - 1 ? ", " : "");
        if (bytes_written < 0) {
            custom_die("cvec_to_string: buffer overflow");
        }
    }
    
    strcat(str, " ]");
    return str;
}

#define cvec_at(arr, index, type) (*(type*)cvec_get(&(arr), (index)))

#endif //C_VECTOR