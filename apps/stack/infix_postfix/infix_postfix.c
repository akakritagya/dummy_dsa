#include "infix_postfix.h"
#include "ds/stack/stack.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// static bool is_operand(char ch) {
//     return isalpha((unsigned char)ch) || isdigit((unsigned char)ch);
// }

static bool is_operator(char ch) {
    return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^';
}

static int precedence(char op) {
    switch (op) {
    case '^':
        return 3;
    case '*':
    case '/':
        return 2;
    case '+':
    case '-':
        return 1;
    default:
        return 0;
    }
}

static bool is_right_associative(char op) {
    return op == '^';
}

static bool should_pop(char top_op, char op) {
    int p_top = precedence(top_op);
    int p_op  = precedence(op);

    if (p_top > p_op) {
        return true;
    }
    if (p_top < p_op) {
        return false;
    }

    return !is_right_associative(op);
}

static bool append_char(char *out, size_t capacity, size_t *index, char ch) {
    if (*index + 1 >= capacity) {
        return false;
    }
    out[(*index)++] = ch;
    return true;
}

static bool append_space(char *out, size_t capacity, size_t *index) {
    return append_char(out, capacity, index, ' ');
}

static bool append_token_char(char *out, size_t capacity, size_t *index,
                              char ch) {
    return append_char(out, capacity, index, ch);
}

char *infix_to_postfix(const char *expr) {
    if (!expr) {
        return NULL;
    }

    size_t n = strlen(expr);
    if (n == 0) {
        char *empty = (char *)malloc(1);
        if (empty) {
            empty[0] = '\0';
        }
        return empty;
    }

    char *output = (char *)malloc(2 * n + 1);
    if (!output) {
        return NULL;
    }

    size_t out_index = 0;
    Stack *stack     = create_stack();
    if (!stack) {
        free(output);
        return NULL;
    }

    for (size_t i = 0; i < n; i++) {
        char ch = expr[i];

        if (isspace((unsigned char)ch)) {
            continue;
        }

        if (isdigit((unsigned char)ch)) {
            while (i < n && isdigit((unsigned char)expr[i])) {
                if (!append_token_char(output, 2 * n + 1, &out_index,
                                       expr[i])) {
                    destroy_stack(stack);
                    free(output);
                    return NULL;
                }
                i++;
            }
            i--;

            if (!append_space(output, 2 * n + 1, &out_index)) {
                destroy_stack(stack);
                free(output);
                return NULL;
            }
        } else if (isalpha((unsigned char)ch)) {
            if (!append_token_char(output, 2 * n + 1, &out_index, ch) ||
                !append_space(output, 2 * n + 1, &out_index)) {
                destroy_stack(stack);
                free(output);
                return NULL;
            }
        } else if (ch == '(') {
            char *cptr = (char *)malloc(sizeof(char));
            if (!cptr) {
                destroy_stack(stack);
                free(output);
                return NULL;
            }
            *cptr = ch;

            if (!push_stack(stack, cptr)) {
                free(cptr);
                destroy_stack(stack);
                free(output);
                return NULL;
            }
        } else if (ch == ')') {
            bool found_left_paren = false;

            while (!is_empty_stack(stack)) {
                void *top_ptr = NULL;
                if (!pop_stack(stack, &top_ptr)) {
                    destroy_stack(stack);
                    free(output);
                    return NULL;
                }

                char top_ch = *(char *)top_ptr;
                free(top_ptr);

                if (top_ch == '(') {
                    found_left_paren = true;
                    break;
                }

                if (!append_token_char(output, 2 * n + 1, &out_index, top_ch) ||
                    !append_space(output, 2 * n + 1, &out_index)) {
                    destroy_stack(stack);
                    free(output);
                    return NULL;
                }
            }

            if (!found_left_paren) {
                destroy_stack(stack);
                free(output);
                return NULL;
            }
        } else if (is_operator(ch)) {
            while (!is_empty_stack(stack)) {
                void *top_ptr = NULL;
                if (!pop_stack(stack, &top_ptr)) {
                    destroy_stack(stack);
                    free(output);
                    return NULL;
                }

                char top_ch = *(char *)top_ptr;

                if (top_ch == '(') {
                    if (!push_stack(stack, top_ptr)) {
                        free(top_ptr);
                        destroy_stack(stack);
                        free(output);
                        return NULL;
                    }
                    break;
                }

                if (is_operator(top_ch) && should_pop(top_ch, ch)) {
                    if (!append_token_char(output, 2 * n + 1, &out_index,
                                           top_ch) ||
                        !append_space(output, 2 * n + 1, &out_index)) {
                        free(top_ptr);
                        destroy_stack(stack);
                        free(output);
                        return NULL;
                    }
                    free(top_ptr);
                } else {
                    if (!push_stack(stack, top_ptr)) {
                        free(top_ptr);
                        destroy_stack(stack);
                        free(output);
                        return NULL;
                    }
                    break;
                }
            }

            char *cptr = (char *)malloc(sizeof(char));
            if (!cptr) {
                destroy_stack(stack);
                free(output);
                return NULL;
            }
            *cptr = ch;

            if (!push_stack(stack, cptr)) {
                free(cptr);
                destroy_stack(stack);
                free(output);
                return NULL;
            }
        } else {
            destroy_stack(stack);
            free(output);
            return NULL;
        }
    }

    while (!is_empty_stack(stack)) {
        void *top_ptr = NULL;
        if (!pop_stack(stack, &top_ptr)) {
            destroy_stack(stack);
            free(output);
            return NULL;
        }

        char top_ch = *(char *)top_ptr;
        free(top_ptr);

        if (top_ch == '(' || top_ch == ')') {
            destroy_stack(stack);
            free(output);
            return NULL;
        }

        if (!append_token_char(output, 2 * n + 1, &out_index, top_ch) ||
            !append_space(output, 2 * n + 1, &out_index)) {
            destroy_stack(stack);
            free(output);
            return NULL;
        }
    }

    if (out_index > 0 && output[out_index - 1] == ' ') {
        out_index--;
    }

    output[out_index] = '\0';
    destroy_stack(stack);
    return output;
}

