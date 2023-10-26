#include "Utils.h"

#include "../collections/Vector.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    enum { LITERAL, IDENTIFIER, OPERATOR, LPAREN, RPAREN } type;
    union {
        int value;
        char op;   // unique char values for multi-char operators
        char *name;
    } data;
} Token;

typedef struct Node {
    Token token;
    const struct Node* left;
    const struct Node* right;
} ASTNode;

Vector *tokenize(const char* expr) {
    Vector *list = vector_default();

    const char *root_expr = expr;
    while (*expr) {
        //printf("id: %ld\n", expr - root_expr);
        while (isspace(*expr)) expr++;

        Token *token = malloc(sizeof(Token));
        if (isdigit(*expr)) {
            token->type = LITERAL;
            token->data.value = strtol(expr, (char**)&expr, 10);
        } else if (isalpha(*expr)) {
            token->type = IDENTIFIER;
            char *start = (char*)expr;
            while (isalpha(*expr) || *expr == '_') expr++;
            token->data.name = malloc(expr - start + 1);
            memcpy(token->data.name, start, expr - start);
            token->data.name[expr - start] = '\0';
        } else {
            switch (*expr) {
                case '+': case '-': case '*': case '/': case '%':
                    token->type = OPERATOR;
                    token->data.op = *expr;
                    expr++;
                    break;
                case '<': 
                    expr++;
                    if (*expr == '=') {
                        token->type = OPERATOR;
                        token->data.op = 'L'; // Using 'L' to represent '<='
                        expr++;
                    } else {
                        token->type = OPERATOR;
                        token->data.op = *(expr - 1);
                    }
                    break;
                case '>':
                    expr++;
                    if (*expr == '=') {
                        token->type = OPERATOR;
                        token->data.op = 'G'; // Using 'G' to represent '>='
                        expr++;
                    } else {
                        token->type = OPERATOR;
                        token->data.op = *(expr - 1);
                    }
                    break;
                case '=':
                    expr++;
                    if (*expr == '=') {
                        token->type = OPERATOR;
                        token->data.op = 'E'; // Using 'E' to represent '=='
                        expr++;
                    } else {
                        // Handle error, unexpected token
                    }
                    break;
                case '!':
                    expr++;
                    if (*expr == '=') {
                        token->type = OPERATOR;
                        token->data.op = 'N'; // Using 'N' to represent '!='
                        expr++;
                    } else {
                        token->type = OPERATOR;
                        token->data.op = '!';
                    }
                    break;
                case '&':
                    expr++;
                    if (*expr == '&') {
                        token->type = OPERATOR;
                        token->data.op = 'A'; // Using 'A' to represent '&&'
                        expr++;
                    } else {
                        // Handle error, unexpected token
                    }
                    break;
                case '|':
                    expr++;
                    if (*expr == '|') {
                        token->type = OPERATOR;
                        token->data.op = 'O'; // Using 'O' to represent '||'
                        expr++;
                    } else {
                        // Handle error, unexpected token
                    }
                    break;
                case '(':
                    token->type = LPAREN;
                    token->data.op = *expr;
                    expr++;
                    break;
                case ')':
                    token->type = RPAREN;
                    token->data.op = *expr;
                    expr++;
                    break;
                default:
                    // Invalid character
                    printf("Invalid character in expression: '%c', ascii: %d\n", *expr, *expr);
                    exit(EXIT_FAILURE);
            }
        }

        list->push(list, token);
    }

    return list;
}

int get_precedence(char op) {
    switch(op) {
        case '+':
        case '-':
            return 2;
        case '*':
        case '/':
        case '%':
            return 3;
        case '<':
        case '>':
        case 'E': // ==
        case 'N': // !=
        case 'G': // >=
        case 'L': // <=
            return 1;
        case 'A': // &&
        case 'O': // ||
            return 0;
        default:
            fprintf(stderr, "Undefined operator: %c\n", op);
            return -1; // Undefined operator
    }
}

int is_left_associative(char op) {
    switch(op) {
        // Assuming all provided operators are left-associative
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case '<':
        case '>':
        case 'E': // ==
        case 'N': // !=
        case 'A': // &&
        case 'O': // ||
        case 'G': // >=
        case 'L': // <=
            return 1;
        default:
            fprintf(stderr, "Undefined operator: %c\n", op);
            return 0; // Undefined operator or right-associative
    }
}

