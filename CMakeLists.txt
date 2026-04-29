cmake_minimum_required(VERSION 4.0)
project(c_vector C)

set(CMAKE_C_STANDARD 23)

add_executable(c_vector
        main.c
        include/vector.h
)
add_executable(c_vector_examples_different_types
        examples/different_types.c
)

add_executable(c_vector_examples_basic_usage
        examples/basic_usage.c
)

add_executable(c_vector_examples_custom_types
        examples/custom_types.c
)

add_executable(c_vector_examples_advanced_operations
        examples/advanced_operations.c
)
add_executable(c_vector_examples_real_world_usage 
        examples/real_world_usage.c
)

add_executable(c_vector_test
        tests/comprehensive_test.c
)
