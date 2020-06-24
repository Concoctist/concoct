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
#include <stdio.h>   // FILE, fclose(), fgets(), fprintf(), printf(), puts(), stdin
#include <stdlib.h>  // exit()
#include <string.h>  // memset(), strcasecmp()/stricmp(), strcspn(), strerror(), strlen()
#include "concoct.h" // lex_file(), lex_string()
#include "version.h" // VERSION

#include "concoct_lexer.h"

int main(int argc, char** argv)
{
	printf("Concoct v%s\n", VERSION);

	if(argc == 1)
		interactive_mode();

	if(argc > 2)
	{
		puts("Too many arguments!");
		exit(EXIT_FAILURE);
	}

	lex_file(argv[1]);

	return 0;
}

void lex_file(const char* file_name)
{
	FILE* input_file = fopen(file_name, "r");
	if(input_file == NULL)
	{
		fprintf(stderr, "Error opening %s: %s\n", file_name, strerror(errno));
		exit(EXIT_FAILURE);
	}

	// Creates a new lexer and iterates through the tokens
	struct ConcoctLexer* file_lexer = cct_new_file_lexer(input_file);
	struct ConcoctToken token = cct_next_token(file_lexer);
	// Continues printing until an EOF is reached
	printf("Lexing %s:\n", file_name);
	while(token.type != CCT_TOKEN_EOF)
	{
		if(file_lexer->error != NULL)
		{
			printf("Error on line %i.\n", token.line_number);
			printf("%s\n", file_lexer->error);
			break;
		}
		printf("[%i] %s : %s\n", token.line_number, file_lexer->token_text, cct_token_type_to_string(token.type));
		token = cct_next_token(file_lexer);
	}
	fclose(input_file);
	cct_delete_lexer(file_lexer);
	return;
}

void lex_string(const char* input_string)
{
	// Lexer also can be created for strings
	//const char* input = "func test() { return a + b }";
	struct ConcoctLexer* string_lexer = cct_new_string_lexer(input_string);
	struct ConcoctToken token = cct_next_token(string_lexer);

	//printf("Lexing string...\n");
	while(token.type != CCT_TOKEN_EOF)
	{
		if(string_lexer->error != NULL)
		{
			printf("Error on line %i\n", token.line_number);
			printf("%s\n", string_lexer->error);
			break;
		}
		printf("[%i] %s : %s\n", token.line_number, string_lexer->token_text, cct_token_type_to_string(token.type));
		token = cct_next_token(string_lexer);
	}
	cct_delete_lexer(string_lexer);
	return;
}

void interactive_mode()
{
	char input[1024];
	puts("Note: Only lexing is currently supported.");
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
			exit(EXIT_SUCCESS);
#else
		if(strcasecmp(input, "quit") == 0)
			exit(EXIT_SUCCESS);
#endif
		lex_string(input);
	}
	return;
}
