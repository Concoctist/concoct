/*
 * Concoct - An imperative, dynamically-typed, interpreted, general-purpose programming language
 * Copyright (c) 2020 BlakeTheBlock and Lloyd Dilley
 * http://concoct.dev/
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef CONCOCT_PARSER_H
#define CONCOCT_PARSER_H

#include "lexer.h"

typedef struct ConcoctNode
{
    struct ConcoctToken token;
    char* text;
    struct ConcoctNode* parent;
    int child_count;
    struct ConcoctNode** children;
} ConcoctNode;

typedef struct concoct_parser
{
    struct ConcoctLexer* lexer;
    struct ConcoctToken current_token;
    int error_line;
    const char* error;
} ConcoctParser;

ConcoctNode* cct_new_node(struct ConcoctToken token, const char* text);
void cct_delete_node(ConcoctNode* node);

ConcoctParser* cct_new_parser(struct ConcoctLexer* lexer);
ConcoctParser* cct_new_parser_str(const char* str);
void cct_delete_parser(ConcoctParser* parser);

ConcoctNode* cct_node_add_child(ConcoctNode* node, ConcoctNode* child);

ConcoctNode* cct_parse_program(ConcoctParser* parser);
ConcoctNode* cct_parse_stat(ConcoctParser* parser);
struct ConcoctToken cct_next_parser_token(ConcoctParser* parser);

void cct_print_node(ConcoctNode* node, int tab_level);
#endif /* CONCOCT_PARSER_H */