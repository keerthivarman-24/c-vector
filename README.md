# C-Vector

A lightweight, generic dynamic array (vector) implementation in C23, providing type-safe operations with automatic memory management and overflow protection.

## Features

- **Generic Type Support** - Works with any data type using `void*` pointers
- **Dynamic Resizing** - Automatically grows capacity with 1.5x growth factor
- **Memory Safe** - Comprehensive error checking, overflow protection, and cleanup functions
- **Header-Only** - Simple single-header library design

## Quick Start

### Installation

Simply include the header file in your project:

```c
#define C_VECTOR
#include "vector.h"
```

### Basic Usage

```c
#include <stdio.h>
#define C_VECTOR
#include "vector.h"

int main(void) {
  c_vector_t vec;

  /* Initialize vector for integers */
  cvec_error_code status = cvec_init(&vec, sizeof(int));
  if (status != CVEC_OK) {
    fprintf(stderr, "cvec_init failed: %s\n", cvec_error_message(status));
    return 1;
  }

  /* Add elements */
  int a = 10, b = 20, c = 30;
  if (cvec_append(&vec, &a) != CVEC_OK) { fprintf(stderr, "append failed\n"); cvec_delete(&vec); return 1; }
  if (cvec_append(&vec, &b) != CVEC_OK) { fprintf(stderr, "append failed\n"); cvec_delete(&vec); return 1; }
  if (cvec_append(&vec, &c) != CVEC_OK) { fprintf(stderr, "append failed\n"); cvec_delete(&vec); return 1; }

  /* Access elements safely */
  for (size_t i = 0; i < cvec_size(&vec); i++) {
    void* ptr;
    cvec_error_code status = cvec_get(&vec, i, &ptr);
    if (status != CVEC_OK) { fprintf(stderr, "%s\n", cvec_error_message(status)); break; }
    int value = *(int*)ptr;
    printf("vector[%zu] = %d\n", i, value);
  }

  /* Clean up */
  cvec_delete(&vec);
  return 0;
}
```

## API Reference

### Data Structure

```c
typedef struct {
    void*  data;           // Pointer to the data buffer
    size_t element_size;   // Size of each element in bytes
    size_t size;           // Current number of elements
    size_t capacity;       // Total allocated capacity
} c_vector_t;
```

### Core Functions

#### `cvec_init`
```
static cvec_error_code cvec_init(c_vector_t* arr, const size_t element_size)
```
Initializes a new vector with the specified element size. Returns a `cvec_error_code` indicating success or error.

- **Parameters:**
  - `arr` - Pointer to the vector to initialize
  - `element_size` - Size of each element in bytes (use `sizeof(type)`)
- **Initial Capacity:** 8 elements
- **Example:** `cvec_init(&vec, sizeof(int));`

---

#### `cvec_delete`
```
static void cvec_delete(c_vector_t* arr)
```
Frees all memory associated with the vector and resets its state. Safe to call multiple times.

- **Parameters:**
  - `arr` - Pointer to the vector to delete
- **Example:** `cvec_delete(&vec);`

---

#### `cvec_append`
```
static cvec_error_code cvec_append(c_vector_t* arr, const void* value)
```
Appends an element to the end of the vector. Automatically resizes if needed (1.5x growth). Returns `cvec_error_code` and does not abort on failure.

- **Parameters:**
  - `arr` - Pointer to the vector
  - `value` - Pointer to the value to append
- **Time Complexity:** O(1) amortized, O(n) when resizing
- **Example:** 
  ```c
  int value = 42;
  cvec_append(&vec, &value);
  ```

---

#### `cvec_remove`
```
static cvec_error_code cvec_remove(c_vector_t* arr, const size_t index)
```
Removes element at the specified index. Shifts subsequent elements left. Returns `cvec_error_code`.

- **Parameters:**
  - `arr` - Pointer to the vector
  - `index` - Index of element to remove
- **Time Complexity:** O(n) due to element shifting
- **Example:** `cvec_remove(&vec, 2);`

---

#### `cvec_clear`
```
static cvec_error_code cvec_clear(c_vector_t* arr)
```
Resets the vector size to 0 without freeing memory. Keeps capacity unchanged. Returns `cvec_error_code`.

- **Parameters:**
  - `arr` - Pointer to the vector
- **Example:** `cvec_clear(&vec);`

---

#### `cvec_is_empty`
```
static bool cvec_is_empty(const c_vector_t* arr)
```
Checks if the vector is empty. (This remains a simple boolean accessor.)

- **Parameters:**
  - `arr` - Pointer to the vector
- **Returns:** `true` if vector is empty, `false` otherwise
- **Time Complexity:** O(1)
- **Example:** 
  ```c
  if (cvec_is_empty(&vec)) {
      printf("Vector is empty\n");
  }
  ```

