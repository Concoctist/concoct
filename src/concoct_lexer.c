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

#include "concoct_lexer.h"

struct ConcoctLexer cct_new_lexer(FILE* in_stream)
{
    struct ConcoctLexer lexer;
    lexer.input_stream = in_stream;
    lexer.next_char = (char)getc(in_stream);
    lexer.line_number = 1;
    lexer.error = NULL;
    return lexer;
}
// Gets the next character in the lexing stream
char cct_next_char(struct ConcoctLexer* lexer)
{
    lexer->next_char = (char)getc(lexer->input_stream);
    return lexer->next_char;
}

void cct_set_error(struct ConcoctLexer* lexer, const char* message)
{
    lexer->error = message;
}

struct ConcoctToken cct_new_token(enum ConcoctTokenType type, char* text, int line_number)
{
    struct ConcoctToken token;
    token.type = type;
    token.text = text;
    token.line_number = line_number;
    return token;
}
struct ConcoctToken cct_next_token(struct ConcoctLexer* lexer)
{
    // Allocates the text part of the token
    char* text = malloc(TOKEN_TEXT_LENGTH);
    // Had to be initialized, so Error is a good default
    enum ConcoctTokenType type = CCT_TOKEN_ERROR;
    // How far into the text we are
    int text_index = 0;
    // Skips whitespace and comments, repeatedly
    while(1)
    {
        if(!isspace(lexer->next_char) && lexer->next_char != '#')
        {
            break;
        }
        while(isspace(lexer->next_char))
        {
            // But if it's a new line it makes it a token
            if(lexer->next_char == '\n')
            {
                cct_next_char(lexer);
                strcpy(text, "\\n");
                struct ConcoctToken line_token = cct_new_token(CCT_TOKEN_NEWLINE, text, lexer->line_number++);
                return line_token;
            }
            cct_next_char(lexer);
        }
        if(lexer->next_char == '#')
        {
            if(cct_next_char(lexer) == '#')
            {
                // Multi-line comment
                while(1)
                {
                    while(cct_next_char(lexer) != '#' && !feof(lexer->input_stream))
                    {
                        // Registers new lines even in a comment
                        if(lexer->next_char == '\n')
                        {
                            lexer->line_number++;
                        }
                    }
                    if(feof(lexer->input_stream))
                    {
                        // Might change, but currently sends an Error token with 'EOF' as text
                        // End of file shouldn't occur during a multiline comment
                        cct_set_error(lexer, "Reached %s during multi-line comment");
                        strcpy(text, cct_token_type_to_string(CCT_TOKEN_EOF));
                        struct ConcoctToken eof_token = cct_new_token(CCT_TOKEN_ERROR, text, lexer->line_number);
                        return eof_token;
                    }
                    // End of comment
                    cct_next_char(lexer);
                    break;
                }
            }
            else
            {
                // Single line comment
                while(lexer->next_char != '\n' && !feof(lexer->input_stream))
                {
                    cct_next_char(lexer);
                }
                if(feof(lexer->input_stream))
                {
                    // End of file is perfectly fine on single-line comments
                    strcpy(text, cct_token_type_to_string(CCT_TOKEN_EOF));
                    struct ConcoctToken eof_token = cct_new_token(CCT_TOKEN_ERROR, text, lexer->line_number);
                    return eof_token;
                }
                // Return the newline that terminated the comment
                cct_next_char(lexer);
                strcpy(text, "\\n");
                struct ConcoctToken line_token = cct_new_token(CCT_TOKEN_NEWLINE, text, lexer->line_number++);
                return line_token;
            }
        }
    }


