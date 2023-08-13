/*
 * Concoct - An imperative, dynamically-typed, interpreted, general-purpose programming language
 * Copyright (c) 2020-2023 BlakeTheBlock and Lloyd Dilley
 * http://concoct.ist/
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

#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

static const size_t CCT_NODE_COUNT_PER_BLOCK = 256;

typedef struct ConcoctNode
{
  ConcoctToken token;
  char* text;
  struct ConcoctNode* parent;
  int child_count;
  struct ConcoctNode** children;
} ConcoctNode;

typedef struct concoct_node_tree
{
  ConcoctNode** nodes;
  int node_count;
  int node_max;
  ConcoctNode* root;
} ConcoctNodeTree;

typedef struct concoct_parser
{
  ConcoctLexer* lexer;
  ConcoctNodeTree* tree;
  ConcoctToken current_token;
  int error_line;
  const char* error;
} ConcoctParser;

ConcoctNode* cct_new_node(ConcoctNodeTree* tree, ConcoctToken token, const char* text);
ConcoctParser* cct_new_parser(ConcoctLexer* lexer);
void cct_delete_parser(ConcoctParser* parser);
void cct_delete_node_tree(ConcoctNodeTree* tree);
ConcoctNode* cct_node_add_child(ConcoctNode* node, ConcoctNode* child);
ConcoctNodeTree* cct_parse_program(ConcoctParser* parser);
ConcoctNode* cct_parse_stat(ConcoctParser* parser);
ConcoctNode* cct_parse_expr(ConcoctParser* parser);
ConcoctToken cct_next_parser_token(ConcoctParser* parser);
void cct_print_node(const ConcoctNode* node, int tab_level);

#endif // PARSER_H
