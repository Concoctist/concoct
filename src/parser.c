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

#include "parser.h"

ConcoctParser* cct_new_parser_str(const char* str)
{
    return cct_new_parser(cct_new_string_lexer(str));
}

ConcoctParser* cct_new_parser(struct ConcoctLexer* lexer)
{
    ConcoctParser* parser = malloc(sizeof(ConcoctParser));
    parser->lexer = lexer;
    parser->current_token = cct_next_token(lexer);
    parser->error_line = 0;
    parser->error = NULL;
    return parser;
}

void cct_delete_parser(ConcoctParser* parser)
{
    free(parser);
    cct_delete_lexer(parser->lexer);
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
    {
        text = node->text;
    }
    for(int i = 0;i < tab_level;i++)
    {
        printf("  ");
    }
    printf("%s (%s)\n", cct_token_type_to_string(node->token.type), text);
    for(int i =0;i < node->child_count;i++)
    {
        cct_print_node(node->children[i], tab_level + 1);
    }
}

/*
Parses the program

    program
    -stat1
    -stat2
    ..
    -statN
*/
ConcoctNode* cct_parse_program(ConcoctParser* parser)
{
    // For the root of the tree, a NewLine token is used, but it can be whatever
    ConcoctNode* root = cct_new_node(cct_new_token(CCT_TOKEN_NEWLINE, 0), NULL);
    ConcoctNode* stat;
    while(parser->current_token.type != CCT_TOKEN_EOF)
    {
        stat = cct_parse_stat(parser);
        if(stat == NULL)
        {
            cct_delete_node(root);
            return NULL;
        }
        cct_node_add_child(root, stat);
    }
    return root;
}

/*
Skips new lines in the token stream.
Useful for when parts of the grammar allow lines in between tokens
*/
void cct_parser_skip_new_lines(ConcoctParser* parser)
{
    while(parser->current_token.type == CCT_TOKEN_NEWLINE)
    {
        cct_next_parser_token(parser);
    }
}

/*
Parses an expression consisting of one token

    expr
*/
ConcoctNode* cct_parse_single_expr(ConcoctParser* parser)
{
    switch (parser->current_token.type)
    {
        case CCT_TOKEN_INT:
        case CCT_TOKEN_FLOAT:
        case CCT_TOKEN_CHAR:
        case CCT_TOKEN_STRING:
        case CCT_TOKEN_TRUE:
        case CCT_TOKEN_FALSE:
        case CCT_TOKEN_IDENTIFIER:
            return cct_new_node(parser->current_token, parser->lexer->token_text);
        default:
            cct_set_parser_error(parser, "Expected an expression");
            return NULL;
    }
}

/*
Parses an expression
*/
ConcoctNode* cct_parse_expr(ConcoctParser* parser)
{
    ConcoctNode* expr = cct_parse_single_expr(parser);
    if(expr != NULL)
    {
        cct_next_parser_token(parser);
    }
    return expr;
}

