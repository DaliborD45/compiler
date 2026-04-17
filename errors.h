/**
 * @file errors.h
 * @brief Error codes returned by the compiler
 */
#ifndef COMPILER_ERRORS_H
#define COMPILER_ERRORS_H

/**
 * ERROR CODE RETURNED BY THE FUNCS
 */
#define OK 0                                                    // 0
#define LEXER_ERR 1                                             // 1
#define PARSER_SYNTAX_ERR 2                                     // 2
#define SEMANTIC_ERR_UNDEFINED_VAR_FUNC 3                       // 3
#define SEMANTIC_ERR_INVALID_FUNC_PARAM_COUNT_AND_RETURN_TYPE 4 // 4
#define SEMANTIC_ERR_VAR_FUNC_REDEFINITION 5                    // 5 redefinice proměnné nebo funkce; přiřazení do nemodifikovatelné proměnné
#define SEMANTIC_ERR_IN_RETURN 6                                // 6 chybějící/přebývající výraz v příkazu návratu z funkce
#define SEMANTIC_ERR_TYPE_COMPATIBILITY 7                       // 7 chyba typové kompatibility v aritmetických, řetězcových a relačních výrazech; nekompatibilní typ výrazu
#define SEMANTIC_ERR_TYPE_INFERENCE 8                           // 8
#define SEMANTIC_ERR_UNUSED_VAR 9                               // 9
#define SEMANTIC_ERR_OTHER 10                                   // 10
#define INTERNAL_ERR 99                                         // 99
#define SKIP_PARSE 100                                          // 100


#endif //COMPILER_ERRORS_H
