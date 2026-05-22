#ifndef BALANCED_PARENTHESES_H
#define BALANCED_PARENTHESES_H

#include <stdbool.h>

/**
 * This module provides functionality to check if an expression has balanced
 * parentheses, brackets, and braces. It supports three types of bracket pairs:
 * - Parentheses: ( )
 * - Brackets: [ ]
 * - Braces: { }
 *
 * ALGORITHM: Stack-based Bracket Matching
 *
 *   function is_balanced(expr):
 *       if expr is NULL or empty:
 *           return true
 *
 *       stack = create_stack()
 *
 *       for each character c in expr:
 *           if c is opening bracket ('(', '[', or '{'):
 *               push c onto stack
 *           else if c is closing bracket (')', ']', or '}'):
 *               if stack is empty:
 *                   return false  // unmatched closing bracket
 *               top = pop from stack
 *               if top and c don't match:
 *                   return false  // mismatched pair
 *           else:
 *               ignore other characters
 *
 *       return is_stack_empty()  // true if all brackets matched
 *
 * COMPLEXITY:
 * - TIME: O(n) where n is the length of the expression (single pass)
 * - SPACE: O(h) where h is the maximum nesting depth
 *
 * KEY INSIGHTS:
 * - Stack is ideal for matching nested structures (LIFO principle)
 * - Each opening bracket must have a corresponding closing bracket
 * - Brackets must be properly nested (no interleaving like ([)]
 * - Non-bracket characters are ignored
 */

/**
 * Check if an expression has balanced parentheses, brackets, and braces
 * @param expr The expression string to check (can be NULL)
 * @return true if balanced or empty, false otherwise
 */
bool is_balanced(const char *expr);

/**
 * Demo function that tests the is_balanced() function
 * Tests various balanced and unbalanced expressions and prints results
 */
void demo_balanced_parentheses(void);

#endif // BALANCED_PARENTHESES_H
