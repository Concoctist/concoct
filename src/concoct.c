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

#include <ctype.h>       // isspace()
#include <errno.h>       // errno
#include <signal.h>      // signal(), SIGINT
#include <stdbool.h>     // false, true
#include <stddef.h>      // size_t
#include <stdio.h>       // FILE, fclose(), fflush(), fgets(), fprintf(), printf(), puts(), stdin, stderr, stdout
#include <stdlib.h>      // exit(), EXIT_FAILURE, EXIT_SUCCESS
#include <string.h>      // memset(), strcasecmp()/stricmp(), strcspn(), strerror(), strlen()
#include "char_stream.h"
#include "compiler.h"
#include "concoct.h"
#include "debug.h"
#include "lexer.h"
#include "parser.h"
#include "types.h"
#include "version.h"     // VERSION
#include "vm/vm.h"

int main(int argc, char** argv)
{
  char *input_file = NULL;
  int nonopt_count = 0;

  if(argc > 1)
  {
    for(int i = 1; i < argc; i++)
    {
      // Check for command-line options
      if(argv[i][0] == ARG_PREFIX)
        handle_options(argc, argv);
      else
      {
        nonopt_count++;
        input_file = argv[i];
      }
    }
  }

  print_version();
  init_vm();

  if(debug_mode)
  {
    debug_print("argc: %d", argc);
    for(int i = 0; i < argc; i++)
      debug_print("argv[%d]: %s", i, argv[i]);
  }

  if(nonopt_count > 1)
  {
    fprintf(stderr, "Ambiguous input!\n");
    clean_exit(EXIT_FAILURE);
  }

  if(nonopt_count == 0)
    interactive_mode();

  if(argc > 3)
  {
    fprintf(stderr, "Too many arguments!\n");
    clean_exit(EXIT_FAILURE);
  }

  if(input_file)
  {
    lex_file(input_file);
    parse_file(input_file);
  }
  else
  {
    fprintf(stderr, "No input file!\n");
    clean_exit(EXIT_FAILURE);
  }

  clean_exit(EXIT_SUCCESS);
  return 0;
}

// Exits gracefully
void clean_exit(int status)
{
  stop_vm();
  if(status == EXIT_SUCCESS)
    exit(EXIT_SUCCESS);
  else
    exit(EXIT_FAILURE);
  return;
}

// Parses file
void parse_file(const char* file_name)
{
  FILE* input_file = fopen(file_name, "r");
  if(input_file == NULL)
  {
    fprintf(stderr, "Error opening %s: %s\n", file_name, strerror(errno));
    clean_exit(EXIT_FAILURE);
  }

  // Creates a new lexer and iterates through the tokens
  ConcoctCharStream* char_stream = cct_new_file_char_stream(input_file);
  ConcoctLexer* file_lexer = cct_new_lexer(char_stream);
  ConcoctParser* parser = cct_new_parser(file_lexer);
  ConcoctNodeTree* node_tree = cct_parse_program(parser);
  if(parser->error == NULL)
  {
    if(debug_mode)
      cct_print_node(node_tree->root, 0);
    compile(node_tree);
  }
  else
    fprintf(stderr, "Parsing error: [%i] %s, got %s\n", parser->error_line, parser->error, cct_token_type_to_string(parser->current_token.type));
  fclose(input_file);
  cct_delete_parser(parser);
  cct_delete_char_stream(char_stream);
  cct_delete_node_tree(node_tree);
  return;
}

