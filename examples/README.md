# C-Vector Examples

Comprehensive examples demonstrating the c_vector library features and real-world usage patterns.

## Examples Overview

### 1. basic_usage.c
**Core Operations & Data Structure Basics**

Demonstrates fundamental vector operations:
- Initialization with `cvec_init()`
- Checking if empty with `cvec_is_empty()`
- Adding elements with `cvec_append()`
- Accessing first/last elements with `cvec_front()` and `cvec_back()`
- Element removal with `cvec_remove()`
- Clearing vectors with `cvec_clear()`
- Proper cleanup with `cvec_delete()`

**Key takeaway:** Essential operations you'll use in every project.

### 2. different_types.c
**Type Flexibility**

Shows vectors working with various data types:
- Integers (`int`)
- Characters (`char`)
- Floating-point numbers (`float`)
- Pointers (e.g., `char*` for strings)

**Key takeaway:** One vector implementation works for any type - just specify `sizeof(type)` during initialization.

### 3. custom_types.c
**Custom Structures**

Demonstrates using vectors with user-defined `struct` types:
- Storing complex Student records
- Accessing and modifying struct fields
- Using both `cvec_get()` (pointer) and `cvec_at()` (value) access patterns
- Updating struct members safely

**Key takeaway:** Vectors handle structs just as easily as primitives.

### 4. advanced_operations.c
**Performance Optimization & Advanced Features**

Covers performance and memory management:
- Creating vectors from arrays with `cvec()` macro
- Pre-allocating capacity with `cvec_reserve()` to avoid reallocations
- Safe vs unsafe element access
- Updating elements with `cvec_set()`
- Reclaiming memory with `cvec_shrink_to_fit()`
- Converting to C arrays with `cvec_to_array()`

**Key takeaway:** Understanding capacity management can significantly improve performance.

### 5. real_world_usage.c (NEW!)
**Practical Applications**

Three complete real-world examples:
1. **Stack Implementation** - Classic LIFO data structure built on c_vector
2. **Game Entity Manager** - Managing dynamic game objects with health/position
3. **Data Collection & Statistics** - Processing numerical data and computing stats

**Key takeaway:** Vectors serve as building blocks for higher-level data structures and applications.

## Compiling Examples

### Individual Compilation
```bash
# Windows (MSVC)
cl /std:c17 examples\basic_usage.c

# Windows (MinGW)
gcc -std=c23 examples/basic_usage.c -o basic_usage.exe

# Linux/Mac
gcc -std=c23 examples/basic_usage.c -o basic_usage
./basic_usage
```

### Compile All Examples (Unix-like)
```bash
for file in examples/*.c; do
    gcc -std=c23 "$file" -o "${file%.c}"
done
```

## API Quick Reference

### Initialization & Cleanup
| Function | Description |
|----------|-------------|
| `cvec_result cvec_init(&vec, sizeof(type))` | Initialize vector for specific type; returns `cvec_result` — check `result.success` before using the vector |
| `cvec_result cvec_delete(&vec)` | Free all memory and reset; returns `cvec_result` |

### Adding/Removing Elements
| Function | Description |
|----------|-------------|
| `cvec_append(&vec, &value)` | Add element to end |
| `cvec_remove(&vec, index)` | Remove element at index |
| `cvec_clear(&vec)` | Remove all elements (keeps capacity) |

### Accessing Elements
| Function | Description |
|----------|-------------|
| `cvec_get(&vec, index)` | Get pointer with bounds check |
| `cvec_set(&vec, index, &value)` | Set value with bounds check |
| `cvec_at(vec, index, type)` | Get value directly (macro) |
| `cvec_front(&vec)` | Pointer to first element |
| `cvec_back(&vec)` | Pointer to last element |

### Status & Utility
| Function | Description |
|----------|-------------|
| `cvec_is_empty(&vec)` | Returns true if size is 0 |
| `cvec_size(&vec)` | Current number of elements |
| `cvec_capacity(&vec)` | Total allocated slots |

### Memory Management
| Function | Description |
|----------|-------------|
| `cvec_reserve(&vec, capacity)` | Pre-allocate space |
| `cvec_shrink_to_fit(&vec)` | Reduce capacity to size |

### Array Conversion
| Function | Description |
|----------|-------------|
| `cvec(array)` | Create vector from C array |
| `cvec_to_array(&vec)` | Convert to C array (caller must free) |

## Common Patterns

### Pattern 1: Pre-allocation for Known Sizes
```c
c_vector_t vec;
cvec_init(&vec, sizeof(int));
cvec_reserve(&vec, 1000);  // If you know you'll add ~1000 elements

for (int i = 0; i < 1000; i++) {
    cvec_append(&vec, &i);  // No reallocations!
}
```

### Pattern 2: Safe Iteration
```c
for (size_t i = 0; i < cvec_size(&vec); i++) {
    int value = cvec_at(vec, i, int);
    // Use value...
}
```

### Pattern 3: Modifying Elements
```c
// Method 1: Direct pointer access
int* elem = (int*)cvec_get(&vec, 5);
*elem = 100;

// Method 2: Using set
int new_value = 100;
cvec_set(&vec, 5, &new_value);
```

### Pattern 4: Removing Elements in Loop
```c
// Always iterate backwards when removing!
for (int i = (int)cvec_size(&vec) - 1; i >= 0; i--) {
    if (should_remove(i)) {
        cvec_remove(&vec, i);
    }
}
```

## Memory Considerations

1. **Growth Strategy**: Vector grows by 1.5x when full (more memory-efficient than 2x)
2. **Initial Capacity**: 8 elements by default
3. **Reallocation**: Happens automatically when capacity is exceeded
4. **Shrinking**: Use `cvec_shrink_to_fit()` after removing many elements
5. **Cleanup**: Always call `cvec_delete()` - even if size is 0


## Error Handling

The library no longer aborts on error. Most operations return a result structure (e.g., `cvec_result`, `cvec_vector_ptr`, `cvec_ptr_result`) that contains a `success` boolean and an `error` payload when `success` is false. Callers should check `result.success` and handle `result.error` (code/message) appropriately.

Common failure reasons:
- NULL pointer arguments
- Index out of bounds
- Allocation failures
- Size/overflow errors

Example usage pattern:
```c
// init
cvec_result r = cvec_init(&vec, sizeof(int));
if (!r.success) { fprintf(stderr, "init failed: %s\n", r.error.message); return 1; }

// get element
cvec_vector_ptr p = cvec_get(&vec, 0);
if (!p.success) { fprintf(stderr, "get failed: %s\n", p.error.message); }
else { int v = *(int*)p.data.ptr; }
```

## Next Steps

1. Read through examples in order (basic → advanced → real-world)
2. Run each example to see output
3. Modify examples to experiment with different types
4. Build your own applications using c_vector as a foundation

## Questions?

Refer to the main [README.md](../README.md) for complete API documentation and additional details.