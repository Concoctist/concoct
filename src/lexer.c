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

#include <ctype.h>
#include <errno.h>
#include <stdbool.h> // true
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

void cct_init_lexer_keyword_map(ConcoctHashMap* map)
{
  cct_hash_map_set(map, "break", (void*)CCT_TOKEN_BREAK);
  cct_hash_map_set(map, "continue", (void*)CCT_TOKEN_CONTINUE);
  cct_hash_map_set(map, "case", (void*)CCT_TOKEN_CASE);
  cct_hash_map_set(map, "class", (void*)CCT_TOKEN_CLASS);
  cct_hash_map_set(map, "do", (void*)CCT_TOKEN_DO);
  cct_hash_map_set(map, "default", (void*)CCT_TOKEN_DEFAULT);
  cct_hash_map_set(map, "else", (void*)CCT_TOKEN_ELSE);
  cct_hash_map_set(map, "enum", (void*)CCT_TOKEN_ENUM);
  cct_hash_map_set(map, "false", (void*)CCT_TOKEN_FALSE);
  cct_hash_map_set(map, "for", (void*)CCT_TOKEN_FOR);
  cct_hash_map_set(map, "func", (void*)CCT_TOKEN_FUNC);
  cct_hash_map_set(map, "goto", (void*)CCT_TOKEN_GOTO);
  cct_hash_map_set(map, "if", (void*)CCT_TOKEN_IF);
  cct_hash_map_set(map, "namespace", (void*)CCT_TOKEN_NAMESPACE);
  cct_hash_map_set(map, "null", (void*)CCT_TOKEN_NULL);
  cct_hash_map_set(map, "return", (void*)CCT_TOKEN_RETURN);
  cct_hash_map_set(map, "switch", (void*)CCT_TOKEN_SWITCH);
  cct_hash_map_set(map, "super", (void*)CCT_TOKEN_SUPER);
  cct_hash_map_set(map, "true", (void*)CCT_TOKEN_TRUE);
  cct_hash_map_set(map, "use", (void*)CCT_TOKEN_CONTINUE);
  cct_hash_map_set(map, "var", (void*)CCT_TOKEN_VAR);
  cct_hash_map_set(map, "while", (void*)CCT_TOKEN_WHILE);
  cct_hash_map_set(map, "in", (void*)CCT_TOKEN_IN);
}

ConcoctLexer* cct_new_lexer(ConcoctCharStream* source)
{
  ConcoctLexer* lexer = malloc(sizeof(ConcoctLexer));

  if(lexer == NULL)
  {
    fprintf(stderr, "Error allocating memory for lexer: %s\n", strerror(errno));
    return NULL;
  }

  lexer->keyword_map = cct_new_hash_map(48);
  if(lexer->keyword_map == NULL)
  {
    free(lexer);
    return NULL;
  }

  cct_init_lexer_keyword_map(lexer->keyword_map);
  lexer->source = source;
  lexer->line_number = 1;
  lexer->error = NULL;

  lexer->token_text = malloc(MAX_TOKEN_TEXT_LENGTH);
  cct_next_char(lexer);
  return lexer;
}
void cct_delete_lexer(ConcoctLexer* lexer)
{
  if(lexer->error != NULL)
  {
    free(lexer->error);
  }

  free(lexer->token_text);
  cct_delete_hash_map(lexer->keyword_map);
  free(lexer);
}


// Gets the next character in the lexing stream
char cct_next_char(ConcoctLexer* lexer)
{
  lexer->next_char = cct_get_char_from_stream(lexer->source);
  return lexer->next_char;
}

int cct_lexer_is_eof(const ConcoctLexer* lexer)
{
  return lexer->next_char == '\0';
}

void cct_set_error(ConcoctLexer* lexer, const char* message)
{
  if(lexer->error == NULL)
    lexer->error = malloc(MAX_ERROR_STRING_LENGTH);

  if(lexer->error != NULL)
    strcpy(lexer->error, message);
}

