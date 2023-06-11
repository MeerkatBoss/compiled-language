#ifndef FRONT_UTILS
#define FRONT_UTILS

#include "data_structures/ast/ast.h"
#include "data_structures/token_list/token_list.h"

const char FRONT_DEFAULT_OUTPUT[] = "out.ast";

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
 * @return `true` upon successful write to file, `false` otherwise
 */
bool save_tree_to_file(const abstract_syntax_tree* tree, const char* filename);

/**
 * @brief Read AST from given file
 * @param[in] filename Path to input file
 * @param[out] tree Parsed tree
 * @return `true` upon successful read from file, `false` otherwise
 */
bool read_tree_from_file(const char* filename, abstract_syntax_tree* tree);

/**
 * @brief Restore array of lexemes from syntax tree
 * @param[in] tree AST
 * @param[out] tokens Restored lexemes
 * @param[in] print    `true` if lexemes will need to be printed, `false` otherwise
 * @return `true` if all parameters were valid, `false` otherwise
 */
bool get_lexemes_from_tree(const abstract_syntax_tree* tree, dynamic_array(token)* tokens, bool print = false);

/**
 * @brief Restore source file from array of lexemes
 * @param[in] tokens Array of lexemes
 * @param[inout] filename Path to output file
 * @return `true` upon successful write to file, `false` otherwise
 */
bool make_source_file(const dynamic_array(token)* tokens, const char* filename);

#endif