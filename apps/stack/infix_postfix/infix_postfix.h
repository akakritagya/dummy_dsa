#ifndef INFIX_POSTFIX_H
#define INFIX_POSTFIX_H

#include <stdbool.h>

/*
 * Infix to postfix (Reverse Polish) conversion using a stack.
 *
 * This module converts arithmetic expressions written in infix notation
 * (for example, (A+B)*C or 10+(25*3)) to postfix notation. It demonstrates
 * a classic stack application: operators and parentheses are pushed to and
 * popped from a stack based on precedence and associativity, while operands
 * are written directly to the output.
 *
 * Supported input:
 *   - Single-letter variables, such as A, b, x.
 *   - Multi-digit integers, such as 10, 123.
 *   - Operators: +, -, *, /, ^.
 *   - Parentheses: ( and ).
 *   - Spaces are ignored.
 *
 * Postfix evaluation:
 *   - Numeric operands only.
 *   - Multi-digit integers are supported.
 *   - Operators: +, -, *, /, ^.
 *
 * High-level algorithm:
 *
 *   function infix_to_postfix(expr):
 *       create empty stack for operators
 *       create empty output string
 *
 *       for each character ch in expr:
 *           if ch is a space:
 *               continue
 *           else if ch is a number:
 *               read the full number and append it to output
 *           else if ch is a letter:
 *               append it to output as a token
 *           else if ch is '(':
 *               push it onto stack
 *           else if ch is ')':
 *               pop operators until '(' is found
 *           else if ch is an operator:
 *               while stack top has higher precedence, or equal precedence
 *               and the current operator is left-associative:
 *                   pop stack top to output
 *               push current operator
 *
 *       pop remaining operators from the stack to output
 *
 * The returned postfix string uses spaces between tokens.
 */

char *infix_to_postfix(const char *expr);

/*
 * Evaluate a postfix expression containing numeric operands.
 *
 * The postfix expression may contain multi-digit integers separated by
 * spaces. On success, returns the computed result. On error, returns 0.0.
 */
double evaluate_postfix(const char *postfix);

/*
 * Demo function that converts sample infix expressions to postfix and
 * evaluates the postfix result.
 */
void demo_infix_to_postfix(void);

#endif // INFIX_POSTFIX_H