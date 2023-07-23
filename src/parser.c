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

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

ConcoctParser* cct_new_parser_str(const char* str)
{
  return cct_new_parser(cct_new_string_lexer(str));
}

ConcoctParser* cct_new_parser(ConcoctLexer* lexer)
{
  ConcoctParser* parser = malloc(sizeof(ConcoctParser));

  if(lexer == NULL)
  {
    fprintf(stderr, "Error allocating memory for lexer: %s\n", strerror(errno));
    return NULL;
  }

  parser->lexer = lexer;
  parser->current_token = cct_next_token(lexer);
  parser->error_line = 0;
  parser->error = NULL;
  return parser;
}

void cct_delete_parser(ConcoctParser* parser)
{
  cct_delete_lexer(parser->lexer);
  if(parser != NULL)
    free(parser);
}

void cct_set_parser_error(ConcoctParser* parser, const char* text)
{
  parser->error = text;
  parser->error_line = parser->current_token.line_number;
}

/*
Prints a node and it's children, using indents to show relationship
*/
void cct_print_node(ConcoctNode* node, int tab_level)
{
  const char* text = "";

  if(node->text != NULL)
    text = node->text;

  for(int i = 0; i < tab_level; i++)
    printf("  ");
  printf("%s (%s)\n", cct_token_type_to_string(node->token.type), text);

  for(int i = 0; i < node->child_count; i++)
    cct_print_node(node->children[i], tab_level + 1);
}

/*
Parses the program

  program
  -stat1
  -stat2
  ..
  -statN
*/
ConcoctNodeTree* cct_parse_program(ConcoctParser* parser)
{
  ConcoctNode* stat;
  ConcoctNodeTree* tree = malloc(sizeof(ConcoctNodeTree));

  if(tree == NULL)
  {
    fprintf(stderr, "Error allocating memory for node tree: %s\n", strerror(errno));
    return NULL;
  }

  tree->node_count = 0;
  tree->node_max = 256;
  tree->nodes = malloc(tree->node_max * sizeof(ConcoctNode*));

  if(tree->nodes == NULL)
  {
    fprintf(stderr, "Error allocating memory for tree nodes: %s\n", strerror(errno));
    return NULL;
  }

  // For the root of the tree, a NewLine token is used, but it can be whatever
  tree->root = cct_new_node(tree, cct_new_token(CCT_TOKEN_NEWLINE, 0), NULL);
  parser->tree = tree;

  while(parser->current_token.type != CCT_TOKEN_EOF)
  {
    stat = cct_parse_stat(parser);
    if(stat == NULL)
      return tree;
    if(tree->root != NULL)
      cct_node_add_child(tree->root, stat);
  }
  return tree;
}

/*
Skips new lines in the token stream.
Useful for when parts of the grammar allow lines in between tokens
*/
void cct_parser_skip_new_lines(ConcoctParser* parser)
{
  while(parser->current_token.type == CCT_TOKEN_NEWLINE)
    cct_next_parser_token(parser);
}

/*
Parses an expression consisting of one token

  expr
*/
ConcoctNode* cct_parse_single_expr(ConcoctParser* parser)
{
  ConcoctNode* node;

  switch (parser->current_token.type)
  {
    case CCT_TOKEN_INT:
    case CCT_TOKEN_FLOAT:
    case CCT_TOKEN_CHAR:
    case CCT_TOKEN_STRING:
    case CCT_TOKEN_TRUE:
    case CCT_TOKEN_FALSE:
    case CCT_TOKEN_NULL:
    case CCT_TOKEN_IDENTIFIER:
      node = cct_new_node(parser->tree, parser->current_token, parser->lexer->token_text);
      if(node == NULL)
      {
        fprintf(stderr, "Single expression node is NULL!\n");
        return NULL;
      }
      cct_next_parser_token(parser);
      return node;
    case CCT_TOKEN_LEFT_PAREN:
      cct_next_parser_token(parser);
      ConcoctNode* inside_expr = cct_parse_expr(parser);
      if(!inside_expr)
      {
        return NULL;
      }
      if(parser->current_token.type != CCT_TOKEN_RIGHT_PAREN)
      {
        cct_set_parser_error(parser, "Expected ')'");
        return NULL;
      }
      cct_next_parser_token(parser);
      return inside_expr;
    default:
      cct_set_parser_error(parser, "Expected an expression");
      return NULL;
  }
}

