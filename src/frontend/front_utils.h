#ifndef FRONT_UTILS
#define FRONT_UTILS

#include "ast.h"
#include "token_list.h"

const char DEFAULT_OUTPUT[] = "out.ast";

/**
 * @brief Fill array with lexemes from file
 * @param[in] filename Input file path
 * @param[out] tokens  Filled array
 * @param[in] print    `true` if lexemes will need to be printed, `false` otherwise
 * @return `true` upon successful lexical analysis, `false` otherwise
 */
bool get_lexemes_from_file(const char* filename, dynamic_array(token)* tokens, bool print = false);

/**
 * @brief Build syntax tree from given lexemes
 * @param[in] tokens Array of lexemes
 * @param[out] tree  AST
 * @return `true` upon successful syntax analysis, `false` otherwise
 */
bool get_tree_from_lexemes(const dynamic_array(token)* tokens, abstract_syntax_tree* tree);

/**
 * @brief Print AST to given file
 * @param[in] tree Program syntax tree
 * @param[out] filename 
 * @return 
 */
bool save_tree_to_file(const abstract_syntax_tree* tree, const char* filename);

#endif