#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "logger.h"

#include "ast.h"

ast_node* make_node(node_type type, node_value val, ast_node* parent)
{
    ast_node* node = (ast_node*)calloc(1, sizeof(*node));
    *node = {
        .type = type,
        .value = val,
        .parent = parent,
        .left = NULL,
        .right = NULL
    };

    return node;
}

ast_node* copy_subtree(ast_node * node)
{
    if (!node) return NULL;

    ast_node* res = make_node(node->type, node->value);

    res->left  = copy_subtree(node->left);
    res->right = copy_subtree(node->right);

    if (res-> left) res-> left->parent = res;
    if (res->right) res->right->parent = res;

    return res;
}

void delete_node(ast_node* node)
{
    LOG_ASSERT(node != NULL, return);
    LOG_ASSERT(node-> left == NULL, return);
    LOG_ASSERT(node->right == NULL, return);

    if (node->type == NODE_NVAR ||
        node->type == NODE_NFUN ||
        node->type == NODE_ARG  ||
        node->type == NODE_ASS  ||
        node->type == NODE_CALL ||
        node->type == NODE_VAR)
    {
        free(node->value.name);
    }

    free(node);
}

void delete_subtree(ast_node* node)
{
    LOG_ASSERT(node != NULL, return);

    if (node-> left) delete_subtree(node->left);
    if (node->right) delete_subtree(node->right);

    node-> left = NULL;
    node->right = NULL;

    delete_node(node);
}

void tree_ctor(abstract_syntax_tree *tree)
{
    *tree = {};
    return;
}

void tree_dtor(abstract_syntax_tree *tree)
{
    LOG_ASSERT(tree != NULL, return);

    if (tree->root) delete_subtree(tree->root);
    tree->root = NULL;
}

static void node_print(const ast_node* node, FILE* output, int indent = 0);

void tree_print(const abstract_syntax_tree *tree, FILE *output)
{
    node_print(tree->root, output);
}

static ast_node* node_read(FILE* input);

void tree_read(abstract_syntax_tree *tree, FILE *input)
{
    tree->root = node_read(input);
}

tree_iterator tree_begin(abstract_syntax_tree *tree)
{
    LOG_ASSERT(tree != NULL, return NULL);
    LOG_ASSERT(tree->root != NULL, return NULL);

    tree_iterator node = tree->root;
    while (node->left)
        node = node->left;
    
    return node;
}

tree_iterator tree_get_next(tree_iterator node)
{
    LOG_ASSERT(node, return NULL);

    if (node->right)
    {
        node = node->right;
        while (node->left)
            node = node->left;
        return node;
    }

    tree_iterator parent = node->parent;
    
    while (parent && parent->left != node)
    {
        node = parent;
        parent = node->parent;
    }

    return parent;
}

tree_iterator tree_get_prev(tree_iterator node)
{
    LOG_ASSERT(node, return NULL);

    if (node->left)
    {
        node = node->left;
        while (node->right)
            node = node->right;
        return node;
    }

    tree_iterator parent = node->parent;
    while (parent && parent->right != node)
    {
        node = parent;
        parent = node->parent;
    }

    return parent;
}

tree_iterator tree_end(abstract_syntax_tree* tree)
{
    LOG_ASSERT(tree != NULL, return NULL);
    LOG_ASSERT(tree->root != NULL, return NULL);

    tree_iterator node = tree->root;
    while (node->right)
        node = node->right;
    
    return node;
}

static void print_type(node_type type, FILE* output);
static void print_op(op_type op, FILE* output);
static void print_cmp(cmp_type cmp, FILE* output) __attribute__((unused));
static void print_logic(logic_type logic, FILE* output) __attribute__((unused));
static void print_indent(int indent, FILE* output);