ConcoctNode* cct_parse_primary_expr(ConcoctParser* parser)
{
  ConcoctNode* current_node = cct_parse_single_expr(parser);
  int parsing = 1;

  while(parsing)
  {
    ConcoctNode* op_node;
    switch (parser->current_token.type)
    {
      case CCT_TOKEN_DOT:
        op_node = cct_new_node(parser->tree, parser->current_token, NULL);
        if(current_node == NULL)
        {
          fprintf(stderr, "Primary expression node is NULL!\n");
          return NULL;
        }
        cct_node_add_child(op_node, current_node);
        cct_next_parser_token(parser);
        ConcoctNode* second_expr = cct_parse_single_expr(parser);
        if(!second_expr)
          return NULL;
        cct_node_add_child(op_node, second_expr);
        current_node = op_node;
        break;
      default:
        parsing = 0;
        break;
    }
  }
  return current_node;
}
ConcoctNode* cct_parse_unary_expr(ConcoctParser* parser)
{
  ConcoctNode* op_node;

  switch (parser->current_token.type)
  {
    case CCT_TOKEN_ADD:
    case CCT_TOKEN_SUB:
    case CCT_TOKEN_NOT:
    case CCT_TOKEN_INC:
    case CCT_TOKEN_DEC:
      op_node = cct_new_node(parser->tree, parser->current_token, NULL);
      if(op_node == NULL)
      {
        fprintf(stderr, "Unary expression node is NULL!\n");
        return NULL;
      }
      cct_next_parser_token(parser);
      ConcoctNode* expr = cct_parse_primary_expr(parser);
      if(expr == NULL)
        return NULL;
      cct_node_add_child(op_node, expr);
      return op_node;
    default:
      return cct_parse_primary_expr(parser);
  }
}
ConcoctNode* cct_parse_mult_expr(ConcoctParser* parser)
{
  ConcoctNode* current_node = cct_parse_unary_expr(parser);
  int parsing = 1;

  if(current_node == NULL)
  {
    fprintf(stderr, "Expression node is NULL!\n");
    return NULL;
  }

  while(parsing)
  {
    ConcoctNode* op_node;
    switch (parser->current_token.type)
    {
      case CCT_TOKEN_MUL:
      case CCT_TOKEN_DIV:
      case CCT_TOKEN_MOD:
      case CCT_TOKEN_EXP:
        op_node = cct_new_node(parser->tree, parser->current_token, NULL);
        cct_node_add_child(op_node, current_node);
        cct_next_parser_token(parser);
        ConcoctNode* second_expr = cct_parse_unary_expr(parser);
        if(!second_expr)
          return NULL;
        cct_node_add_child(op_node, second_expr);
        current_node = op_node;
        break;
      default:
        parsing = 0;
        break;
    }
  }
  return current_node;
}

ConcoctNode* cct_parse_additive_expr(ConcoctParser* parser)
{
  ConcoctNode* current_node = cct_parse_mult_expr(parser);
  int parsing = 1;

  if(current_node == NULL)
  {
    fprintf(stderr, "Expression node is NULL!\n");
    return NULL;
  }

  while(parsing)
  {
    ConcoctNode* op_node;
    switch (parser->current_token.type)
    {
      case CCT_TOKEN_ADD:
      case CCT_TOKEN_SUB:
        op_node = cct_new_node(parser->tree, parser->current_token, NULL);
        cct_node_add_child(op_node, current_node);
        cct_next_parser_token(parser);
        ConcoctNode* second_expr = cct_parse_mult_expr(parser);
        if(!second_expr)
          return NULL;
        cct_node_add_child(op_node, second_expr);
        current_node = op_node;
        break;
      default:
        parsing = 0;
        break;
    }
  }
  return current_node;
}

