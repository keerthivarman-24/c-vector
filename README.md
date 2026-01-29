# C-Vector

A lightweight, generic dynamic array (vector) implementation in C23, providing type-safe operations with automatic memory management.

## Features

- **Generic Type Support** - Works with any data type using `void*` pointers
- **Dynamic Resizing** - Automatically grows capacity when needed
- **Memory Safe** - Comprehensive error checking and cleanup functions
- **Type-Safe Access** - Macro-based element access with compile-time type checking
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

int main() {
    c_vector vec;
    
    // Initialize vector for integers
    cvec_init(&vec, sizeof(int));
    
    // Add elements
    int a = 10, b = 20, c = 30;
    cvec_append(&vec, &a);
    cvec_append(&vec, &b);
    cvec_append(&vec, &c);
    
    // Access elements
    for (int i = 0; i < vec.size; i++) {
        printf("vector[%d] = %d\n", i, cvec_at(vec, i, int));
    }
    
    // Clean up
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
} c_vector;
```

### Functions

#### `cvec_init`
```c
void cvec_init(c_vector* arr, const size_t element_size)
```
Initializes a new vector with the specified element size.

- **Parameters:**
  - `arr` - Pointer to the vector to initialize
  - `element_size` - Size of each element in bytes (use `sizeof(type)`)
- **Example:** `cvec_init(&vec, sizeof(int));`

#### `cvec_append`
```c
static void cvec_append(c_vector* arr, const void* value)
```
Appends an element to the end of the vector. Automatically resizes if needed.

- **Parameters:**
  - `arr` - Pointer to the vector
  - `value` - Pointer to the value to append
- **Example:** 
  ```c
  int value = 42;
  cvec_append(&vec, &value);
  ```

#### `cvec_delete`
```c
static void cvec_delete(c_vector* arr)
```
Frees all memory associated with the vector and resets its state.

- **Parameters:**
  - `arr` - Pointer to the vector to delete
- **Example:** `cvec_delete(&vec);`

#### `cvec_at`
```c
#define cvec_at(arr, index, type)
```
Macro for type-safe element access at a given index.

- **Parameters:**
  - `arr` - The vector (passed by value, not pointer)
  - `index` - Zero-based index of the element
  - `type` - The data type to cast to
- **Returns:** The element at the specified index
- **Example:** `int val = cvec_at(vec, 0, int);`

## Advanced Usage

### Working with Custom Types

```c
typedef struct {
    int id;
    char name[50];
} Person;

c_vector people;
cvec_init(&people, sizeof(Person));

Person p1 = {1, "Alice"};
Person p2 = {2, "Bob"};

cvec_append(&people, &p1);
cvec_append(&people, &p2);

// Access elements
Person retrieved = cvec_at(people, 0, Person);
printf("Person: %s (ID: %d)\n", retrieved.name, retrieved.id);

cvec_delete(&people);
```

### Working with Pointers

```c
c_vector ptr_vec;
cvec_init(&ptr_vec, sizeof(int*));

int* values[3];
for (int i = 0; i < 3; i++) {
    values[i] = malloc(sizeof(int));
    *values[i] = i * 10;
    cvec_append(&ptr_vec, &values[i]);
}

// Remember to free pointer contents
for (int i = 0; i < ptr_vec.size; i++) {
    int* ptr = cvec_at(ptr_vec, i, int*);
    free(ptr);
}
cvec_delete(&ptr_vec);
```

## Building

### Requirements
- C23 compatible compiler (GCC 14+, Clang 18+, or MSVC 2022+)
- CMake 3.20+ (optional, for building examples)

### Compile with CMake
```bash
mkdir build && cd build
cmake ..
cmake --build .
./c_vector  # Run the example
```

### Compile Manually
```bash
gcc -std=c23 -o main main.c
```

## Memory Management

The vector automatically manages its internal memory:
- **Initial Capacity:** 8 elements
- **Growth Strategy:** Doubles capacity when full
- **Manual Cleanup:** Always call `cvec_delete()` to free memory

## Error Handling

The library includes built-in error checking:
- Invalid arguments trigger error messages and exit
- Memory allocation failures are caught and reported
- All errors exit with `EXIT_FAILURE`

## Performance Characteristics

| Operation | Time Complexity | Notes |
|-----------|----------------|-------|
| `cvec_init` | O(1) | Allocates initial capacity |
| `cvec_append` | O(1) amortized | O(n) when resizing |
| `cvec_at` | O(1) | Direct memory access |
| `cvec_delete` | O(1) | Frees all memory |

## Safety Features

- **Type Safety:** Macro-based access provides compile-time type checking
- **Bounds Checking:** No automatic bounds checking (use manual checks if needed)
- **Double Evaluation Protection:** Macro parameters properly parenthesized
- **Memory Leak Prevention:** Single `cvec_delete()` call frees all memory

## Acknowledgments

Built with modern C23 features for improved type safety and code clarity.