static void node_print(const ast_node *node, FILE *output, int indent)
{
    print_indent(indent, output);
    fputc('{', output);
    if (!node)
    {
        fputc('}', output);
        return;
    }
    indent++;
    putc('\n', output);

    print_indent(indent, output);
    print_type(node->type, output);
    fputs(",\n", output);

    print_indent(indent, output);
    switch (node->type)
    {
        case NODE_OP:    print_op   (node->value.op,    output); break;

        // TODO: make standard compliance optional, because "The Standard Is Shit" (c)

        case NODE_CMP:   //print_cmp  (node->value.cmp,   output); break;
        case NODE_LOGIC: //print_logic(node->value.logic, output); break;
            log_message(MSG_ERROR, "Type '%s' is not standard-compliant",
                node->type == NODE_CMP ? "CMP" : "LOGIC");
            return;

        case NODE_NUM:   fprintf(output, "%.3lf", node->value.num); break;

        case NODE_NVAR:
        case NODE_NFUN:
        case NODE_ARG:
        case NODE_ASS:
        case NODE_CALL:
        case NODE_VAR:
            fputs(node->value.name, output);
            break;

        case NODE_DEFS:
        case NODE_BLOCK:
        case NODE_SEQ:
        case NODE_IF:
        case NODE_BRANCH:
        case NODE_WHILE:
        case NODE_RET:
        case NODE_PAR:
            fputs("NULL", output);
            break;
        
        default:
            log_message(MSG_WARNING, "Unknown 'node_type' enum value: %d", (int)node->type);
            return;
    }
    fputs(",\n", output);

    node_print(node-> left, output, indent);
    fputs(",\n", output);
    node_print(node->right, output, indent);

    fputc('\n', output);
    print_indent(indent-1, output);
    fputc('}', output);
}

static node_type  read_type(FILE* input);
static op_type    read_op(FILE* input);
static cmp_type   read_cmp(FILE* input) __attribute__((unused));
static logic_type read_logic(FILE* input) __attribute__((unused));

static ast_node *node_read(FILE *input)
{
    char c = '\0';
    LOG_ASSERT_ERROR(fscanf(input, " %c", &c) == 1 && c == '{', return NULL,
        "Invalid tree file format", NULL);
    LOG_ASSERT_ERROR(fscanf(input, " %c", &c) == 1, return NULL,
        "Invalid tree file format", NULL);

    if (c == '}') return NULL;
    ungetc(c, input);

    node_type type = read_type(input);
    LOG_ASSERT((int)type != -1, return NULL);
    ast_node* node = make_node(type, {});

    LOG_ASSERT_ERROR(fscanf(input, " %c", &c) == 1 && c == ',', return NULL,
        "Invalid tree file format", NULL);

    switch (node->type)
    {
        case NODE_OP:
            node->value.op = read_op(input);
            LOG_ASSERT((int)node->value.op != -1, { delete_node(node); return NULL; });
            break;

        // TODO: make standard compliance optional, because "The Standard Is Shit" (c)

        case NODE_CMP:   //node->value.cmp = read_cmp(input);
        case NODE_LOGIC: //node->value.logic = read_logic(input);
            log_message(MSG_ERROR, "Type '%s' is not standard-compliant",
                node->type == NODE_CMP ? "CMP" : "LOGIC");
            return NULL;

        case NODE_NUM:
            LOG_ASSERT_ERROR(fscanf(input, " %lf", &node->value.num) == 1, { delete_node(node); return NULL; },
                "Invalid number format in tree file.", NULL);
            break;

        case NODE_NVAR:
        case NODE_NFUN:
        case NODE_ARG:
        case NODE_ASS:
        case NODE_CALL:
        case NODE_VAR:
            LOG_ASSERT_ERROR(fscanf(input, " %m[a-zA-Z0-9_]", &node->value.name) == 1,
                { delete_node(node); return NULL; },
                "Invalid name in tree file.", NULL);
            LOG_ASSERT_ERROR(!isdigit(node->value.name[0]), { delete_node(node); return NULL; },
                "Identifier cannot start with a number.", NULL);
            break;

        case NODE_DEFS:
        case NODE_BLOCK:
        case NODE_SEQ:
        case NODE_IF:
        case NODE_BRANCH:
        case NODE_WHILE:
        case NODE_RET:
        case NODE_PAR:
        {
            char BUFFER[5] = "";
            LOG_ASSERT_ERROR(fscanf(input, " %4s", BUFFER) == 1 && strcmp(BUFFER, "NULL") == 0,
                { delete_node(node); return NULL; },
                "Expected 'NULL'.", NULL);
            break;
        }
        
        default:
            log_message(MSG_WARNING, "Unknown 'node_type' enum value: %d", (int)node->type);
            return NULL;

    }

    LOG_ASSERT_ERROR(fscanf(input, " %c", &c) == 1 && c == ',', return NULL,
        "Invalid tree file format", NULL);

    node->left = node_read(input);
    LOG_ASSERT_ERROR(fscanf(input, " %c", &c) == 1 && c == ',', return NULL,
        "Invalid tree file format", NULL);
    node->right = node_read(input);

    LOG_ASSERT_ERROR(fscanf(input, " %c", &c) == 1 && c == '}', return NULL,
        "Invalid tree file format", NULL);
    
    return node;
}