static double perform_operation(double operand1, double operand2, char op) {
    switch (op) {
    case '+':
        return operand1 + operand2;
    case '-':
        return operand1 - operand2;
    case '*':
        return operand1 * operand2;
    case '/':
        if (operand2 == 0.0) {
            fprintf(stderr, "Error: division by zero\n");
            return 0.0;
        }
        return operand1 / operand2;
    case '^':
        if (operand1 < 0.0 && operand2 != (int)operand2) {
            fprintf(stderr, "Error: unsupported power operation\n");
            return 0.0;
        }
        return pow(operand1, operand2);
    default:
        return 0.0;
    }
}

double evaluate_postfix(const char *postfix) {
    if (!postfix || postfix[0] == '\0') {
        return 0.0;
    }

    size_t len   = strlen(postfix);
    Stack *stack = create_stack();
    if (!stack) {
        fprintf(stderr, "Error: could not create stack for evaluation\n");
        return 0.0;
    }

    for (size_t i = 0; i < len; i++) {
        char ch = postfix[i];

        if (isspace((unsigned char)ch)) {
            continue;
        }

        if (isdigit((unsigned char)ch)) {
            double value = 0.0;
            while (i < len && isdigit((unsigned char)postfix[i])) {
                value = value * 10.0 + (double)(postfix[i] - '0');
                i++;
            }
            i--;

            double *ptr = (double *)malloc(sizeof(double));
            if (!ptr) {
                destroy_stack(stack);
                return 0.0;
            }
            *ptr = value;

            if (!push_stack(stack, ptr)) {
                free(ptr);
                destroy_stack(stack);
                return 0.0;
            }
        } else if (isalpha((unsigned char)ch)) {
            double value = (double)(tolower((unsigned char)ch) - 'a' + 1);

            double *ptr = (double *)malloc(sizeof(double));
            if (!ptr) {
                destroy_stack(stack);
                return 0.0;
            }
            *ptr = value;

            if (!push_stack(stack, ptr)) {
                free(ptr);
                destroy_stack(stack);
                return 0.0;
            }
        } else if (is_operator(ch)) {
            void *operand2_ptr = NULL;
            void *operand1_ptr = NULL;

            if (!pop_stack(stack, &operand2_ptr) ||
                !pop_stack(stack, &operand1_ptr)) {
                fprintf(stderr, "Error: invalid postfix expression\n");
                if (operand2_ptr) {
                    free(operand2_ptr);
                }
                if (operand1_ptr) {
                    free(operand1_ptr);
                }
                destroy_stack(stack);
                return 0.0;
            }

            double operand1 = *(double *)operand1_ptr;
            double operand2 = *(double *)operand2_ptr;
            free(operand1_ptr);
            free(operand2_ptr);

            double result = perform_operation(operand1, operand2, ch);

            double *result_ptr = (double *)malloc(sizeof(double));
            if (!result_ptr) {
                destroy_stack(stack);
                return 0.0;
            }
            *result_ptr = result;

            if (!push_stack(stack, result_ptr)) {
                free(result_ptr);
                destroy_stack(stack);
                return 0.0;
            }
        } else {
            fprintf(stderr, "Error: invalid character in postfix expression\n");
            destroy_stack(stack);
            return 0.0;
        }
    }

    void *result_ptr = NULL;
    double result    = 0.0;

    if (pop_stack(stack, &result_ptr)) {
        result = *(double *)result_ptr;
        free(result_ptr);
    } else {
        fprintf(stderr, "Error: invalid postfix expression\n");
    }

    while (!is_empty_stack(stack)) {
        void *ptr = NULL;
        if (pop_stack(stack, &ptr)) {
            free(ptr);
        } else {
            break;
        }
    }

    destroy_stack(stack);
    return result;
}

void demo_infix_to_postfix(void) {
    printf("=== Infix to Postfix Conversion & Evaluation ===\n\n");

    const char *tests[] = {
        "2+3*4", "(2+3)*4",  "1+2-3",     "2^3",    "6/2*3", "5", "10+20",
        "12*3",  "100+50*2", "(25+15)*2", "123-45", "999/3", NULL};

    for (int i = 0; tests[i] != NULL; i++) {
        char *postfix = infix_to_postfix(tests[i]);
        if (!postfix) {
            printf("Test %d: %s\n  -> Error (invalid or memory failure)\n\n",
                   i + 1, tests[i]);
        } else {
            double result = evaluate_postfix(postfix);
            printf(
                "Test %d:\n  Infix:   %s\n  Postfix: %s\n  Result:  %.2f\n\n",
                i + 1, tests[i], postfix, result);
            free(postfix);
        }
    }
}