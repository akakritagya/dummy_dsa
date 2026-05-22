#define _POSIX_C_SOURCE 200809L

#include "html_checker.h"
#include "ds/stack/stack.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* List of simple void elements we treat as self-contained. */
static bool is_void_element(const char *tag_name) {
    /* Not exhaustive, but enough for a simple checker. */
    const char *void_tags[] = {"br",    "img",    "input", "meta", "link",
                               "hr",    "source", "area",  "base", "col",
                               "embed", "param",  "track", NULL};

    for (int i = 0; void_tags[i] != NULL; i++) {
        if (strcmp(tag_name, void_tags[i]) == 0) {
            return true;
        }
    }
    return false;
}

/* Trim leading/trailing whitespace in-place around a tag name portion. */
static void trim_whitespace(char *s) {
    if (!s)
        return;

    char *start = s;
    while (*start && isspace((unsigned char)*start))
        start++;

    char *end = start + strlen(start);
    while (end > start && isspace((unsigned char)end[-1]))
        end--;
    *end = '\0';

    if (start != s) {
        memmove(s, start, (size_t)(end - start + 1));
    }
}

/*
 * Extract tag name from inside `<...>` content (without angle brackets),
 * ignoring attributes. For example:
 *   "div class=\"x\""   -> "div"
 *   "/div"              -> "div" (caller decides it is closing)
 *   "br/"               -> "br"
 */
static char *extract_tag_name(const char *tag_content, bool *is_closing,
                              bool *is_self_closing) {
    *is_closing      = false;
    *is_self_closing = false;

    if (!tag_content)
        return NULL;

    /* Work on a temporary copy to trim and parse. */
    char *tmp = strdup(tag_content);
    if (!tmp)
        return NULL;

    trim_whitespace(tmp);

    /* Check for closing tag: starts with '/' like "/div". */
    if (tmp[0] == '/') {
        *is_closing = true;
        memmove(tmp, tmp + 1, strlen(tmp)); /* remove leading '/' */
        trim_whitespace(tmp);
    }

    size_t len = strlen(tmp);
    if (len == 0) {
        free(tmp);
        return NULL;
    }

    /* Check for self-closing end '/', e.g., "br/" or "img />". */
    if (tmp[len - 1] == '/') {
        *is_self_closing = true;
        tmp[len - 1]     = '\0';
        trim_whitespace(tmp);
        len = strlen(tmp);
        if (len == 0) {
            free(tmp);
            return NULL;
        }
    }

    /* Tag name ends at first whitespace or end of string. */
    size_t i = 0;
    while (tmp[i] && !isspace((unsigned char)tmp[i])) {
        i++;
    }
    tmp[i] = '\0';

    if (tmp[0] == '\0') {
        free(tmp);
        return NULL;
    }

    /* Return just the tag name as a fresh allocation. */
    char *name = strdup(tmp);
    free(tmp);
    return name;
}