ConcoctNode* cct_parse_shift_expr(ConcoctParser* parser)
{
  ConcoctNode* current_node = cct_parse_additive_expr(parser);
  int parsing = 1;

  if(current_node == NULL)
  {
    fprintf(stderr, "Expression node is NULL!\n");
    return NULL;
  }

  while(parsing)
  {
    ConcoctNode* op_node;
    switch (parser->current_token.type)
    {
      case CCT_TOKEN_SHL:
      case CCT_TOKEN_SHR:
        op_node = cct_new_node(parser->tree, parser->current_token, NULL);
        cct_node_add_child(op_node, current_node);
        cct_next_parser_token(parser);
        ConcoctNode* second_expr = cct_parse_additive_expr(parser);
        if(!second_expr)
          return NULL;
        cct_node_add_child(op_node, second_expr);
        current_node = op_node;
        break;
      default:
        parsing = 0;
        break;
    }
  }
  return current_node;
}

ConcoctNode* cct_parse_relational_expr(ConcoctParser* parser)
{
  ConcoctNode* current_node = cct_parse_shift_expr(parser);
  int parsing = 1;

  if(current_node == NULL)
  {
    fprintf(stderr, "Current relational expression node is NULL!\n");
    return NULL;
  }

  while(parsing)
  {
    ConcoctNode* op_node;
    switch (parser->current_token.type)
    {
      case CCT_TOKEN_LESS:
      case CCT_TOKEN_LESS_EQUAL:
      case CCT_TOKEN_GREATER:
      case CCT_TOKEN_GREATER_EQUAL:
        op_node = cct_new_node(parser->tree, parser->current_token, NULL);
        if(op_node == NULL)
        {
          fprintf(stderr, "Relational expression node is NULL!\n");
          return NULL;
        }
        cct_node_add_child(op_node, current_node);
        cct_next_parser_token(parser);
        ConcoctNode* second_expr = cct_parse_shift_expr(parser);
        if(!second_expr)
          return NULL;
        cct_node_add_child(op_node, second_expr);
        current_node = op_node;
        break;
      default:
        parsing = 0;
        break;
    }
  }
  return current_node;
}

ConcoctNode* cct_parse_equality_expr(ConcoctParser* parser)
{
  ConcoctNode* current_node = cct_parse_relational_expr(parser);
  int parsing = 1;

  if(current_node == NULL)
  {
    fprintf(stderr, "Current equality expression node is NULL!\n");
    return NULL;
  }

  while(parsing)
  {
    ConcoctNode* op_node;
    switch (parser->current_token.type)
    {
      case CCT_TOKEN_EQUAL:
      case CCT_TOKEN_NOT_EQUAL:
      case CCT_TOKEN_STRLEN_EQUAL:
      case CCT_TOKEN_STRLEN_NOT_EQUAL:
        op_node = cct_new_node(parser->tree, parser->current_token, NULL);
        if(op_node == NULL)
        {
          fprintf(stderr, "Equality expression node is NULL!\n");
          return NULL;
        }
        cct_node_add_child(op_node, current_node);
        cct_next_parser_token(parser);
        ConcoctNode* second_expr = cct_parse_relational_expr(parser);
        if(!second_expr)
          return NULL;
        cct_node_add_child(op_node, second_expr);
        current_node = op_node;
        break;
      default:
        parsing = 0;
        break;
    }
  }
  return current_node;
}