static void print_type(node_type type, FILE *output)
{
    switch (type)
    {
    #define NODE_TYPE(name, ...) case NODE_##name: fputs(#name, output); break;
    #include "node_types.h"
    #undef NODE_TYPE
    default:
        log_message(MSG_WARNING, "Unknown 'node_type' enum value: %d", (int)type);
        return;
    }
}

static void print_op(op_type op, FILE *output)
{
    switch (op)
    {
    #define OP_TYPE(name, ...) case OP_##name: fputs(#name, output); break;
    #define CMP_TYPE(name, ...) case OP_##name: fputs(#name, output); break;    // Standard compliance
    #define LOGIC_TYPE(name, ...) case OP_##name: fputs(#name, output); break;  // Standard compliance
    #include "op_types.h"
    #include "cmp_types.h"                                                      // Standard compliance
    #include "logic_types.h"                                                    // Standard compliance
    #undef OP_TYPE
    #undef CMP_TYPE                                                             // Standard compliance
    #undef LOGIC_TYPE                                                           // Standard compliance
    default:
        log_message(MSG_WARNING, "Unknown 'op_type' enum value: %d", (int)op);
        return;
    }
}

static void print_cmp(cmp_type cmp, FILE *output)
{
    switch (cmp)
    {
    #define CMP_TYPE(name, ...) case CMP_##name: fputs(#name, output); break;
    #include "cmp_types.h"
    #undef CMP_TYPE
    default:
        log_message(MSG_WARNING, "Unknown 'cmp_type' enum value: %d", (int)cmp);
        return;
    }
}

static void print_logic(logic_type logic, FILE *output)
{
    switch (logic)
    {
    #define LOGIC_TYPE(name, ...) case LOGIC_##name: fputs(#name, output); break;
    #include "logic_types.h"
    #undef LOGIC_TYPE
    default:
        log_message(MSG_WARNING, "Unknown 'logic_type' enum value: %d", (int)logic);
        return;
    }
}

static void print_indent(int indent, FILE *output)
{
    fprintf(output, "%*s", 2*indent, "");
}

node_type read_type(FILE *input)
{
    static char BUFFER[16] = "";
    fscanf(input, " %15[A-Z]", BUFFER);
    
    #define NODE_TYPE(name, ...) if (strcmp(#name, BUFFER) == 0) return NODE_##name;
    #include "node_types.h"
    #undef NODE_TYPE

    log_message(MSG_ERROR, "Unknown node type: %s", BUFFER);
    return (node_type)-1;
}

op_type read_op(FILE *input)
{
    static char BUFFER[16] = "";
    fscanf(input, " %15[A-Z]", BUFFER);
    
    #define OP_TYPE(name, ...) if (strcmp(#name, BUFFER) == 0) return OP_##name;

    #define CMP_TYPE(name, ...) if (strcmp(#name, BUFFER) == 0) return OP_##name;   // Standard compliance
    #define LOGIC_TYPE(name, ...) if (strcmp(#name, BUFFER) == 0) return OP_##name; // Standard compliance
    #include "op_types.h"
    #include "cmp_types.h"                                                          // Standard compliance
    #include "logic_types.h"                                                        // Standard compliance
    #undef OP_TYPE
    
    #undef CMP_TYPE     // Standard compliance
    #undef LOGIC_TYPE   // Standard compliance

    log_message(MSG_ERROR, "Unknown operation type: %s", BUFFER);
    return (op_type)-1;
}

cmp_type read_cmp(FILE *input)
{
    static char BUFFER[16] = "";
    fscanf(input, " %15[A-Z]", BUFFER);
    
    #define CMP_TYPE(name, ...) if (strcmp(#name, BUFFER) == 0) return CMP_##name;
    #include "cmp_types.h"
    #undef CMP_TYPE

    log_message(MSG_ERROR, "Unknown comparison operation type: %s", BUFFER);
    return (cmp_type)-1;
}

logic_type read_logic(FILE *input)
{
    static char BUFFER[16] = "";
    fscanf(input, " %15[A-Z]", BUFFER);
    
    #define LOGIC_TYPE(name, ...) if (strcmp(#name, BUFFER) == 0) return LOGIC_##name;
    #include "logic_types.h"
    #undef LOGIC_TYPE

    log_message(MSG_ERROR, "Unknown comparison operation type: %s", BUFFER);
    return (logic_type)-1;
}
