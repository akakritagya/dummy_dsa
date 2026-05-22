#ifndef HTML_SYNTAX_CHECKER_H
#define HTML_SYNTAX_CHECKER_H

#include <stdbool.h>

/*
 * Simple HTML tag-balance checker using a stack.
 *
 * This module checks whether HTML-like markup has properly matched and
 * nested tags, such as <div>...</div> or <p>...</p>. It uses a stack of
 * tag names: each opening tag pushes its name, and each closing tag
 * pops and compares. The input is considered valid if all tags match
 * correctly and the stack is empty at the end. This is a simplified
 * checker and does not implement the full HTML parsing specification:
 * it ignores attributes, treats text outside tags as plain content, and
 * can optionally treat known void elements (e.g., <br>, <img>) as
 * self-contained.
 *
 * Algorithm (high-level):
 *
 *   function is_html_syntax_valid(html):
 *       create empty stack
 *
 *       while not at end of html:
 *           if current char is '<':
 *               parse until next '>' to get tag token
 *               if token is a comment or DOCTYPE:
 *                   skip it
 *               else if token is a closing tag like </tag>:
 *                   if stack is empty:
 *                       return false
 *                   open_tag = pop from stack
 *                   if open_tag != tag:
 *                       return false
 *               else if token is an opening tag like <tag ...>:
 *                   if tag is not a void/self-closing tag:
 *                       push tag name onto stack
 *           else:
 *               advance to next character
 *
 *       if stack is empty:
 *           return true
 *       else:
 *           return false
 */

/**
 * Check if an HTML string has properly balanced and nested tags.
 * Simplified: ignores attributes and treats only basic tag structure.
 *
 * @param html The HTML source code as a null-terminated string.
 * @return true if tags are balanced and properly nested, false otherwise.
 */
bool is_html_syntax_valid(const char *html);

/**
 * Demo function that runs a few HTML samples through the syntax checker
 * and prints the results.
 */
void demo_html_syntax_checker(void);

#endif // HTML_SYNTAX_CHECKER_H