ConcoctNode* cct_parse_bit_and_expr(ConcoctParser* parser)
{
  ConcoctNode* current_node = cct_parse_equality_expr(parser);
  int parsing = 1;

  if(current_node == NULL)
  {
    fprintf(stderr, "Current bitwise AND expression node is NULL!\n");
    return NULL;
  }

  while(parsing)
  {
    ConcoctNode* op_node;
    switch (parser->current_token.type)
    {
      case CCT_TOKEN_BIN_AND:
        op_node = cct_new_node(parser->tree, parser->current_token, NULL);
        if(op_node == NULL)
        {
          fprintf(stderr, "Bitwise AND expression node is NULL!\n");
          return NULL;
        }
        cct_node_add_child(op_node, current_node);
        cct_next_parser_token(parser);
        ConcoctNode* second_expr = cct_parse_equality_expr(parser);
        if(!second_expr)
          return NULL;
        cct_node_add_child(op_node, second_expr);
        current_node = op_node;
        break;
      default:
        parsing = 0;
        break;
    }
  }
  return current_node;
}

ConcoctNode* cct_parse_bit_xor_expr(ConcoctParser* parser)
{
  ConcoctNode* current_node = cct_parse_bit_and_expr(parser);
  int parsing = 1;

  if(current_node == NULL)
  {
    fprintf(stderr, "Current bitwise XOR expression node is NULL!\n");
    return NULL;
  }

  while(parsing)
  {
    ConcoctNode* op_node;
    switch (parser->current_token.type)
    {
      case CCT_TOKEN_BIN_XOR:
        op_node = cct_new_node(parser->tree, parser->current_token, NULL);
        if(op_node == NULL)
        {
          fprintf(stderr, "Bitwise XOR expression node is NULL!\n");
          return NULL;
        }
        cct_node_add_child(op_node, current_node);
        cct_next_parser_token(parser);
        ConcoctNode* second_expr = cct_parse_bit_and_expr(parser);
        if(!second_expr)
          return NULL;
        cct_node_add_child(op_node, second_expr);
        current_node = op_node;
        break;
      default:
        parsing = 0;
        break;
    }
  }
  return current_node;
}

ConcoctNode* cct_parse_bit_or_expr(ConcoctParser* parser)
{
  ConcoctNode* current_node = cct_parse_bit_xor_expr(parser);
  int parsing = 1;

  if(current_node == NULL)
  {
    fprintf(stderr, "Current bitwise OR expression node is NULL!\n");
    return NULL;
  }

  while(parsing)
  {
    ConcoctNode* op_node;
    switch (parser->current_token.type)
    {
      case CCT_TOKEN_BIN_OR:
        op_node = cct_new_node(parser->tree, parser->current_token, NULL);
        if(op_node == NULL)
        {
          fprintf(stderr, "Bitwise OR expression node is NULL!\n");
          return NULL;
        }
        cct_node_add_child(op_node, current_node);
        cct_next_parser_token(parser);
        ConcoctNode* second_expr = cct_parse_bit_xor_expr(parser);
        if(!second_expr)
          return NULL;
        cct_node_add_child(op_node, second_expr);
        current_node = op_node;
        break;
      default:
        parsing = 0;
        break;
    }
  }
  return current_node;
}

ConcoctNode* cct_parse_and_expr(ConcoctParser* parser)
{
  ConcoctNode* current_node = cct_parse_bit_or_expr(parser);
  int parsing = 1;

  if(current_node == NULL)
  {
    fprintf(stderr, "Expression node is NULL!\n");
    return NULL;
  }

  while(parsing)
  {
    ConcoctNode* op_node;
    switch (parser->current_token.type)
    {
      case CCT_TOKEN_AND:
        op_node = cct_new_node(parser->tree, parser->current_token, NULL);
        cct_node_add_child(op_node, current_node);
        cct_next_parser_token(parser);
        ConcoctNode* second_expr = cct_parse_bit_or_expr(parser);
        if(!second_expr)
          return NULL;
        cct_node_add_child(op_node, second_expr);
        current_node = op_node;
        break;
      default:
        parsing = 0;
        break;
    }
  }
  return current_node;
}

