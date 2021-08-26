/*
 * Concoct - An imperative, dynamically-typed, interpreted, general-purpose programming language
 * Copyright (c) 2020-2021 BlakeTheBlock and Lloyd Dilley
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

#ifndef LEXER_H
#define LEXER_H

#include <stdio.h> // FILE

#define TOKEN_TEXT_LENGTH 1024

typedef enum concoct_token_type
{
  CCT_TOKEN_IDENTIFIER,
  CCT_TOKEN_INT,
  CCT_TOKEN_FLOAT,
  CCT_TOKEN_STRING,
  CCT_TOKEN_CHAR,
  CCT_TOKEN_ASSIGN,
  CCT_TOKEN_ADD_ASSIGN,
  CCT_TOKEN_SUB_ASSIGN,
  CCT_TOKEN_MUL_ASSIGN,
  CCT_TOKEN_DIV_ASSIGN,
  CCT_TOKEN_MOD_ASSIGN,
  CCT_TOKEN_EXP_ASSIGN,
  CCT_TOKEN_ADD,
  CCT_TOKEN_SUB,
  CCT_TOKEN_MUL,
  CCT_TOKEN_DIV,
  CCT_TOKEN_MOD,
  CCT_TOKEN_EXP,
  CCT_TOKEN_INC,
  CCT_TOKEN_DEC,
  CCT_TOKEN_EQUAL,
  CCT_TOKEN_NOT_EQUAL,
  CCT_TOKEN_STRLEN_EQUAL,
  CCT_TOKEN_STRLEN_NOT_EQUAL,
  CCT_TOKEN_GREATER,
  CCT_TOKEN_LESS,
  CCT_TOKEN_GREATER_EQUAL,
  CCT_TOKEN_LESS_EQUAL,
  CCT_TOKEN_AND,
  CCT_TOKEN_OR,
  CCT_TOKEN_NOT,
  CCT_TOKEN_BIN_AND,
  CCT_TOKEN_BIN_OR,
  CCT_TOKEN_BIN_XOR,
  CCT_TOKEN_SHL,
  CCT_TOKEN_SHR,
  CCT_TOKEN_DOT,
  CCT_TOKEN_COMMA,
  CCT_TOKEN_LEFT_BRACE,
  CCT_TOKEN_RIGHT_BRACE,
  CCT_TOKEN_LEFT_BRACKET,
  CCT_TOKEN_RIGHT_BRACKET,
  CCT_TOKEN_LEFT_PAREN,
  CCT_TOKEN_RIGHT_PAREN,
  CCT_TOKEN_NEWLINE,
  CCT_TOKEN_EOF,
  CCT_TOKEN_BREAK,
  CCT_TOKEN_CONTINUE,
  CCT_TOKEN_CASE,
  CCT_TOKEN_CLASS,
  CCT_TOKEN_DO,
  CCT_TOKEN_DEFAULT,
  CCT_TOKEN_ELSE,
  CCT_TOKEN_ENUM,
  CCT_TOKEN_FALSE,
  CCT_TOKEN_FOR,
  CCT_TOKEN_FUNC,
  CCT_TOKEN_GOTO,
  CCT_TOKEN_IF,
  CCT_TOKEN_NAMESPACE,
  CCT_TOKEN_NULL,
  CCT_TOKEN_RETURN,
  CCT_TOKEN_SWITCH,
  CCT_TOKEN_SUPER,
  CCT_TOKEN_TRUE,
  CCT_TOKEN_USE,
  CCT_TOKEN_VAR,
  CCT_TOKEN_WHILE,
  CCT_TOKEN_IN,
  CCT_TOKEN_ERROR,
} ConcoctTokenType;

#define CCT_KEYWORD_COUNT 23
static const char* cct_keywords[CCT_KEYWORD_COUNT] = {
  "break",
  "continue",
  "case",
  "class",
  "do",
  "default",
  "else",
  "enum",
  "false",
  "for",
  "func",
  "goto",
  "if",
  "namespace",
  "null",
  "return",
  "switch",
  "super",
  "true",
  "use",
  "var",
  "while",
  "in"
};

static ConcoctTokenType cct_keyword_types[CCT_KEYWORD_COUNT] = {
  CCT_TOKEN_BREAK,
  CCT_TOKEN_CONTINUE,
  CCT_TOKEN_CASE,
  CCT_TOKEN_CLASS,
  CCT_TOKEN_DO,
  CCT_TOKEN_DEFAULT,
  CCT_TOKEN_ELSE,
  CCT_TOKEN_ENUM,
  CCT_TOKEN_FALSE,
  CCT_TOKEN_FOR,
  CCT_TOKEN_FUNC,
  CCT_TOKEN_GOTO,
  CCT_TOKEN_IF,
  CCT_TOKEN_NAMESPACE,
  CCT_TOKEN_NULL,
  CCT_TOKEN_RETURN,
  CCT_TOKEN_SWITCH,
  CCT_TOKEN_SUPER,
  CCT_TOKEN_TRUE,
  CCT_TOKEN_USE,
  CCT_TOKEN_VAR,
  CCT_TOKEN_WHILE,
  CCT_TOKEN_IN
};

typedef struct concoct_token
{
  ConcoctTokenType type;
  int line_number;
} ConcoctToken;

typedef enum concoct_lexer_type
{
  CCT_LEXER_FILE,
  CCT_LEXER_STRING
} ConcoctLexerType;

typedef struct concoct_lexer
{
  ConcoctLexerType type;
  union
  {
    FILE* file_input;
    const char* string_input;
  } input;
  char next_char;
  int line_number;
  int string_index;
  char* token_text;
  const char* error;
} ConcoctLexer;

ConcoctToken cct_new_token( ConcoctTokenType type, int line_number);

// Helper function for getting type names
const char* cct_token_type_to_string(ConcoctTokenType type);

ConcoctLexer* cct_new_file_lexer(FILE* in_file);
ConcoctLexer* cct_new_string_lexer(const char* in_string);
void cct_delete_lexer(ConcoctLexer* lexer);

int cct_lexer_is_eof(ConcoctLexer* lexer);

void cct_set_error(ConcoctLexer* lexer, const char* message);

char cct_next_char(ConcoctLexer* lexer);
ConcoctToken cct_next_token(ConcoctLexer* lexer);

#endif // LEXER_H
