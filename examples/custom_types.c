// Example using custom struct types with c_vector_t
#include <stdio.h>
#include <string.h>

#define C_VECTOR
#include "../include/vector.h"

typedef struct {
    int id;
    char name[50];
    double score;
} Student;

void print_student(const Student* s) {
    printf("ID: %d, Name: %-20s, Score: %.2f\n", s->id, s->name, s->score);
}

int main() {
    c_vector_t students;
    cvec_error_code status;

    // Initialize vector for Student structs
    cvec_init(&students, sizeof(Student));

    printf("=== Custom Types Example ===\n\n");
    
    // Create and add students
    Student s1 = {1, "Alice Johnson", 95.5};
    Student s2 = {2, "Bob Smith", 87.3};
    Student s3 = {3, "Charlie Brown", 91.8};
    Student s4 = {4, "Diana Prince", 98.2};
    
    status = cvec_append(&students, &s1);
    if (status != CVEC_OK) goto fail;
    status = cvec_append(&students, &s2);
    if (status != CVEC_OK) goto fail;
    status = cvec_append(&students, &s3);
    if (status != CVEC_OK) goto fail;
    status = cvec_append(&students, &s4);
    if (status != CVEC_OK) goto fail;
    
    // Display all students
    printf("Student Records:\n");
    for (size_t i = 0; i < students.size; i++) {
        Student student;
        status = cvec_at(&students, i, &student);
        if (status != CVEC_OK) goto fail;
        printf("%zu. ", i + 1);
        print_student(&student);
    }
    printf("\n");
    
    // Update a student's score using cvec_set
    printf("Updating Bob's score to 90.0\n");
    void* ptr = nullptr;
    status = cvec_get(&students, 1, &ptr);
    if (status != CVEC_OK) goto fail;
    Student* bob = (Student*)ptr;
    bob->score = 90.0;
    
    // Alternative: using cvec_set
    Student updated_charlie = {3, "Charlie Brown", 95.0};
    status = cvec_set(&students, 2, &updated_charlie);
    if (status != CVEC_OK) goto fail;
    printf("Updating Charlie's score to 95.0\n\n");
    
    // Display updated records
    printf("Updated Student Records:\n");
    for (size_t i = 0; i < students.size; i++) {
        Student student;
        status = cvec_at(&students, i, &student);
        if (status != CVEC_OK) goto fail;
        printf("%zu. ", i + 1);
        print_student(&student);
    }
    
    // Cleanup
    cvec_delete(&students);
    printf("\nVector deleted successfully\n");
    
    return 0;

fail:
    fprintf(stderr, "operation failed: %s\n", cvec_error_message(status));
    cvec_delete(&students);
    return 1;
}