ConcoctNode* cct_parse_or_expr(ConcoctParser* parser)
{
  ConcoctNode* current_node = cct_parse_and_expr(parser);
  int parsing = 1;

  if(current_node == NULL)
  {
    fprintf(stderr, "Current OR expression node is NULL!\n");
    return NULL;
  }

  while(parsing)
  {
    ConcoctNode* op_node;
    switch (parser->current_token.type)
    {
      case CCT_TOKEN_OR:
        op_node = cct_new_node(parser->tree, parser->current_token, NULL);
        if(op_node == NULL)
        {
          fprintf(stderr, "OR expression node is NULL!\n");
          return NULL;
        }
        cct_node_add_child(op_node, current_node);
        cct_next_parser_token(parser);
        ConcoctNode* second_expr = cct_parse_and_expr(parser);
        if(!second_expr)
          return NULL;
        cct_node_add_child(op_node, second_expr);
        current_node = op_node;
        break;
      default:
        parsing = 0;
        break;
    }
  }
  return current_node;
}

/*
Parses an expression
*/
ConcoctNode* cct_parse_expr(ConcoctParser* parser)
{
  ConcoctNode* expr = cct_parse_or_expr(parser);
  return expr;
}

/*
Parses an if statement

  if
  -expr
  -statement
  -[else statement]
*/
ConcoctNode* cct_parse_if_stat(ConcoctParser* parser)
{
  ConcoctNode* if_stat = cct_new_node(parser->tree, parser->current_token, NULL);
  cct_next_parser_token(parser);

  ConcoctNode* expr = cct_parse_expr(parser);
  if(expr == NULL)
    return NULL;
  cct_node_add_child(if_stat, expr);

  ConcoctNode* stat = cct_parse_stat(parser);
  if(stat == NULL)
    return NULL;
  cct_node_add_child(if_stat, stat);

  if(parser->current_token.type == CCT_TOKEN_ELSE)
  {
    cct_next_parser_token(parser);
    ConcoctNode* else_stat = cct_parse_stat(parser);
    if(else_stat == NULL)
      return NULL;
    cct_node_add_child(if_stat, else_stat);
  }
  return if_stat;
}

/*
Parses a while statement

  while
  -expr
  -statement
*/
ConcoctNode* cct_parse_while_stat(ConcoctParser* parser)
{
  ConcoctNode* while_stat = cct_new_node(parser->tree, parser->current_token, NULL);
  cct_next_parser_token(parser);

  ConcoctNode* expr = cct_parse_expr(parser);
  if(expr == NULL)
    return NULL;
  cct_node_add_child(while_stat, expr);

  ConcoctNode* stat = cct_parse_stat(parser);
  if(stat == NULL)
    return NULL;
  cct_node_add_child(while_stat, stat);

  return while_stat;
}

/*
Parses a do-while statement

  do while
  -statement
  -expr
*/
ConcoctNode* cct_parse_do_while_stat(ConcoctParser* parser)
{
  ConcoctNode* while_stat = cct_new_node(parser->tree, parser->current_token, NULL);
  if(while_stat == NULL)
  {
    fprintf(stderr, "Do...while statement node is NULL!\n");
    return NULL;
  }
  cct_next_parser_token(parser);

  ConcoctNode* stat = cct_parse_stat(parser);
  if(stat == NULL)
    return NULL;
  cct_node_add_child(while_stat, stat);

  cct_parser_skip_new_lines(parser);
  if(parser->current_token.type != CCT_TOKEN_WHILE)
  {
    cct_set_parser_error(parser, "Expected 'while' keyword");
    return NULL;
  }
  cct_next_parser_token(parser);

  ConcoctNode* expr = cct_parse_expr(parser);
  if(expr == NULL)
    return NULL;
  cct_node_add_child(while_stat, expr);

  return while_stat;
}

