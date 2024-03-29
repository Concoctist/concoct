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

#ifndef LEXER_H
#define LEXER_H

#include <stdint.h>      // uint8_t
#include "char_stream.h"
#include "hash_map.h"

#define MAX_TOKEN_TEXT_LENGTH ((size_t)1024)
#define MAX_ERROR_STRING_LENGTH ((uint8_t)64)

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
  CCT_TOKEN_UNARY_MINUS,
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
  CCT_TOKEN_BIN_NOT,
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

#define CCT_KEYWORD_COUNT ((uint8_t)23)
extern const char* cct_keywords[CCT_KEYWORD_COUNT];

extern ConcoctTokenType cct_keyword_types[CCT_KEYWORD_COUNT];

typedef struct concoct_token
{
  ConcoctTokenType type;
  size_t line_number;
} ConcoctToken;

typedef struct concoct_lexer
{
  ConcoctCharStream* source;
  ConcoctHashMap* keyword_map;
  char next_char;
  size_t line_number;
  char* token_text;
  char* error;
} ConcoctLexer;

ConcoctToken cct_new_token(ConcoctTokenType type, size_t line_number);

// Helper function for getting type names
const char* cct_token_type_to_string(ConcoctTokenType type);

ConcoctLexer* cct_new_lexer(ConcoctCharStream* source);
void cct_delete_lexer(ConcoctLexer* lexer);

int cct_lexer_is_eof(const ConcoctLexer* lexer);

void cct_set_error(ConcoctLexer* lexer, const char* message);

char cct_next_char(ConcoctLexer* lexer);
ConcoctToken cct_next_token(ConcoctLexer* lexer);

#endif // LEXER_H