// Parsing: Convert tokens to AST
const ASTNode* parse(Vector *tokens) {
    Vector *output_queue = vector_default();
    Vector *operator_stack = vector_default();

    size_t tokens_len = tokens->len(tokens);

    for (size_t i = 0; i < tokens_len; ++i) {
        const Token *token = tokens->get(tokens, i);

        if (token->type == LITERAL) {
            output_queue->push(output_queue, token);
        } else if (token->type == IDENTIFIER) {
            output_queue->push(output_queue, token);
        } else if (token->type == OPERATOR) {
            Token *last_token = operator_stack->last(operator_stack);
            while (last_token && 
                    last_token->type != LPAREN && 
                    (get_precedence(last_token->data.op) > get_precedence(token->data.op) || 
                    (get_precedence(last_token->data.op) == get_precedence(token->data.op) && is_left_associative(token->data.op)))) {
                output_queue->push(output_queue, last_token);
                operator_stack->pop(operator_stack);

                last_token = operator_stack->last(operator_stack);
            }
            operator_stack->push(operator_stack, token);
        } else if (token->type == LPAREN) {
            operator_stack->push(operator_stack, token);
        } else if (token->type == RPAREN) {
            Token *last_token = operator_stack->last(operator_stack);
            while (last_token && last_token->type != LPAREN) {
                assert(operator_stack->len(operator_stack) != 0);

                output_queue->push(output_queue, last_token);
                operator_stack->pop(operator_stack);

                last_token = operator_stack->last(operator_stack);
            }
            operator_stack->pop(operator_stack);
        } else {
            // Handle error, unexpected token
            fprintf(stderr, "Unexpected token type: %d\n", token->type);
        }
        // add handling for function
    }

    for (size_t i = 0; i < operator_stack->len(operator_stack); ++i) {
        Token *last_token = operator_stack->last(operator_stack);
        output_queue->push(output_queue, last_token);
        operator_stack->pop(operator_stack);
    }

    //printf("Output queue: ");
    //for (size_t i = 0; i < output_queue->len(output_queue); ++i) {
    //    Token *token = output_queue->get(output_queue, i);
    //    if (token->type == LITERAL) {
    //        printf("{%d} ", token->data.value);
    //    } else if (token->type == OPERATOR) {
    //        printf("{%c} ", token->data.op);
    //    } else if (token->type == LPAREN) {
    //        printf("{(} ");
    //    } else if (token->type == RPAREN) {
    //        printf("{)} ");
    //    }
    //}
    //printf("\n");
    //
    //printf("Creating AST...\n");
    Vector *stack = vector_default();
    
    size_t output_queue_len = output_queue->len(output_queue);
    for (size_t i = 0; i < output_queue_len; ++i) {
        const Token *current_token = output_queue->get(output_queue, i);
        
        ASTNode *new_node = (ASTNode*) malloc(sizeof(ASTNode));
        new_node->token = *current_token;
        new_node->left = NULL;
        new_node->right = NULL;

        if (current_token->type == LITERAL) {
            stack->push(stack, new_node);
        } else if (current_token->type == IDENTIFIER) {
            stack->push(stack, new_node);
        } else if (current_token->type == OPERATOR) {
            if (stack->len(stack) < 2) {
                // Error: Not enough operands for operator
                free(new_node);
                // Clean-up other allocated nodes if needed.
                return NULL;
            }

            const ASTNode *right_child = stack->pop(stack);
            const ASTNode *left_child = stack->pop(stack);
            
            new_node->left = left_child;
            new_node->right = right_child;
            
            stack->push(stack, new_node);
        } else {
            // Unexpected token type
            fprintf(stderr, "Unexpected token type: %d\n", current_token->type);
            free(new_node);
            // Clean-up other allocated nodes if needed.
            return NULL;
        }
    }

    if (stack->len(stack) != 1) {
        // Error: Invalid postfix notation
        // Clean-up allocated nodes if needed.
        fprintf(stderr, "Invalid postfix notation\n");
        return NULL;
    }

    const ASTNode *root = stack->pop(stack);
    // Remember to free the stack after use.
    return root;
}

bool isnumber(const char *str)
{
    while (*str) {
        if (!isdigit(*str)) return false;
        str++;
    }
    return true;
}

