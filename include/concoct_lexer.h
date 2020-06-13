/*
 * Concoct - An imperative, dynamically-typed, interpreted, general-purpose programming language
 * Copyright (C) 2020 BlakeTheBlock, Lloyd Dilley
 * https://github.com/FragLand/concoct
 *
 * This file is part of Concoct.
 *
 * Concoct is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version
 * 3 of the License, or (at your option) any later version.
 *
 * Concoct is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Concoct. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CONCOCT_LEXER_H
#define CONCOCT_LEXER_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOKEN_TEXT_LENGTH 32
#define STRING_TOKEN_LENGTH 512

enum ConcoctTokenType
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
  CCT_TOKEN_ERROR,
};

#define CCT_KEYWORD_COUNT 22
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
  "while"
};

static enum ConcoctTokenType cct_keyword_types[CCT_KEYWORD_COUNT] = {
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
  CCT_TOKEN_WHILE
};
struct ConcoctToken
{
  enum ConcoctTokenType type;
  char* text;
  int line_number;
};

struct ConcoctLexer
{
  FILE* input_stream;
  char next_char;
  int line_number;
  const char* error;
};
// Helper function for getting type names
const char* cct_token_type_to_string(enum ConcoctTokenType);

struct ConcoctLexer cct_new_lexer(FILE* in_stream);

void cct_set_error(struct ConcoctLexer* lexer, const char* message);

char cct_next_char(struct ConcoctLexer* lexer);
struct ConcoctToken cct_next_token(struct ConcoctLexer* lexer);

#endif /* CONCOCT_LEXER_H */