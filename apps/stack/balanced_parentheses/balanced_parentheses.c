#include "balanced_parentheses.h"
#include "ds/stack/stack.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static bool is_matching_pair(const char open, const char close) {
    return (open == '(' && close == ')') || (open == '[' && close == ']') ||
           (open == '{' && close == '}');
}

bool is_balanced(const char *expr) {
    if (!expr)
        return true; // treat NULL as empty (balanced)

    Stack *s = create_stack();
    if (!s) {
        fprintf(stderr, "Error: could not create stack\n");
        return false;
    }

    for (size_t i = 0; expr[i] != '\0'; i++) {
        char c = expr[i];

        // Push opening brackets
        if (c == '(' || c == '[' || c == '{') {
            // store pointer to this character
            if (!push_stack(s, (void *)&expr[i])) {
                fprintf(stderr, "Error: push_stack failed\n");
                destroy_stack(s);
                return false;
            }
        }
        // Handle closing brackets
        else if (c == ')' || c == ']' || c == '}') {
            void *top_ptr = NULL;
            if (!pop_stack(s, &top_ptr)) {
                // closing bracket without matching opening
                destroy_stack(s);
                return false;
            }

            char open = *(char *)top_ptr;
            if (!is_matching_pair(open, c)) {
                destroy_stack(s);
                return false;
            }
        }
    }

    bool balanced = is_empty_stack(s);
    destroy_stack(s);
    return balanced;
}

void demo_balanced_parentheses(void) {
    printf("=== Balanced Parentheses Demo ===\n\n");

    // Test cases
    const char *test_cases[] = {
        // Balanced cases
        "",
        "()",
        "[]",
        "{}",
        "()[]{}",
        "({[]})",
        "{[()]}",
        "((()))",
        "[{()}]",

        // Unbalanced cases
        "(",
        ")",
        "([)]",
        "{[}]",
        "(((",
        ")))",
        "([{",
        "([)]",
        "{[(])",
    };

    int num_tests = sizeof(test_cases) / sizeof(test_cases[0]);

    for (int i = 0; i < num_tests; i++) {
        bool result        = is_balanced(test_cases[i]);
        const char *status = result ? "BALANCED" : "UNBALANCED";
        printf("Test %2d: %-15s => %s\n", i + 1,
               test_cases[i][0] == '\0' ? "(empty)" : test_cases[i], status);
    }

    printf("\n");
}