ConcoctToken cct_new_token(ConcoctTokenType type, size_t line_number)
{
  ConcoctToken token;
  token.type = type;
  token.line_number = line_number;
  return token;
}

ConcoctToken cct_next_token(ConcoctLexer* lexer)
{
  // Had to be initialized, so Error is a good default
  ConcoctTokenType type = CCT_TOKEN_ERROR;
  // How far into the text we are
  int text_index = 0;
  // Default to an empty string
  lexer->token_text[0] = '\0';
  // Skips whitespace and comments, repeatedly
  while(true)
  {
    if(!isspace(lexer->next_char) && lexer->next_char != '#')
      break;
    while(isspace(lexer->next_char))
    {
      // But if it's a new line it makes it a token
      if(lexer->next_char == '\n')
      {
        cct_next_char(lexer);
        ConcoctToken line_token = cct_new_token(CCT_TOKEN_NEWLINE, lexer->line_number++);
        return line_token;
      }
      cct_next_char(lexer);
    }
    if(lexer->next_char == '#')
    {
      if(cct_next_char(lexer) == '#')
      {
        // Multi-line comment
        while(cct_next_char(lexer) != '#' && !cct_lexer_is_eof(lexer))
        {
          // Registers new lines even in a comment
          if(lexer->next_char == '\n')
            lexer->line_number++;
        }
        if(cct_lexer_is_eof(lexer))
        {
          cct_set_error(lexer, "Reached EOF during multi-line comment");
          ConcoctToken eof_token = cct_new_token(CCT_TOKEN_ERROR, lexer->line_number);
          return eof_token;
        }
        // End of comment
        cct_next_char(lexer);
      }
      else
      {
        // Single line comment
        while(lexer->next_char != '\n' && !cct_lexer_is_eof(lexer))
          cct_next_char(lexer);
        if(cct_lexer_is_eof(lexer))
        {
          // End of file is perfectly fine on single-line comments
          ConcoctToken eof_token = cct_new_token(CCT_TOKEN_EOF, lexer->line_number);
          return eof_token;
        }
        // Return the newline that terminated the comment
        cct_next_char(lexer);
        ConcoctToken line_token = cct_new_token(CCT_TOKEN_NEWLINE, lexer->line_number++);
        return line_token;
      }
    }
  }

  if(cct_lexer_is_eof(lexer))
  {
    ConcoctToken eof_token = cct_new_token(CCT_TOKEN_EOF, lexer->line_number);
    return eof_token;
  }
  if(isalpha(lexer->next_char) || lexer->next_char == '_')
  {
    while(isalnum(lexer->next_char) || lexer->next_char == '_')
    {
      lexer->token_text[text_index++] = lexer->next_char;
      cct_next_char(lexer);
    }
    // Null termination to make it a proper string
    lexer->token_text[text_index] = '\0';
    // Determines if this identifier is a keyword or not
    if(cct_hash_map_has_key(lexer->keyword_map, lexer->token_text))
    {
      void* void_pointer = cct_hash_map_get(lexer->keyword_map, lexer->token_text);
      const int* pointer_to_type = (int *)(&void_pointer);
      type = *pointer_to_type;
    }
    else
    {
      type = CCT_TOKEN_IDENTIFIER;
    }
  }
  else if(isdigit(lexer->next_char))
  {
    while(isdigit(lexer->next_char))
    {
      lexer->token_text[text_index++] = lexer->next_char;
      cct_next_char(lexer);
    }
    // If it contains a '.' it is treated as a Float
    if(lexer->next_char == '.')
    {
      lexer->token_text[text_index++] = lexer->next_char;
      while(isdigit(cct_next_char(lexer)))
        lexer->token_text[text_index++] = lexer->next_char;
      type = CCT_TOKEN_FLOAT;
    }
    else
      type = CCT_TOKEN_INT;
  }
  else if(lexer->next_char == '"')
  {
    while(cct_next_char(lexer) != '"')
    {
      if(cct_lexer_is_eof(lexer))
      {
        cct_set_error(lexer, "Unterminated string, got EOF");
        ConcoctToken error_token = cct_new_token(CCT_TOKEN_ERROR, lexer->line_number);
        return error_token;
      }
      if(lexer->next_char == '\n')
      {
        cct_set_error(lexer, "Unterminated string, got New Line");
        ConcoctToken error_token = cct_new_token(CCT_TOKEN_ERROR, lexer->line_number);
        return error_token;
      }
      lexer->token_text[text_index++] = lexer->next_char;
    }
    cct_next_char(lexer);
    type = CCT_TOKEN_STRING;
  }
  else if(lexer->next_char == '\'')
  {
    cct_next_char(lexer);
    if(cct_lexer_is_eof(lexer))
    {
      cct_set_error(lexer, "Unterminated character literal, got EOF");
      ConcoctToken error_token = cct_new_token(CCT_TOKEN_ERROR, lexer->line_number);
      return error_token;
    }
    if(lexer->next_char == '\n')
    {
      cct_set_error(lexer, "Unterminated character literal, got New Line");
      ConcoctToken error_token = cct_new_token(CCT_TOKEN_ERROR, lexer->line_number);
      return error_token;
    }
    if(lexer->next_char == '\'')
    {
      cct_set_error(lexer, "Empty character literal");
      ConcoctToken error_token = cct_new_token(CCT_TOKEN_ERROR, lexer->line_number);
      return error_token;
    }
    lexer->token_text[text_index++] = lexer->next_char;
    cct_next_char(lexer);
    type = CCT_TOKEN_CHAR;
  }
  else
  {
    // These should be easier to test with a switch
    // Depending on the character following each operator, it may become a different operator
    switch(lexer->next_char)
    {
      case '=':
        if(cct_next_char(lexer) == '=')
        {
          cct_next_char(lexer);
          type = CCT_TOKEN_EQUAL;
        }
        else
        {
          type = CCT_TOKEN_ASSIGN;
        }
        break;
      case '$':
        if(cct_next_char(lexer) == '=')
        {
          cct_next_char(lexer);
          type = CCT_TOKEN_STRLEN_EQUAL;
        }
        if(cct_next_char(lexer) == '!')
        {
          cct_next_char(lexer);
          type = CCT_TOKEN_STRLEN_NOT_EQUAL;
        }
        break;
      case '+':
        switch(cct_next_char(lexer))
        {
          case '=':
            cct_next_char(lexer);
            type = CCT_TOKEN_ADD_ASSIGN;
            break;
          case '+':
            cct_next_char(lexer);
            type = CCT_TOKEN_INC;
            break;
          default:
            type = CCT_TOKEN_ADD;
            break;
        }
        break;
      case '-':
        switch(cct_next_char(lexer))
        {
          case '=':
            cct_next_char(lexer);
            type = CCT_TOKEN_SUB_ASSIGN;

            break;
          case '-':
            cct_next_char(lexer);
            type = CCT_TOKEN_DEC;
            break;
          default:
            type = CCT_TOKEN_SUB;
            break;
        }
        break;
      case '*':
        switch(cct_next_char(lexer))
        {
          case '=':
            cct_next_char(lexer);
            type = CCT_TOKEN_MUL_ASSIGN;
            break;
          case '*':
            if(cct_next_char(lexer) == '=')
            {
              cct_next_char(lexer);
              type = CCT_TOKEN_EXP_ASSIGN;
              break;
            }
            else
            {
              type = CCT_TOKEN_EXP;
              break;
            }
          default:
            type = CCT_TOKEN_MUL;
            break;
        }
        break;
      case '/':
        if(cct_next_char(lexer) == '=')
        {
          cct_next_char(lexer);
          type = CCT_TOKEN_DIV_ASSIGN;
        }
        else
          type = CCT_TOKEN_DIV;
        break;
      case '%':
        if(cct_next_char(lexer) == '=')
        {
          cct_next_char(lexer);
          type = CCT_TOKEN_MOD_ASSIGN;
        }
        else
          type = CCT_TOKEN_MOD;
        break;
      case '>':
        switch(cct_next_char(lexer))
        {
          case '=':
            cct_next_char(lexer);
            type = CCT_TOKEN_GREATER_EQUAL;
            break;
          case '>':
            cct_next_char(lexer);
            type = CCT_TOKEN_SHR;
            break;
          default:
            type = CCT_TOKEN_GREATER;
            break;
        }
        break;
      case '<':
        switch(cct_next_char(lexer))
        {
          case '=':
            cct_next_char(lexer);
            type = CCT_TOKEN_LESS_EQUAL;
            break;
          case '<':
            cct_next_char(lexer);
            type = CCT_TOKEN_SHL;
            break;
          default:
            type = CCT_TOKEN_LESS;
            break;
        }
        break;
      case '!':
        if(cct_next_char(lexer) == '=')
        {
          cct_next_char(lexer);
          type = CCT_TOKEN_NOT_EQUAL;
        }
        else
          type = CCT_TOKEN_NOT;
        break;
      case '&':
        if(cct_next_char(lexer) == '&')
        {
          cct_next_char(lexer);
          type = CCT_TOKEN_AND;
        }
        else
          type = CCT_TOKEN_BIN_AND;
        break;
      case '|':
        if(cct_next_char(lexer) == '|')
        {
          cct_next_char(lexer);
          type = CCT_TOKEN_OR;
        }
        else
          type = CCT_TOKEN_BIN_OR;
        break;
      case '^':
        cct_next_char(lexer);
        type = CCT_TOKEN_BIN_XOR;
        break;
      case '.':
        cct_next_char(lexer);
        type = CCT_TOKEN_DOT;
        break;
      case '(':
        cct_next_char(lexer);
        type = CCT_TOKEN_LEFT_PAREN;
        break;
      case ')':
        cct_next_char(lexer);
        type = CCT_TOKEN_RIGHT_PAREN;
        break;
      case '{':
        cct_next_char(lexer);
        type = CCT_TOKEN_LEFT_BRACE;
        break;
      case '}':
        cct_next_char(lexer);
        type = CCT_TOKEN_RIGHT_BRACE;
        break;
      case '[':
        cct_next_char(lexer);
        type = CCT_TOKEN_LEFT_BRACKET;
        break;
      case ']':
        cct_next_char(lexer);
        type = CCT_TOKEN_RIGHT_BRACKET;
        break;
      case ',':
        cct_next_char(lexer);
        type = CCT_TOKEN_COMMA;
        break;
      case '~':
        cct_next_char(lexer);
        type = CCT_TOKEN_BIN_NOT;
        break;
      default:
        // cct_set_error is called to allocate memory for storing the error message; need a better way of doing this
        cct_set_error(lexer, "");
        sprintf(lexer->error, "Unexpected character '%c'", lexer->next_char);

        // This means it's some sort of unsupported character
        // We just set the text to the original text, and set the type to Error
        lexer->token_text[text_index++] = lexer->next_char;
        lexer->token_text[text_index] = '\0';
        cct_next_char(lexer);
        ConcoctToken token = cct_new_token(CCT_TOKEN_ERROR, lexer->line_number);

        return token;
    }
  }
  // Ensures null-termination
  lexer->token_text[text_index] = '\0';
  ConcoctToken token = cct_new_token(type, lexer->line_number);
  return token;
}

