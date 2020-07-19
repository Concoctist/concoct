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

#include <ctype.h>   // isspace()
#include <errno.h>   // errno
#include <stdbool.h> // false, true
#include <stddef.h>  // size_t
#include <stdio.h>   // FILE, fclose(), fgets(), fprintf(), printf(), puts(), sprintf(), stdin, stderr
#include <stdlib.h>  // exit(), EXIT_FAILURE, EXIT_SUCCESS
#include <string.h>  // memset(), strcasecmp()/stricmp(), strcspn(), strerror(), strlen()
#include "concoct.h"
#include "debug.h"
#include "lexer.h"
#include "parser.h"
#include "types.h"
#include "version.h" // VERSION
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
			{
				handle_options(argc, argv);
			}
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
	ConcoctLexer* file_lexer = cct_new_file_lexer(input_file);
	ConcoctParser* parser = cct_new_parser(file_lexer);
	ConcoctNode* root_node = cct_parse_program(parser);
	if(parser->error == NULL)
	{
		cct_print_node(root_node, 0);
		cct_delete_node(root_node);
	}
	else
	{
		fprintf(stderr, "Parsing error: [%i] %s, got %s", parser->error_line, parser->error, cct_token_type_to_string(parser->current_token.type));
	}
	fclose(input_file);
	cct_delete_parser(parser);
	return;
}

// Parses string
void parse_string(const char* input_string)
{
	ConcoctLexer* string_lexer = cct_new_string_lexer(input_string);
	ConcoctParser* parser = cct_new_parser(string_lexer);
	ConcoctNode* root_node = cct_parse_program(parser);
	if(parser->error == NULL)
	{
		cct_print_node(root_node, 0);
		cct_delete_node(root_node);
	}
	else
	{
		fprintf(stderr, "Parsing error: [%i] %s, got %s", parser->error_line, parser->error, cct_token_type_to_string(parser->current_token.type));
	}
	cct_delete_parser(parser);
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
	ConcoctLexer* file_lexer = cct_new_file_lexer(input_file);
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
		printf("[%i] %s : %s\n", token.line_number, file_lexer->token_text, cct_token_type_to_string(token.type));
		token = cct_next_token(file_lexer);
	}
	fclose(input_file);
	cct_delete_lexer(file_lexer);
	return;
}

// Lexes string
void lex_string(const char* input_string)
{
	// Lexer also can be created for strings
	//const char* input = "func test() { return a + b }";
	ConcoctLexer* string_lexer = cct_new_string_lexer(input_string);
	ConcoctToken token = cct_next_token(string_lexer);

	//printf("Lexing string...\n");
	while(token.type != CCT_TOKEN_EOF)
	{
		if(string_lexer->error != NULL)
		{
			fprintf(stderr, "Error on line %i:\n", token.line_number);
			fprintf(stderr, "%s\n", string_lexer->error);
			break;
		}
		printf("[%i] %s : %s\n", token.line_number, string_lexer->token_text, cct_token_type_to_string(token.type));
		token = cct_next_token(string_lexer);
	}
	cct_delete_lexer(string_lexer);
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

void print_version()
{
	printf("Concoct v%s\n", VERSION);
	return;
}

// Interactive mode
void interactive_mode()
{
	char input[1024];
	puts("Note: Only lexing and partial parsing is currently supported.");
	while(true)
	{
		memset(input, 0, sizeof(input)); // reset input every iteration
		printf("> ");
		fgets(input, 1024, stdin);

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
#ifdef _WIN32
		if(_stricmp(input, "quit") == 0)
			clean_exit(EXIT_SUCCESS);
#else
		if(strcasecmp(input, "quit") == 0)
			clean_exit(EXIT_SUCCESS);
#endif
		lex_string(input);
		parse_string(input);
	}
	return;
}
