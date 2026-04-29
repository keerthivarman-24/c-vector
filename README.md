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
#include "include/vector.h"
```

### Basic Usage

```c
#include <stdio.h>
#define C_VECTOR
#include "include/vector.h"

int main(void) {
  c_vector_t vec;

  /* Initialize vector for integers (returns cvec_result) */
  cvec_result init_res = cvec_init(&vec, sizeof(int));
  if (!init_res.success) {
    fprintf(stderr, "cvec_init failed: %s\n", init_res.error.message);
    return 1;
  }

  /* Add elements (check each result) */
  int a = 10, b = 20, c = 30;
  cvec_result r = cvec_append(&vec, &a);
  if (!r.success) { fprintf(stderr, "%s\n", r.error.message); cvec_delete(&vec); return 1; }
  r = cvec_append(&vec, &b);
  if (!r.success) { fprintf(stderr, "%s\n", r.error.message); cvec_delete(&vec); return 1; }
  r = cvec_append(&vec, &c);
  if (!r.success) { fprintf(stderr, "%s\n", r.error.message); cvec_delete(&vec); return 1; }

  /* Access elements (use safe getter which returns a pointer result) */
  for (size_t i = 0; i < cvec_size(&vec); i++) {
    cvec_vector_ptr vptr = cvec_get(&vec, i);
    if (!vptr.success) { fprintf(stderr, "%s\n", vptr.error.message); break; }
    int value = *(int*)(vptr.data.ptr);
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
static cvec_result cvec_init(c_vector_t* arr, const size_t element_size)
```
Initializes a new vector with the specified element size. Returns a `cvec_result` with `success` or `error` information.

- **Parameters:**
  - `arr` - Pointer to the vector to initialize
  - `element_size` - Size of each element in bytes (use `sizeof(type)`)
- **Initial Capacity:** 8 elements
- **Example:** `cvec_init(&vec, sizeof(int));`

---

#### `cvec_delete`
```
static cvec_result cvec_delete(c_vector_t* arr)
```
Frees all memory associated with the vector and resets its state. Returns `cvec_result` to indicate success or error (safe to call multiple times).

- **Parameters:**
  - `arr` - Pointer to the vector to delete
- **Example:** `cvec_delete(&vec);`

---

#### `cvec_append`
```
static cvec_result cvec_append(c_vector_t* arr, const void* value)
```
Appends an element to the end of the vector. Automatically resizes if needed (1.5x growth). Returns `cvec_result` and does not abort on failure.

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
static cvec_result cvec_remove(c_vector_t* arr, const size_t index)
```
Removes element at the specified index. Shifts subsequent elements left. Returns `cvec_result`.

- **Parameters:**
  - `arr` - Pointer to the vector
  - `index` - Index of element to remove
- **Time Complexity:** O(n) due to element shifting
- **Example:** `cvec_remove(&vec, 2);`

---

#### `cvec_clear`
```
static cvec_result cvec_clear(c_vector_t* arr)
```
Resets the vector size to 0 without freeing memory. Keeps capacity unchanged. Returns `cvec_result`.

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
static cvec_vector_ptr cvec_front(c_vector_t* arr)
```
Returns a `cvec_vector_ptr` result containing a pointer to the first element or an error if the vector is empty or invalid.

- **Parameters:**
  - `arr` - Pointer to the vector
- **Returns:** Pointer to the first element
- **Time Complexity:** O(1)
- **Example:** 
  ```c
  int* first = (int*)cvec_front(&vec);
  printf("First element: %d\n", *first);
  ```

---

#### `cvec_back`
```
static cvec_vector_ptr cvec_back(c_vector_t* arr)
```
Returns a `cvec_vector_ptr` result containing a pointer to the last element or an error if the vector is empty or invalid.

- **Parameters:**
  - `arr` - Pointer to the vector
- **Returns:** Pointer to the last element
- **Time Complexity:** O(1)
- **Example:** 
  ```c
  int* last = (int*)cvec_back(&vec);
  printf("Last element: %d\n", *last);
  ```

---

### Safe Access Functions

#### `cvec_get`
```
static cvec_vector_ptr cvec_get(c_vector_t* arr, const size_t index)
```
Returns a `cvec_vector_ptr` result containing a pointer to the element at `index` with bounds checking. Check `result.success` before using `result.data.ptr`.

- **Parameters:**
  - `arr` - Pointer to the vector
  - `index` - Index of element to retrieve
- **Returns:** Pointer to the element (must cast to correct type)
- **Example:** 
  ```c
  int* value_ptr = (int*)cvec_get(&vec, 0);
  printf("Value: %d\n", *value_ptr);
  ```

---

#### `cvec_set`
```
static cvec_result cvec_set(c_vector_t* arr, const size_t index, const void* value)
```
Sets element at index with bounds checking. Returns `cvec_result`.

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
#define cvec_at(arr, index, type)
```
Fast, unchecked macro for element access. Use when performance is critical and bounds are verified. This macro remains unchecked and is intended for hot paths where you have already validated the index.

- **Parameters:**
  - `arr` - The vector (passed by value, not pointer)
  - `index` - Zero-based index
  - `type` - The data type to cast to
- **Returns:** The element at the specified index
- **Warning:** No bounds checking - unsafe if index is out of range
- **Example:** `int val = cvec_at(vec, 0, int);`

---

### Advanced Functions

#### `cvec_from_array` (and `cvec` macro)
```c
#define cvec(array) cvec_from_array((array), sizeof(array) / sizeof((array)[0]), sizeof((array)[0]))

static c_vector_t cvec_from_array(const void* data, size_t count, size_t element_size)
```
Creates a new vector initialized with data from an existing array.

- **Parameters:**
  - `data` - Pointer to the array data
  - `count` - Number of elements in the array
  - `element_size` - Size of each element in bytes
- **Returns:** A new `c_vector_t` initialized with the array data
- **Time Complexity:** O(n)
- **Example:** 
  ```c
  int arr[] = {10, 20, 30, 40, 50};
  c_vector_t vec = cvec(arr);  // Using the convenient macro
  
  // Manual usage:
  c_vector_t vec2 = cvec_from_array(arr, 5, sizeof(int));
  ```

---

#### `cvec_reserve`
```
static cvec_result cvec_reserve(c_vector_t* arr, const size_t capacity)
```
Pre-allocates memory for the specified capacity. Prevents multiple reallocations. Returns `cvec_result`.

- **Parameters:**
  - `arr` - Pointer to the vector
  - `capacity` - Desired capacity
- **Use Case:** When you know the final size in advance
- **Example:** `cvec_reserve(&vec, 1000);`

---

#### `cvec_to_array`
```
static cvec_ptr_result cvec_to_array(c_vector_t* arr)
```
Creates a heap-allocated copy of the vector's data as a regular array. Returns a pointer result; caller must check and free returned pointer on success.

- **Parameters:**
  - `arr` - Pointer to the vector
- **Returns:** Pointer to new array (caller must `free()`)
- **Warning:** Caller is responsible for freeing the returned pointer
- **Example:** 
  ```c
  int* array = (int*)cvec_to_array(&vec);
  // Use array...
  free(array);
  ```

---

#### `cvec_shrink_to_fit`
```
static cvec_result cvec_shrink_to_fit(c_vector_t* arr)
```
Reduces the vector's capacity to match its current size, freeing unused memory. Returns `cvec_result`.

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
cvec_init(&students, sizeof(Student));

Student s1 = {1, "Alice", 95.5};
cvec_append(&students, &s1);

// Access with macro
Student retrieved = cvec_at(students, 0, Student);

// Access with safe function
Student* ptr = (Student*)cvec_get(&students, 0);

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
