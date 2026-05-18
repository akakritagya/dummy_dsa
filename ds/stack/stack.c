#include "stack.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define STACK_INIT_CAP 8
#define STACK_MAX_CAP (SIZE_MAX / sizeof(void *))

struct Stack {
    void **data;
    size_t size;
    size_t capacity;
};

Stack *create_stack(void) {
    Stack *s = malloc(sizeof(Stack));
    if (!s)
        return NULL;

    s->data = malloc(STACK_INIT_CAP * sizeof(void *));
    if (!s->data) {
        free(s);
        return NULL;
    }

    s->size     = 0;
    s->capacity = STACK_INIT_CAP;
    return s;
}

void destroy_stack(Stack *s) {
    if (!s)
        return;
    free(s->data);
    free(s);
}

bool push_stack(Stack *s, void *value) {
    if (!s)
        return false;

    if (s->size == s->capacity) {
        /* Guard against size_t overflow on capacity doubling */
        if (s->capacity > STACK_MAX_CAP / 2)
            return false;

        size_t new_cap = s->capacity * 2;
        void **resized = realloc(s->data, new_cap * sizeof(void *));
        if (!resized)
            return false;

        s->data     = resized;
        s->capacity = new_cap;
    }

    s->data[s->size++] = value;
    return true;
}

bool pop_stack(Stack *s, void **out) {
    assert(out != NULL);
    if (!s || s->size == 0)
        return false;
    *out = s->data[--s->size];
    return true;
}

bool peek_stack(const Stack *s, void **out) {
    assert(out != NULL);
    if (!s || s->size == 0)
        return false;
    *out = s->data[s->size - 1];
    return true;
}

bool is_empty_stack(const Stack *s) {
    return !s || s->size == 0;
}

size_t size_stack(const Stack *s) {
    return s ? s->size : 0;
}

void demo_stack(void) {
    printf("\n=== Stack Demo ===\n\n");

    /* Test 1: Create and verify empty stack */
    printf("Test 1: Create empty stack\n");
    Stack *s = create_stack();
    if (!s) {
        printf("  FAIL: Could not create stack\n");
        return;
    }
    printf("  Stack created: is_empty=%d, size=%zu\n", is_empty_stack(s),
           size_stack(s));

    /* Test 2: Push integers (stored as pointers) */
    printf("\nTest 2: Push 5 integers onto stack\n");
    int values[5] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) {
        if (push_stack(s, &values[i])) {
            printf("  Pushed: %d (size now: %zu)\n", values[i], size_stack(s));
        } else {
            printf("  FAIL: Could not push %d\n", values[i]);
        }
    }

    /* Test 3: Peek at top without removing */
    printf("\nTest 3: Peek at top value\n");
    void *top = NULL;
    if (peek_stack(s, &top)) {
        printf("  Top value: %d (size still: %zu)\n", *(int *)top,
               size_stack(s));
    } else {
        printf("  FAIL: Could not peek\n");
    }

    /* Test 4: Pop all elements (verify LIFO order) */
    printf("\nTest 4: Pop all elements (verify LIFO order)\n");
    void *popped          = NULL;
    int expected_order[5] = {50, 40, 30, 20, 10};
    for (int i = 0; i < 5; i++) {
        if (pop_stack(s, &popped)) {
            int val = *(int *)popped;
            printf("  Popped: %d (expected: %d) %s - size: %zu\n", val,
                   expected_order[i], val == expected_order[i] ? "✓" : "✗",
                   size_stack(s));
        } else {
            printf("  FAIL: Could not pop element %d\n", i);
        }
    }

    /* Test 5: Verify stack is empty */
    printf("\nTest 5: Verify stack is empty\n");
    printf("  is_empty=%d, size=%zu\n", is_empty_stack(s), size_stack(s));

    /* Test 6: Try to pop from empty stack */
    printf("\nTest 6: Pop from empty stack (should fail)\n");
    if (pop_stack(s, &popped)) {
        printf("  FAIL: Pop should have failed on empty stack\n");
    } else {
        printf("  ✓ Correctly rejected pop on empty stack\n");
    }

    /* Test 7: Push after emptying */
    printf("\nTest 7: Push after emptying\n");
    int new_val = 999;
    if (push_stack(s, &new_val)) {
        printf("  Pushed: %d (size: %zu)\n", new_val, size_stack(s));
    } else {
        printf("  FAIL: Could not push after emptying\n");
    }

    /* Test 8: NULL pointer handling */
    printf("\nTest 8: NULL pointer handling\n");
    printf("  push_stack(NULL, ptr) = %d\n", push_stack(NULL, &new_val));
    printf("  pop_stack(NULL, out) = %d\n", pop_stack(NULL, &popped));
    printf("  is_empty_stack(NULL) = %d\n", is_empty_stack(NULL));
    printf("  size_stack(NULL) = %zu\n", size_stack(NULL));

    /* Test 9: Push multiple elements and verify size */
    printf("\nTest 9: Push multiple elements and verify size tracking\n");
    int test_vals[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    for (int i = 0; i < 10; i++) {
        push_stack(s, &test_vals[i]);
    }
    printf("  Pushed 10 more elements, total size: %zu\n", size_stack(s));

    /* Test 10: Pop and verify size decreases */
    printf("\nTest 10: Pop elements and verify size decreases\n");
    for (int i = 0; i < 5; i++) {
        pop_stack(s, &popped);
        printf("  Popped: %d, size: %zu\n", *(int *)popped, size_stack(s));
    }

    /* Clean up */
    printf("\nCleaning up...\n");
    destroy_stack(s);
    printf("Stack destroyed.\n\n");
}