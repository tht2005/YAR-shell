/**
 * @file
 * @brief Lexer of yar script
 */

#ifndef __YAR_LEXER
#define __YAR_LEXER

#include "yar_token.h"
#include "yar_literal.h"
#include "data_structure/list_head.h"
#include "data_structure/string.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    list_head link_node;
    token tok;
} token_list;

void free_token_list (list_head *head);

#endif