---

#### `cvec_front`
```
static cvec_error_code cvec_front(c_vector_t* arr, void** out_ptr)
```
Retrieves a pointer to the first element. Returns `cvec_error_code` and fills `out_ptr` on success, or returns an error if the vector is empty or invalid.

- **Parameters:**
  - `arr` - Pointer to the vector
  - `out_ptr` - Output pointer to receive the element pointer
- **Returns:** Error code (CVEC_OK on success)
- **Time Complexity:** O(1)
- **Example:** 
  ```c
  void* ptr;
  if (cvec_front(&vec, &ptr) == CVEC_OK) {
    int first = *(int*)ptr;
    printf("First element: %d\n", first);
  }
  ```

---

#### `cvec_back`
```
static cvec_error_code cvec_back(c_vector_t* arr, void** out_ptr)
```
Retrieves a pointer to the last element. Returns `cvec_error_code` and fills `out_ptr` on success, or returns an error if the vector is empty or invalid.

- **Parameters:**
  - `arr` - Pointer to the vector
  - `out_ptr` - Output pointer to receive the element pointer
- **Returns:** Error code (CVEC_OK on success)
- **Time Complexity:** O(1)
- **Example:** 
  ```c
  void* ptr;
  if (cvec_back(&vec, &ptr) == CVEC_OK) {
    int last = *(int*)ptr;
    printf("Last element: %d\n", last);
  }
  ```

---

### Safe Access Functions

#### `cvec_get`
```
static cvec_error_code cvec_get(c_vector_t* arr, const size_t index, void** out_ptr)
```
Retrieves a pointer to the element at `index` with bounds checking. Returns `cvec_error_code` and fills `out_ptr` on success.

- **Parameters:**
  - `arr` - Pointer to the vector
  - `index` - Index of element to retrieve
  - `out_ptr` - Output pointer to receive the element pointer
- **Returns:** Error code (CVEC_OK on success, CVEC_ERROR_OUT_OF_BOUNDS on invalid index)
- **Example:** 
  ```c
  void* ptr;
  if (cvec_get(&vec, 0, &ptr) == CVEC_OK) {
    int value = *(int*)ptr;
    printf("Value: %d\n", value);
  }
  ```

---

#### `cvec_set`
```
static cvec_error_code cvec_set(c_vector_t* arr, const size_t index, const void* value)
```
Sets element at index with bounds checking. Returns `cvec_error_code`.

- **Parameters:**
  - `arr` - Pointer to the vector
  - `index` - Index of element to modify
  - `value` - Pointer to new value
- **Example:** 
  ```c
  int new_value = 100;
  cvec_set(&vec, 0, &new_value);
  ```

---

#### `cvec_at` (Macro)
```
#define cvec_at(arr, index, out_value_ptr)
```
Safe macro for copying element at `index` into a provided variable. Returns `cvec_error_code`. Includes bounds checking and size validation.

- **Parameters:**
  - `arr` - Pointer to the vector
  - `index` - Zero-based index
  - `out_value_ptr` - Pointer to a variable of the correct type to receive the value
- **Returns:** Error code (CVEC_OK on success)
- **Example:** 
  ```c
  int val;
  if (cvec_at(&vec, 0, &val) == CVEC_OK) {
    printf("Element: %d\n", val);
  }
  ```

---

### Advanced Functions

#### `cvec_from_array` (and `cvec_from_c_array` macro)
```c
#define cvec_from_c_array(out_vec, arr) \
    cvec_from_array((out_vec), (arr), cvec_array_count(arr), sizeof((arr)[0]))

static cvec_error_code cvec_from_array(
    c_vector_t* out_vec,
    const void* data,
    const size_t count,
    const size_t element_size
)
```
Initializes a vector with data from an existing array. Returns `cvec_error_code`.

- **Parameters:**
  - `out_vec` - Output vector to initialize
  - `data` - Pointer to the array data
  - `count` - Number of elements in the array
  - `element_size` - Size of each element in bytes
- **Returns:** Error code (CVEC_OK on success)
- **Time Complexity:** O(n)
- **Example:** 
  ```c
  int arr[] = {10, 20, 30, 40, 50};
  c_vector_t vec;
  cvec_from_c_array(&vec, arr);  // Using the convenient macro
  
  // Manual usage:
  c_vector_t vec2;
  cvec_from_array(&vec2, arr, 5, sizeof(int));
  ```

---

#### `cvec_reserve`
```
static cvec_error_code cvec_reserve(c_vector_t* arr, const size_t capacity)
```
Pre-allocates memory for the specified capacity. Prevents multiple reallocations. Returns `cvec_error_code`.

- **Parameters:**
  - `arr` - Pointer to the vector
  - `capacity` - Desired capacity