/*
    Parses an if statement

    if
    -expr
    -statement
*/
ConcoctNode* cct_parse_if_stat(ConcoctParser* parser)
{
    ConcoctNode* if_stat = cct_new_node(parser->current_token, NULL);
    cct_next_parser_token(parser);

    ConcoctNode* expr = cct_parse_expr(parser);
    if(expr == NULL)
    {
        cct_delete_node(if_stat);
        return NULL;
    }
    cct_node_add_child(if_stat, expr);

    ConcoctNode* stat = cct_parse_stat(parser);
    if(stat == NULL)
    {
        cct_delete_node(if_stat);
        return NULL;
    }
    cct_node_add_child(if_stat, stat);

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
    ConcoctNode* while_stat = cct_new_node(parser->current_token, NULL);
    cct_next_parser_token(parser);

    ConcoctNode* expr = cct_parse_expr(parser);
    if(expr == NULL)
    {
        cct_delete_node(while_stat);
        return NULL;
    }
    cct_node_add_child(while_stat, expr);

    ConcoctNode* stat = cct_parse_stat(parser);
    if(stat == NULL)
    {
        cct_delete_node(while_stat);
        return NULL;
    }
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
    ConcoctNode* while_stat = cct_new_node(parser->current_token, NULL);
    cct_next_parser_token(parser);

    ConcoctNode* stat = cct_parse_stat(parser);
    if(stat == NULL)
    {
        cct_delete_node(while_stat);
        return NULL;
    }
    cct_node_add_child(while_stat, stat);

    cct_parser_skip_new_lines(parser);
    if(parser->current_token.type != CCT_TOKEN_WHILE)
    {
        cct_delete_node(while_stat);
        cct_set_parser_error(parser, "Expected 'while' keyword");
        return NULL;
    }
    cct_next_parser_token(parser);

    ConcoctNode* expr = cct_parse_expr(parser);
    if(expr == NULL)
    {
        cct_delete_node(while_stat);
        return NULL;
    }
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
    ConcoctNode* for_stat = cct_new_node(parser->current_token, NULL);
    cct_next_parser_token(parser);

    if(parser->current_token.type != CCT_TOKEN_IDENTIFIER)
    {
        cct_delete_node(for_stat);
        cct_set_parser_error(parser, "Expected an identifier");
        return NULL;
    }

    ConcoctNode* identifier = cct_new_node(parser->current_token, parser->lexer->token_text);
    cct_node_add_child(for_stat, identifier);
    cct_next_parser_token(parser);

    if(parser->current_token.type != CCT_TOKEN_IN)
    {
        cct_delete_node(for_stat);
        cct_set_parser_error(parser, "Expected the 'in' keyword");
        return NULL;
    }

    cct_next_parser_token(parser);
    ConcoctNode* expr = cct_parse_expr(parser);
    if(expr == NULL)
    {
        cct_delete_node(for_stat);
        return NULL;
    }
    cct_node_add_child(for_stat, expr);

    ConcoctNode* stat = cct_parse_stat(parser);
    if(stat == NULL)
    {
        cct_delete_node(for_stat);
        return NULL;
    }
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
    ConcoctNode* compound_stat = cct_new_node(parser->current_token, NULL);
    cct_next_parser_token(parser);
    cct_parser_skip_new_lines(parser);
    while(parser->current_token.type != CCT_TOKEN_RIGHT_BRACE)
    {
        ConcoctNode* stat = cct_parse_stat(parser);
        if(stat == NULL)
        {
            cct_delete_node(compound_stat);
            return NULL; 
        }
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
    ConcoctNode* stat = cct_new_node(parser->current_token, NULL);
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
    ConcoctNode* stat = cct_new_node(parser->current_token, NULL);
    cct_next_parser_token(parser);
    ConcoctNode* expr = cct_parse_expr(parser);
    if(expr == NULL)
    {
        cct_delete_node(stat);
        return NULL;
    }
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
    ConcoctNode* id_node = cct_new_node(parser->current_token, parser->lexer->token_text);
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
            assign_op_node = cct_new_node(parser->current_token, NULL);
            break;
        default:
            cct_delete_node(id_node);
            cct_set_parser_error(parser, "Expected an assignment");
            return NULL;
    }
    cct_node_add_child(assign_op_node, id_node);

    cct_next_parser_token(parser);
    ConcoctNode* expr = cct_parse_expr(parser);
    if(expr == NULL)
    {
        cct_delete_node(assign_op_node);
        return NULL;
    }
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
        case CCT_TOKEN_IF: return cct_parse_if_stat(parser);
        case CCT_TOKEN_WHILE: return cct_parse_while_stat(parser);
        case CCT_TOKEN_DO: return cct_parse_do_while_stat(parser);
        case CCT_TOKEN_FOR: return cct_parse_for_stat(parser);
        case CCT_TOKEN_LEFT_BRACE: return cct_parse_compound_stat(parser);
        case CCT_TOKEN_BREAK:
        case CCT_TOKEN_CONTINUE: return cct_parse_one_word_stat(parser);
        case CCT_TOKEN_RETURN: return cct_parse_return(parser);
        case CCT_TOKEN_IDENTIFIER: return cct_parse_assign(parser);
        default:
            cct_set_parser_error(parser, "Expected a statement");
            return NULL;
    }
}

/*
Advances to the next token
*/
struct ConcoctToken cct_next_parser_token(ConcoctParser* parser)
{
    parser->current_token = cct_next_token(parser->lexer);
    return parser->current_token;
}

/*
ConcoctNode constructor
*/
ConcoctNode* cct_new_node(struct ConcoctToken token, const char* text)
{
    ConcoctNode* node = malloc(sizeof(ConcoctNode));
    node->child_count = 0;
    node->children = NULL;
    node->parent = NULL;
    node->token = token;
    node->text = NULL;
    if(text != NULL)
    {
        node->text = malloc(strlen(text)+1);
        strcpy(node->text, text);
    }
    return node;
}

/*
Frees the node and all it's children from memory
*/
void cct_delete_node(ConcoctNode* node)
{
    // Frees this node and it's children from memory
    for(int i = 0;i < node->child_count;i++)
    {
        cct_delete_node(node->children[i]);
    }
    if(node->text != NULL)
    {
        free(node->text);
    }
    free(node);
}

/*
Adds a child to a node, also setting the child's parent
*/
ConcoctNode* cct_node_add_child(ConcoctNode* node, ConcoctNode* child)
{
    node->child_count++;
    ConcoctNode** new_children_mem = malloc(sizeof(ConcoctNode*) * node->child_count);
    if(node->children != NULL)
    {
        memcpy(new_children_mem, node->children, sizeof(ConcoctNode*) * (node->child_count - 1));
        free(node->children);
    }
    node->children = new_children_mem;
    child->parent = node;
    ConcoctNode** new_child_mem = node->children + (node->child_count - 1);
    *new_child_mem = child;
    return child;
}