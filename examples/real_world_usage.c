// Real-world usage examples: stack, dynamic list, game entities
#include <stdio.h>
#include <string.h>

#define C_VECTOR
#include "../include/vector.h"

// Example 1: Stack implementation using c_vector_t
typedef c_vector_t Stack;

void stack_init(Stack* s, size_t element_size) {
    cvec_init(s, element_size);
}

void stack_push(Stack* s, const void* value) {
    cvec_append(s, value);
}

void* stack_top(Stack* s) {
    void* ptr = nullptr;

    if (cvec_is_empty(s)) return nullptr;
    if (cvec_back(s, &ptr) != CVEC_OK) return nullptr;
    return ptr;
}

void stack_pop(Stack* s) {
    if (!cvec_is_empty(s)) {
        cvec_remove(s, s->size - 1);
    }
}

int stack_is_empty(Stack* s) {
    return cvec_is_empty(s);
}

size_t stack_size(Stack* s) {
    return s->size;
}

void stack_delete(Stack* s) {
    cvec_delete(s);
}

void demo_stack() {
    printf("=== Example 1: Stack Implementation ===\n\n");
    
    Stack stack;
    stack_init(&stack, sizeof(int));
    
    printf("Pushing: ");
    for (int i = 1; i <= 5; i++) {
        printf("%d ", i);
        stack_push(&stack, &i);
    }
    printf("\n");
    printf("Stack size: %zu\n\n", stack_size(&stack));
    
    printf("Popping: ");
    while (!stack_is_empty(&stack)) {
        int* top = (int*)stack_top(&stack);
        if (top) {
            printf("%d ", *top);
            stack_pop(&stack);
        }
    }
    printf("\n");
    printf("Stack size after popping all: %zu\n\n", stack_size(&stack));
    
    stack_delete(&stack);
}

// Example 2: Game entity management
typedef struct {
    int id;
    char name[30];
    int health;
    int x, y;  // position
} Entity;

void print_entity(const Entity* e) {
    printf("   [%d] %-15s HP: %3d  Pos: (%d, %d)\n", 
           e->id, e->name, e->health, e->x, e->y);
}

void demo_game_entities() {
    printf("=== Example 2: Game Entity Manager ===\n\n");

    c_vector_t entities;
    cvec_init(&entities, sizeof(Entity));
    void* ptr = nullptr;

    // Spawn entities
    Entity player = {1, "Player", 100, 0, 0};
    Entity enemy1 = {2, "Goblin", 50, 10, 5};
    Entity enemy2 = {3, "Orc", 80, -5, 8};
    Entity enemy3 = {4, "Skeleton", 30, 12, 12};
    Entity npc = {5, "Merchant", 100, 20, 20};

    cvec_error_code status = cvec_append(&entities, &player);
    if (status != CVEC_OK) goto fail;
    status = cvec_append(&entities, &enemy1);
    if (status != CVEC_OK) goto fail;
    status = cvec_append(&entities, &enemy2);
    if (status != CVEC_OK) goto fail;
    status = cvec_append(&entities, &enemy3);
    if (status != CVEC_OK) goto fail;
    status = cvec_append(&entities, &npc);
    if (status != CVEC_OK) goto fail;

    printf("Initial entities (%zu total):\n", entities.size);
    for (size_t i = 0; i < entities.size; i++) {
        status = cvec_get(&entities, i, &ptr);
        if (status != CVEC_OK) goto fail;
        Entity* e = (Entity*)ptr;
        print_entity(e);
    }
    printf("\n");

    // Simulate damage
    printf("Combat: Goblin and Skeleton defeated!\n");
    status = cvec_get(&entities, 1, &ptr);
    if (status != CVEC_OK) goto fail;
    Entity* goblin = (Entity*)ptr;
    goblin->health = 0;
    status = cvec_get(&entities, 3, &ptr);
    if (status != CVEC_OK) goto fail;
    Entity* skeleton = (Entity*)ptr;
    skeleton->health = 0;

    // Remove dead entities (backwards to avoid index shifts)
    printf("Removing dead entities...\n");
    for (int i = (int)entities.size - 1; i >= 0; i--) {
        Entity e;
        status = cvec_at(&entities, (size_t)i, &e);
        if (status != CVEC_OK) goto fail;
        if (e.health <= 0) {
            printf("   Removed: %s\n", e.name);
            status = cvec_remove(&entities, (size_t)i);
            if (status != CVEC_OK) goto fail;
        }
    }
    printf("\n");

    printf("Remaining entities (%zu total):\n", entities.size);
    for (size_t i = 0; i < entities.size; i++) {
        status = cvec_get(&entities, i, &ptr);
        if (status != CVEC_OK) goto fail;
        Entity* e = (Entity*)ptr;
        print_entity(e);
    }
    printf("\n");

    cvec_delete(&entities);
    return;

fail:
    fprintf(stderr, "operation failed: %s\n", cvec_error_message(status));
    cvec_delete(&entities);
}

// Example 3: Dynamic data collection and statistics
void demo_statistics() {
    printf("=== Example 3: Data Collection & Statistics ===\n\n");

    c_vector_t numbers;
    cvec_error_code status;
    cvec_init(&numbers, sizeof(double));

    // Simulate reading data
    double data[] = {45.2, 67.8, 23.1, 89.5, 34.7, 56.3, 78.9, 12.4, 91.2, 43.6};
    printf("Collecting data: ");
    for (size_t i = 0; i < 10; i++) {
        printf("%.1f ", data[i]);
        status = cvec_append(&numbers, &data[i]);
        if (status != CVEC_OK) goto fail;
    }
    printf("\n\n");

    // Calculate statistics
    if (!cvec_is_empty(&numbers)) {
        double sum = 0.0;
        double min = 0.0;
        double max = 0.0;

        status = cvec_at(&numbers, 0, &min);
        if (status != CVEC_OK) goto fail;
        status = cvec_at(&numbers, 0, &max);
        if (status != CVEC_OK) goto fail;

        for (size_t i = 0; i < numbers.size; i++) {
            double val = 0.0;
            status = cvec_at(&numbers, i, &val);
            if (status != CVEC_OK) goto fail;
            sum += val;
            if (val < min) min = val;
            if (val > max) max = val;
        }

        double avg = sum / numbers.size;

        printf("Statistics:\n");
        printf("   Count:   %zu\n", numbers.size);
        printf("   Sum:     %.2f\n", sum);
        printf("   Average: %.2f\n", avg);
        printf("   Min:     %.2f\n", min);
        printf("   Max:     %.2f\n", max);
        printf("   Range:   %.2f\n\n", max - min);
    }

    cvec_delete(&numbers);
    return;

fail:
    fprintf(stderr, "operation failed: %s\n", cvec_error_message(status));
    cvec_delete(&numbers);
}

int main() {
    demo_stack();
    printf("\n");
    
    demo_game_entities();
    printf("\n");
    
    demo_statistics();
    
    printf("All examples completed successfully!\n");
    return 0;
}