- **Use Case:** When you know the final size in advance
- **Example:** `cvec_reserve(&vec, 1000);`

---

#### `cvec_to_array`
```
static cvec_error_code cvec_to_array(const c_vector_t* arr, void** out_array)
```
Creates a heap-allocated copy of the vector's data as a regular array. Returns `cvec_error_code` and fills `out_array` on success.

- **Parameters:**
  - `arr` - Pointer to the vector
  - `out_array` - Output pointer to receive the allocated array
- **Returns:** Error code (CVEC_OK on success)
- **Warning:** Caller is responsible for freeing the returned pointer
- **Example:** 
  ```c
  void* array;
  if (cvec_to_array(&vec, &array) == CVEC_OK) {
    int* int_array = (int*)array;
    // Use array...
    free(int_array);
  }
  ```

---

#### `cvec_shrink_to_fit`
```
static cvec_error_code cvec_shrink_to_fit(c_vector_t* arr)
```
Reduces the vector's capacity to match its current size, freeing unused memory. Returns `cvec_error_code`.

- **Parameters:**
  - `arr` - Pointer to the vector
- **Time Complexity:** O(n) if reallocation needed, O(1) if size equals capacity
- **Use Case:** When you need to minimize memory usage after removing many elements
- **Example:** 
  ```c
  cvec_reserve(&vec, 1000);
  cvec_append(&vec, &value);  // Only 1 element
  cvec_shrink_to_fit(&vec);    // Capacity becomes 1
  ```

---

## Examples

Check the `examples/` folder for detailed examples:

- **[basic_usage.c](examples/basic_usage.c)** - Basic operations (init, append, remove, clear)
- **[custom_types.c](examples/custom_types.c)** - Using vectors with custom struct types
- **[advanced_operations.c](examples/advanced_operations.c)** - Reserve, get/set, to_array
- **[different_types.c](examples/different_types.c)** - Vectors with various data types

### Working with Custom Types

```c
typedef struct {
    int id;
    char name[50];
    double score;
} Student;

c_vector_t students;
cvec_error_code status = cvec_init(&students, sizeof(Student));
if (status != CVEC_OK) {
  fprintf(stderr, "Init failed: %s\n", cvec_error_message(status));
  return 1;
}

Student s1 = {1, "Alice", 95.5};
if (cvec_append(&students, &s1) != CVEC_OK) {
  fprintf(stderr, "Append failed\n");
  cvec_delete(&students);
  return 1;
}

// Access with safe macro
Student retrieved;
if (cvec_at(&students, 0, &retrieved) == CVEC_OK) {
  printf("Student: %s (score: %.1f)\n", retrieved.name, retrieved.score);
}

// Alternative: access with function
void* ptr;
if (cvec_get(&students, 0, &ptr) == CVEC_OK) {
  Student* student = (Student*)ptr;
  printf("ID: %d\n", student->id);
}

cvec_delete(&students);
```

## Building

### Requirements
- C23 compatible compiler (GCC 14+, Clang 18+, or MSVC 2022+)
- CMake 3.20+ (optional, for building examples)

### Compile Examples
```bash
# Compile specific example
gcc -std=c23 -o basic examples/basic_usage.c

# Compile with CMake
mkdir build && cd build
cmake ..
cmake --build .
```

### Compile Main Demo
```bash
gcc -std=c23 -o main main.c
./main
```

## Memory Management

The vector automatically manages its internal memory:
- **Initial Capacity:** 8 elements
- **Growth Strategy:** 1.5x growth factor (more memory-efficient than 2x)
- **Overflow Protection:** All allocations checked for SIZE_MAX overflow
- **Manual Cleanup:** Always call `cvec_delete()` to free memory

## Performance Characteristics

| Operation | Time Complexity | Notes |
|-----------|----------------|-------|
| `cvec_init` | O(1) | Allocates initial capacity of 8 |
| `cvec_append` | O(1) amortized | O(n) when resizing needed |
| `cvec_get` | O(1) | Includes bounds check |
| `cvec_set` | O(1) | Includes bounds check |
| `cvec_at` | O(1) | No bounds check (fastest) |
| `cvec_front` | O(1) | Direct access to first element |
| `cvec_back` | O(1) | Direct access to last element |
| `cvec_is_empty` | O(1) | Checks size counter |
| `cvec_remove` | O(n) | Must shift elements |
| `cvec_clear` | O(1) | Only resets size counter |
| `cvec_delete` | O(1) | Frees all memory |
| `cvec_reserve` | O(n) | If reallocation needed |
| `cvec_from_array` | O(n) | Copies all elements |
| `cvec_to_array` | O(n) | Copies all elements |
| `cvec_shrink_to_fit` | O(n) | If reallocation needed |

## Acknowledgments

Built with modern C23 features for improved type safety, code clarity, and memory safety.