/*
Parses a for statement

  for
  -identifier
  -expr
  -statement
*/
ConcoctNode* cct_parse_for_stat(ConcoctParser* parser)
{
  ConcoctNode* for_stat = cct_new_node(parser->tree, parser->current_token, NULL);
  if(for_stat == NULL)
  {
    fprintf(stderr, "For statement node is NULL!\n");
    return NULL;
  }
  cct_next_parser_token(parser);

  if(parser->current_token.type != CCT_TOKEN_IDENTIFIER)
  {
    cct_set_parser_error(parser, "Expected an identifier");
    return NULL;
  }

  ConcoctNode* identifier = cct_new_node(parser->tree, parser->current_token, parser->lexer->token_text);
  if(identifier == NULL)
  {
    fprintf(stderr, "For statement identifier node is NULL!\n");
    return NULL;
  }
  cct_node_add_child(for_stat, identifier);
  cct_next_parser_token(parser);

  if(parser->current_token.type != CCT_TOKEN_IN)
  {
    cct_set_parser_error(parser, "Expected the 'in' keyword");
    return NULL;
  }

  cct_next_parser_token(parser);
  ConcoctNode* expr = cct_parse_expr(parser);
  if(expr == NULL)
    return NULL;
  cct_node_add_child(for_stat, expr);

  ConcoctNode* stat = cct_parse_stat(parser);
  if(stat == NULL)
    return NULL;
  cct_node_add_child(for_stat, stat);

  return for_stat;
}

/*
Parses a compound statement

  compound stat
  -stat1
  -stat2
  ..
  -statN
*/
ConcoctNode* cct_parse_compound_stat(ConcoctParser* parser)
{
  ConcoctNode* compound_stat = cct_new_node(parser->tree, parser->current_token, NULL);

  if(compound_stat == NULL)
  {
    fprintf(stderr, "Compound statement node is NULL!\n");
    return NULL;
  }

  cct_next_parser_token(parser);
  cct_parser_skip_new_lines(parser);
  while(parser->current_token.type != CCT_TOKEN_RIGHT_BRACE)
  {
    ConcoctNode* stat = cct_parse_stat(parser);
    if(stat == NULL)
      return NULL;
    cct_node_add_child(compound_stat, stat);
    cct_parser_skip_new_lines(parser);
  }
  cct_next_parser_token(parser);
  return compound_stat;
}

/*
Parses 'break' or 'continue'

    stat
*/
ConcoctNode* cct_parse_one_word_stat(ConcoctParser* parser)
{
  ConcoctNode* stat = cct_new_node(parser->tree, parser->current_token, NULL);
  cct_next_parser_token(parser);
  return stat;
}

/*
Parses a return statement

  return
  - expr
*/
ConcoctNode* cct_parse_return(ConcoctParser* parser)
{
  ConcoctNode* stat = cct_new_node(parser->tree, parser->current_token, NULL);

  cct_next_parser_token(parser);
  ConcoctNode* expr = cct_parse_expr(parser);
  if(expr == NULL)
    return NULL;
  cct_node_add_child(stat, expr);
  return stat;
}

/*
Parses an assign statement. Note that the node has the assign token as the root,
with the identifier and expression as children

  assign
  -identifier
  -expr
*/
ConcoctNode* cct_parse_assign(ConcoctParser* parser)
{
  ConcoctNode* id_node = cct_new_node(parser->tree, parser->current_token, parser->lexer->token_text);
  cct_next_parser_token(parser);
  ConcoctNode* assign_op_node;

  switch (parser->current_token.type)
  {
    case CCT_TOKEN_ASSIGN:
    case CCT_TOKEN_ADD_ASSIGN:
    case CCT_TOKEN_SUB_ASSIGN:
    case CCT_TOKEN_MUL_ASSIGN:
    case CCT_TOKEN_DIV_ASSIGN:
    case CCT_TOKEN_MOD_ASSIGN:
    case CCT_TOKEN_EXP_ASSIGN:
      assign_op_node = cct_new_node(parser->tree, parser->current_token, NULL);
      break;
    default:
      cct_set_parser_error(parser, "Expected an assignment");
      return NULL;
  }
  cct_node_add_child(assign_op_node, id_node);

  cct_next_parser_token(parser);
  ConcoctNode* expr = cct_parse_expr(parser);
  if(expr == NULL)
    return NULL;
  cct_node_add_child(assign_op_node, expr);
  return assign_op_node;
}