const char* cct_token_type_to_string(ConcoctTokenType type)
{
  switch(type)
  {
    case CCT_TOKEN_IDENTIFIER:       return "Identifier";
    case CCT_TOKEN_INT:              return "Int";
    case CCT_TOKEN_FLOAT:            return "Float";
    case CCT_TOKEN_STRING:           return "String";
    case CCT_TOKEN_CHAR:             return "Char";
    case CCT_TOKEN_NEWLINE:          return "New line";
    case CCT_TOKEN_EOF:              return "EOF";
    case CCT_TOKEN_ADD:              return "+";
    case CCT_TOKEN_SUB:              return "-";
    case CCT_TOKEN_MUL:              return "*";
    case CCT_TOKEN_DIV:              return "/";
    case CCT_TOKEN_MOD:              return "%";
    case CCT_TOKEN_EXP:              return "**";
    case CCT_TOKEN_ADD_ASSIGN:       return "+=";
    case CCT_TOKEN_SUB_ASSIGN:       return "-=";
    case CCT_TOKEN_MUL_ASSIGN:       return "*=";
    case CCT_TOKEN_DIV_ASSIGN:       return "/=";
    case CCT_TOKEN_MOD_ASSIGN:       return "%=";
    case CCT_TOKEN_EXP_ASSIGN:       return "**=";
    case CCT_TOKEN_ASSIGN:           return "=";
    case CCT_TOKEN_INC:              return "++";
    case CCT_TOKEN_DEC:              return "--";
    case CCT_TOKEN_UNARY_MINUS:      return "- (Unary)";
    case CCT_TOKEN_EQUAL:            return "==";
    case CCT_TOKEN_NOT_EQUAL:        return "!=";
    case CCT_TOKEN_STRLEN_EQUAL:     return "$=";
    case CCT_TOKEN_STRLEN_NOT_EQUAL: return "$!";
    case CCT_TOKEN_GREATER:          return ">";
    case CCT_TOKEN_LESS:             return "<";
    case CCT_TOKEN_GREATER_EQUAL:    return ">=";
    case CCT_TOKEN_LESS_EQUAL:       return "<=";
    case CCT_TOKEN_AND:              return "&&";
    case CCT_TOKEN_OR:               return "||";
    case CCT_TOKEN_NOT:              return "!";
    case CCT_TOKEN_BIN_AND:          return "&";
    case CCT_TOKEN_BIN_OR:           return "|";
    case CCT_TOKEN_BIN_XOR:          return "^";
    case CCT_TOKEN_BIN_NOT:          return "~";
    case CCT_TOKEN_SHL:              return "<<";
    case CCT_TOKEN_SHR:              return ">>";
    case CCT_TOKEN_DOT:              return ".";
    case CCT_TOKEN_LEFT_PAREN:       return "(";
    case CCT_TOKEN_RIGHT_PAREN:      return ")";
    case CCT_TOKEN_LEFT_BRACE:       return "{";
    case CCT_TOKEN_RIGHT_BRACE:      return "}";
    case CCT_TOKEN_LEFT_BRACKET:     return "[";
    case CCT_TOKEN_RIGHT_BRACKET:    return "]";
    case CCT_TOKEN_BREAK:            return "break";
    case CCT_TOKEN_CASE:             return "case";
    case CCT_TOKEN_CLASS:            return "class";
    case CCT_TOKEN_CONTINUE:         return "continue";
    case CCT_TOKEN_DEFAULT:          return "default";
    case CCT_TOKEN_DO:               return "do";
    case CCT_TOKEN_ELSE:             return "else";
    case CCT_TOKEN_ENUM:             return "enum";
    case CCT_TOKEN_FALSE:            return "false";
    case CCT_TOKEN_FOR:              return "for";
    case CCT_TOKEN_FUNC:             return "func";
    case CCT_TOKEN_GOTO:             return "goto";
    case CCT_TOKEN_IF:               return "if";
    case CCT_TOKEN_NAMESPACE:        return "namespace";
    case CCT_TOKEN_NULL:             return "null";
    case CCT_TOKEN_RETURN:           return "return";
    case CCT_TOKEN_SUPER:            return "super";
    case CCT_TOKEN_SWITCH:           return "switch";
    case CCT_TOKEN_TRUE:             return "true";
    case CCT_TOKEN_USE:              return "use";
    case CCT_TOKEN_VAR:              return "var";
    case CCT_TOKEN_WHILE:            return "while";
    case CCT_TOKEN_ERROR:            return "Error";
    default:                         return "Token";
  }
}
