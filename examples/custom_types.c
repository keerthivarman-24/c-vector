// Example using custom struct types with c_vector
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
    c_vector students;
    
    // Initialize vector for Student structs
    cvec_init(&students, sizeof(Student));
    
    printf("=== Custom Types Example ===\n\n");
    
    // Create and add students
    Student s1 = {1, "Alice Johnson", 95.5};
    Student s2 = {2, "Bob Smith", 87.3};
    Student s3 = {3, "Charlie Brown", 91.8};
    Student s4 = {4, "Diana Prince", 98.2};
    
    cvec_append(&students, &s1);
    cvec_append(&students, &s2);
    cvec_append(&students, &s3);
    cvec_append(&students, &s4);
    
    // Display all students
    printf("Student Records:\n");
    for (size_t i = 0; i < students.size; i++) {
        Student student = cvec_at(students, i, Student);
        printf("%zu. ", i + 1);
        print_student(&student);
    }
    printf("\n");
    
    // Update a student's score using cvec_set
    printf("Updating Bob's score to 90.0\n");
    Student* bob = (Student*)cvec_get(&students, 1);
    bob->score = 90.0;
    
    // Alternative: using cvec_set
    Student updated_charlie = {3, "Charlie Brown", 95.0};
    cvec_set(&students, 2, &updated_charlie);
    printf("Updating Charlie's score to 95.0\n\n");
    
    // Display updated records
    printf("Updated Student Records:\n");
    for (size_t i = 0; i < students.size; i++) {
        Student student = cvec_at(students, i, Student);
        printf("%zu. ", i + 1);
        print_student(&student);
    }
    
    // Cleanup
    cvec_delete(&students);
    printf("\nVector deleted successfully\n");
    
    return 0;
}