/*
Parses one of many types of statements, or NULL in case of error
*/
ConcoctNode* cct_parse_stat(ConcoctParser* parser)
{
  cct_parser_skip_new_lines(parser);
  // Tests the current token and chooses the associated parsing function
  switch (parser->current_token.type)
  {
    case CCT_TOKEN_IF:         return cct_parse_if_stat(parser);
    case CCT_TOKEN_WHILE:      return cct_parse_while_stat(parser);
    case CCT_TOKEN_DO:         return cct_parse_do_while_stat(parser);
    case CCT_TOKEN_FOR:        return cct_parse_for_stat(parser);
    case CCT_TOKEN_LEFT_BRACE: return cct_parse_compound_stat(parser);
    case CCT_TOKEN_BREAK:
    case CCT_TOKEN_CONTINUE:   return cct_parse_one_word_stat(parser);
    case CCT_TOKEN_RETURN:     return cct_parse_return(parser);
    case CCT_TOKEN_IDENTIFIER: return cct_parse_assign(parser);
    default:
      cct_set_parser_error(parser, "Expected a statement");
      return NULL;
  }
}

/*
Advances to the next token
*/
ConcoctToken cct_next_parser_token(ConcoctParser* parser)
{
  parser->current_token = cct_next_token(parser->lexer);
  return parser->current_token;
}

/*
ConcoctNode constructor
*/
ConcoctNode* cct_new_node(ConcoctNodeTree* tree, ConcoctToken token, const char* text)
{
  // Constructs a new node and registers it with the tree
  if(tree->node_count == tree->node_max)
  {
    tree->node_max *= 2;
    tree->nodes = realloc(tree->nodes, tree->node_max * sizeof(ConcoctNode*));
    if(tree->nodes == NULL)
    {
      fprintf(stderr, "Error reallocating memory for tree nodes: %s\n", strerror(errno));
      return NULL;
    }
  }
  ConcoctNode* node = malloc(sizeof(ConcoctNode));
  if(node == NULL)
  {
    fprintf(stderr, "Error allocating memory for node: %s\n", strerror(errno));
    return NULL;
  }
  node->child_count = 0;
  node->children = NULL;
  node->parent = NULL;
  node->token = token;
  node->text = NULL;
  if(text != NULL)
  {
    node->text = malloc(strlen(text) + 1);
    if(node->text == NULL)
    {
      fprintf(stderr, "Error allocating memory for node text: %s\n", strerror(errno));
      return NULL;
    }
    strcpy(node->text, text);
  }
  tree->nodes[tree->node_count] = node;
  tree->node_count += 1;
  return node;
}

void cct_delete_node_tree(ConcoctNodeTree* tree)
{
  for(int i = 0; i < tree->node_count; i++)
  {
    ConcoctNode* node = tree->nodes[i];
    free(node->text);
    free(node->children);
    free(node);
  }
  free(tree->nodes);
  free(tree);
}

/*
Adds a child to a node, also setting the child's parent
*/
ConcoctNode* cct_node_add_child(ConcoctNode* node, ConcoctNode* child)
{
  node->child_count++;
  node->children = realloc(node->children, sizeof(ConcoctNode*) * node->child_count);
  if(node->children == NULL)
  {
    fprintf(stderr, "Error reallocating memory for node children: %s\n", strerror(errno));
    return NULL;
  }
  child->parent = node;
  node->children[node->child_count - 1] = child;
  return child;
}