// Evaluation
int evaluate(const ASTNode* root, const Context *ctx) {
    if (!root) return 0;
    if (root->token.type == LITERAL) {
        return root->token.data.value;
    } else if (root->token.type == IDENTIFIER) {
        char *value = ctx->get(ctx, root->token.data.name)->value.string;
        if (value) {
            if (strncmp(value, "true", 4) == 0) return 1;
            if (strncmp(value, "false", 5) == 0) return 0;
            if (isnumber(value)) return atoi(value);
            return 1;
        } else {
            fprintf(stderr, "Undefined identifier: %s\n", root->token.data.name);
            exit(EXIT_FAILURE);
        }
    } else if (root->token.type == OPERATOR) {
        int left = evaluate(root->left, ctx);
        int right = evaluate(root->right, ctx);
        switch (root->token.data.op) {
            case '+': return left + right;
            case '-': return left - right;
            case '*': return left * right;
            case '/': return left / right;
            case '%': return left % right;
            case '<': return left < right;
            case '>': return left > right;
            case 'E': return left == right;
            case 'N': return left != right;
            case 'A': return left && right;
            case 'O': return left || right;
            case 'G': return left >= right;
            case 'L': return left <= right;
            default: {
                fprintf(stderr, "Invalid operator: %c\n", root->token.data.op);
                exit(EXIT_FAILURE);
            }
        }
    }
    return 0;
}

void print_tokens(Vector *tokens)
{
    size_t tokens_len = tokens->len(tokens);
    for (size_t i = 0; i < tokens_len; ++i) {
        const Token *token = tokens->get(tokens, i);

        switch (token->type) {
            case LITERAL: printf("Token: { type: LITERAL, value: %d }\n", token->data.value); break;
            case OPERATOR: printf("Token: { type: OPERATOR, op: %c }\n", token->data.op); break;
            case LPAREN: printf("Token: { type: LPAREN, op: %c }\n", token->data.op); break;
            case RPAREN: printf("Token: { type: RPAREN, op: %c }\n", token->data.op); break;
            case IDENTIFIER: printf("Token: { type: IDENTIFIER, name: %s }\n", token->data.name); break;
        }
    }
}

bool eval_condition(const char *expr, const Context *ctx) {
    Vector *tokens = tokenize(expr);
    //print_tokens(tokens);
    const ASTNode* root = parse(tokens);
    int result = evaluate(root, ctx);
    //free(tokens.tokens);
    // Free AST nodes here (if you use dynamic allocation in your parse function)
    return result != 0;
}

char *str_slice(const char *str, size_t from, size_t to) {
    if (from > to || from >= strlen(str)) exit(1); // TODO handle error invalid slice

    size_t slice_length = to - from;
    char* slice = malloc(slice_length + 1); // +1 for null terminator

    strncpy(slice, str + from, slice_length);
    slice[slice_length] = '\0'; // Null-terminate the slice

    return slice;
}

char *str_trim(const char *str) {
    if (!str) exit(1); // TODO handle error invalid string

    const char *start = str;
    while (*start && isspace((unsigned char)*start)) start++;

    const char *end = str + strlen(str) - 1;
    while (end > start && isspace((unsigned char)*end)) end--;

    size_t length = end - start + 1;
    char *trimmed = malloc(length + 1);

    strncpy(trimmed, start, length);
    trimmed[length] = '\0';

    return trimmed;
}

char **str_split(const char *str, const char *delim) {
    if (!str || !delim) exit(1); // TODO handle error invalid string or delimiter

    size_t capacity = 10;
    char **result = malloc(capacity * sizeof(char *));
    char *copy = strdup(str);

    char *token = strtok(copy, delim);
    size_t count = 0;

    while (token) {
        if (count >= capacity) {
            capacity *= 2;
            result = realloc(result, capacity * sizeof(char *));
        }

        result[count] = strdup(token);

        ++count;
        token = strtok(NULL, delim);
    }

    // Resize the result array to the exact number of tokens
    char **temp = realloc(result, count * sizeof(char *));
    if (temp) {
        result = temp;
    } else {
        // Failed to shrink the array, but the original data is still valid
    }

    // Add a NULL pointer at the end to indicate the end of the array
    result[count] = NULL;

    free(copy);
    return result;
}

size_t str_arr_size(char **arr) {
    size_t count = 0;
    if (arr) {
        while (arr[count]) {
            ++count;
        }
    }
    return count;
}