bool is_html_syntax_valid(const char *html) {
    if (!html)
        return true; /* treat NULL as empty, thus trivially valid */

    Stack *stack = create_stack();
    if (!stack) {
        fprintf(stderr, "Error: could not create stack for HTML checker\n");
        return false;
    }

    /* Helper macro to clean up and return on error */
#define CLEANUP_AND_RETURN_FALSE()                                             \
    do {                                                                       \
        while (!is_empty_stack(stack)) {                                       \
            void *ptr = NULL;                                                  \
            if (pop_stack(stack, &ptr)) {                                      \
                free(ptr);                                                     \
            } else {                                                           \
                break;                                                         \
            }                                                                  \
        }                                                                      \
        destroy_stack(stack);                                                  \
        return false;                                                          \
    } while (0)

    const char *p = html;

    while (*p != '\0') {
        if (*p == '<') {
            const char *start = p + 1;

            /* Handle comments like <!-- ... --> quickly: skip them. */
            if (strncmp(start, "!--", 3) == 0) {
                const char *comment_end = strstr(start + 3, "-->");
                if (!comment_end) {
                    /* Unterminated comment -> invalid. */
                    CLEANUP_AND_RETURN_FALSE();
                }
                p = comment_end + 3;
                continue;
            }

            /* Find closing '>' for this tag. */
            const char *end = strchr(start, '>');
            if (!end) {
                /* No closing '>' -> invalid tag. */
                CLEANUP_AND_RETURN_FALSE();
            }

            size_t len = (size_t)(end - start);
            if (len == 0) {
                CLEANUP_AND_RETURN_FALSE();
            }

            char *tag_content = (char *)malloc(len + 1);
            if (!tag_content) {
                CLEANUP_AND_RETURN_FALSE();
            }
            memcpy(tag_content, start, len);
            tag_content[len] = '\0';

            bool is_closing      = false;
            bool is_self_closing = false;
            char *tag_name =
                extract_tag_name(tag_content, &is_closing, &is_self_closing);
            free(tag_content);

            if (!tag_name) {
                CLEANUP_AND_RETURN_FALSE();
            }

            /* Lowercase the tag name for simpler matching (HTML is
             * case-insensitive). */
            for (char *c = tag_name; *c; ++c) {
                *c = (char)tolower((unsigned char)*c);
            }

            if (is_closing) {
                /* Closing tag: must match top of stack. */
                void *top_ptr = NULL;
                if (!pop_stack(stack, &top_ptr)) {
                    /* Nothing to match with. */
                    free(tag_name);
                    CLEANUP_AND_RETURN_FALSE();
                }

                char *open_tag = (char *)top_ptr;
                if (strcmp(open_tag, tag_name) != 0) {
                    free(open_tag);
                    free(tag_name);
                    CLEANUP_AND_RETURN_FALSE();
                }

                /* Matched pair; free both open_tag and tag_name. */
                free(open_tag);
                free(tag_name);
            } else {
                /* Opening or self-closing tag. */
                if (is_self_closing || is_void_element(tag_name)) {
                    /* No need to push; tag is self-contained. */
                    free(tag_name);
                } else {
                    /* Push opening tag name onto stack. */
                    if (!push_stack(stack, tag_name)) {
                        free(tag_name);
                        CLEANUP_AND_RETURN_FALSE();
                    }
                }
            }

            p = end + 1;
        } else {
            /* Regular text; just move on. */
            p++;
        }
    }

    /* If anything left on stack, there are unclosed tags. */
    bool ok = is_empty_stack(stack);

    /* Free any remaining tag names if not empty. */
    while (!is_empty_stack(stack)) {
        void *ptr = NULL;
        if (pop_stack(stack, &ptr)) {
            free(ptr);
        } else {
            break;
        }
    }

    destroy_stack(stack);
    return ok;

#undef CLEANUP_AND_RETURN_FALSE
}

void demo_html_syntax_checker(void) {
    printf("=== HTML Syntax Checker Demo ===\n\n");

    /* Test cases: valid and invalid HTML structures */
    const char *test_cases[] = {
        /* Valid HTML */
        "<html><body><h1>Hello</h1></body></html>",
        "<div><p>Paragraph</p></div>",
        "<div><img src=\"x.png\" /></div>",
        "<p><strong>Bold</strong> and <em>italic</em></p>",
        "<ul><li>Item 1</li><li>Item 2</li></ul>",
        "<div class=\"container\"><br><hr></div>",
        "<!-- comment --><p>Text</p>",
        "<footer><p>Copyright 2026</p></footer>",
        "", /* empty string is valid */

        /* Invalid HTML */
        "<div><p>Paragraph</div></p>",    /* mismatched closing */
        "<p><strong>Missing close</p>",   /* unclosed strong tag */
        "<div><img src=\"x.png\"></div>", /* img should be self-closing */
        "<ul><li>Item 1<li>Item 2</ul>",  /* unclosed li tags */
        "<p>Text</div>",                  /* wrong closing tag */
        "<!-- unterminated comment",      /* unterminated comment */
        "<div><span></div></span>",       /* incorrectly nested */
        "<",                              /* incomplete tag */
    };

    int num_tests = sizeof(test_cases) / sizeof(test_cases[0]);

    for (int i = 0; i < num_tests; i++) {
        bool result        = is_html_syntax_valid(test_cases[i]);
        const char *status = result ? "✓ VALID" : "✗ INVALID";
        const char *display =
            test_cases[i][0] == '\0' ? "(empty)" : test_cases[i];

        printf("Test %2d: %-45s => %s\n", i + 1, display, status);
    }

    printf("\n");
}