// Parses string
void parse_string(const char* input_string)
{
  ConcoctCharStream* char_stream = cct_new_string_char_stream(input_string);
  ConcoctLexer* string_lexer = cct_new_lexer(char_stream);
  ConcoctParser* parser = cct_new_parser(string_lexer);
  ConcoctNodeTree* node_tree = cct_parse_program(parser);
  if(parser->error == NULL)
  {
    if(debug_mode)
      cct_print_node(node_tree->root, 0);
    compile(node_tree);
  }
  else
    fprintf(stderr, "Parsing error: [%i] %s, got %s\n", parser->error_line, parser->error, cct_token_type_to_string(parser->current_token.type));
  cct_delete_parser(parser);
  if(debug_mode)
    debug_print("Freed parser.");
  cct_delete_node_tree(node_tree);
  if(debug_mode)
    debug_print("Freed node tree.");
  cct_delete_char_stream(char_stream);
  return;
}

// Lexes file
void lex_file(const char* file_name)
{
  FILE* input_file = fopen(file_name, "r");
  if(input_file == NULL)
  {
    fprintf(stderr, "Error opening %s: %s\n", file_name, strerror(errno));
    clean_exit(EXIT_FAILURE);
  }

  // Creates a new lexer and iterates through the tokens
  ConcoctCharStream* char_stream = cct_new_file_char_stream(input_file);
  ConcoctLexer* file_lexer = cct_new_lexer(char_stream);
  if(file_lexer == NULL)
  {
    fprintf(stderr, "File lexer is NULL!\n");
    fclose(input_file);
    cct_delete_char_stream(char_stream);
    return;
  }
  ConcoctToken token = cct_next_token(file_lexer);
  // Continues printing until an EOF is reached
  printf("Lexing %s:\n", file_name);
  while(token.type != CCT_TOKEN_EOF)
  {
    if(file_lexer->error != NULL)
    {
      fprintf(stderr, "Error on line %i:\n", token.line_number);
      fprintf(stderr, "%s\n", file_lexer->error);
      break;
    }
    if(debug_mode)
      printf("[%i] %s : %s\n", token.line_number, file_lexer->token_text, cct_token_type_to_string(token.type));
    token = cct_next_token(file_lexer);
  }
  fclose(input_file);
  cct_delete_lexer(file_lexer);
  cct_delete_char_stream(char_stream);
  return;
}

// Lexes string
void lex_string(const char* input_string)
{
  // Lexer also can be created for strings
  //const char* input = "func test() { return a + b }";
  ConcoctCharStream* char_stream = cct_new_string_char_stream(input_string);
  ConcoctLexer* string_lexer = cct_new_lexer(char_stream);
  ConcoctToken token = cct_next_token(string_lexer);

  if(string_lexer == NULL)
  {
    fprintf(stderr, "String lexer is NULL!\n");
    return;
  }

  //printf("Lexing string...\n");
  while(token.type != CCT_TOKEN_EOF)
  {
    if(string_lexer->error != NULL)
    {
      fprintf(stderr, "Error on line %i:\n", token.line_number);
      fprintf(stderr, "%s\n", string_lexer->error);
      break;
    }
    if(debug_mode)
      printf("[%i] %s : %s\n", token.line_number, string_lexer->token_text, cct_token_type_to_string(token.type));
    token = cct_next_token(string_lexer);
  }
  cct_delete_lexer(string_lexer);
  cct_delete_char_stream(char_stream);
  return;
}

// Handle command-line options
void handle_options(int argc, char *argv[])
{
  for(int i = 1; i < argc; i++)
  {
    if(argv[i][0] == ARG_PREFIX && strlen(argv[i]) == 2)
    {
      switch(argv[i][1])
      {
        case 'd':
          debug_mode = true;
          break;
        case 'h':
          print_usage();
          exit(EXIT_SUCCESS);
          break;
        case 'l':
          print_license();
          exit(EXIT_SUCCESS);
          break;
        case 'v':
          print_version();
          exit(EXIT_SUCCESS);
          break;
        default:
          fprintf(stderr, "Invalid option!\n");
          print_usage();
          exit(EXIT_FAILURE);
          break;
      }
    }
    if(argv[i][0] == ARG_PREFIX && strlen(argv[i]) != 2)
    {
      fprintf(stderr, "Invalid option!\n");
      print_usage();
      exit(EXIT_FAILURE);
    }
  }
  return;
}