    if(feof(lexer->input_stream))
    {
        strcpy(text, cct_token_type_to_string(CCT_TOKEN_EOF));
        struct ConcoctToken eof_token = cct_new_token(CCT_TOKEN_EOF, text, lexer->line_number);
        return eof_token;
    }
    if(isalpha(lexer->next_char) || lexer->next_char == '_')
    {
        while(isalnum(lexer->next_char) || lexer->next_char == '_')
        {
            text[text_index++] = lexer->next_char;
            cct_next_char(lexer);
        }
        // Null termination so strcmp works
        text[text_index++] = '\0';
        // Goes trhough each keyword to see if this identifier is actually a keyword
        // Should be optimized to a hash map
        int is_keyword = 0;
        for(int i = 0;i < CCT_KEYWORD_COUNT;i++)
        {
            if(strcmp(cct_keywords[i], text) == 0)
            {
                type = cct_keyword_types[i];
                is_keyword = 1;
                break;
            }
        }
        if(!is_keyword)
        {
            type = CCT_TOKEN_IDENTIFIER;
        }
    }
    else if(isdigit(lexer->next_char))
    {
        while(isdigit(lexer->next_char))
        {
            text[text_index++] = lexer->next_char;
            cct_next_char(lexer);
        }
        // If it contains a '.' it is treated as a Float
        if(lexer->next_char == '.')
        {
            text[text_index++] = lexer->next_char;
            while(isdigit(cct_next_char(lexer)))
            {
                text[text_index++] = lexer->next_char;
            }
            type = CCT_TOKEN_FLOAT;
        }
        else
        {
            type = CCT_TOKEN_INT;
        }
    }
    else if(lexer->next_char == '"')
    {
        // Strings are generally much larger than other tokens
        free(text);
        text = malloc(STRING_TOKEN_LENGTH);
        text[text_index++] = '"';
        while(cct_next_char(lexer) != '"')
        {
            if(feof(lexer->input_stream))
            {
                cct_set_error(lexer, "Unterminated string, got %s");
                strcpy(text, cct_token_type_to_string(CCT_TOKEN_EOF));
                struct ConcoctToken error_token = cct_new_token(CCT_TOKEN_ERROR, text, lexer->line_number);
                return error_token;
            }
            if(lexer->next_char == '\n')
            {
                cct_set_error(lexer, "Unterminated string, got %s");
                strcpy(text, cct_token_type_to_string(CCT_TOKEN_NEWLINE));
                struct ConcoctToken error_token = cct_new_token(CCT_TOKEN_ERROR, text, lexer->line_number);
                return error_token;
            }
            text[text_index++] = lexer->next_char;
        }
        text[text_index++] = '"';
        cct_next_char(lexer);
        type = CCT_TOKEN_STRING;
    }
    else if(lexer->next_char == '\'')
    {
        text[text_index++] = '\'';
        cct_next_char(lexer);
        if(feof(lexer->input_stream))
        {
            cct_set_error(lexer, "Unterminated character literal, got %s");
            strcpy(text, cct_token_type_to_string(CCT_TOKEN_EOF));
            struct ConcoctToken error_token = cct_new_token(CCT_TOKEN_ERROR, text, lexer->line_number);
            return error_token;
        }
        if(lexer->next_char == '\n')
        {
            cct_set_error(lexer, "Unterminated character literal, got %s");
            strcpy(text, cct_token_type_to_string(CCT_TOKEN_NEWLINE));
            struct ConcoctToken error_token = cct_new_token(CCT_TOKEN_ERROR, text, lexer->line_number);
            return error_token;
        }
        if(lexer->next_char == '\'')
        {
            cct_set_error(lexer, "Empty character literal %s");
            strcpy(text, "''");
            struct ConcoctToken error_token = cct_new_token(CCT_TOKEN_ERROR, text, lexer->line_number);
            return error_token;
        }
        text[text_index++] = lexer->next_char;
        text[text_index++] = '\'';
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
            case '+':
                switch (cct_next_char(lexer))
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
                switch (cct_next_char(lexer))
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
                switch(cct_next_char(lexer))
                {
                    case '=':
                        cct_next_char(lexer);
                        type = CCT_TOKEN_DIV_ASSIGN;
                        break;
                    default:
                        type = CCT_TOKEN_DIV;
                        break;
                }
                break;
            case '%':
                switch(cct_next_char(lexer))
                {
                    case '=':
                        cct_next_char(lexer);
                        type = CCT_TOKEN_MOD_ASSIGN;
                        break;
                    default:
                        type = CCT_TOKEN_MOD;
                        break;
                }
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
                switch(cct_next_char(lexer))
                {
                    case '=':
                        cct_next_char(lexer);
                        type = CCT_TOKEN_NOT_EQUAL;
                        break;
                    default:
                        type = CCT_TOKEN_NOT;
                        break;
                }
                break;
            case '&':
                switch(cct_next_char(lexer))
                {
                    case '&':
                        cct_next_char(lexer);
                        type = CCT_TOKEN_AND;
                        break;
                    default:
                        type = CCT_TOKEN_BIN_AND;
                        break;
                }
                break;
            case '|':
                switch(cct_next_char(lexer))
                {
                    case '|':
                        cct_next_char(lexer);
                        type = CCT_TOKEN_OR;
                        break;
                    default:
                        type = CCT_TOKEN_BIN_OR;
                        break;
                }
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
            default:
                // This means it's some sort of unsupported character
                // We just set the text to the original text, and set the type to Error
                cct_set_error(lexer, "Unexpected character '%s'");
                text[text_index++] = lexer->next_char;
                text[text_index++] = '\0';
                cct_next_char(lexer);
                struct ConcoctToken token = cct_new_token(CCT_TOKEN_ERROR, text, lexer->line_number);
                return token;
        }
        strcpy(text, cct_token_type_to_string(type));
        text_index = 5;
    }
    // Ensures null-termination
    text[text_index] = '\0';
    struct ConcoctToken token = cct_new_token(type, text, lexer->line_number);
    return token;
}

