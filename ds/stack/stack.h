#ifndef STACK_H
#define STACK_H

/**
 * @file stack.h
 * @brief Generic dynamic-array stack.
 *
 * A LIFO (Last-In-First-Out) data structure backed by a heap-allocated array
 * that grows automatically as elements are pushed. Values are stored as void *,
 * allowing any pointer type to be held.
 *
 * Usage:
 *   Stack *s = create_stack();
 *   push_stack(s, &value);
 *
 *   void *out = NULL;
 *   pop_stack(s, &out);
 *
 *   destroy_stack(s);
 *
 * The caller owns the memory of the values stored — stack_destroy()
 * does not free them.
 */

#include <stdbool.h>
#include <stddef.h>

/**
 * Opaque dynamic-array stack.
 *
 * Values are stored as void *, so any pointer or cast integer can be held.
 * The caller owns the memory of whatever is pushed.
 *
 * All functions that accept a Stack * are no-ops (return false/0) on NULL.
 */
typedef struct Stack Stack;

/**
 * Allocates and initialises an empty stack.
 *
 * @return Pointer to a new Stack on success, NULL on allocation failure.
 */
Stack *create_stack(void);

/**
 * Releases all memory owned by the stack.
 *
 * Does not free the values stored inside; the caller owns those.
 * Safe to call with NULL (no-op).
 *
 * @param s Pointer to the stack to destroy. May be NULL.
 */
void destroy_stack(Stack *s);

/**
 * Pushes a value onto the top of the stack.
 *
 * Grows the internal array automatically if capacity is exhausted.
 *
 * @param s     Pointer to the stack. Must not be NULL.
 * @param value The value to push. May be NULL.
 * @return true on success, false if s is NULL or allocation fails.
 */
bool push_stack(Stack *s, void *value);

/**
 * Removes and retrieves the top value.
 *
 * @param s   Pointer to the stack. Must not be NULL.
 * @param out Output pointer that receives the popped value. Must not be NULL.
 * @return true on success, false if the stack is empty or s is NULL.
 */
bool pop_stack(Stack *s, void **out);

/**
 * Retrieves the top value without removing it.
 *
 * @param s   Pointer to the stack. Must not be NULL.
 * @param out Output pointer that receives the top value. Must not be NULL.
 * @return true on success, false if the stack is empty or s is NULL.
 */
bool peek_stack(const Stack *s, void **out);

/**
 * Checks whether the stack contains no elements.
 *
 * @param s Pointer to the stack. May be NULL.
 * @return true if s is NULL or the stack has no elements, false otherwise.
 */
bool is_empty_stack(const Stack *s);

/**
 * Returns the number of elements currently in the stack.
 *
 * @param s Pointer to the stack. May be NULL.
 * @return Element count, or 0 if s is NULL.
 */
size_t size_stack(const Stack *s);

/**
 * Demonstrates various stack operations with multiple test cases.
 */
void demo_stack(void);

#endif /* STACK_H */