// Displays license
void print_license()
{
  puts("BSD 2-Clause License\n");
  puts("Copyright (c) 2020-2023 BlakeTheBlock and Lloyd Dilley");
  puts("All rights reserved.\n");
  puts("Redistribution and use in source and binary forms, with or without");
  puts("modification, are permitted provided that the following conditions are met:\n");
  puts("1. Redistributions of source code must retain the above copyright notice, this");
  puts("list of conditions and the following disclaimer.\n");
  puts("2. Redistributions in binary form must reproduce the above copyright notice,");
  puts("this list of conditions and the following disclaimer in the documentation");
  puts("and/or other materials provided with the distribution.\n");

  puts("THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\"");
  puts("AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE");
  puts("IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE");
  puts("DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE");
  puts("FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL");
  puts("DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR");
  puts("SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER");
  puts("CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,");
  puts("OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE");
  puts("OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.");
  return;
}

// Displays usage
void print_usage()
{
  print_version();
  printf("Usage: concoct [%c<option>] [file]\n", ARG_PREFIX);
  puts("Options:");
  printf("%cd: debug mode\n", ARG_PREFIX);
  printf("%ch: print usage\n", ARG_PREFIX);
  printf("%cv: print version\n", ARG_PREFIX);
  return;
}

// Displays version
void print_version()
{
  if(strlen(GIT_REV) == 0) // git not detected in path
    printf("Concoct v%s (%s %s) (%s) built at %s on %s\n", VERSION, BITNESS, PLATFORM, BUILD_TYPE, BUILD_TIME, BUILD_DATE);
  else
    printf("Concoct v%s rev %s (%s) (%s %s) (%s) built at %s on %s\n", VERSION, GIT_REV, GIT_HASH, BITNESS, PLATFORM, BUILD_TYPE, BUILD_TIME, BUILD_DATE);
  return;
}

// Compares command input
bool compare_input(const char* input, const char* command)
{
  bool is_match = false;
#ifdef _WIN32
  if(_stricmp(input, command) == 0)
    is_match = true;
#else
  if(strcasecmp(input, command) == 0)
    is_match = true;
#endif
  return is_match;
}

// Catches interrupt
void handle_sigint(int sig)
{
  signal(SIGINT, handle_sigint);
  puts("");
  if(debug_mode)
    debug_print("Caught interrupt signal: %d", sig);
  printf("> ");
  fflush(stdout);
  return;
}

// Interactive mode
void interactive_mode()
{
  signal(SIGINT, handle_sigint);
  char input[1024];
  puts("Warning: Expect things to break.");
  while(true)
  {
    memset(input, 0, sizeof(input)); // reset input every iteration
    printf("> ");
    if(fgets(input, 1024, stdin) == NULL)
    {
      puts("");
      if(debug_mode)
      {
#ifdef _WIN32
        debug_print("ctrl+z (EOT) detected.");
#else
        debug_print("ctrl+d (EOT) detected.");
#endif // _WIN32
      }
      clean_exit(EXIT_SUCCESS); // ctrl+d (Unix) or ctrl+z (Windows) EOT detected
    }

    // Check if input is just whitespace
    bool blank = false;
    for(size_t i = 0; i < strlen(input); i++)
    {
      if(!isspace(input[i]))
      {
        blank = false;
        break;
      }
      blank = true;
    }
    if(blank)
      continue;

    input[strcspn(input, "\n")] = '\0';  // remove trailing newline

    // Check if string is empty
    if(input[0] == '\0')
      continue;

    // Check for valid commands
    if(compare_input(input, "license"))
    {
      print_license();
      continue;
    }
    if(compare_input(input, "quit"))
      clean_exit(EXIT_SUCCESS);
    if(compare_input(input, "version"))
    {
      print_version();
      continue;
    }

    lex_string(input);
    parse_string(input);
  }
  return;
}