const char* cct_token_type_to_string(enum ConcoctTokenType type)
{
    switch(type)
    {
        case CCT_TOKEN_IDENTIFIER:
            return "Identifier";
        case CCT_TOKEN_INT:
            return "Int";
        case CCT_TOKEN_FLOAT:
            return "Float";
        case CCT_TOKEN_STRING:
            return "String";
        case CCT_TOKEN_CHAR:
            return "Char";
        case CCT_TOKEN_NEWLINE:
            return "New line";
        case CCT_TOKEN_EOF:
            return "EOF";
        case CCT_TOKEN_ADD:
            return "+";
        case CCT_TOKEN_SUB:
            return "-";
        case CCT_TOKEN_MUL:
            return "*";
        case CCT_TOKEN_DIV:
            return "/";
        case CCT_TOKEN_MOD:
            return "%";
        case CCT_TOKEN_EXP:
            return "**";
        case CCT_TOKEN_ADD_ASSIGN:
            return "+=";
        case CCT_TOKEN_SUB_ASSIGN:
            return "-=";
        case CCT_TOKEN_MUL_ASSIGN:
            return "*=";
        case CCT_TOKEN_DIV_ASSIGN:
            return "/=";
        case CCT_TOKEN_MOD_ASSIGN:
            return "%=";
        case CCT_TOKEN_EXP_ASSIGN:
            return "**=";
        case CCT_TOKEN_ASSIGN:
            return "=";
        case CCT_TOKEN_INC:
            return "++";
        case CCT_TOKEN_DEC:
            return "--";
        case CCT_TOKEN_EQUAL:
            return "==";
        case CCT_TOKEN_NOT_EQUAL:
            return "!=";
        case CCT_TOKEN_GREATER:
            return ">";
        case CCT_TOKEN_LESS:
            return "<";
        case CCT_TOKEN_GREATER_EQUAL:
            return ">=";
        case CCT_TOKEN_LESS_EQUAL:
            return "<=";
        case CCT_TOKEN_AND:
            return "&&";
        case CCT_TOKEN_OR:
            return "||";
        case CCT_TOKEN_NOT:
            return "!";
        case CCT_TOKEN_BIN_AND:
            return "&";
        case CCT_TOKEN_BIN_OR:
            return "|";
        case CCT_TOKEN_BIN_XOR:
            return "^";
        case CCT_TOKEN_SHL:
            return "<<";
        case CCT_TOKEN_SHR:
            return ">>";
        case CCT_TOKEN_DOT:
            return ".";
        case CCT_TOKEN_LEFT_PAREN:
            return "(";
        case CCT_TOKEN_RIGHT_PAREN:
            return ")";
        case CCT_TOKEN_LEFT_BRACE:
            return "{";
        case CCT_TOKEN_RIGHT_BRACE:
            return "}";
        case CCT_TOKEN_LEFT_BRACKET:
            return "[";
        case CCT_TOKEN_RIGHT_BRACKET:
            return "]";
        case CCT_TOKEN_BREAK:
            return "break";
        case CCT_TOKEN_CASE:
            return "case";
        case CCT_TOKEN_CLASS:
            return "class";
        case CCT_TOKEN_CONTINUE:
            return "continue";
        case CCT_TOKEN_DEFAULT:
            return "default";
        case CCT_TOKEN_DO:
            return "do";
        case CCT_TOKEN_ELSE:
            return "else";
        case CCT_TOKEN_ENUM:
            return "enum";
        case CCT_TOKEN_FALSE:
            return "false";
        case CCT_TOKEN_FOR:
            return "for";
        case CCT_TOKEN_FUNC:
            return "func";
        case CCT_TOKEN_GOTO:
            return "goto";
        case CCT_TOKEN_IF:
            return "if";
        case CCT_TOKEN_NAMESPACE:
            return "namespace";
        case CCT_TOKEN_NULL:
            return "null";
        case CCT_TOKEN_RETURN:
            return "return";
        case CCT_TOKEN_SUPER:
            return "super";
        case CCT_TOKEN_SWITCH:
            return "switch";
        case CCT_TOKEN_TRUE:
            return "true";
        case CCT_TOKEN_USE:
            return "use";
        case CCT_TOKEN_VAR:
            return "var";
        case CCT_TOKEN_WHILE:
            return "while";
        case CCT_TOKEN_ERROR:
            return "Error";
        default:
            return "Token";
    }
}
